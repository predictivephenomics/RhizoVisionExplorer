/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: main.cpp

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

#include <cvutil.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include "common_types.h"
#include "roottopology.h"
#include "rootsegmentprop.h"
#include "demo.h"
#include "MainUI.h"

#include <thread>

using namespace std;
using namespace cv;
using namespace cvutil;

//#ifdef NDEBUG
//#define DEBUG_MODE
//#endif

///////////////// Program configuration data ///////////////
const string saveloc = "C:/Users/aseethepalli/Documents/projects/hundredhours2/";
const string features_filename = "./pics/features.txt";
//const double pixel2mm = 127 / 2408.2; //1.0; // 76.0 / 784.2;
const bool showprocessedimage = true;
Ptr<IVWriter> vseg, vpro;
int rwidth1 = 400, rwidth2 = 400, cwidth1 = 800, cwidth2 = 800;
////////////////////////////////////////////////////////////

void getMedianMaxRootNumbers(Mat seg, double &medrtno, double &maxrtno)
{
    Mat seg1, seg2, diff, m, m2;
    double *mptr;
    int zidx = 0;

    hconcat(Mat::zeros(seg.rows, 1, seg.type()), seg, seg1);
    hconcat(seg, Mat::zeros(seg.rows, 1, seg.type()), seg2);
    diff = seg1 - seg2;

    diff = (diff > 0) / 255;
    reduce(diff, m, 1, CV_REDUCE_SUM, CV_MAKE_TYPE(CV_MAT_DEPTH(CV_64F), diff.channels()));

    std::sort(m.ptr<double>(), m.ptr<double>() + m.rows);
    //cv::sort(m, m2, cv::SortFlags::SORT_DESCENDING);
    mptr = m.ptr<double>();
    maxrtno = mptr[m.rows - 1];

    for (; mptr[zidx] == 0; zidx++);

    medrtno = ((m.rows - zidx) % 2 == 0) ? ((mptr[(m.rows - zidx) / 2 + zidx] + mptr[(m.rows - zidx) / 2 - 1 + zidx]) / 2.0)
        : mptr[(m.rows - zidx) / 2 + zidx];
}

double getperimeterlength(vector<vector<Point>> &contours)
{
    double perimeter = 0.0;
    Point diff;

    for (auto &c : contours)
    {
        for (int i = 0; i < (c.size() - 1); i++)
        {
            diff = c[i] - c[i + 1];
            perimeter += sqrt(diff.ddot(diff));
        }

        diff = c[0] - c[c.size() - 1];
        perimeter += sqrt(diff.ddot(diff));
    }

    return perimeter;
}

void getavgrootradius(ListofListsRef<double> radii, double &medradius, double& avgradius)
{
    int rsize = 0; 
    avgradius = 0.0;
    medradius = 0.0;
    Values rd;

    if (radii.size() == 0)
        return;

    for (int i = 0; i < radii.size(); i++)
        rd.insert(rd.end(), radii[i].begin(), radii[i].end());

    sort(rd.begin(), rd.end());

    if ((rd.size() % 2) == 0)
        medradius = (rd[rd.size() / 2 - 1] + rd[rd.size() / 2]) / 2.0;
    else
        medradius = rd[rd.size() / 2 - 1];

    for (int i = 0; i < radii.size(); i++)
    {
        for (int j = 0; j < radii[i].size(); j++)
            avgradius += radii[i][j];
        
        rsize += static_cast<int>(radii[i].size());
    }

    avgradius /= double(rsize);
}

void getvolumesurfacearea(Mat skeleton, Mat dist, ListofListsRef<Point> segments, PointList overlap, double& volume, double& surfacearea)
{
    int i, j, x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt, fxinc, fyinc;
    double rad = 0, sqr = sqrt(2), dv = 0, dvol = 0, dsa = 0;
    size_t cisz;

    float *dtptr = dist.ptr<float>();
    uchar *skel = skeleton.ptr<uchar>();

    volume = 0.0;
    surfacearea = 0.0;

    for (i = 0; i < segments.size(); i++)
    {
        cisz = segments[i].size() - 1;
        
        for (j = 0; j < cisz; j++)
        {
            nnextpt = j + 1;

            currx = segments[i][j].x;
            curry = segments[i][j].y;
            nextx = segments[i][nnextpt].x;
            nexty = segments[i][nnextpt].y;

            if (nextx != currx)
                xinc = abs(currx - nextx) / (nextx - currx);
            else
                xinc = 0;

            if (nexty != curry)
                yinc = abs(curry - nexty) / (nexty - curry);
            else
                yinc = 0;

            fxinc = fabs(xinc);
            fyinc = fabs(yinc);
            rad = fxinc + fyinc;
            rad = (rad == 2.0) ? CVUTIL_SQRT2 : rad;

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                dv = dtptr[y * dist.cols + x];
                dvol += (CV_PI * dv * dv * rad);
                dsa += (2.0 * CV_PI * dv * rad);
            }
        }

        /*dv = dtptr[y * dist.cols + x];
        dvol += (CV_PI * dv * dv);
        dsa += (2.0 * CV_PI * dv);*/
    }

    for (auto &overlappt : overlap)
    {
        // To check if the overlappt is a diagonal or orthogonal
        if (skel[overlappt.y * skeleton.cols + overlappt.x + 1] > 0)
        {
            // Orthogonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            dvol += (CV_PI * dv * dv);
            dsa += (2.0 * CV_PI * dv);
        }
        else
        {
            // Diagonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            dvol += (CV_PI * dv * dv * CVUTIL_SQRT2);
            dsa += (2.0 * CV_PI * dv * CVUTIL_SQRT2);
        }
    }

    volume = dvol;
    surfacearea = dsa;
}

void getdiameterrangehistograms(Mat skeleton, Mat dist, ListofListsRef<Point> segments, PointList overlap, Values rranges,
    ValueList rootlengthhist, ValueList projareahist, ValueList sahist, ValueList vhist, double &rootlength, double &volume, double &sarea)
    /*double& coarse, double& medium, double& fine*///double dlo, double dhi)
{
    int i, j, k, x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt, fxinc, fyinc, didx;
    double rad = 0, sqr = sqrt(2.0), dv = 0;
    size_t cisz;
    
    size_t rsize = rranges.size();
    rootlengthhist.resize(rsize + 1);
    projareahist.resize(rsize + 1);
    sahist.resize(rsize + 1);
    vhist.resize(rsize + 1);

    Mat radrange = Mat::zeros(dist.size(), CV_8UC1);

    if (rsize > 0)
    {
        for (k = 0; k < (rsize - 1); k++)
        {
            rootlengthhist[k] = 0.0;
            projareahist[k] = 0.0;
            sahist[k] = 0.0;
            vhist[k] = 0.0;

            radrange = radrange + (((dist <= rranges[k + 1]) - (dist <= rranges[k])) / 255u) * uint8_t(k + 1);
        }

        rootlengthhist[rsize - 1] = 0.0;
        projareahist[rsize - 1] = 0.0;
        sahist[rsize - 1] = 0.0;
        vhist[rsize - 1] = 0.0;

        radrange = radrange + ((dist > rranges[rsize - 1]) / 255u) * uint8_t(rsize);
    }
    else
    {
        rootlengthhist[0] = 0.0;
        projareahist[0] = 0.0;
        sahist[0] = 0.0;
        vhist[0] = 0.0;
    }

    float *dtptr = dist.ptr<float>();
    uchar *skel = skeleton.ptr<uchar>();
    uchar *rptr = radrange.ptr<uchar>();

    for (i = 0; i < segments.size(); i++)
    {
        cisz = segments[i].size() - 1;
        
        for (j = 0; j < cisz; j++)
        {
            nnextpt = j + 1;

            currx = segments[i][j].x;
            curry = segments[i][j].y;
            nextx = segments[i][nnextpt].x;
            nexty = segments[i][nnextpt].y;

            if (nextx != currx)
                xinc = abs(currx - nextx) / (nextx - currx);
            else
                xinc = 0;

            if (nexty != curry)
                yinc = abs(curry - nexty) / (nexty - curry);
            else
                yinc = 0;

            fxinc = fabs(xinc);
            fyinc = fabs(yinc);
            rad = fxinc + fyinc;
            rad = (rad == 2.0) ? CVUTIL_SQRT2 : rad;

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                dv = dtptr[y * dist.cols + x];
                didx = rptr[y * dist.cols + x];
                
                rootlengthhist[didx] += rad;
                projareahist[didx] += (2 * dv * rad);
                vhist[didx] += (CV_PI * dv * dv * rad);
                sahist[didx] += (2.0 * CV_PI * dv * rad);
            }
        }

        /*dv = dtptr[y * dist.cols + x];
        didx = rptr[y * dist.cols + x];

        rootlengthhist[didx] += rad;
        projareahist[didx] += (2 * dv * rad);
        vhist[didx] += (CV_PI * dv * dv * rad);
        sahist[didx] += (2.0 * CV_PI * dv * rad);
        */
    }

    for (auto &overlappt : overlap)
    {
        // To check if the overlappt is a diagonal or orthogonal
        if (skel[overlappt.y * skeleton.cols + overlappt.x + 1] > 0)
        {
            // Orthogonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            didx = rptr[overlappt.y * dist.cols + overlappt.x];
            
            rootlengthhist[didx] += 1.0;
            //projareahist[didx] += (2 * dv * rad);
            vhist[didx] += (CV_PI * dv * dv);
            sahist[didx] += (2.0 * CV_PI * dv);
        }
        else
        {
            // Diagonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            didx = rptr[overlappt.y * dist.cols + overlappt.x];
            
            rootlengthhist[didx] += CVUTIL_SQRT2;
            //projareahist[didx] += (2 * dv * rad);
            vhist[didx] += (CV_PI * dv * dv * CVUTIL_SQRT2);
            sahist[didx] += (2.0 * CV_PI * dv * CVUTIL_SQRT2);
        }
    }

    // Now to add the histograms to get total root length, volume
    // and surface area.
    rootlength = 0;
    volume = 0;
    sarea = 0;
    for (i = 0; i < rootlengthhist.size(); i++)
    {
        rootlength += rootlengthhist[i];
        volume += vhist[i];
        sarea += sahist[i];
    }
}

void getdiameterrangehistograms(Mat skeleton, Mat dist, ListofListsRef<Point> segments, PointList overlap, Values rranges,
    ValueList rootlengthhist, ValueList projareahist, ValueList sahist, ValueList vhist, double &rootlength, double &volume, double &sarea, Mat& rtdpoints)
    /*double& coarse, double& medium, double& fine*///double dlo, double dhi)
{
    int i, j, k, x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt, fxinc, fyinc, didx;
    double rad = 0, sqr = sqrt(2.0), dv = 0;
    size_t cisz;
    map<double, double> rdpoints;

    size_t rsize = rranges.size();
    rootlengthhist.resize(rsize + 1);
    projareahist.resize(rsize + 1);
    sahist.resize(rsize + 1);
    vhist.resize(rsize + 1);

    Mat radrange = Mat::zeros(dist.size(), CV_8UC1);

    if (rsize > 0)
    {
        for (k = 0; k < (rsize - 1); k++)
        {
            rootlengthhist[k] = 0.0;
            projareahist[k] = 0.0;
            sahist[k] = 0.0;
            vhist[k] = 0.0;

            radrange = radrange + (((dist <= rranges[k + 1]) - (dist <= rranges[k])) / 255u) * uint8_t(k + 1);
        }

        rootlengthhist[rsize - 1] = 0.0;
        projareahist[rsize - 1] = 0.0;
        sahist[rsize - 1] = 0.0;
        vhist[rsize - 1] = 0.0;

        radrange = radrange + ((dist > rranges[rsize - 1]) / 255u) * uint8_t(rsize);
    }
    else
    {
        rootlengthhist[0] = 0.0;
        projareahist[0] = 0.0;
        sahist[0] = 0.0;
        vhist[0] = 0.0;
    }

    float *dtptr = dist.ptr<float>();
    uchar *skel = skeleton.ptr<uchar>();
    uchar *rptr = radrange.ptr<uchar>();

    for (i = 0; i < segments.size(); i++)
    {
        cisz = segments[i].size() - 1;

        for (j = 0; j < cisz; j++)
        {
            nnextpt = j + 1;

            currx = segments[i][j].x;
            curry = segments[i][j].y;
            nextx = segments[i][nnextpt].x;
            nexty = segments[i][nnextpt].y;

            if (nextx != currx)
                xinc = abs(currx - nextx) / (nextx - currx);
            else
                xinc = 0;

            if (nexty != curry)
                yinc = abs(curry - nexty) / (nexty - curry);
            else
                yinc = 0;

            fxinc = fabs(xinc);
            fyinc = fabs(yinc);
            rad = fxinc + fyinc;
            rad = (rad == 2.0) ? CVUTIL_SQRT2 : rad;

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                dv = dtptr[y * dist.cols + x];
                didx = rptr[y * dist.cols + x];

                rdpoints[dv] += rad;

                rootlengthhist[didx] += rad;
                projareahist[didx] += (2 * dv * rad);
                vhist[didx] += (CV_PI * dv * dv * rad);
                sahist[didx] += (2.0 * CV_PI * dv * rad);
            }
        }

        /*dv = dtptr[y * dist.cols + x];
        didx = rptr[y * dist.cols + x];

        rootlengthhist[didx] += rad;
        projareahist[didx] += (2 * dv * rad);
        vhist[didx] += (CV_PI * dv * dv * rad);
        sahist[didx] += (2.0 * CV_PI * dv * rad);
        */
    }

    for (auto &overlappt : overlap)
    {
        // To check if the overlappt is a diagonal or orthogonal
        if (skel[overlappt.y * skeleton.cols + overlappt.x + 1] > 0)
        {
            // Orthogonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            didx = rptr[overlappt.y * dist.cols + overlappt.x];

            rdpoints[dv] += 1.0;

            rootlengthhist[didx] += 1.0;
            //projareahist[didx] += (2 * dv * rad);
            vhist[didx] += (CV_PI * dv * dv);
            sahist[didx] += (2.0 * CV_PI * dv);
        }
        else
        {
            // Diagonal
            dv = dtptr[overlappt.y * dist.cols + overlappt.x];
            didx = rptr[overlappt.y * dist.cols + overlappt.x];

            rdpoints[dv] += CVUTIL_SQRT2;

            rootlengthhist[didx] += CVUTIL_SQRT2;
            //projareahist[didx] += (2 * dv * rad);
            vhist[didx] += (CV_PI * dv * dv * CVUTIL_SQRT2);
            sahist[didx] += (2.0 * CV_PI * dv * CVUTIL_SQRT2);
        }
    }

    rtdpoints = Mat::zeros(static_cast<int>(rdpoints.size()), 2, CV_64FC1);
    double *rtdptr = rtdpoints.ptr<double>();

    int rtdidx = 0;
    for (auto& kv : rdpoints)
    {
        rtdptr[2 * rtdidx] = kv.first;
        rtdptr[2 * rtdidx + 1] = kv.second;

        rtdidx++;
    }

    // Now to add the histograms to get total root length, volume
    // and surface area.
    rootlength = 0;
    volume = 0;
    sarea = 0;
    for (i = 0; i < rootlengthhist.size(); i++)
    {
        rootlength += rootlengthhist[i];
        volume += vhist[i];
        sarea += sahist[i];
    }
}

void getshallowdeeproots(ListofListsRef<Point> segments, double& shallow, double& medium, double& deep, double &avgangle)
{
    shallow = 0.0;
    medium = 0.0;
    deep = 0.0;
    avgangle = 0.0;

    Mat m, m2, cov, me, lam, ev;
    double *evptr = nullptr;
    int i, j, k, x, y, nnextpt, currx, curry, nextx, nexty, xinc, yinc;
    size_t csz = segments.size(), cisz = 0, szcount = 0;
    int wsize = 20, nsamples = 0, npoints = 0, minv = 0, maxv = 0;
    double angle = 0;
    
    m.create(10000, 2, CV_64FC1);
    m2.create(wsize * wsize * 4, 2, CV_64FC1);

    double *mptr = m.ptr<double>(), *mptr2 = m2.ptr<double>();

    for (i = 0, szcount = 0; i < csz; i++)
    {
        if (segments[i].size() <= 1)
            continue;

        // To reconstruct the full segment before getting
        // angle stats.
        cisz = segments[i].size() - 1;
        //szcount += cisz;

        //cout << "szcount = " << szcount << endl;

        for (j = 0, nsamples = 0; j < cisz; j++)
        {
            nnextpt = (j + 1);
            currx = segments[i][j].x;
            curry = segments[i][j].y;
            nextx = segments[i][nnextpt].x;
            nexty = segments[i][nnextpt].y;
            xinc = (currx == nextx) ? 0 : abs(currx - nextx) / (nextx - currx);
            yinc = (curry == nexty) ? 0 : abs(curry - nexty) / (nexty - curry);

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                mptr[nsamples * 2] = x;
                mptr[nsamples * 2 + 1] = y;
                nsamples++;
            }
        }
        
        mptr[nsamples * 2] = nextx;
        mptr[nsamples * 2 + 1] = nexty;
        nsamples++;

        // Now to browse through the segment using a 
        // (2 * wsize + 1)-by-(2 * wsize + 1) window
        // and recording the orientation using the
        // eigen vectors from the covariance matix.
        for (j = 0; j < nsamples; j++)
        {
            npoints = 0;

            for (k = j; 
                k < nsamples &&
                abs(mptr[j * 2] - mptr[k * 2]) <= wsize &&
                abs(mptr[j * 2 + 1] - mptr[k * 2 + 1]) <= wsize;
                k++)
            {
                mptr2[npoints * 2] = mptr[k * 2];
                mptr2[npoints * 2 + 1] = mptr[k * 2 + 1];
                npoints++;
            }

            for (k = (j - 1);
                k >= 0 &&
                abs(mptr[j * 2] - mptr[k * 2]) <= wsize &&
                abs(mptr[j * 2 + 1] - mptr[k * 2 + 1]) <= wsize;
                k--)
            {
                mptr2[npoints * 2] = mptr[k * 2];
                mptr2[npoints * 2 + 1] = mptr[k * 2 + 1];
                npoints++;
            }
            
            if (npoints > 1)
            {
                calcCovarMatrix(m2.rowRange(0, npoints), cov, me, COVAR_ROWS | COVAR_NORMAL);
                eigen(cov, lam, ev);
                evptr = ev.ptr<double>();
                angle = abs((atan(evptr[1] / evptr[0]) * 180.0) / CV_PI);
                
                avgangle = ((szcount * avgangle) + angle) / (szcount + 1);
                szcount++;
                
                if (angle <= 30) //(CV_PI / 6))
                    shallow++;
                else if (angle <= 60) //(CV_PI / 3))
                    medium++;
                else
                    deep++;
            }
        }
    }

    angle = deep + medium + shallow;
    shallow /= angle;
    medium /= angle;
    deep /= angle;
    //avgangle /= szcount;

    m.release();
    m2.release();
}

void getshallowdeeprootsth(ListofListsRef<Point> segments, double& shallow, double& medium, double& deep, double &avgangle)
{
    shallow = 0.0;
    medium = 0.0;
    deep = 0.0;
    avgangle = 0.0;

    int nthreads = getNumberOfCPUs() - 3;
    vector<thread> t(nthreads);
    vector<double> tshallow(nthreads), tdeep(nthreads), tmedium(nthreads), tavgangle(nthreads), tcount(nthreads);
    vector<Mat> m(nthreads), m2(nthreads);
    double anglev = 0, ncount = 0;

    for (int th = 0; th < nthreads; th++)
    {
        t[th] = thread([&](int thr)
        {
            int npoints = 0;
            Mat cov, me, lam, ev;
            double *evptr = nullptr;
            int i, j, k, x, y, nnextpt, currx, curry, nextx, nexty, xinc, yinc;
            size_t csz = segments.size(), cisz = 0, szcount = 0;
            int wsize = 20, nsamples = 0, minv = 0, maxv = 0;
            double angle = 0;

            m[thr].create(10000, 2, CV_64FC1);
            m2[thr].create(wsize * wsize * 4, 2, CV_64FC1);

            double *mptr = m[thr].ptr<double>(), *mptr2 = m2[thr].ptr<double>();

            for (i = 0; i < csz; i++)
            {
                if (segments[i].size() <= 1)
                    continue;

                // To reconstruct the full segment before getting
                // angle stats.
                cisz = segments[i].size() - 1;
                szcount += cisz;

                //cout << "szcount = " << szcount << endl;

                for (j = 0, nsamples = 0; j < cisz; j++)
                {
                    nnextpt = (j + 1);
                    currx = segments[i][j].x;
                    curry = segments[i][j].y;
                    nextx = segments[i][nnextpt].x;
                    nexty = segments[i][nnextpt].y;
                    xinc = (currx == nextx) ? 0 : abs(currx - nextx) / (nextx - currx);
                    yinc = (curry == nexty) ? 0 : abs(curry - nexty) / (nexty - curry);

                    for (x = currx, y = curry; (x != nextx && y != nexty); x += xinc, y += yinc)
                    {
                        mptr[nsamples * 2] = x;
                        mptr[nsamples * 2 + 1] = y;
                        nsamples++;
                    }
                }

                mptr[nsamples * 2] = nextx;
                mptr[nsamples * 2 + 1] = nexty;
                nsamples++;

                // Now to browse through the segment using a 
                // (2 * wsize + 1)-by-(2 * wsize + 1) window
                // and recording the orientation using the
                // eigen vectors from the covariance matix.
                for (j = 0; j < nsamples; j++)
                {
                    npoints = 0;

                    for (k = j;
                        k < nsamples &&
                        abs(mptr[j * 2] - mptr[k * 2]) <= wsize &&
                        abs(mptr[j * 2 + 1] - mptr[k * 2 + 1]) <= wsize;
                        k++)
                    {
                        mptr2[npoints * 2] = mptr[k * 2];
                        mptr2[npoints * 2 + 1] = mptr[k * 2 + 1];
                        npoints++;
                    }

                    for (k = (j - 1);
                        k >= 0 &&
                        abs(mptr[j * 2] - mptr[k * 2]) <= wsize &&
                        abs(mptr[j * 2 + 1] - mptr[k * 2 + 1]) <= wsize;
                        k--)
                    {
                        mptr2[npoints * 2] = mptr[k * 2];
                        mptr2[npoints * 2 + 1] = mptr[k * 2 + 1];
                        npoints++;
                    }

                    if (npoints > 1)
                    {
                        calcCovarMatrix(m2[thr].rowRange(0, npoints), cov, me, COVAR_ROWS);
                        eigen(cov, lam, ev);
                        evptr = ev.ptr<double>();
                        angle = abs((atan(evptr[1] / evptr[0]) * 180.0) / CV_PI);
                        tavgangle[thr] += angle;

                        if (angle <= 30) //(CV_PI / 6))
                            tshallow[thr]++;
                        else if (angle <= 60) //(CV_PI / 3))
                            tmedium[thr]++;
                        else
                            tdeep[thr]++;
                    }
                }
            }

            m[thr].release();
            m2[thr].release();

            tcount[thr] = szcount;
        }, th);
    }

    for (int th = 0; th < nthreads; th++)
    {
        t[th].join();

        deep += tdeep[th];
        medium += tmedium[th];
        shallow += tshallow[th];

        ncount += tcount[th];
        avgangle += tavgangle[th];
    }

    anglev = deep + medium + shallow;
    shallow /= anglev;
    medium /= anglev;
    deep /= anglev;
    avgangle /= ncount;
}

bool checkProgress(feature_config* config, QString str)
{
    config->ui->updateProgress(str);
    QApplication::processEvents();
    return config->abortprocess;
}

//void feature_extractor(string filename, string savefolder, vector<double>& featurevals, bool invertimage, bool saveseg, bool saveimages, bool pixelconv, double conversion, double rootlo, double roothi)
void feature_extractor(feature_config* config)
{
    Mat img, out, seg, lsim, lsimcropped, skel, dilatedskel, dist;
    Mat perimeter, hull, fhull, chull, dilatedhull, selement, merged, holes, pmap, nearest;
    vector<vector<Point>> contours;
    vector<vector<int>> holepts;
    vector<Mat> channels, hch;
    vector<vector<Point>> hullpt;
    vector<Vec4i> hierarchy;
    vector<int> centroids, colorcodes;
    Points branchpts, endpts, overlappts;
    ListofLists<Point> rootsegments;
    ListofLists<double> radii;
    double rootlength = -1.0, mediannoroots = -1.0, maxnoroots = -1.0;
    double perimeterlength = -1.0, medradius = -1.0, avgradius = -1.0, maxradius = -1.0;
    //double coarser = 0.0, mediumr = 0.0, finer = 0.0;
    Values rootlengthhist = {}, projareahist = {}, sahist = {}, vhist = {};
    double shallowo = 0.0, mediumo = 0.0, deepo = 0.0, avgang = 0.0;
    double volume = -1.0, surfacearea = -1.0, vtime;
    int maxradrowidx = -1, nholes = 0;
    double pixel2mm = 1.0, avgholesize = 0.0;
    bool isborder = false, isborderholefound = false;
    //double diameterlo = config->rootlo, diameterhi = config->roothi;
    vector<double> radiusranges;
    double nwarea = 0, convarea = 0, widthtodepthratio = 0, lowerrootarea = 0;
    std::random_device rng;
    std::mt19937 urng(rng());
    int nfeatures;
    Mat rtdpoints;
    RotateFlags flag = ROTATE_90_CLOCKWISE, revflag = ROTATE_90_COUNTERCLOCKWISE;

    switch (config->rotation)
    {
    case 90:
        flag = ROTATE_90_CLOCKWISE;
        revflag = ROTATE_90_COUNTERCLOCKWISE;
        break;
    case 180:
        flag = ROTATE_180;
        revflag = ROTATE_180;
        break;
    case 270:
        flag = ROTATE_90_COUNTERCLOCKWISE;
        revflag = ROTATE_90_CLOCKWISE;
        break;
    }
    
    // Determine the number of features to be extracted
    if (config->roottype == 0) // crown root
        nfeatures = 24 + 4 * (static_cast<int>(config->dranges.size()) + 1);
    else
        nfeatures = 12 + 4 * (static_cast<int>(config->dranges.size()) + 1);

#ifndef _DEBUG
    thread *t1;
#endif
    
    vector<vector<int>> cc, colors =
    { { 228, 26, 28 },
    { 55, 126, 184 },
    { 77, 175, 74 },
    { 152, 78, 163 },
    { 255, 127, 0 } };

    if (config->pixelconv)
    {
        if (config->pixelspermm == 0) // For DPI conversion
            pixel2mm /= (config->conversion / 25.4);
        else
            pixel2mm /= config->conversion;
        //diameterlo *= config->conversion;
        //diameterhi *= config->conversion;
    }

    // If the diameter ranges are given in mm, we need 
    // to convert it to pixel units.
    for (auto &d : config->dranges)
    {
        if (!config->pixelconv)
            radiusranges.push_back(d / 2.0);
        else
            radiusranges.push_back(d / 2.0 / pixel2mm);
    }

    if (config->rotation != 0)
        rotate(config->input, img, flag);
    else
        img = config->input.clone();

    if ((!config->batchmode) && checkProgress(config, "Thresholding the input image ..."))
        return;
    
    for (int ii = 0; ii < nfeatures; ii++)
        config->features.push_back(NAN);

    if (img.empty())
    {
        //for (int ii = 0; ii < nfeatures; ii++)
        //    config->features.push_back(NAN);

        Mat m1 = Mat::zeros(out.size(), out.type());
        Mat m2 = Mat::ones(out.size(), out.type()) * 255u;
        if (!config->invertimage)
            m1 = (m1 < 128);

        if (config->rotation != 0)
        {
            rotate(m1, config->input, revflag);
            rotate(m2, config->processed, revflag);
        }
        else
        {
            config->seg = m1;
            config->processed = m2;
        }

        return;
    }

    srand(time(0));
    tic();

    if (config->invertimage)
        img = Scalar::all(255) - img;
    
    out = getSingleChannel(img < config->threshold, CV_8UC1);
    
    /*if (countNonZero(out) == 0)
    {
        Mat m1 = Mat::zeros(out.size(), out.type());
        Mat m2 = Mat::ones(out.size(), out.type()) * 255u;
        if (!config->invertimage)
            m1 = (m1 < 128);
        
        if (config->rotation != 0)
        {
            rotate(m1, config->input, revflag);
            rotate(m2, config->processed, revflag);
        }
        else
        {
            config->seg = m1;
            config->processed = m2;
        }

        return;
    }*/

    if ((!config->batchmode) && checkProgress(config, "Filtering noise ..."))
        return;

    if (config->roottype == 0 && config->keepLargest == true)
        seg = getLargestConnectedComponent(out);
    else
    {
        if ((!config->filterbknoise) && (!config->filterfgnoise))
            seg = out.clone();
        else
        {
            if (config->filterbknoise)
                seg = getImageFromComponents(out.size(), getConnectedComponents(out, (config->maxcompsizebk / pixel2mm) / pixel2mm));
            if (config->filterfgnoise)
                seg = 255 - getImageFromComponents(seg.size(), getConnectedComponents(255 - seg, (config->maxcompsizefg / pixel2mm) / pixel2mm));
        }
    }

    if (countNonZero(seg) == 0)
    {
        Mat m1 = Mat::zeros(out.size(), out.type());
        Mat m2 = Mat::ones(out.size(), out.type()) * 255u;
        if (!config->invertimage)
            m1 = (m1 < 128);

        if (config->rotation != 0)
        {
            rotate(m1, config->input, revflag);
            rotate(m2, config->processed, revflag);
        }
        else
        {
            config->seg = m1;
            config->processed = m2;
        }

        return;
    }

    out = Mat();

    Mat segcropped;
    auto fresultseg = find(seg>0, FindType::Subscripts);
    double rminseg = seg.rows, rmaxseg = -1, cminseg = seg.cols, cmaxseg = -1;
    int *rcpseg = fresultseg.first.ptr<int>();
    
#ifndef _DEBUG
    for (int ii = 0; ii < fresultseg.first.rows; ii++)
    {
        if (rminseg > rcpseg[2 * ii])
            rminseg = rcpseg[2 * ii];
        if (cminseg > rcpseg[2 * ii + 1])
            cminseg = rcpseg[2 * ii + 1];
        if (rmaxseg < rcpseg[2 * ii])
            rmaxseg = rcpseg[2 * ii];
        if (cmaxseg < rcpseg[2 * ii + 1])
            cmaxseg = rcpseg[2 * ii + 1];
    }

    if ((rmaxseg + 2) <= seg.rows)
        rmaxseg += 2;
    else
        rmaxseg++;

    if ((cmaxseg + 2) <= seg.cols)
        cmaxseg += 2;
    else
        cmaxseg++;

    if ((cminseg - 1) >= 0)
        cminseg --;
#else
    rminseg = 0;
    rmaxseg = seg.rows;
    cminseg = 0;
    cmaxseg = seg.cols;
#endif

    if ((rmaxseg - rminseg) == 1 || (cmaxseg - cminseg) == 1)
    {
        return;
    }

    if (rmaxseg > rminseg || cmaxseg > cminseg)
        segcropped = seg.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg).clone();
    else
        segcropped = seg.clone();
    
    if ((!config->batchmode) && checkProgress(config, "Simplifying contours ..."))
        return;

    if (config->enablesmooththresh)
        lsim = linesim(segcropped, LineSimplificationType::DouglasPeucker, config->smooththresh);
    else
        lsim = segcropped.clone();
    
    if ((!config->batchmode) && checkProgress(config, "Determining distance map ..."))
        return;

    dist = bwdist(lsim);
    
    if ((!config->batchmode) && checkProgress(config, "Skeletonizing ..."))
        return;

    skel = bwskel(lsim, dist);
    
    if ((!config->batchmode) && checkProgress(config, "Finding the contours ..."))
        return;

    findContours(lsim.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    
    // Convex hull is determined only for whole roots.
    if (config->roottype == 0)
    {
        if ((!config->batchmode) && checkProgress(config, "Finding the convex hull ..."))
            return;

        hullpt.resize(1);
        if (contours.size() > 0)
        {
            if (config->keepLargest)
                convexHull(contours[0], hullpt[0], false);
            else
            {
                // Combine all the contours before finding the convex hull that 
                // encapsulates all the contours.
                size_t sz = 0;
                vector<Point> cont;
                for (auto& co : contours)
                    sz += co.size();
                cont.reserve(sz);
                for (auto& co : contours)
                    cont.insert(cont.end(), co.begin(), co.end());

                convexHull(cont, hullpt[0], false);
            }
        }
    }

    if ((!config->batchmode) && checkProgress(config, "Finding the root topology ..."))
        return;

    getroottopology(skel, dist, branchpts, endpts, overlappts, rootsegments, config->enableRootPruning, config->rootPruningThreshold);
    
    if ((!config->batchmode) && checkProgress(config, "Extracting the features ..."))
        return;

#ifndef _DEBUG
    t1 = new thread([&]() {
#endif
        if (config->roottype == 0)
        {
            getshallowdeeproots(rootsegments, shallowo, mediumo, deepo, avgang);

            holes = addBorder(lsim, 0);

            holepts = getConnectedComponents(255 - holes, -1, 4);
            nholes = 0;
            avgholesize = 0.0;

            for (auto &h : holepts)
            {
                isborder = false;

                if (!isborderholefound)
                {
                    for (auto &hv : h)
                    {
                        if (hv == 0)
                        {
                            isborder = true;
                            isborderholefound = true;
                            break;
                        }
                    }
                }

                if (!isborder)
                {
                    avgholesize += h.size();
                    nholes++;
                }
            }

            avgholesize /= nholes;

            convarea = 0.0;


            for (int i = 0, ni = 0; i < hullpt[0].size(); i++)
            {
                ni = ((i + 1) % hullpt[0].size());
                convarea += double(hullpt[0][i].x * hullpt[0][ni].y - hullpt[0][ni].x * hullpt[0][i].y);
            }

            convarea /= 2.0;
        }
        else
        {
            //getrootlength(skel, rootsegments, overlappts, rootlength);
            //getvolumesurfacearea(skel, dist, rootsegments, overlappts, volume, surfacearea);

            //getMedianMaxRootNumbers(lsim, mediannoroots, maxnoroots);
            perimeterlength = getperimeterlength(contours) * pixel2mm;
        }

        nwarea = double(countNonZero(lsim));
        
#ifndef _DEBUG
    });
#endif
        // We extract the root length, volume and surface area in a separate 
        // thread if the root type we are working is disconnected roots.
        getrootradii(skel, lsim, dist, rootsegments, radii, maxradius, maxradrowidx);
        //if (config->roottype == 0)
        //    getrootlength(skel, rootsegments, overlappts, rootlength);
        getavgrootradius(radii, medradius, avgradius);
        //if (config->roottype == 0)
        //    getvolumesurfacearea(skel, dist, rootsegments, overlappts, volume, surfacearea);
        
        if (config->batchmode)
            getdiameterrangehistograms(skel, dist, rootsegments, overlappts, radiusranges, rootlengthhist, projareahist, sahist, vhist, rootlength, volume, surfacearea);
        else
        {
            getdiameterrangehistograms(skel, dist, rootsegments, overlappts, radiusranges, rootlengthhist, projareahist, sahist, vhist, rootlength, volume, surfacearea, rtdpoints);
            config->rtdpoints = rtdpoints;
        }

        double rmin = 10000, rmax = -1, cmin = 10000, cmax = -1;
        
        if (config->roottype == 0)
        {
            auto fresult = find(lsim > 0, FindType::Subscripts);
            int *rcp = fresult.first.ptr<int>();
            rmin = fresult.first.rows;
            cmin = fresult.first.rows;

            for (int ii = 0; ii < fresult.first.rows; ii++)
            {
                if (rmin > rcp[2 * ii])
                    rmin = rcp[2 * ii];
                if (cmin > rcp[2 * ii + 1])
                    cmin = rcp[2 * ii + 1];
                if (rmax < rcp[2 * ii])
                    rmax = rcp[2 * ii];
                if (cmax < rcp[2 * ii + 1])
                    cmax = rcp[2 * ii + 1];
            }

            rmax++;
            cmax++;
            widthtodepthratio = double(cmax - cmin) / double(rmax - rmin);

            getMedianMaxRootNumbers(lsim, mediannoroots, maxnoroots);
            perimeterlength = getperimeterlength(contours) * pixel2mm;
        }
        
        if (maxradius > 0 && lsim.rows > 1 && config->roottype == 0)
            lowerrootarea = double(countNonZero(lsim.rowRange(maxradrowidx, lsim.rows)));
        else
            lowerrootarea = NAN;
        
#ifndef _DEBUG
    t1->join();

    delete t1;
#endif
    
    toc(vtime);
    //cout << "Done. Processing time : " << vtime << " seconds." << endl;
    

    // File name,Median number of roots,Maximum number of roots,Number of root tips,Total root length,
    // Depth,Maximum width,Width-to-depth ratio,Network area,Convex area,Solidity,
    // Lower Root Area,Average diameter,Median diameter,Maximum diameter,
    // Perimeter,Volume,Surface area,
    // Holes,Average Hole size,Computation,Average root orientation
    // Shallow Angle Frequency,Medium Angle Frequency,Steep Angle Frequency,Shallowness Index,
    // Fine Radius Frequency,Medium Radius Frequency,Coarse Radius Frequency,Fineness Index,
    int featureidx = 0;
    if (config->roottype == 0)
    {
        config->features[featureidx++] = mediannoroots;
        config->features[featureidx++] = maxnoroots;
    }
    config->features[featureidx++] = endpts.size();
    if (config->roottype != 0)
        config->features[featureidx++] = branchpts.size();
    config->features[featureidx++] = rootlength * pixel2mm;
    if (config->roottype != 0)
        config->features[featureidx++] = branchpts.size() / (rootlength * pixel2mm);
    if (config->roottype == 0)
    {
        config->features[featureidx++] = double(rmax - rmin) * pixel2mm;
        config->features[featureidx++] = double(cmax - cmin) * pixel2mm;
        config->features[featureidx++] = widthtodepthratio;
    }
    config->features[featureidx++] = nwarea * pixel2mm * pixel2mm;
    if (config->roottype == 0)
    {
        config->features[featureidx++] = convarea * pixel2mm * pixel2mm;
        config->features[featureidx] = config->features[featureidx - 2] / config->features[featureidx - 1];
        featureidx++;
        config->features[featureidx++] = lowerrootarea * pixel2mm * pixel2mm;
    }
    config->features[featureidx++] = avgradius * pixel2mm * 2.0;
    config->features[featureidx++] = medradius * pixel2mm * 2.0;
    if (maxradius >= 0)
        config->features[featureidx++] = maxradius * pixel2mm * 2.0;
    else
        config->features[featureidx++] = NAN;
    config->features[featureidx++] = perimeterlength;
    config->features[featureidx++] = volume * pixel2mm * pixel2mm * pixel2mm;
    config->features[featureidx++] = surfacearea  * pixel2mm * pixel2mm;
    if (config->roottype == 0)
    {
        config->features[featureidx++] = nholes;
        config->features[featureidx++] = avgholesize  * pixel2mm * pixel2mm;
    }
    config->features[featureidx++] = vtime;
    if (config->roottype == 0)
    {
        config->features[featureidx++] = avgang;
        config->features[featureidx++] = shallowo;
        config->features[featureidx++] = mediumo;
        config->features[featureidx++] = deepo;
    }
    
    if (!config->batchmode)
        config->rootlengthhist.clear();
    for (int k = 0; k <= radiusranges.size(); k++)
    {
        config->features[featureidx++] = rootlengthhist[k] * pixel2mm;
        if (!config->batchmode)
            config->rootlengthhist.push_back(rootlengthhist[k] * pixel2mm);
    }
    for (int k = 0; k <= radiusranges.size(); k++)
        config->features[featureidx++] = projareahist[k] * pixel2mm * pixel2mm;
    for (int k = 0; k <= radiusranges.size(); k++)
        config->features[featureidx++] = sahist[k] * pixel2mm * pixel2mm;
    for (int k = 0; k <= radiusranges.size(); k++)
        config->features[featureidx++] = vhist[k] * pixel2mm * pixel2mm * pixel2mm;

    if (!config->batchmode)
        config->rtdpoints *= pixel2mm;

    if ((!config->batchmode) || (config->batchmode && config->savesegmented))
    {
        Mat m;

        //if (config->invertimage)
        //{
            m = Mat::zeros(seg.size(), seg.type());
            lsim.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
            m = (m < 128);
        //}
        //else
        //{
            /*m = Mat::zeros(seg.size(), seg.type());
            lsim.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
            m = (m < 128);*/
        //}

        //imwrite(sefilename, m);
        if (config->rotation != 0)
        {
            rotate(m, config->seg, revflag);
        }
        else
        {
            m.copyTo(config->seg);
        }
    }

    if ((!config->batchmode) && checkProgress(config, "Generating output image ..."))
        return;

    if (config->batchmode && !config->saveprocessed)
        return;
    else
    {
        Mat m;

        m = Mat::zeros(seg.size(), lsim.type());
        lsim.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
        lsim = m.clone();
        m = Mat::zeros(seg.size(), CV_32FC1);
        dist.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
        dist = m.clone();
        m = Mat::zeros(seg.size(), seg.type());
        skel.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
        skel = m.clone();
    }
    
    // Convex hull is drawn on feature images only for whole roots.
    if (config->roottype == 0)
    {
        Mat m = Mat::zeros(seg.size(), lsim.type());
        
        if (config->showConvexHull)
        {
            hull = Mat::zeros(rmaxseg - rminseg, cmaxseg - cminseg, CV_8UC1);
            drawContours(hull, hullpt, -1, Scalar(255), 2, 8, noArray(), 0, Point());
            hull.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
        }

        hull = m.clone();

        if (config->showConvexHull)
            dilate(hull, dilatedhull, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
    }

    Mat m = Mat::zeros(seg.size(), lsim.type());
    
    if (config->showContours)
    {
        perimeter = Mat::zeros(rmaxseg - rminseg, cmaxseg - cminseg, CV_8UC1);
        drawContours(perimeter, contours, -1, Scalar(255), config->contourwidth, 8, hierarchy);
        perimeter.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
    }

    perimeter = m.clone();

    // Invert the color so the background is white
    lsim = (lsim < 128);
    
    // Convert distance transform to an image.
    double minv, maxv;
    dist.copyTo(m);
    minMaxLoc(m, &minv, &maxv);
    m = (m / maxv) * 255;
    m.convertTo(m, CV_8UC1);
    // To dilate skel, perimeter and convex hull.
    
    /*sefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_seg6.png";
    img2 = drawtopology2(lsim, skel, branchpts, endpts, overlappts);
    img2 = img2.rowRange(rmi, rma).colRange(cmi, cma).clone();
    imwrite(sefilename, img2);*/
    if (config->roottype == 0)
    {
        if (config->showMedialAxis && (!config->showMedialAxisDiameter))
        {
            //skel = skel + perimeter;
            selement = getStructuringElement(MORPH_ELLIPSE, Size(config->medialaxiswidth, config->medialaxiswidth));
            dilate(skel, dilatedskel, selement);
            //dilate(hull, dilatedhull, selement);
            dilatedskel = dilatedskel + perimeter;
        }
        else
            dilatedskel = perimeter;

        Mat dskel;
        
        channels.push_back(lsim - dilatedskel);

        if (config->showDistMap)
            channels.push_back(lsim - dilatedskel - dilatedhull + m);
        else
            channels.push_back(lsim - dilatedskel - dilatedhull);

        channels.push_back((lsim - dilatedhull - dilatedskel) + dilatedskel);
        
        if (config->showMedialAxisDiameter)
        {
            vector<Mat> dch;
            dskel = drawmedialaxis(dist, skel, radiusranges, config->medialaxiswidth);
            split(dskel, dch);
            
            auto t = (dch[1] > 0);

            // We subtract the green channel of the medial axis
            // to set the medial axis pixels to zero
            for (int cidx = 0; cidx < channels.size(); cidx++)
            {
                channels[cidx] = channels[cidx] - t;
                channels[cidx] = channels[cidx] + dch[cidx];
            }
        }
        
        merge(channels, merged);

        holes = lsim - dilatedskel;
        holes = addBorder(255 - holes, 0);

        if (holepts.size() <= 1)
        {
            ////merged = drawtopology2(lsim, skel, branchpts, endpts, overlappts);
            //imwrite(fefilename, merged);
            cvtColor(merged, merged, COLOR_BGR2RGB);
            if (config->rotation != 0)
            {
                rotate(merged, config->processed, revflag);
            }
            else
            {
                merged.copyTo(config->processed);
            }
            
            return;
        }

        if (config->showHoles)
        {
            holepts = getholes(holepts, Size(cmaxseg - cminseg + 2, rmaxseg - rminseg + 2));
            if (holepts.size() > 5)
            {
                centroids = getComponentCentroid(holepts, holes.size());
                pmap = pdist2(centroids, holes.size());
                nearest = getnearestcomponents(pmap);
                colorcodes = getcolorcodes(nearest);
            }
            else
            {
                vector<int> colvalues = { 1,2,3,4,5 };
                std::shuffle(colvalues.begin(), colvalues.end(), urng);
                colorcodes.resize(holepts.size());

                for (int i = 0; i < colorcodes.size(); i++)
                    colorcodes[i] = colvalues[i];
            }

            merged = drawtopology_wholeroot(merged, Scalar_<int>(rmaxseg, rminseg, cmaxseg, cminseg), holepts, colorcodes, colors);
        }
    }
    else
    {
        if (config->showMedialAxis)
        {
            selement = getStructuringElement(MORPH_ELLIPSE, Size(config->medialaxiswidth, config->medialaxiswidth));
            dilate(skel, dilatedskel, selement);
        }
        else
        {
            skel = Mat::zeros(lsim.size(), CV_8UC1);
            dilatedskel = Mat::zeros(lsim.size(), CV_8UC1);
            branchpts.clear();
            endpts.clear();
            overlappts.clear();
        }

        Mat dmap;
        if (config->showDistMap)
            dmap = m;
        else
            dmap = Mat::zeros(lsim.size(), CV_8UC1);

        if (config->showMedialAxis)
        {
            if (!config->showMedialAxisDiameter)
                merged = drawtopology_disconnectedroot(lsim, dmap, Scalar_<int>(rmaxseg, rminseg, cmaxseg, cminseg), dilatedskel, branchpts, endpts, overlappts, config->medialaxiswidth);
            else
            {
                vector<Mat> lch;
                lch.push_back(lsim - dilatedskel);
                lch.push_back(lsim + dmap - dilatedskel);
                lch.push_back(lsim - dilatedskel);
                merge(lch, merged);

                Mat dskel = drawmedialaxis(dist, skel, radiusranges, config->medialaxiswidth);
                merged = merged + dskel;
                //vector<Mat> dch;
                //split(dskel, dch);

                //auto t = (dch[0] > 0);
                //

                //// We subtract the green channel of the medial axis
                //// to set the medial axis pixels to zero
                //for (int cidx = 0; cidx < lch.size(); cidx++)
                //{
                //    lch[cidx] = lch[cidx] - t;
                //    window("adad", t);
                //    window("adad2", dch[cidx]);
                //    lch[cidx] = lch[cidx] + dch[cidx];
                //    window("adad3", lch[cidx]);
                //}


                //cvtColor(merged, merged, COLOR_BGR2RGB);
            }
        }
        else
        {
            vector<Mat> lch;
            lch.push_back(lsim - dilatedskel);
            lch.push_back(lsim + dmap - dilatedskel);
            lch.push_back(lsim - dilatedskel);
            merge(lch, merged);
        }
    }

    cvtColor(merged, merged, COLOR_BGR2RGB);
    
    if (config->rotation != 0)
    {
        rotate(merged, config->processed, revflag);
    }
    else
    {
        merged.copyTo(config->processed);
    }

    //merged.copyTo(config->processed);
    //imwrite(fefilename, merged);
}

int main(int argc, char *argv[])
{
    setUseOptimized(true);

    if (!checkHardwareSupport(CV_CPU_AVX2))
    {
        init(argc, argv, false);
        setUseOptimized(false);
    }
    else
    {
        init(argc, argv, true);
    }

    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication *app = QApplication::instance();
    app->setOrganizationName("Noble Research Institute");
    app->setOrganizationDomain("www.noble.org");
    app->setApplicationName("RhizoVision Explorer");

    Ptr<cvutilWindow> wnd = getImageProcessorWindow(QIcon(":/icons/RVElogoclearback.png"));
    wnd->enableROI(true);
    wnd->setVisibleROI(true);
    wnd->enableActions(true);
    wnd->setVisibleActions(true);
    wnd->setInitialBanner(QPixmap(":/icons/RVElogoclearback.png").scaledToHeight(200, Qt::SmoothTransformation),
        QPixmap(":/icons/noble-logo-color.png").scaledToHeight(75, Qt::SmoothTransformation),
        "RhizoVision Explorer",
        "Load a plant root image from File menu\n             or drag and drop it here.");
    MainUI *dialog = new MainUI();
    dialog->setprocessfunction(feature_extractor);
    dialog->setHostWindow(wnd.get());

    QObject::connect(dialog, &MainUI::updateVisualOutput, [&](Mat m) { wnd->setImage(m); });
    //QObject::connect(dialog, &MainUI::updateProgress, [&](QString status) { wnd->updateProgress(status); });

    wnd->loadPlugins(dialog);
    wnd->show();

    return QApplication::exec();
}
