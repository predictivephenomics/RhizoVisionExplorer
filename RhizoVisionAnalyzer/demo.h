/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: demo.h

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

#ifndef DEMO_H
#define DEMO_H

#include "cvutil.h"

#include <random>

std::vector<std::vector<int>> getholes(std::vector<std::vector<int>> cc, cv::Size sz)
{
    std::vector<std::vector<int>> result;
    int outercomp = -1, nr, nc;
    result.resize(cc.size() - 1);

    for (int i = 0; i < cc.size(); i++)
    {
        for (int j = 0; j < cc[i].size(); j++)
        {
            nr = cc[i][j] / sz.width;
            nc = cc[i][j] % sz.width;

            if (nr == 0 || nc == 0 ||
                nr == (sz.height - 1) ||
                nc == (sz.width - 1))
            {
                outercomp = i;
                break;
            }
        }
    }

    for (int i = 0, j = 0; i < cc.size(); i++)
    {
        if (i != outercomp)
        {
            result[j] = cc[i];
            j++;
        }
    }

    return result;
}

std::vector<int> getComponentCentroid(std::vector<std::vector<int>> cc, cv::Size sz)
{
    unsigned long long rowsum, colsum;
    std::vector<int> result;
    result.resize(cc.size());

    for (int i = 0; i < cc.size(); i++)
    {
        rowsum = 0;
        colsum = 0;

        for (int j = 0; j < cc[i].size(); j++)
        {
            rowsum += (cc[i][j] / sz.width);
            colsum += (cc[i][j] % sz.width);
        }

        rowsum /= cc[i].size();
        colsum /= cc[i].size();
        result[i] = rowsum * sz.width + colsum;
    }

    return result;
}

cv::Mat pdist2(std::vector<int> pts, cv::Size sz)
{
    cv::Mat m = cv::Mat(pts);
    m.convertTo(m, CV_32FC1);
    cv::Mat mr, mc, repmr, repmc, sqrepmr, sqrepmc, result;

    mr = floor(m / sz.width);
    mc = m - floor(m / sz.width) * sz.width;

    repmr = repeat(mr, 1, mr.rows);
    sqrepmr = repmr.mul(repmr);

    repmc = repeat(mc, 1, mc.rows);
    sqrepmc = repmc.mul(repmc);

    result = sqrepmr + sqrepmr.t() + ((repmr.mul(repmr.t())) * (-2));
    result = result + sqrepmc + sqrepmc.t() + ((repmc.mul(repmc.t())) * (-2));

    return result;
}

cv::Mat getnearestcomponents(cv::Mat distmap)
{
    cv::Mat result = cv::Mat::zeros(distmap.rows, 4, CV_32SC1);
    int *resultptr = result.ptr<int>();
    float *distptr = distmap.ptr<float>();
    std::vector<float> val = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
    std::vector<int> pos = { -1, -1, -1, -1 };

    for (int i = 0; i < distmap.rows; i++)
    {
        val = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
        pos = { -1, -1, -1, -1 };

        for (int j = i + 1; j < distmap.rows; j++)
        {
            if (distptr[j] < val[0])
            {
                pos[3] = pos[2];
                val[3] = val[2];

                pos[2] = pos[1];
                val[2] = val[1];

                pos[1] = pos[0];
                val[1] = val[0];

                pos[0] = j;
                val[0] = distptr[j];
            }
            else if (distptr[j] < val[1])
            {
                pos[3] = pos[2];
                val[3] = val[2];

                pos[2] = pos[1];
                val[2] = val[1];

                pos[1] = j;
                val[1] = distptr[j];
            }
            else if (distptr[j] < val[2])
            {
                pos[3] = pos[2];
                val[3] = val[2];

                pos[2] = j;
                val[2] = distptr[j];
            }
            else if (distptr[j] < val[3])
            {
                pos[3] = j;
                val[3] = distptr[j];
            }
        }

        resultptr[i * 4] = pos[0];
        resultptr[i * 4 + 1] = pos[1];
        resultptr[i * 4 + 2] = pos[2];
        resultptr[i * 4 + 3] = pos[3];
    }

    return result;
}

std::vector<int> getcolorcodes(cv::Mat nearest)
{
    std::vector<int> result;
    int *nearestptr = nearest.ptr<int>();
    result.resize(nearest.rows);
    std::vector<int> colors = { 1,2,3,4,5 }, cols, vcols;
    std::unordered_map<int, int> cm;
    int found = 0;
    std::random_device rng;
    std::mt19937 urng(rng());

    for (int i = 0; i < nearest.rows; i++)
    {
        if (i == 0)
        {
            result[i] = colors[0];
            cm[i] = colors[0];

            for (int j = 0; j < 4; j++)
            {
                result[nearestptr[i * 4 + j]] = colors[j + 1];
                cm[nearestptr[i * 4 + j]] = colors[j + 1];
            }

            continue;
        }

        cols.clear();
        vcols.clear();

        if (cm[i] != 0)
            cols.push_back(cm[i]);

        for (int j = 0; j < 4; j++)
        {
            if (nearestptr[i * 4 + j] != -1 && cm[nearestptr[i * 4 + j]] != 0)
                cols.push_back(cm[nearestptr[i * 4 + j]]);
        }
        
        for (int j = 0; j < colors.size(); j++)
        {
            found = 0;

            for (int k = 0; k < cols.size(); k++)
            {
                if (cols[k] == colors[j])
                {
                    found = 1;
                    break;
                }
            }

            if (found == 0)
                vcols.push_back(colors[j]);
        }

        std::shuffle(vcols.begin(), vcols.end(), urng);

        if (cm[i] == 0)
        {
            result[i] = vcols[0];
            cm[i] = vcols[0];

            for (int j = 0, k = 1; j < 4; j++)
            {
                if (nearestptr[i * 4 + j] != -1 && cm[nearestptr[i * 4 + j]] == 0)
                {
                    result[nearestptr[i * 4 + j]] = vcols[k];
                    cm[nearestptr[i * 4 + j]] = vcols[k];
                    k++;
                }
            }
        }
        else
        {
            for (int j = 0, k = 0; j < 4; j++)
            {
                if (nearestptr[i * 4 + j] != -1 && cm[nearestptr[i * 4 + j]] == 0)
                {
                    result[nearestptr[i * 4 + j]] = vcols[k];
                    cm[nearestptr[i * 4 + j]] = vcols[k];
                    k++;
                }
            }
        }
    }
    
    return result;
}

cv::Mat colorimage(cv::Mat img, cv::Scalar_<int> ranges, std::vector<std::vector<int>> cc, std::vector<int> colorcodes, std::vector<std::vector<int>> colors)
{
    int c = ranges[2] - ranges[3] + 2; // img.cols + 2;
    cv::Mat m = img.rowRange(ranges[1], ranges[0]).colRange(ranges[3], ranges[2]).clone();
    
    for (int i = 0; i < cc.size(); i++)
    {
        for (int j = 0; j < cc[i].size(); j++)
        {
            circle(m, { ((cc[i][j] % c) - 1), ((cc[i][j] / c) - 1) }, 1, cv::Scalar(colors[colorcodes[i] - 1][2], colors[colorcodes[i] - 1][1], colors[colorcodes[i] - 1][0]));
        }
    }

    cv::Mat result = img.clone();
    m.copyTo(result.rowRange(ranges[1], ranges[0]).colRange(ranges[3], ranges[2]));

    return result;
}

#endif
