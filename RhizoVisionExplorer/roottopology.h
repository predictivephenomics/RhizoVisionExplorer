/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: roottopology.h

Author: Anand Seethepalli (aseethepalli@noble.org)
Assistant Professor: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of RhizoVision Explorer.

RhizoVision Explorer is free software. You can redistribute it
and/or modify it as permissible under the terms of the Noble
General Public License as published by the Noble Research
Institute, LLC. This license is available at the following link.

RhizoVision Explorer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the Noble General Public License for more details.

You should have received a copy of the Noble General Public License
along with RhizoVision Explorer.  If not, see <https://github.com/noble-research-institute/RhizoVisionExplorer/blob/master/LICENSE>.
*/

#pragma once

#ifndef ROOTTOPOLOGY_H
#define ROOTTOPOLOGY_H

#include "common_types.h"

void getroottopology(cv::Mat &_skeleton, cv::Mat dist, PointList branchpts, PointList endpts, PointList overlappts, ListofListsRef<cv::Point> rootsegments, bool pruneroots, int rootPruningThreshold);

cv::Mat drawtopology(cv::Mat segmented, cv::Mat skeleton, PointList branchpts, PointList endpts, PointList overlappts);
cv::Mat drawtopology2(cv::Mat segmented, cv::Scalar_<int> ranges, cv::Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts);

cv::Mat drawtopology_wholeroot(cv::Mat img, cv::Scalar_<int> ranges, std::vector<std::vector<int>> cc, std::vector<int> colorcodes, std::vector<std::vector<int>> colors);
cv::Mat drawtopology_disconnectedroot(cv::Mat segmented, cv::Mat dist, cv::Scalar_<int> ranges, cv::Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts, int width);

cv::Mat drawmedialaxis(cv::Mat dist, cv::Mat skel, std::vector<double> rranges, int width);

#endif

