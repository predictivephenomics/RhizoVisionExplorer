# ![RhiZoVision Analyzer Logo](./rvanalyzernew.png) RhizoVision Analyzer

RhizoVision Analyzer is an application to extract features from plant root images
by performing image processing operations. This program can optionally save the 
processed and thresholded images. Also, it can optionally convert the extracted 
traits to physical units.

#### Downloading
The software binary can be downloaded [here](https://zenodo.org/record/2585891).

The source code repository is located [here](https://github.com/rootphenomicslab/RhizoVisionAnalyzer).

#### Running the Program

The program opens a GUI where we specify the location of root crown images and
output location to save the processed images and the extracted features. We can
change the options that suit the input images and click the start button to start
extracting the feature traits from the root crown images.

![](./RV&#32;Analyzer&#32;Screenshot.png)

#### Pre-requisites for Running the Program

This program needs an x86_64 processor. Any Intel or AMD processor may be used 
for running the program. If the CPU supports Intel AVX 2.0, the program runs 
faster for feature extraction.

#### Compiling the Source

The source code is compiled using Microsoft Visual Studio. The program depends on
Qt, OpenCV and cvutil libraries for compiling the source.

#### Copyright and License

Copyright (C) 2018-2019 Noble Research Institute, LLC

**RhizoVision Analyzer**

RhizoVision Analyzer is free software: you can redistribute it and/or modify
it under the terms of the NOBLE RESEARCH INSTITUTE, GENERAL PUBLIC LICENSE.

RhizoVision Analyzer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
NOBLE RESEARCH INSTITUTE GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the Noble Research Institute General Public License
along with RhizoVision Analyzer.  If not, see <https://github.com/noble-research-institute/RhizoVisionAnalyzer/blob/master/LICENSE>.

#### Citing
Please use the following citation when using the software.

> Anand Seethepalli, & Larry York. (2019, March 6). RhizoVision Analyzer: Software for high-throughput measurements from images of crop root crowns (Version 1.0). Zenodo. http://doi.org/10.5281/zenodo.2585892  
> 
> [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2585892.svg)](https://doi.org/10.5281/zenodo.2585892)

#### Authors

Please send any comments/suggestions/improvements for the program to the authors:

Anand Seethepalli,  
Computer Vision Specialist,  
Root Phenomics Lab,  
Noble Research Institute, LLC  

email:  
aseethepalli@noble.org or anand_seethepalli@yahoo.co.in  

Larry York,  
Principal Investigator  
Root Phenomics Lab,  
Noble Research Institute, LLC  

email:  
lmyork@noble.org  

