/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: rootsegmentprop.h

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
