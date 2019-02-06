/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: common_types.h

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

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <cvutil.h>

using namespace cvutil;

template<typename T>
using ListRef = std::vector<T>&;

typedef ListRef<int> IndexList;
typedef std::vector<int> Indices;
typedef ListRef<double> ValueList;
typedef std::vector<double> Values;

typedef ListRef<cv::Point> PointList;
typedef std::vector<cv::Point> Points;

template<typename T>
using ListofListsRef = std::vector<std::vector<T>>&;

template<typename T>
using ListofLists = std::vector<std::vector<T>>;

template<typename T>
using ListofListsPtr = std::vector<std::vector<T>>*;

template<typename TKey, typename TVal>
using Dictionary = std::unordered_map<TKey, TVal>;

template<typename TKey, typename TVal>
using DictionaryRef = std::unordered_map<TKey, TVal>&;

#endif
