# SloDB

## Introduction
SloDB (short for Slovenian video Database) is a dataset useful for testing/evaluating computer vision algorithms. The dataset captures video sequences containing repeatable motion but varying environmental conditions and at varying times of day. This makes it useful for evaluating computer vision algorithms under changeable illumination and weather conditions. This dataset was captured during the course of my PhD. If you would like detailed information on this dataset please read chapter 4 of [my thesis](https://hdl.handle.net/10289/2581). What follows is an abridged explanation along with links to the dataset and code to process it.

## Details
The dataset is ~116GB and covers a capture period between 23/01/2006 and 09/02/2006 (videos were captured on 11 days during this period). The scene being captured was one from a University building in [Ljubljana, Slovenia](https://en.wikipedia.org/wiki/Ljubljana). The view from this window can be seen in the below panorama.

![SloDB panorama](/readme_images/slo_panorama.jpg)

Unlike other datasets, SloDB sought to capture the same scene but with repeatable motion. This motion was achieved by mounting the camera onto a Arrick Robotics XY positioning table with MD-2 Dual Stepper Motor system enabling programmatic control and a repeatable motion path for each video capture. Furthermore, repeatable orientation changes during the motion sequence were obtained using a Directed Perception PTU (Pan/tilt unit).

The following image shows the camera mounted on the XY positioning table with PTU. Note also that an Intersense IC2 IMU was mounted on top of the camera but upon analysis of the results, it was found to be unstable to be useful, even though I elevated the IC2 above the camera in an attempt to reduce the influence of magnetic distortions on the measurements.

### Position & orientation measurements
One goal of the dataset is to enable experiments on computer vision algorithms where sequences of frames undergoing the same motion path can be tested but under varying illumination and environmental conditions. Position & orientation measurements were recorded per

The camera used for data collection was a Sony DCR-TRV70 DV HandyCam. 



### Example video sequence

### Limitations

## Source code



## Citations
If you find this project useful please cite the following work:

Catchpole, J. J. (2008). Adaptive Vision Based Scene Registration for Outdoor Augmented Reality (Thesis, Doctor of Philosophy (PhD)). The University of Waikato, Hamilton, New Zealand. Retrieved from https://hdl.handle.net/10289/2581
