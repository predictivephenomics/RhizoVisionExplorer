/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: roottopology.h

Author: Anand Seethepalli (aseethepalli@noble.org)
Principal Investigator: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of RhizoVision Analyzer.

RhizoVision Analyzer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cvutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RhizoVision Analyzer.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef ROOTTOPOLOGY_H
#define ROOTTOPOLOGY_H

#include "common_types.h"

void getroottopology(cv::Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts, ListofListsRef<cv::Point> rootsegments);

cv::Mat drawtopology(cv::Mat segmented, cv::Mat skeleton, PointList branchpts, PointList endpts, PointList overlappts);
cv::Mat drawtopology2(cv::Mat segmented, cv::Scalar_<int> ranges, cv::Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts);
#endif

