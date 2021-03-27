# SloDB

## Introduction
SloDB (short for Slovenian video Database) is a dataset useful for testing/evaluating computer vision algorithms. The dataset captures video sequences containing repeatable motion but varying environmental conditions and at various times of day. This makes it useful for evaluating computer vision algorithms under changeable illumination and weather conditions. This dataset was captured during the course of my PhD. If you would like detailed information on this dataset please read chapter 4 of [my thesis](https://hdl.handle.net/10289/2581). What follows is an abridged explanation along with links to the dataset and code to process it (code coming soon).

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

It is highly recommended to use the python version as not only does it have a GUI but there are also several improvements and bug fixes not present in the original C++. Check out [the readme in the python folder](/src/python/README.md) for more details about the python code.

## Citations
If you find this project useful please cite the following work:

Catchpole, J. J. (2008). Adaptive Vision Based Scene Registration for Outdoor Augmented Reality (Thesis, Doctor of Philosophy (PhD)). The University of Waikato, Hamilton, New Zealand. Retrieved from https://hdl.handle.net/10289/2581
