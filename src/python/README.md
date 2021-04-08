# Python Code

The python code is ported from the original C++ but includes some bug fixes and improvements not in the C++ code. Additionally (and most importantly) the python version of the code includes a simple GUI that makes it easy to load in videos, and output synchronized frames from the videos loaded. The GUI is built using [Python's tkinter](https://docs.python.org/3/library/tkinter.html) package, however, if you are looking for a great example of a GUI then keep looking as i'm not known for my GUIs :)
That being said, if my tkinter code helps you then right on.

## Required python packages
Checked into the `src/python` directory is a requirements.txt file that you can use to install the required pip packages. In short the following are used: OpenCV, numpy, and Pillow. I recommend you setup a virtual environment to install these required packages into, see [this page](https://docs.python.org/3/library/venv.html) for more info if you are new to them.

## The GUI
Use of the GUI is pretty simple. The first video you load in is the "reference video". As you load subsequent videos in and seek (using either the seek bar or the next/previous frame buttons) all other videos loaded in will be synchronized so they should show (roughly) the same position/orientation as in the reference video. The below image shows this GUI with a single video loaded.

![Python GUI - single video](/readme_images/python_gui_single_vid.png)

### Loading videos
To load in another video click the "Add new source" button and then select the video from the SloDB collection to load in. The "video seek slider" can be used to seek within the reference video to any location desired. After seeking all other videos loaded in will be automatically synchronized to the corresponding position. The image below shows 5 videos loaded in with the current seek position at 6.8 percent through the video.

![Python GUI - multiple video](/readme_images/python_gui_multi_vid.png)

Along with each video the GUI shows some information about the video. For instance the reference video in this case (shown in the top left slot) is `2-2-2006_12-9.avi`. Below the file name it shows the current frame number (618 in this case) and in brackets the total number of frames in this video (9092 in this example).

In order to find the closest matching frame for each video to the reference a distance measure is computed which linearly combines the eucliden distance of the orientation as well the euclidean distance of the position to produce a final distance measure. Each video pane shows the 'Distance to reference' which is the distance of the closest matching frame. The smaller the distance the better. If the distance text is red it means the distance is starting to become quite large and the matching frame may not be a good one.

If you wish to debug the matching process and understand more about the orientation and position of each frame you can go to the 'Debug' menu at the top and select "Show Stage Debug". When activated this mode shows the orientation (pan and tilt) measurements for the current frame as well as the position (X and Y) and which stage of the motion sequence the frame is currently in (stage ranges from 0-8). 

### Navigating the videos
In order to advance the video there are several buttons. The primary one is the previously mentioned "Video seek slider" which allows you to easily jump to any point in the video. For finer grain control there are the "<<" and ">>" buttons which allow the playback position to be reversed or advanced by one frame respectively. If you wish to simply play out the video as though it was captured you can click the "Play" button and "Stop" to cease playback. During playback the other videos will remain synced with the reference video.

### Capturing synchronised frames
