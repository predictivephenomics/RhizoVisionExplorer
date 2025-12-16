/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: feature_config.h

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

This file is part of RhizoVision Explorer.

RhizoVision Explorer is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

RhizoVision Explorer is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with RhizoVision Explorer; see the file COPYING.
If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef FEATURE_CONFIG_H
#define FEATURE_CONFIG_H

#include <opencv2/opencv.hpp>
#include <vector>

#ifdef BUILD_GUI
#include <QtCore/QString>
#else
#include <string>
typedef std::string QString;
#endif

#define RHIZOVISION_EXPLORER_VERSION "2.5.0 Beta"

class MainUI;

typedef struct _feature_config
{
    cv::Mat input;
    cv::Mat seg;
    cv::Mat processed;
    std::vector<double> features;
    std::vector<double> rootlengthhist;
    std::vector<std::vector<double>> roifeatures;
    std::vector<std::vector<double>> roirootlengthhist;
    
    // Options for the console application rv
    std::string inputPath;
    std::string imagefilename;
    std::string outputPath = ""; // New option for output directory
    std::string outputFile = "features.csv";
    bool recursive = false;
    bool verbose = false;
    bool showHelp = false;
    bool showHelpMain = false; // To show main help only. False when showing help for errors.
    bool showVersion = false;
    bool showLicense = false;
    bool showCredits = false;
    bool noappend = false; // To not append to existing feature file when this is set.
    bool consolemode = false; // To indicate if the program is running in console mode.
    
    cv::Mat rtdpoints; // To get length-diameter profile

    int rotation = 0;

    // UI option for type of root being processed
    // 0 = Whole root
    // 1 = Broken roots
    int roottype = 1;

    // UI option for threshold to be applied for segmentation
    int threshold = 200;

    // UI option for image color inversion
    bool invertimage = false;

    // UI options for line smoothing
    bool enablesmooththresh = false;
    double smooththresh = 2.0;

    // UI options for filtering noisy components
    bool keepLargest = true;
    bool filterbknoise = false;
    bool filterfgnoise = false;
    double maxcompsizebk = 1.0;
    double maxcompsizefg = 1.0;

    // UI option for root pruning
    bool enableRootPruning = false;
    int rootPruningThreshold = 1;

    // UI options for convert pixels to physical units
    bool pixelconv = false;
    double conversion = 1.0;
    int pixelspermm = 0; // 0 for DPI, 1 for Pixels per mm

    // UI options for diameter ranges
    std::vector<double> dranges = {2.0, 5.0};

    // Setting to interactively change the image being shown
    int displayOutputIndex = 0;

    // UI options for displaying output options
    bool showConvexHull = true; // For whole root only
    bool showHoles = true;      // For whole roots only
    bool showDistMap = false;
    bool showMedialAxis = true;
    int medialaxiswidth = 3;
    bool showMedialAxisDiameter = true;
    bool showContours = true;
    int contourwidth = 1;

    // Batch processing options
    bool batchmode = false;
    bool savesegmented = false;
    bool saveprocessed = false;
    QString segsuffix, prosuffix;
    QString featurecsvfile = "features.csv";
    QString metadatacsvfile = "metadata.csv";

    // Needed reporting progress in interactive mode.
    MainUI *ui;
    bool abortprocess = false;
    
    // For logging messages
    QString imagename;
} feature_config;

#endif // FEATURE_CONFIG_H
