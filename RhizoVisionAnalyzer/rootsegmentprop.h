/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: rootsegmentprop.h

Author: Anand Seethepalli (aseethepalli@noble.org)
Principal Investigator: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of RhizoVision Analyzer.

RhizoVision Analyzer is free software: you can redistribute it and/or modify
it under the terms of the NOBLE RESEARCH INSTITUTE, GENERAL PUBLIC LICENSE.

RhizoVision Analyzer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
NOBLE RESEARCH INSTITUTE GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the Noble Research Institute General Public License
along with RhizoVision Analyzer.  If not, see <https://github.com/noble-research-institute/RhizoVisionAnalyzer/blob/master/LICENSE>.
*/

#pragma once

#ifndef ROOTSEGMENTPROP_H
#define ROOTSEGMENTPROP_H

#include "common_types.h"

void getrootlength(cv::Mat skeleton, ListofListsRef<cv::Point> segments, PointList overlap, double &rootlen);
//double getrootlength(cv::Mat skeleton);

void getrootradii(cv::Mat skeleton, cv::Mat segment, cv::Mat dist, ListofListsRef<cv::Point> segments, ListofListsRef<double> radii, double& maxradius, int& maxradrowidx);
void getrootradiiforsegments(ListofListsRef<double> radii, ValueList avgradii);

void gethistogramofrootradii(cv::Mat skeleton, cv::Mat segment, ValueList x, IndexList y);

bool validatesegments(cv::Mat skeleton, PointList branch, PointList end, PointList overlap, ListofListsRef<cv::Point> rootsegments);

#endif
