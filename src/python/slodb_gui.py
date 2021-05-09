"""Main entry point for the GUI application. Sets up the window and all the controls"""

from tkinter import *
import math
import threading
import cv2
from PIL import Image
from PIL import ImageTk
import tkinter.filedialog as tkFileDialog
import tkinter.font as font
from pathlib import Path
from ground_truth_parser import GroundTruthParser
from pose_refiner import PoseRefiner
from frame_matcher import FrameMatcher

video_frame_scale_factor = 0.7
window_width = 2150
window_height = 1200

number_sources = 0
sources = []
video_panels = []
video_panel_labels = []
frame_matcher = None
all_frames = []
current_reference_frame = 0
capture_outputpath = ""
capture_number = 0
should_stop_playback = threading.Event()

class GuiVideoSource:
    def __init__(self, video_capture_source, video_path, is_reference=False):
        """Handles everything related to a video source used inside the seeva Gui application"""

        global video_frame1, video_frame2, number_sources

        self._setup_successfully = False
        self._file_source = video_capture_source
        self._num_frames_file_source = int(self._file_source.get(cv2.CAP_PROP_FRAME_COUNT))
        self._is_reference = is_reference

        if not video_path.endswith(".avi"):
            print(f'ERROR: Cannot find the .avi extension in video file with path {video_path}')
            return

        self._video_name = Path(video_path).name
        self.video_panel = None
        if number_sources < 5:
            self.gui_panel = LabelFrame(video_frame1, text=self._video_name, labelanchor='n')
        else:
            self.gui_panel = LabelFrame(video_frame2, text=self._video_name, labelanchor='n')
        self.gui_panel.pack(side="left")
        self.frame_number_label = Label(self.gui_panel, text='0')
        self.frame_number_label.pack()
        if not self._is_reference:
            self.dist_to_ref_label = Label(self.gui_panel, text='0')
        else:
            self.dist_to_ref_label = Label(self.gui_panel)
        self.dist_to_ref_label.pack()
        self.debug_info = Label(self.gui_panel, text='')
        self.debug_info.pack()
        self.debug_on = False
        self.frame_data = None
        self._video_file_path = video_path
        
        if not self.__read_ground_truth():
            print('Failed to read in the ground truth data')
            return

        self.num_frames = len(self.frame_data)

        if abs(self._num_frames_file_source - self.num_frames) > 100:
            print(f'ERROR: {self._video_name} - Number of frames in video does not match ground truth! Video has {self._num_frames_file_source} while ground truth indicates {self.num_frames}')

        self.current_frame = None
        self.current_frame_number = None

        # read in the first frame and set the panel to this value
        ret, frame = self._file_source.read()

        self.__set_current_image(frame, 0, 0)
        
        self.video_panel.pack(side="left", padx=10, pady=10)

        self._setup_successfully = True
    
    def __set_current_image(self, opencv_frame, frame_num, distance_to_reference):
        """Sets the current image for the gui panel to that provided"""

        # perform some conversion to change from opencv image to tkinter

        #  represents images in BGR order; however PIL represents
        # images in RGB order, so we need to swap the channels
        self.current_frame = opencv_frame.copy()
        image = cv2.cvtColor(opencv_frame, cv2.COLOR_BGR2RGB)
        self.current_frame_number = frame_num
        
        image = cv2.resize(image, (math.ceil(image.shape[1] * video_frame_scale_factor),\
            math.ceil(image.shape[0] * video_frame_scale_factor)))
        
        # convert the images to PIL format...
        image = Image.fromarray(image)

        # ...and then to ImageTk format
        image = ImageTk.PhotoImage(image)

        if self.video_panel is None:
            self.video_panel = Label(self.gui_panel, image=image)
            self.video_panel.image = image
        else:
            self.video_panel.configure(image=image)
            self.video_panel.image = image
        
        self.frame_number_label['text'] = 'Frame: ' + str(frame_num) + " (" + str(self._num_frames_file_source) + ")"
        
        if not self._is_reference:
            if distance_to_reference > 30:
                self.dist_to_ref_label['text'] = 'Distance to reference: ' + f'{distance_to_reference:.3f}' + ' - POOR MATCH'
                self.dist_to_ref_label.configure(foreground="red")
            else:
                self.dist_to_ref_label['text'] = 'Distance to reference: ' + f'{distance_to_reference:.3f}'
                self.dist_to_ref_label.configure(foreground="green")
        
        # if debug info is enabled we had better update it now
        self.__update_frame_debug_info()
    
    def __read_ground_truth(self) -> bool:
        """Reads all required ground truth files and performs the pose refinement"""
        # determine if this video file name has any additional information inside brackets. If so we'll need
        # to remove this info as the ground truth files won't have that same info
        if self._video_name.find("(") == -1:
            ground_truth_path = self._video_file_path[:-4] + '.csv'
            pandata_path = self._video_file_path[:-4] + '_PANDATA.csv'
        else:
            startSubstr = self._video_file_path.rfind("(")
            endSubstr = self._video_file_path.rfind(")")
            ground_truth_path = self._video_file_path[0:startSubstr-1] + ".csv"
            pandata_path = self._video_file_path[0:startSubstr-1] + '_PANDATA.csv'
        
        if not Path(ground_truth_path).is_file():
            print(f'ERROR: Ground truth file does not exist. Path = {ground_truth_path}')
            return False
        if not Path(pandata_path).is_file():
            print(f'ERROR: Ground truth PANDATA file does not exist. Path = {pandata_path}')
            return False

        file_parser = GroundTruthParser()
        file_lines = file_parser.read_file(ground_truth_path)
        pandata_lines = file_parser.read_file(pandata_path)
        self.frame_data = file_parser.parse_lines(file_lines)
        pan_data = file_parser.parse_pandata_lines(pandata_lines)

        refiner = PoseRefiner()
        refiner.refine_frame_poses(self.frame_data, pan_data)

        return True

    def set_debug(self, enable) -> None:
        """Enables or disables showing of debug information along with the video image.
        Shows pan, tilt, X & Y position, stage number, and number of frames in current stage"""
        self.debug_on = enable
        self.__update_frame_debug_info()
    
    def __update_frame_debug_info(self) -> None:
        """Updates the text string containing the debug info"""
        if not self.debug_on:
            self.debug_info['text'] = ''
            return
        # check to make sure we have the frame data element in question
        if self.current_frame_number >= len(self.frame_data):
            print(f'Error: attempting to update the debug info for frame {self.current_frame_number} but only {len(self.frame_data)} frames in source')
            self.debug_info['text'] = ''
            return
        self.debug_info['text'] = 'Pan: ' + f'{self.frame_data[self.current_frame_number].pan:.1f}' + ', Tilt: ' + f'{self.frame_data[self.current_frame_number].tilt:.1f}' +\
            ', X: ' + f'{self.frame_data[self.current_frame_number].x_pos}' + ', Y: ' + f'{self.frame_data[self.current_frame_number].y_pos}' +\
            ', Stage: ' + f'{self.frame_data[self.current_frame_number].stage}'

    def seek(self, frame_number, distance_to_reference):
        """Performs the seek in the video file source to that frame provided in 'frame_number'"""
        # perform sanity check on the frame number requested
        if frame_number >= self.num_frames:
            print(f'Error - Cannot seek to frame {frame_number} as there are only {self.num_frames} in this source')
            return

        self._file_source.set(cv2.CAP_PROP_POS_FRAMES, frame_number)

        # now read the new frame
        ret, frame = self._file_source.read()

        self.__set_current_image(frame, frame_number, distance_to_reference)

    def get_current_image(self):
        """Returns the current image based on the seek position in the video"""
        return self.current_frame, self.current_frame_number


def select_video():
    """Called when the user wishes to open another video. Sets up the video to be read in and updates the frame
    matcher to be able to match (synchronize) between all the videos including the new one"""
    # grab a reference to the image panels
    global video_panels, sources, number_sources, frame_matcher

    # open a file chooser dialog and allow the user to select an input
    # image
    path = tkFileDialog.askopenfilename(filetypes = [("avi files","*.avi")])

    # ensure a file path was selected
    if len(path) > 0:
        vid_source = cv2.VideoCapture(path)

        if not vid_source.isOpened():
            print(f'Failed to open video file with path = {path}')
            return
        
        number_sources += 1

        sources.append(GuiVideoSource(vid_source, path, number_sources==1))
        all_frames.append(sources[number_sources-1].frame_data)
        frame_matcher = FrameMatcher(all_frames)

        seek_event(None)

def select_output_dir():
    """Called when the user wishes to set the output directory to be used when the capture images button is pressed"""
    global capture_outputpath
    capture_outputpath = tkFileDialog.askdirectory()
    print(f'About to set output dir to {capture_outputpath}')

def capture_images():
    """Called when the capture images button is clicked"""
    global capture_outputpath, sources, number_sources, capture_number

    if number_sources == 0:
        # nothing to do
        return
    if capture_outputpath is None or capture_outputpath == "":
        print('Error - Cannot capture images as output capture directory has not been set')
        return

    # go through each source video and get a copy of the current frame as well as the name of the video and the frame number
    for src_index, source in enumerate(sources):
        # form the output filename, it will be of the form <capture number>-<video name>-<frame number>-<optional: ref>.jpg. Here the 'capture number'
        # is used to associate each of the frames together so they are grouped
        ref_frame = ""
        if src_index == 0:
            ref_frame = "-ref"
        out_filename = str(capture_number) + '-' + str(source._video_name) + '-' + str(source.current_frame_number) + ref_frame + '.jpg'
        output_path = Path(capture_outputpath, out_filename)

        print(f'Capturing image to {output_path}. Img has dimensions {source.current_frame.shape}')

        cv2.imwrite(str(output_path), source.current_frame)

    capture_number += 1

def next_frame() -> bool:
    """Called when the '>>' button is pressed which advances to the next frame"""
    global current_reference_frame

    if number_sources == 0:
        # nothing to do
        return False
    num_frames_reference = sources[0].num_frames

    if current_reference_frame >= num_frames_reference:
        return False

    current_reference_frame += 1
    perform_seek(current_reference_frame)
    update_scale_position()

    return True

def previous_frame():
    """Called when the '<<' button is pressed which goes back to the previous frame"""
    global current_reference_frame
    if current_reference_frame <= 0:
        return

    current_reference_frame -= 1
    perform_seek(current_reference_frame)
    update_scale_position()

def playout_video():
    """Performs the playout of the video, called by begin_playback() in a separate thread"""
    global should_stop_playback, current_reference_frame
    while not should_stop_playback.is_set():
        if not next_frame(): # if this fails it either means we have reached the end of the file or we don't even have a file open
            break

def begin_playback():
    """Called when the play button is clicked. Begins playback of the video along with synchronization
    of the other videos"""
    # kicks off a thread that continually calls "next_frame" in order to cycle through frames. Only stops
    # when the last frame of the reference video is reached or if instructed to stop by a stop button press
    global should_stop_playback
    should_stop_playback.clear()

    threading.Thread(target=playout_video).start()

def stop_playback():
    """Called when the stop button is clicked. Stops ongoing playback"""
    global should_stop_playback
    should_stop_playback.set()

def set_debug():
    """Called when the user opts to enable the debug information to show pan, tilt and stage info on top of the video"""
    global sources, show_stage_debug

    # iterate over all the video sources and instruct them to show debug info
    for source in sources:
        source.set_debug(show_stage_debug.get())

def seek_event(event):
    """Called when there is a change in the seek slider in the GUI. Updates all video sources to seek to the correct location"""
    global seek_slider, frame_matcher, sources, number_sources

    if number_sources == 0:
        # nothing to do
        return

    # convert the floating value from seek_slider between 0-100 to a frame number based on num of frames in reference
    num_frames_reference = sources[0].num_frames
    reference_frame_to_seek = math.floor((seek_slider.get() / 100) * num_frames_reference)
    perform_seek(reference_frame_to_seek)

def perform_seek(reference_frame_to_seek):
    """Performs the actual seek operation given the frame requested to see to in the reference video. This method
    also advances all other videos to their matching frames"""
    global sources, current_reference_frame, number_sources
    if number_sources == 0:
        # nothing to do
        return
    num_frames_reference = sources[0].num_frames
    if reference_frame_to_seek >= num_frames_reference:
        # nothing to do, frame requested is beyond end of video
        return
    sources[0].seek(reference_frame_to_seek, 0)
    current_reference_frame = reference_frame_to_seek

    if number_sources <= 1:
        # nothing more to do
        return

    # pull out the new seek positions for this new frame
    seek_positions, distances = frame_matcher.seek(reference_frame_to_seek)

    # now iterate over the sources and get each one to seek to the appropriate position
    for src_index, source in enumerate(sources[1:], 1):
        source.seek(seek_positions[src_index-1], distances[src_index-1])
    
def update_scale_position():
    """Updates the current value for the Scale slider indicating position through the video"""
    global seek_slider, current_reference_frame

    if number_sources == 0:
        # nothing to do
        return False
    num_frames_reference = sources[0].num_frames

    # determine the seek sliders value in order to accurately reflect the reference frame number we are now on
    reference_frame_to_seek = (current_reference_frame / num_frames_reference) * 100
    seek_slider.set(reference_frame_to_seek)

# setup tkinter window
window = Tk(className="SloDB GUI")
window.geometry(str(window_width) + "x" + str(window_height))

# setup menu bar for tkinter window
menubar = Menu(window)
file_menu = Menu(menubar, tearoff=0)
file_menu.add_command(label="Add new source", command=select_video)
file_menu.add_command(label="Exit", command=window.quit)
menubar.add_cascade(label="File", menu=file_menu)
debug_menu = Menu(menubar, tearoff=0)
show_stage_debug = BooleanVar(window)
debug_menu.add_checkbutton(label="Show Stage Debug", onvalue=1, offvalue=0, variable=show_stage_debug, command=set_debug)
menubar.add_cascade(label="Debug", menu=debug_menu)

# create three frames:
# - the first will store the top row of video panes
# - the second will store the bottom row of video panes
# - the third will store the controls
video_frame1 = Frame(window, width=window_width-50, height=window_height-700, pady=3)
video_frame1.pack(fill=X, expand=True)
video_frame2 = Frame(window, width=window_width-50, height=window_height-700, pady=3)
video_frame2.pack(fill=X, expand=True)
button_frame = Frame(window)
button_frame.pack(fill=X, expand=True, side="bottom")

# button font
button_font = font.Font(weight="bold", size=15)

# setup the button for setting the output dir
output_dir_btn = Button(button_frame, text="Set output dir", command=select_output_dir)
output_dir_btn.pack(side="right", expand="yes", padx="10", pady="10")
output_dir_btn['font'] = button_font

# setup button for capturing the images
capture_images_btn = Button(button_frame, text="Capture images", command=capture_images)
capture_images_btn.pack(side="right", expand="yes", padx="10", pady="10")
capture_images_btn['font'] = button_font

# create a button that when pressed, will trigger a file chooser
# dialog and allow the user to select an input image
select_image_btn = Button(button_frame, text="Add new source", command=select_video)
select_image_btn.pack(side="right", expand="yes", padx="10", pady="10")
select_image_btn['font'] = button_font

# next two buttons advance video by one frame or go back by one respectively
next_frame_btn = Button(button_frame, text='>>', command=next_frame)
next_frame_btn.pack(side="right", expand="yes", padx="10", pady="10")
next_frame_btn['font'] = button_font

prev_frame_btn = Button(button_frame, text="<<", command=previous_frame)
prev_frame_btn.pack(side="right", expand="yes", padx="10", pady="10")
prev_frame_btn['font'] = button_font

# stops continuous playback if initiated by the play button below
stop_btn = Button(button_frame, text="Stop", command=stop_playback)
stop_btn.pack(side="right", expand="yes", padx="10", pady="10")
stop_btn['font'] = button_font

# begins continuous playback
play_btn = Button(button_frame, text="Play", command=begin_playback)
play_btn.pack(side="right", expand="yes", padx="10", pady="10")
play_btn['font'] = button_font

# the slider at the bottom that allows seeking to any point in the video
seek_slider = Scale(button_frame, from_=0, to=100, resolution=0.1, orient=HORIZONTAL, length=window_width-200, label="Video seek slider")
seek_slider.bind("<ButtonRelease-1>", seek_event)
seek_slider.pack(side="bottom")

window.config(menu=menubar)
window.mainloop()