# ![RhiZoVision Explorer Logo](./RVElogoclearback-80.png) RhizoVision Explorer

RhizoVision Explorer is free and open-source software developed for estimating root traits from images acquired from a flatbed scanner or camera. Root images are expected to have a high contrast of roots with the background, homogenous lighting, and minimal overlapping of roots. The software is built in C++ for speed and stability, using QT for the graphical user interface and OpenCV for image processing. Traits extracted by RhizoVision Explorer have been extensively validated using a physical copper wire ground truth image set, thousands of simulated roots, and comparisons with other image analysis software. The default “Broken Roots” mode is meant for roots washed out from soil cores or pots that are disconnected and provides length, volume, branching frequency, among other features, with the ability to bin measurements based on diameter thresholds. The “Whole Root” mode extracts additional root system architecture features of more intact root systems, especially excavated root crowns or rhizoboxes, such as the convex hull, angles, and holes. The software supports multiple regions of interest, batch mode, and user-defined export of processed images with overlaid features for use in reports.

#### Downloading
The software binary can be downloaded [here](https://doi.org/10.5281/zenodo.3747697).

The source code repository is located [here](https://github.com/noble-research-institute/RhizoVisionExplorer).

#### Running the Program

The program opens a GUI where we specify the location of root images and output location to save the processed images and the extracted features. We can change the options that suit the input images and click the start button to start extracting the feature traits from the root images.

![](./RhizoVisionExplorer-window.png)

#### Pre-requisites for Running the Program

This program needs an x86_64 processor. Any Intel or AMD processor may be used for running the program. If the CPU supports Intel AVX 2.0, the program runs faster for feature extraction.

#### Compiling the Source

The source code is compiled using Microsoft Visual Studio. The program depends on Qt 5.12.3, OpenCV 4.1 and [cvutil](https://github.com/noble-research-institute/cvutil) 2.0.2 libraries for compiling the source.

#### Copyright and License

Copyright (C) 2018-2020 Noble Research Institute, LLC

**RhizoVision Explorer**

RhizoVision Explorer is free software. You can redistribute it and/or modify it as permissible under the terms of the Noble General Public License as published by the Noble Research Institute, LLC. This license is available at the following link.

RhizoVision Explorer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Noble General Public License for more details.

You should have received a copy of the Noble General Public License along with RhizoVision Explorer.  If not, see <https://github.com/noble-research-institute/RhizoVisionExplorer/blob/master/LICENSE>.

#### Citing
Please use the following citation when using the software.

> Anand Seethepalli, & Larry York. RhizoVision Explorer - Interactive software for generalized root image analysis designed for everyone (Version 2.0.2). Zenodo. https://doi.org/10.5281/zenodo.3747697

#### Authors

Please send any comments/suggestions/improvements for the program to the authors:

Anand Seethepalli  
Computer Vision Analyst  
Root Phenomics Lab  
Noble Research Institute, LLC  


email:  
aseethepalli@noble.org  
anand_seethepalli@yahoo.co.in  

Larry York  
Assistant Professor  
Root Phenomics Lab  
Noble Research Institute, LLC

email:   
lmyork@noble.org

