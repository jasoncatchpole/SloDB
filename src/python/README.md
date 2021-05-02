# Python Code

The python code is ported from the original C++ but includes some bug fixes and improvements not in the C++ code. Additionally (and most importantly) the python version of the code includes a simple GUI that makes it easy to load in videos, and output synchronized frames from the videos loaded. The GUI is built using [Python's tkinter](https://docs.python.org/3/library/tkinter.html) package, however, if you are looking for a great example of a GUI then keep looking as i'm not known for my GUIs :)
That being said, if my tkinter code helps you then right on.

## Required python packages
Checked into the `src/python` directory is a requirements.txt file that you can use to install the required pip packages. In short the following are used: OpenCV, numpy, and Pillow. I recommend you setup a virtual environment to install these required packages into, see [this page](https://docs.python.org/3/library/venv.html) for more info if you are new to them.

## Broad code overview
`slodb_gui.py` is the entry point for the GUI application. It mainly implements the tkinter GUI and holds instances of the `GroundTruthParser`, the `PoseRefiner` and the `FrameMatcher`. These three classes are the most important and are described at a high level here.

`GroundTruthParser` is the class that provides the functionality to load in the custom ground truth files that are stored with each video file. These ground truth files store information of the orientation and position for each frame in the video sequence.

`PoseRefiner` is the class that combines two different sources of ground truth information on camera pose for each frame to attempt to have the most accurate pose possible given the information captured at the time (it still isn't perfect in many cases).

`FrameMatcher` is used to take the refined pose estimates for multiple videos and allows one to query for matching, synchronized frames over many videos. e.g. it allows you to find out what is the matching frame in videos 1, 2, 3, and 4 for frame 27 in the reference video.

For more information please consult the code and the comments therein.
