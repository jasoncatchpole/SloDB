# SloDB

## Introduction
SloDB (short for Slovenian video Database) is a dataset useful for testing/evaluating computer vision algorithms. The dataset captures video sequences containing repeatable motion but varying environmental conditions and at various times of day. This makes it useful for evaluating computer vision algorithms under changeable illumination and weather conditions. This dataset was captured during the course of my PhD. If you would like detailed information on this dataset please read chapter 4 of [my thesis](https://hdl.handle.net/10289/2581). What follows is an abridged explanation along with links to the dataset and code to process it.

## Download
The dataset is stored on Google drive in the following folder [here](https://drive.google.com/drive/folders/1jD-LzSZnSQcRjP8Sm4DT5-TbUbxw2byG?usp=sharing).

The following are the individual links to the 7zip compressed files:
- [2006-01-23 - download - 7GB](https://drive.google.com/open?id=1MHblfCz5b0WikmQbJEWJPaXn76P51Xne)
- [2006-01-24 - download - 28GB](https://drive.google.com/open?id=1eH_LLBVun67xYKusnyGBIG-xXKbTzwO8)
- [2006-01-25 - download - 7GB](https://drive.google.com/file/d/1Khlq0nMzzk6Unr1z8IiIqBB4gUcF-fkN/view?usp=sharing)
- [2006-01-26 - download - 9GB](https://drive.google.com/open?id=15vSmEI7ZxsQ53ppvhR136U1mk61c9Sqm)
- [2006-01-27 - download - 6GB](https://drive.google.com/file/d/1bW52IKOam1V3lqlU7ZU2Xj5Vgp3rIcyc/view?usp=sharing)
- [2006-01-28 - download - 16GB](https://drive.google.com/open?id=1e9klK0HJxtTNTF_n8G6u44jquLFUioyC)
- [2006-01-30 - download - 5GB](https://drive.google.com/open?id=1BfOYdI6XbLcdIE7NBiUnjef3caLiKN__)
- [2006-01-31 - download - 4GB](https://drive.google.com/open?id=1ZaP3p5STHHAHuO1EoFPBKTzS57Qny-Il)
- [2006-02-02 - download - 6GB](https://drive.google.com/open?id=1qCR7pgBfN3lXbiHKO3eNieybU15ZcmM6)
- [2006-02-08 - download - 4GB](https://drive.google.com/open?id=1V0hIx__Mtp7DAS1SbmtZAmhnY964Pess)
- [2006-02-09 - download - 3GB](https://drive.google.com/open?id=15Awr7MY-BmDi8WaFwqTbIUhKyQu41KjR)

### File structure
Each folder is named based on the capture date (in format YYYY-MM-DD). Inside each folder is a series of .avi files (uses a DV Video codec) using naming format DD-M-YYYY_HH-MM.avi. Together with these files are the ground truth files containing the information required to establish the position and orientation that each frame was captured at.

## Details
The dataset is ~116GB and covers a capture period between 23/01/2006 and 09/02/2006 (videos were captured on 11 days during this period). The scene being captured was one from a University building in [Ljubljana, Slovenia](https://en.wikipedia.org/wiki/Ljubljana). The view from this window can be seen in the below panorama.

![SloDB panorama](/readme_images/slo_panorama.jpg)

Unlike other datasets, SloDB sought to capture the same scene but with repeatable motion. This motion was achieved by mounting the camera onto a Arrick Robotics XY positioning table with MD-2 Dual Stepper Motor system enabling programmatic control and a repeatable motion path for each video capture. Furthermore, repeatable orientation changes during the motion sequence were obtained using a Directed Perception PTU (Pan/tilt unit). The following image shows the camera mounted on the XY positioning table with PTU. 

<img src="/readme_images/capture_in_situ.jpg" width="400" alt="SloDB capture system in situ">

Note also that an Intersense IC2 IMU was mounted on top of the camera but upon analysis of the results, it was found to be unstable to be useful, even though I elevated the IC2 above the camera in an attempt to reduce the influence of magnetic distortions on the measurements.

### Position & orientation measurements
One goal of the dataset is to enable experiments on computer vision algorithms where sequences of frames undergoing the same motion path can be tested but under varying illumination and environmental conditions. The API for the XY positioning table was modified to return position measurements per step. For each frame the following information is stored: frame number, x and y position measured from the positioning table, yaw, pitch and roll from the IC2, time of frame capture, the last known pan and tilt position of the PTU, and the times at which those readings were taken. The stage of the movement is also stored; this breaks the motion sequence into stages based on the type of motion allowing easier retrieval of aligned frames. In addition, at the beginning of each pan-tilt motion the time and frame number at which the movement started is recorded. Using this information together with known parameters such as the speed, acceleration, and position of the PTU, which are precisely controlled, the orientation can be accurately predicted. This provides a secondary method of determining orientation in addition to the measurements recorded directly from the PTU. 

Two methods of establishing the orientation are used because the first method has a limited accuracy. The PTU was controlled using a serial interface. Attempting to retrieve the current position of the PTU per frame was not feasible, as doing so caused the system to move erratically. For this reason the PTU position is queried every 30ms, meaning for any given frame the recorded pan and tilt position could be off by at most 30ms worth of movement. Each time the PTU was queried the results were stored along with the time of the query. This way, for every frame recorded, the last known PTU position and the time at which that result was taken is known. Using the time at which the current frame was captured, the orientation results can be interpolated between frames during post processing. This provides accurate orientation positions for each frame. During this step, the secondary source of orientation information on when a pan tilt motion started is also incorporated to further increase the accuracy of the orientation data.

The camera used for data collection was a Sony DCR-TRV70 DV HandyCam. Video was captured at a resolution of 720x576 (PAL) with a frame rate of 25 fps. Video was captured using an additional Sony VCL-0637 wide angle lens that provides a 0.6x magnification yielding a FOV of 89◦. Additionally, some videos were captured without this lens to provide another avenue for experimentation with the dataset. Without the wide angle lens the FOV was 53◦.

The following image shows frames extracted from different videos all of the same position & orientation.
![Different times of day](/readme_images/different_times.png)

### Example video sequence
Here is one of the videos from the dataset to get a feel for the motion sequence captured.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=CAeCfXi1LsU" target="_blank"><img src="http://img.youtube.com/vi/CAeCfXi1LsU/0.jpg" 
alt="Example video from SloDB" width="360" height="289" border="10" /></a>

### Limitations
There are several limitations to this dataset. Firstly, during some motion sequences there is a reflection of the camera's lens. Secondly, no ground truth information on the weather conditions were recorded. Lastly, due to the relatively short duration the capture system was set up it does not capture any significant seasonal variation. Additionally, during the capture period it did not capture rainfall or snowfall.

## Source code
There are two versions of the source code, one in python and another in C++. The C++ version is the original code developed during my PhD. The python version was ported from the C++ version, the primary purpose of both versions is to read in and take advantage of the ground truth information to enable synchronization between any given video and any number of other videos, e.g. determine frames corresponding to frame 22 from video 1 to videos 6, 9, and 12. Unlike the C++ version the python version has a simple GUI to actually load and display the video files synchronized with each other such that the user can seek to a given frame in the reference video and see all other videos automatically seek to the closest matching frames.

It is highly recommended to use the python version as not only does it have a GUI but there are also several improvements and bug fixes not present in the original C++. Check out [the readme in the python folder](/src/python/) for more details about the python code.

## The GUI

### Running
The GUI is only available with the python version of the SloDB code. For more information about installing the various packages required by this GUI please see [the readme in the python folder](/src/python/). Having installed all the required packages the GUI can be run by running the `slodb_gui.py` source file.

### Overview
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
Before capturing any frames an output directory must be specified where the captured frames will be stored. Capturing synchronised frames can be done using the "Capture images" button. Simply advance the video to the position you want and then click the button. This will produce files with the following format: <capture number>-<video file name>-<frame number>. Additionally, the frame from the reference video will have a `-ref` suffix at the end. For example, if the "Capture images" button was pressed in the above figure showing the 5 videos loaded in the resulting files would be.
  ```
  0-2-2-2006_12-9.avi-618-ref.jpg
  0-8-2-2006_16-13.avi-515.jpg
  0-8-2-2006_17-1.avi-615.jpg
  0-28-1-2006_6-39.avi-414.jpg
  0-30-1-2006_17-19.avi-625.jpg
  ```
If another set of images were captured the prefix would be `1-` as this would be the second capture etc.

## Citations
If you find this project useful please cite the following work:

Catchpole, J. J. (2008). Adaptive Vision Based Scene Registration for Outdoor Augmented Reality (Thesis, Doctor of Philosophy (PhD)). The University of Waikato, Hamilton, New Zealand. Retrieved from https://hdl.handle.net/10289/2581
