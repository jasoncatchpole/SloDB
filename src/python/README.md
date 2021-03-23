# Python Code

The python code is ported from the original C++ but includes some bug fixes and improvements not in the C++ code. Additionally (and most importantly) the python version of the code includes a simple GUI that makes it easy to load in videos, and output synchronized frames from the videos loaded.

## The GUI
Use of the GUI is pretty simple. The first video you load in is the "reference video". As you load in subsequent videos in and seek (using either the seek bar or the next/previous frame buttons) all other videos loaded in will be synchronized so they should show (roughly) the same position/orientation as in the reference video.

## Libraries used
Checked into the `src/python` directory is a requirements.txt file that you can use to install the required pip packages. In short the following are used: OpenCV, numpy, and Pillow. 

