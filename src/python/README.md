# Python Code

The python code is ported from the original C++ but includes some bug fixes and improvements not in the C++ code. Additionally (and most importantly) the python version of the code includes a simple GUI that makes it easy to load in videos, and output synchronized frames from the videos loaded. The GUI is built using [Python's tkinter](https://docs.python.org/3/library/tkinter.html) package, however, if you are looking for a great example of a GUI then keep looking as i'm not known for my GUIs :)
That being said, if my tkinter code helps you then right on.

## Required python packages
Checked into the `src/python` directory is a requirements.txt file that you can use to install the required pip packages. In short the following are used: OpenCV, numpy, and Pillow. I recommend you setup a virtual environment to install these required packages into, see [this page](https://docs.python.org/3/library/venv.html) for more info if you are new to them.

## The GUI
Use of the GUI is pretty simple. The first video you load in is the "reference video". As you load in subsequent videos in and seek (using either the seek bar or the next/previous frame buttons) all other videos loaded in will be synchronized so they should show (roughly) the same position/orientation as in the reference video. The below image shows this GUI with a single video loaded.

![Python GUI - single video](/readme_images/python_gui_single_vid.png)

To load in another video click the "Add new source" button and then select the video from the SloDB collection to load in. The "video seek slider" can be used to seek within the reference video to any location desired. After seeking all other videos loaded in will be automatically synchronized to the corresponding position. The image below shows 5 videos loaded in with the current seek position at 6.8 percent through the video.

![Python GUI - multiple video](/readme_images/python_gui_multi_vid.png)

Along with each video the GUI shows some information about the video. For instance the reference video in this case (shown in the top left slot) is `2-2-2006_12-9.avi`. Below the file name it shows the current frame number (618 in this case) and in brackets the total number of frames in this video (9092 in this example).
