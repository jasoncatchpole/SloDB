from tkinter import *
import math
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
window_height = 1080

# TODO:
# - Button - add new source
# - when button clicked a new VideoCapture is created and added to a list
# - reference video should have a label
# - all videos should have a label with the date/time filename
# - when new video capture source is added add a new panel which will store the current frame
# - Every time a new source added load in the corresponding ground truth.
#     - pose refinement can happen on the source
#     - might need to perform the frame matcher operation again as another source has been added
# - perhaps a button to force all sources to jump to a certain stage
# - a seek slider which causes the reference (first) video source to seek to that location.
#     - other sources will seek based on the reference frame
# - For each video should show the video name but also which frame it is currently showing. Showing 
#   which frame number will make it easier to tell when the seeking ends up getting a frame from some random place in video

number_sources = 0
sources = []
video_panels = []
video_panel_labels = []
frame_matcher = None
all_frames = []

class GuiVideoSource:
    def __init__(self, video_capture_source, video_path):
        """Handles everything related to a video source used inside the seeva Gui application"""

        global video_frame1, video_frame2, number_sources

        self._setup_successfully = False
        self._file_source = video_capture_source
        #self.video_panel = panel

        if not video_path.endswith(".avi"):
            print(f'ERROR: Cannot find the .avi extension in video file with path {video_path}')
            return

        self.video_panel = None
        if number_sources < 5:
            self.gui_panel = LabelFrame(video_frame1, text=video_path)
        else:
            self.gui_panel = LabelFrame(video_frame2, text=video_path)
        self.gui_panel.pack(side="left")
        self.frame_number_label = Label(self.gui_panel, text='0')
        self.frame_number_label.pack()
        self.frame_data = None
        self._video_file_path = video_path
        if not self.__read_ground_truth():
            print('Failed to read in the ground truth data')
            return

        self.num_frames = len(self.frame_data)

        # read in the first frame and set the panel to this value
        ret, frame = self._file_source.read()

        self.__set_current_image(frame, 0)
        
        # image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        # print(f'Size of image before is {image.shape}')
        # image = cv2.resize(image, (math.ceil(image.shape[1] * video_frame_scale_factor),\
        #     math.ceil(image.shape[0] * video_frame_scale_factor)))
        # print(f'Size of image is {image.shape}')

        # # convert the images to PIL format...
        # image = Image.fromarray(image)

        # # ...and then to ImageTk format
        # image = ImageTk.PhotoImage(image)

        # if the panels are None, initialize them
        #if panelA is None:
            # the first panel will store our original image
        #self.video_panel = Label(image=image)
        #self.video_panel.image = image
        self.video_panel.pack(side="left", padx=10, pady=10)

        self._setup_successfully = True
    
    def __set_current_image(self, opencv_frame, frame_num):
        """Sets the current image for the gui panel to that provided"""

        # perform some conversion to change from opencv image to tkinter

        #  represents images in BGR order; however PIL represents
        # images in RGB order, so we need to swap the channels
        image = cv2.cvtColor(opencv_frame, cv2.COLOR_BGR2RGB)
        #print(f'Size of image before is {image.shape}')
        image = cv2.resize(image, (math.ceil(image.shape[1] * video_frame_scale_factor),\
            math.ceil(image.shape[0] * video_frame_scale_factor)))
        #print(f'Size of image is {image.shape}')

        # convert the images to PIL format...
        image = Image.fromarray(image)

        # ...and then to ImageTk format
        image = ImageTk.PhotoImage(image)

        if self.video_panel is None:
            print('Adding label to the combined panel')
            self.video_panel = Label(self.gui_panel, image=image)
            #self.gui_panel = Label(image=image)
            self.video_panel.image = image
        else:
            self.video_panel.configure(image=image)
            self.video_panel.image = image
        
        self.frame_number_label['text'] = 'Frame: ' + str(frame_num)

    
    def __read_ground_truth(self) -> bool:
        """Reads all required ground truth files and performs the pose refinement"""
        ground_truth_path = self._video_file_path[:-4] + '.csv'
        pandata_path = self._video_file_path[:-4] + '_PANDATA.csv'
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

    def seek(self, frame_number):
        # perform sanity check on the frame number requested
        if frame_number >= self.num_frames:
            print(f'Error - Cannot seek to frame {frame_number} as there are only {self.num_frames} in this source')
            return

        print(f'Got request to seek to {frame_number}')
        self._file_source.set(cv2.CAP_PROP_POS_FRAMES, frame_number)

        # now read the new frame
        ret, frame = self._file_source.read()

        self.__set_current_image(frame, frame_number)
        #pass

    def get_current_image(self):
        """Returns the current image based on the seek position in the video"""


def select_video():
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

        sources.append(GuiVideoSource(vid_source, path))
        all_frames.append(sources[number_sources-1].frame_data)
        frame_matcher = FrameMatcher(all_frames)

        # ret, frame = vid_source.read()

        # #  represents images in BGR order; however PIL represents
        # # images in RGB order, so we need to swap the channels
        # image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        # print(f'Size of image before is {image.shape}')
        # image = cv2.resize(image, (math.ceil(image.shape[1] * video_frame_scale_factor),\
        #     math.ceil(image.shape[0] * video_frame_scale_factor)))
        # print(f'Size of image is {image.shape}')

        # # convert the images to PIL format...
        # image = Image.fromarray(image)

        # # ...and then to ImageTk format
        # image = ImageTk.PhotoImage(image)

        # # if the panels are None, initialize them
        # #if panelA is None:
        #     # the first panel will store our original image
        # panelA = Label(image=image)
        # panelA.image = image
        # panelA.pack(side="left", padx=10, pady=10)
        # panel_label = Label(text=path)
        # panel_label.pack(side="left", padx=10, pady=10)
        # video_panel_labels.append(panel_label)
        # video_panels.append(panelA)

        # otherwise, update the image panels
        #else:
            # update the pannels
        #    panelA.configure(image=image)
        #    panelA.image = image

def capture_images():
    """Called when the capture images button is clicked"""
    pass

def seek_event(event):
    """Called when there is a change in the seek slider in the GUI. Updates all video sources to seek to the correct location"""
    global seek_slider, frame_matcher, sources, number_sources
    print(f'Event has occurred with value {seek_slider.get()}')

    if number_sources == 0:
        # nothing to do
        return

    # TODO: convert the floating value from seek_slider between 0-100 to a frame number based on num of frames in reference
    num_frames_reference = sources[0].num_frames
    reference_frame_to_seek = math.floor((seek_slider.get() / 100) * num_frames_reference)

    # pull out the new seek positions for this new frame
    seek_positions = frame_matcher.seek(reference_frame_to_seek)

    # now iterate over the sources and get each one to seek to the appropriate position
    print(f'Slider position is {seek_slider.get()} num frames is {num_frames_reference}')
    sources[0].seek(reference_frame_to_seek)
    if number_sources > 1:
        print(f'Seek positions are {seek_positions}')
        for src_index, source in enumerate(sources[1:], 1):
            source.seek(seek_positions[src_index-1])
    


window = Tk(className="SloDB GUI")
window.geometry(str(window_width) + "x" + str(window_height))

# create two frames:
# - the first will take up the most area and will store the video panes
# - the second will have the seek slider and the buttons
video_frame1 = Frame(window, width=window_width-50, height=window_height-700, pady=3)
video_frame1.pack(fill=X, expand=True)
video_frame2 = Frame(window, width=window_width-50, height=window_height-700, pady=3)
video_frame2.pack(fill=X, expand=True)
button_frame = Frame(window)
button_frame.pack(fill=X, expand=True, side="bottom")

#greeting = Label(text="Hello, Tkinter")
#greeting.pack()

# button font
button_font = font.Font(weight="bold", size=30)


capture_images_btn = Button(button_frame, text="Capture images", command=capture_images)
capture_images_btn.pack(side="right", expand="yes", padx="10", pady="10")
capture_images_btn['font'] = button_font

# create a button, then when pressed, will trigger a file chooser
# dialog and allow the user to select an input image; then add the
# button the GUI
select_image_btn = Button(button_frame, text="Add new source", command=select_video)
select_image_btn.pack(side="right", expand="yes", padx="10", pady="10")
select_image_btn['font'] = button_font



seek_slider = Scale(button_frame, from_=0, to=100, resolution=0.1, orient=HORIZONTAL, length=window_width-200, label="Video seek slider")
seek_slider.bind("<ButtonRelease-1>", seek_event)
seek_slider.pack(side="bottom")

window.mainloop()