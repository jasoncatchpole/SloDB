# Python Code

The python code is ported from the original C++ but includes some bug fixes and improvements not in the C++ code. Additionally (and most importantly) the python version of the code includes a simple GUI that makes it easy to load in videos, and output synchronized frames from the videos loaded. The GUI is built using [Python's tkinter](https://docs.python.org/3/library/tkinter.html) package, however, if you are looking for a great example of a GUI then keep looking as i'm not known for my GUIs :)
That being said, if my tkinter code helps you then right on.

## Required python packages
Checked into the `src/python` directory is a requirements.txt file that you can use to install the required pip packages. In short the following are used: OpenCV, numpy, and Pillow. I recommend you setup a virtual environment to install these required packages into, see [this page](https://docs.python.org/3/library/venv.html) for more info if you are new to them.

