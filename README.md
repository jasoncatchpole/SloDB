# SloDB

## Introduction
SloDB (short for Slovenian video Database) is a dataset useful for testing/evaluating computer vision algorithms. The dataset captures video sequences containing repeatable motion but varying environmental conditions and at varying times of day. This makes it useful for evaluating computer vision algorithms under changeable illumination and weather conditions. This dataset was captured during the course of my PhD. If you would like detailed information on this dataset please read chapter 4 of [my thesis](https://hdl.handle.net/10289/2581). What follows is an abridged explanation along with links to the dataset and code to process it.

## Download
The dataset is stored on Google drive in the following folder [here](https://drive.google.com/drive/folders/1jD-LzSZnSQcRjP8Sm4DT5-TbUbxw2byG?usp=sharing).

The following are the individual links to the 7zip compressed files:
- [2006-01-23 - download - 7GB](https://drive.google.com/open?id=1MHblfCz5b0WikmQbJEWJPaXn76P51Xne)
- [2006-01-24 - download - 28GB](https://drive.google.com/open?id=1eH_LLBVun67xYKusnyGBIG-xXKbTzwO8)
- [2006-01-25 - download - 8GB](https://drive.google.com/open?id=1u1meyvriTj2wRJn27AnuEThBVWmlmF5G)
- [2006-01-26 - download - 9GB](https://drive.google.com/open?id=15vSmEI7ZxsQ53ppvhR136U1mk61c9Sqm)
- [2006-01-27 - download - 7GB](https://drive.google.com/open?id=1Nrli5MmPmS-_dW2Tyw7hTTu5MsfqFzzQ)
- [2006-01-28 - download - 16GB](https://drive.google.com/open?id=1e9klK0HJxtTNTF_n8G6u44jquLFUioyC)
- [2006-01-30 - download - 5GB](https://drive.google.com/open?id=1BfOYdI6XbLcdIE7NBiUnjef3caLiKN__)
- [2006-01-31 - download - 4GB](https://drive.google.com/open?id=1ZaP3p5STHHAHuO1EoFPBKTzS57Qny-Il)
- [2006-02-02 - download - 6GB](https://drive.google.com/open?id=1qCR7pgBfN3lXbiHKO3eNieybU15ZcmM6)
- [2006-02-08 - download - 4GB](https://drive.google.com/open?id=1V0hIx__Mtp7DAS1SbmtZAmhnY964Pess)
- [2006-02-09 - download - 3GB](https://drive.google.com/open?id=15Awr7MY-BmDi8WaFwqTbIUhKyQu41KjR)

## Details
The dataset is ~116GB and covers a capture period between 23/01/2006 and 09/02/2006 (videos were captured on 11 days during this period). The scene being captured was one from a University building in [Ljubljana, Slovenia](https://en.wikipedia.org/wiki/Ljubljana). The view from this window can be seen in the below panorama.

![SloDB panorama](/readme_images/slo_panorama.jpg)

Unlike other datasets, SloDB sought to capture the same scene but with repeatable motion. This motion was achieved by mounting the camera onto a Arrick Robotics XY positioning table with MD-2 Dual Stepper Motor system enabling programmatic control and a repeatable motion path for each video capture. Furthermore, repeatable orientation changes during the motion sequence were obtained using a Directed Perception PTU (Pan/tilt unit). The following image shows the camera mounted on the XY positioning table with PTU. 

![SloDB capture system in situ](/readme_images/capture_in_situ.jpg)

Note also that an Intersense IC2 IMU was mounted on top of the camera but upon analysis of the results, it was found to be unstable to be useful, even though I elevated the IC2 above the camera in an attempt to reduce the influence of magnetic distortions on the measurements.

### Position & orientation measurements
One goal of the dataset is to enable experiments on computer vision algorithms where sequences of frames undergoing the same motion path can be tested but under varying illumination and environmental conditions. Position & orientation measurements were recorded per

The camera used for data collection was a Sony DCR-TRV70 DV HandyCam. 

The following image shows frames extracted from different videos all of the same position & orientation.
![Different times of day](/readme_images/different_times.png)

### Example video sequence
<a href="http://www.youtube.com/watch?feature=player_embedded&v=CAeCfXi1LsU" target="_blank"><img src="http://img.youtube.com/vi/CAeCfXi1LsU/0.jpg" 
alt="Example video from SloDB" width="720" height="578" border="10" /></a>
### Limitations
There are several limitations to this dataset. Firstly, during some motion sequences there is a reflection of the 



## Source code
Coming soon...


## Citations
If you find this project useful please cite the following work:

Catchpole, J. J. (2008). Adaptive Vision Based Scene Registration for Outdoor Augmented Reality (Thesis, Doctor of Philosophy (PhD)). The University of Waikato, Hamilton, New Zealand. Retrieved from https://hdl.handle.net/10289/2581
