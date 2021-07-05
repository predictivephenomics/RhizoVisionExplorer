/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: roottopology.h

Authors:
Anand Seethepalli (anand.seethepalli@yahoo.co.in)
Larry York (larry.york@gmail.com)

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

