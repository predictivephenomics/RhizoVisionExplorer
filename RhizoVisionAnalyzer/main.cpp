/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: main.cpp

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
        
        rsize += radii[i].size();
    }

    avgradius /= double(rsize);
}

void getvolumesurfacearea(Mat skeleton, Mat dist, ListofListsRef<Point> segments, PointList overlap, double& volume, double& surfacearea)
{
    int i, j, x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt, ptcount, fxinc, fyinc;
    double rad = 0, sqr = sqrt(2), dv = 0, dvol = 0, dsa = 0;
    size_t cisz;

    float *dtptr = dist.ptr<float>();
    uchar *skel = skeleton.ptr<uchar>();

    volume = 0.0;
    surfacearea = 0.0;

    for (i = 0; i < segments.size(); i++)
    {
        cisz = segments[i].size() - 1;
        ptcount = 0;

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
            rad = (rad == 2.0) ? sqr : rad;

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                dv = dtptr[y * dist.cols + x];
                dvol += (CV_PI * dv * dv * rad);
                dsa += (2.0 * CV_PI * dv * rad);
            }
        }

        dv = dtptr[y * dist.cols + x];
        dvol += (CV_PI * dv * dv);
        dsa += (2.0 * CV_PI * dv);
    }

    for (auto &overlappt : overlap)
    {
        // To check if the overlappt is a diagonal or orthogonal
        if (skel[overlappt.y * skeleton.cols + overlappt.x + 1] > 0)
        {
            // Orthogonal
            dv = dtptr[y * dist.cols + x];
            dvol += (CV_PI * dv * dv);
            dsa += (2.0 * CV_PI * dv);
        }
        else
        {
            // Diagonal
            dv = dtptr[y * dist.cols + x];
            dvol += (CV_PI * dv * dv * sqr);
            dsa += (2.0 * CV_PI * dv * sqr);
        }
    }

    volume = dvol;
    surfacearea = dsa;
}

void getcoarsefineroots(ListofListsRef<double> radii, double& coarse, double& medium, double& fine, double dlo, double dhi)
{
    coarse = 0.0;
    medium = 0.0;
    fine = 0.0;

    double radlo = dlo / 2;
    double radhi = dhi / 2;

    for (auto& r : radii)
    {
        for (auto rad : r)
        {
            if (rad <= radlo) // 43.2 for soybeans // 13.58 for wheat // 16.78 for chaks dataset // 2.97 for lobet taproot
                fine++;
            else if (rad <= radhi) // 86.4 for soybeans // 27.16 for wheat // 33.56 for chaks dataset // 5.94 for lobet taproot
                medium++;
            else
                coarse++;
        }
    }

    double temp = coarse + medium + fine;
    coarse /= temp;
    medium /= temp;
    fine /= temp;
}

void getshallowdeeproots(ListofListsRef<Point> segments, double& shallow, double& medium, double& deep, double &avgangle)
{
    shallow = 0.0;
    medium = 0.0;
    deep = 0.0;
    avgangle = 0.0;

    Mat m, m2, cov, me, lam, ev;
    double *evptr = nullptr;
    int i, j, k, nsum, x, y, itrx, itry, nnextpt, currx, curry, nextx, nexty, xinc, yinc;
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
            int i, j, k, nsum, x, y, itrx, itry, nnextpt, currx, curry, nextx, nexty, xinc, yinc;
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

//void feature_extractor(string filename, string savefolder, vector<double>& featurevals, bool invertimage, bool saveseg, bool saveimages, bool pixelconv, double conversion, double rootlo, double roothi)
void feature_extractor(feature_config* config, vector<double>& featurevals)
{
    Mat img, out, seg, lsim, lsimcropped, lsimlab, skel, dilatedskel, dist;
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
    double coarser = 0.0, mediumr = 0.0, finer = 0.0;
    double shallowo = 0.0, mediumo = 0.0, deepo = 0.0, avgang = 0.0;
    double volume = -1.0, surfacearea = -1.0, vtime;
    int maxradrowidx = -1, nholes = 0;
    double pixel2mm = 1.0, avgholesize = 0.0;
    bool isborder = false, isborderholefound = false;
    double diameterlo = config->rootlo, diameterhi = config->roothi;
    double nwarea = 0, convarea = 0, widthtodepthratio = 0, lowerrootarea = 0;
    std::random_device rng;
    std::mt19937 urng(rng());
    int nfeatures = 27;

#ifndef _DEBUG
    thread *t1, *t2;
#endif

    vector<vector<int>> cc, colors =
    { { 228, 26, 28 },
    { 55, 126, 184 },
    { 77, 175, 74 },
    { 152, 78, 163 },
    { 255, 127, 0 } };

    if (config->pixelconv)
    {
        pixel2mm /= config->conversion;
        diameterlo *= config->conversion;
        diameterhi *= config->conversion;
    }

    string fefile = experimental::filesystem::path(config->filename).filename().string();
    string fefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_features.png";
    string sefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_seg.png";
    string sefilename2 = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_skel.png";
    
    Mat img2;
    int cmi = 1455, cma = 1531, rmi = 790, rma = 891;

    img = imread(config->filename, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
    //img = addBorder(img, Scalar(255, 255, 255, 255), 1);
    //sefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_seg0.png";
    /*img2 = img.clone();
    rectangle(img2, Point(cmi, rmi), Point(cma + 1, rma + 1), Scalar(0, 0, 255), 4);
    imwrite(sefilename, img2);*/

    /*sefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_seg1.png";
    img2 = img.rowRange(rmi, rma).colRange(cmi, cma).clone();
    imwrite(sefilename, img2);*/

    if (img.empty())
    {
        for (int ii = 0; ii < nfeatures; ii++)
            featurevals.push_back(NAN);
        return;
    }

    srand(time(0));
    tic();

    if (config->invertimage)
        out = getSingleChannel(img > config->threshold, CV_8UC1);
    else
        out = getSingleChannel(img < config->threshold, CV_8UC1);
    
    seg = getLargestConnectedComponent(out);
    
    Mat segcropped;
    auto fresultseg = find(seg>0, FindType::Subscripts);
    double rminseg = seg.rows, rmaxseg = -1, cminseg = seg.cols, cmaxseg = -1;
    int *rcpseg = fresultseg.first.ptr<int>();
    
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

    if ((rmaxseg - rminseg) == 1 || (cmaxseg - cminseg) == 1)
    {
        for (int ii = 0; ii < nfeatures; ii++)
            featurevals.push_back(NAN);
        return;
    }

    if (rmaxseg > rminseg || cmaxseg > cminseg)
        segcropped = seg.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg).clone();
    else
        segcropped = seg.clone();
    
    lsim = linesim(segcropped, LineSimplificationType::DouglasPeucker, 2.0);
    dist = bwdist(lsim);
    skel = bwskel(lsim, dist);
    findContours(lsim.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    
    hullpt.resize(1);
    if (contours.size() > 0)
        convexHull(contours[0], hullpt[0], false);
    
    getroottopology(skel, branchpts, endpts, overlappts, rootsegments);
    
#ifndef _DEBUG
    t1 = new thread([&]() {
#endif
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

        nwarea = double(countNonZero(lsim));
        convarea = 0.0;

        for (int i = 0, ni = 0; i < hullpt[0].size(); i++)
        {
            ni = ((i + 1) % hullpt[0].size());
            convarea += double(hullpt[0][i].x * hullpt[0][ni].y - hullpt[0][ni].x * hullpt[0][i].y);
        }

        convarea /= 2.0;
#ifndef _DEBUG
    });
#endif
        getrootradii(skel, lsim, dist, rootsegments, radii, maxradius, maxradrowidx);
        getrootlength(skel, rootsegments, overlappts, rootlength);
        getavgrootradius(radii, medradius, avgradius);
        getvolumesurfacearea(skel, dist, rootsegments, overlappts, volume, surfacearea);
        getcoarsefineroots(radii, coarser, mediumr, finer, diameterlo, diameterhi);
        
        auto fresult = find(lsim>0, FindType::Subscripts);
        double rmin = fresult.first.rows, rmax = -1, cmin = fresult.first.rows, cmax = -1;
        int *rcp = fresult.first.ptr<int>();
        
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
        
        if (maxradius > 0 && lsim.rows > 1)
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
    
    featurevals.push_back(mediannoroots);
    featurevals.push_back(maxnoroots);
    featurevals.push_back(endpts.size());
    featurevals.push_back(rootlength * pixel2mm);
    featurevals.push_back(double(rmax - rmin) * pixel2mm);
    featurevals.push_back(double(cmax - cmin) * pixel2mm);
    featurevals.push_back(widthtodepthratio);
    featurevals.push_back(nwarea * pixel2mm * pixel2mm);
    featurevals.push_back(convarea * pixel2mm * pixel2mm);
    featurevals.push_back(featurevals[featurevals.size() - 2] / featurevals[featurevals.size() - 1]);
    featurevals.push_back(lowerrootarea * pixel2mm * pixel2mm);
    featurevals.push_back(avgradius * pixel2mm * 2.0);
    featurevals.push_back(medradius * pixel2mm * 2.0);
    if (maxradius >= 0)
        featurevals.push_back(maxradius * pixel2mm * 2.0);
    else
        featurevals.push_back(NAN);
    featurevals.push_back(perimeterlength);
    featurevals.push_back(volume * pixel2mm * pixel2mm * pixel2mm);
    featurevals.push_back(surfacearea  * pixel2mm * pixel2mm);
    featurevals.push_back(nholes);
    featurevals.push_back(avgholesize  * pixel2mm * pixel2mm);
    featurevals.push_back(vtime);
    featurevals.push_back(avgang);
    featurevals.push_back(shallowo);
    featurevals.push_back(mediumo);
    featurevals.push_back(deepo);
    //featurevals.push_back(shallowo / deepo);
    featurevals.push_back(finer);
    featurevals.push_back(mediumr);
    featurevals.push_back(coarser);
    //featurevals.push_back(finer / coarser);
    
    if (config->saveseg)
    {
        Mat m;

        if (config->invertimage)
        {
            m = Mat::zeros(seg.size(), seg.type());
            lsim.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
        }
        else
        {
            m = Mat::zeros(seg.size(), seg.type());
            lsim.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
            m = (m < 128);
        }

        imwrite(sefilename, m);
    }

    if (!config->saveimages)
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
    
    Mat m = Mat::zeros(seg.size(), lsim.type());
    hull = Mat::zeros(rmaxseg - rminseg, cmaxseg - cminseg, CV_8UC1);
    drawContours(hull, hullpt, -1, Scalar(255), 2, 8, noArray(), 0, Point());
    hull.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
    hull = m.clone();

    m = Mat::zeros(seg.size(), lsim.type());
    perimeter = Mat::zeros(rmaxseg - rminseg, cmaxseg - cminseg, CV_8UC1);
    drawContours(perimeter, contours, -1, Scalar(255), 1, 8, hierarchy);
    perimeter.copyTo(m.rowRange(rminseg, rmaxseg).colRange(cminseg, cmaxseg));
    perimeter = m.clone();

    // Invert the color so the background is white
    lsim = (lsim < 128);
    
    // Convert distance transform to an image.
    double minv, maxv;
    minMaxLoc(dist, &minv, &maxv);
    dist = (dist / maxv) * 255;
    dist.convertTo(dist, CV_8UC1);
    // To dilate skel, perimeter and convex hull.
    
    skel = skel + perimeter;
    selement = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(skel, dilatedskel, selement);
    dilate(hull, dilatedhull, selement);
    
    channels.push_back(lsim - dilatedskel);
    channels.push_back(lsim - dilatedskel - dilatedhull + dist);
    channels.push_back((lsim - dilatedhull - dilatedskel) + dilatedskel);
    
    merge(channels, merged);
    holes = lsim - dilatedskel;
    holes = addBorder(255 - holes, 0);
    
    if (holepts.size() <= 1)
    {
        //merged = drawtopology2(lsim, skel, branchpts, endpts, overlappts);
        imwrite(fefilename, merged);
        return;
    }

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

    /*sefilename = config->savefolder + fefile.substr(0, fefile.length() - 4) + "_seg6.png";
    img2 = drawtopology2(lsim, skel, branchpts, endpts, overlappts);
    img2 = img2.rowRange(rmi, rma).colRange(cmi, cma).clone();
    imwrite(sefilename, img2);*/
    //merged = drawtopology2(lsim, Scalar_<int>(rmaxseg, rminseg, cmaxseg, cminseg), skel, branchpts, endpts, overlappts);
    merged = colorimage(merged, Scalar_<int>(rmaxseg, rminseg, cmaxseg, cminseg), holepts, colorcodes, colors);
    imwrite(fefilename, merged);
}

int main(int argc, char *argv[])
{
    setUseOptimized(true);

    if (!checkHardwareSupport(CV_CPU_AVX2))
    {
        init(argc, argv, false);
        setUseOptimized(false);
        //cout << "Disabled\n";
        /*QMessageBox::critical(nullptr, "RhizoVision Analyzer", "The processor on this system does not support AVX2, needed by the program to extract features.", QMessageBox::StandardButton::Ok);
        return -1;*/
    }
    else
    {
        //cout << "Enabled\n";
        init(argc, argv, true);
    }

    QCoreApplication *app = QApplication::instance();
    app->setOrganizationName("Noble Research Institute");
    app->setOrganizationDomain("www.noble.org");
    app->setApplicationName("RhizoVision Analyzer");

    MainUI dialog;
    dialog.setWindowFlags(dialog.windowFlags()); // &~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle("RhizoVision Analyzer");
    dialog.setprocessfunction(feature_extractor);
    dialog.show();

    return QApplication::exec();
}
