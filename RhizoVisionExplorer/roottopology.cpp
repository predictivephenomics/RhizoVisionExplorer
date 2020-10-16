/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: roottopology.cpp

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

#include "roottopology.h"

using namespace std;
using namespace cv;

class rootsegment
{
    bool endptset = false;

    void removeunnecesary();
    
    unsigned long long generatekey(int x1, int y1, int x2, int y2);
public:
    rootsegment(Point startpoint, int _startpointtype); // 1 for end point, 2 for branch point, 3 for overlapped point
    ~rootsegment();

    // Sets end point indicating that the root segment 
    // is complete.
    void setendpoint(Point endpoint, int _endpointtype);

    bool isendpointset();

    bool operator ==(rootsegment r);

    //Points getpoints();
    
    double getLength();

    void merge(rootsegment *r);

    //vector<int> points;
    unsigned long long key;
    Points pts;
    bool prunedelete = false;
    int startpointtype = 0;
    int endpointtype = 0;
};

rootsegment::rootsegment(Point startpoint, int _startpointtype)
{
    pts.reserve(100);
    pts.push_back(startpoint);
    startpointtype = _startpointtype;
}

rootsegment::~rootsegment()
{
    //pts.clear();
}

void rootsegment::removeunnecesary()
{
    int i;
    Points newpoints;

    newpoints.push_back(pts[0]);
    for (i = 1; i < (pts.size() - 1); i++)
    {
        if (pts[i].y == pts[i + 1].y && pts[i].y == pts[i - 1].y)
            continue;
        else if (pts[i].x == pts[i + 1].x && pts[i].x == pts[i - 1].x)
            continue;
        else if ((double(pts[i].y - pts[i + 1].y) / double(pts[i].x - pts[i + 1].x)) == (double(pts[i - 1].y - pts[i + 1].y) / double(pts[i - 1].x - pts[i + 1].x)))
            continue;
        else
            newpoints.push_back(pts[i]);
    }

    newpoints.push_back(pts[i]);
    pts = newpoints;
}

unsigned long long rootsegment::generatekey(int x1, int y1, int x2, int y2)
{
    int hix, lox, hiy, loy;

    if (x1 < x2)
    {
        lox = x1;
        loy = y1;
        hix = x2;
        hiy = y2;
    }
    else if (x1 > x2)
    {
        lox = x2;
        loy = y2;
        hix = x1;
        hiy = y1;
    }
    else // If x coordinates are equal, we base our logic on y coordinates.
    {
        if (y1 < y2)
        {
            lox = x1;
            loy = y1;
            hix = x2;
            hiy = y2;
        }
        else
        {
            lox = x2;
            loy = y2;
            hix = x1;
            hiy = y1;
        }
    }

    return ((unsigned long long)(lox * 10000 + loy) * 10000 + hix) * 10000 + hiy;
}

void rootsegment::setendpoint(Point endpoint, int _endpointtype)
{
    //int lo, hi;

    endptset = true;
    pts.push_back(endpoint);
    removeunnecesary();
    endpointtype = _endpointtype;

    key = generatekey(pts[0].x, pts[0].y, pts[pts.size() - 1].x, pts[pts.size() - 1].y);
}

bool rootsegment::isendpointset()
{
    return endptset;
}

bool rootsegment::operator ==(rootsegment r)
{
    if (this->pts.size() != r.pts.size())
        return false;
    else
    {
        int i = 0;
        bool f1 = true, f2 = true;
        size_t sz = pts.size();

        for (i = 1; i < (sz - 1); i++)
        {
            if (pts[i] != r.pts[i])
            {
                f1 = false;
                break;
            }
        }

        for (i = 1; i < (sz - 1); i++)
        {
            if (pts[i] != r.pts[sz - i - 1])
            {
                f2 = false;
                break;
            }
        }

        return (f1 || f2);
    }
}

//Points rootsegment::getpoints()
//{
//    return pts;
//}

double rootsegment::getLength()
{
    if (!endptset)
        return 0;

    double result = 0;
    Point diff;
    double a = 0, b = 0;

    for (int i = 0; i < (pts.size() - 1); i++)
    {
        diff = pts[i] - pts[i + 1];
        a = fabs(double(diff.x));
        b = fabs(double(diff.y));

        if (a == b)
            result += a * CVUTIL_SQRT2;
        else if (a > b)
            result += a;
        else
            result += b;
    }

    return result;
}

void rootsegment::merge(rootsegment *r)
{
    // Make sure one end point of the rootsegment *r is a root tip.
    //if (r->startpointtype != 1 && r->endpointtype != 1)
    //    return;

    // Find the common point that the segments were connected.
    //if (!((r->startpointtype == 1 && (r->pts.back() == this->pts.back() || r->pts.back() == this->pts.front())) || 
    //      (r->endpointtype == 1 && (r->pts.front() == this->pts.front() || r->pts.front() == this->pts.back()))))
    if (!(r->pts.back() == this->pts.back() || r->pts.back() == this->pts.front() || r->pts.front() == this->pts.front() || r->pts.front() == this->pts.back()))
        return;

    // Add all the points to the new point list.
    this->pts.reserve(this->pts.size() + r->pts.size() - 1);

    //if (r->startpointtype == 1)
    //{
        if (r->pts.back() == this->pts.back())
        {
            this->pts.insert(this->pts.end(), r->pts.rbegin() + 1, r->pts.rend());
            this->endpointtype = r->startpointtype;
        }
        else if (r->pts.back() == this->pts.front())
        {
            this->pts.insert(this->pts.begin(), r->pts.begin(), r->pts.end() - 1);
            this->startpointtype = r->startpointtype;
        }
    //}
    //else
    //{
        else if (r->pts.front() == this->pts.front())
        {
            this->pts.insert(this->pts.begin(), r->pts.rbegin(), r->pts.rend() - 1);
            this->startpointtype = r->endpointtype;
        }
        else
        {
            this->pts.insert(this->pts.end(), r->pts.begin() + 1, r->pts.end());
            this->endpointtype = r->endpointtype;
        }
    //}

    removeunnecesary();
    key = generatekey(pts[0].x, pts[0].y, pts[pts.size() - 1].x, pts[pts.size() - 1].y);
}

namespace std
{
    template<>
    struct hash<Point>
    {
        size_t operator()(Point const& pt) const
        {
            return (size_t)((size_t)(pt.x) * 1000000ULL + (size_t)pt.y);
        }
    };
}

void removeduplicates(ListofListsRef<Point> contours, ListofListsRef<Point> connpts, PointList pts)
{
    int i = 0, nsum = 0;
    int nconnpts = static_cast<int>(connpts.size());

    for (i = 0, nsum = 0; i < nconnpts; i++)
    {
        unordered_set<Point> s;
        for (Point& j : connpts[i])
            s.insert(j);
        connpts[i].assign(s.begin(), s.end());
        nsum += static_cast<int>(connpts[i].size());
    }

    pts.resize(nsum);
    hash<int>();
    for (i = 0, nsum = 0; i < nconnpts; i++)
    {
        for (int j = 0; j < connpts[i].size(); j++)
        {
            pts[nsum] = connpts[i][j];
            nsum++;
        }
    }
}

void removeduplicateswithpruning(ListofListsRef<Point> contours, ListofListsRef<Point> connpts, map<int, map<int, int>> bpts, PointList pts, int pointtype)
{
    int i = 0, nsum = 0;
    int nconnpts = static_cast<int>(connpts.size());
    
    for (i = 0, nsum = 0; i < nconnpts; i++)
    {
        unordered_set<Point> s;
        for (Point& j : connpts[i])
        {
            if (bpts[j.x][j.y] == pointtype)
                s.insert(j);
        }
        connpts[i].assign(s.begin(), s.end());
        nsum += static_cast<int>(connpts[i].size());
    }

    pts.resize(nsum);
    hash<int>();
    for (i = 0, nsum = 0; i < nconnpts; i++)
    {
        for (int j = 0; j < connpts[i].size(); j++)
        {
            pts[nsum] = connpts[i][j];
            nsum++;
        }
    }
}

void getrootsegmentlist(DictionaryRef<unsigned long long, vector<rootsegment *>> segments, ListofListsRef<Point> rootsegments)
{
    int j = 0, nsum = 0;
    int nsegments = static_cast<int>(segments.size());

    // for (i = 0, nsum = 0; i < nsegments; i++)
    for (auto &s : segments)
    {
        nsum += static_cast<int>(s.second.size());
    }

    rootsegments.resize(nsum);

    nsum = 0;
    for (auto &s : segments)
    {
        for (j = 0; j < s.second.size(); j++)
            rootsegments[nsum++] = s.second[j]->pts;
    }
}

void getrootsegmentlistwithpruning(DictionaryRef<unsigned long long, vector<rootsegment *>> segments, ListofListsRef<Point> rootsegments)
{
    int j = 0, nsum = 0;
    int nsegments = static_cast<int>(segments.size());

    // for (i = 0, nsum = 0; i < nsegments; i++)
    for (auto &s : segments)
    {
        for (auto& rs : s.second)
        {
            if (!rs->prunedelete)
                nsum++;
        }
    }

    rootsegments.resize(nsum);

    nsum = 0;
    for (auto &s : segments)
    {
        for (j = 0; j < s.second.size(); j++)
        {
            if (!s.second[j]->prunedelete)
                rootsegments[nsum++] = s.second[j]->pts;
        }
    }
}

void modifycontours(Mat skeleton, ListofListsRef<Point> contours)
{
    int i, j, x, y, nnextpt, currx, curry, nextx, nexty, xinc, yinc; // , npoints;
    unsigned char *skel = skeleton.ptr<unsigned char>();
    int ncols = skeleton.cols;
    int N3, N7;
    //Points branchpts;
    //Indices insertat;
    size_t csz = contours.size(), cisz;
    //unsigned long long key;

    Points tempcontour;
    //int insertidx, csize, iidx, isize, cidx;

    //branchpts.reserve(2000);
    //insertat.reserve(2000);
    
    /*for (i = 0, npoints = 0; i < csz; i++)
    {
        npoints += contours[i].size();
    }*/

    for (i = 0; i < csz; i++)
    {
        cisz = contours[i].size();

        for (j = 0; j < cisz; j++)
        {
            nnextpt = (j == cisz - 1) ? 0 : j + 1;
            
            currx = contours[i][j].x;
            curry = contours[i][j].y;
            nextx = contours[i][nnextpt].x;
            nexty = contours[i][nnextpt].y;
            
            //if (currx < nextx)
            //    key = ((currx * 10000 + curry) * 10000 + nextx) * 10000 + nexty;
            //else if (currx > nextx)
            //    key = ((nextx * 10000 + nexty) * 10000 + currx) * 10000 + curry;
            //else // If x coordinates are equal, we base our logic on y coordinates.
            //{
            //    if (curry < nexty)
            //        key = ((currx * 10000 + curry) * 10000 + nextx) * 10000 + nexty;
            //    else
            //        key = ((nextx * 10000 + nexty) * 10000 + currx) * 10000 + curry;
            //}

            // If the current point to next point is a
            // horizontal line.
            if (curry == nexty)
            {
                tempcontour.push_back(Point(currx, curry));

                if (currx < nextx)
                {
                    for (x = currx + 1; x < nextx; x++)
                    {
                        N3 = int(skel[(curry - 1) * ncols + x] > 0);
                        N7 = int(skel[(curry + 1) * ncols + x] > 0);
                        
                        if (N3 == 1 || N7 == 1) // T found
                        {
                            //branchpts.push_back(Point(x, curry));
                            //insertat.push_back(j + 1);
                            tempcontour.push_back(Point(x, curry));
                        }
                    }
                }
                else
                {
                    for (x = currx - 1; x > nextx; x--)
                    {
                        N3 = int(skel[(curry - 1) * ncols + x] > 0);
                        N7 = int(skel[(curry + 1) * ncols + x] > 0);

                        if (N3 == 1 || N7 == 1) // T found
                        {
                            //branchpts.push_back(Point(x, curry));
                            //insertat.push_back(j + 1);
                            tempcontour.push_back(Point(x, curry));
                        }
                    }
                }
            }
            else // If the contour comes to horizontal T from the perpendicular side.
            {
                tempcontour.push_back(Point(currx, curry));

                if ((currx + 1) == nextx && (curry + 1) == nexty)
                {
                    //tempcontour.push_back(Point(currx, curry));

                    if (skel[(curry) * ncols + currx + 1] > 0)
                    {
                        //branchpts.push_back(Point(currx + 1, curry));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx + 1, curry));
                    }
                    else if (skel[(curry + 1) * ncols + currx] > 0)
                    {
                        //branchpts.push_back(Point(currx, curry + 1));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx, curry + 1));
                    }
                }
                else if ((currx + 1) == nextx && (curry - 1) == nexty)
                {
                    //tempcontour.push_back(Point(currx, curry));

                    if (skel[(curry)* ncols + currx + 1] > 0)
                    {
                        //branchpts.push_back(Point(currx + 1, curry));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx + 1, curry));
                    }
                    else if (skel[(curry - 1) * ncols + currx] > 0)
                    {
                        //branchpts.push_back(Point(currx, curry - 1));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx, curry - 1));
                    }
                }
                else if ((currx - 1) == nextx && (curry + 1) == nexty)
                {
                    //tempcontour.push_back(Point(currx, curry));

                    if (skel[(curry)* ncols + currx - 1] > 0)
                    {
                        //branchpts.push_back(Point(currx - 1, curry));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx - 1, curry));
                    }
                    else if (skel[(curry + 1) * ncols + currx] > 0)
                    {
                        //branchpts.push_back(Point(currx, curry + 1));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx, curry + 1));
                    }
                }
                else if ((currx - 1) == nextx && (curry - 1) == nexty)
                {
                    //tempcontour.push_back(Point(currx, curry));

                    if (skel[(curry)* ncols + currx - 1] > 0)
                    {
                        //branchpts.push_back(Point(currx - 1, curry));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx - 1, curry));
                    }
                    else if (skel[(curry - 1) * ncols + currx] > 0)
                    {
                        //branchpts.push_back(Point(currx, curry - 1));
                        //insertat.push_back(j + 1);
                        tempcontour.push_back(Point(currx, curry - 1));
                    }
                }
                else if (abs(currx - nextx) == abs(curry - nexty)) // To analyze diagonal paths
                {
                    xinc = abs(currx - nextx) / (nextx - currx);
                    yinc = abs(curry - nexty) / (nexty - curry);
                    
                    //tempcontour.push_back(Point(currx, curry));

                    for (x = currx, y = curry; x != nextx; x += xinc, y += yinc)
                    {
                        if (skel[(y) * ncols + x + xinc] > 0)
                        {
                            if (x != currx)
                            {
                                //branchpts.push_back(Point(x, y));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x, y));
                            }

                            //branchpts.push_back(Point(x + xinc, y));
                            //insertat.push_back(j + 1);
                            tempcontour.push_back(Point(x + xinc, y));

                            if ((x + xinc) != nextx)
                            {
                                //branchpts.push_back(Point(x + xinc, y + yinc));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x + xinc, y + yinc));
                            }
                        }
                        else if (skel[(y + yinc) * ncols + x] > 0)
                        {
                            if (x != currx)
                            {
                                //branchpts.push_back(Point(x, y));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x, y));
                            }

                            //branchpts.push_back(Point(x, y + yinc));
                            //insertat.push_back(j + 1);
                            tempcontour.push_back(Point(x, y + yinc));

                            if ((x + xinc) != nextx)
                            {
                                //branchpts.push_back(Point(x + xinc, y + yinc));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x + xinc, y + yinc));
                            }
                        }
                        else if (skel[(y + yinc) * ncols + x - xinc] > 0 && skel[(y + yinc) * ncols + x] == 0 && skel[(y)* ncols + x - xinc] == 0)
                        {
                            if (x != currx)
                            {
                                //branchpts.push_back(Point(x, y));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x, y));
                            }
                        }
                        else if (skel[(y - yinc) * ncols + x + xinc] > 0 && skel[(y - yinc) * ncols + x] == 0 && skel[(y) * ncols + x + xinc] == 0)
                        {
                            if (x != currx)
                            {
                                //branchpts.push_back(Point(x, y));
                                //insertat.push_back(j + 1);
                                tempcontour.push_back(Point(x, y));
                            }
                        }
                    }
                }
            }
        }

        /*insertidx = 0;
        iidx = 0;
        cidx = 0;
        isize = insertat.size();
        csize = contours[i].size() + insertat.size();
        tempcontour.clear();

        do
        {
            if (iidx < isize && insertidx == insertat[iidx])
            {
                tempcontour.push_back(branchpts[iidx]);
                iidx++;
                insertidx++;
            }
            else
            {
                tempcontour.push_back(contours[i][cidx]);
                cidx++;
                insertidx++;
            }
        } while (insertidx < (contours[i].size() + insertat.size()));*/

        contours[i] = tempcontour;
        tempcontour.clear();
        //// Reserve the space for additional pixels.
        //contours[i].reserve(contours[i].size() + insertat.size());

        //// Insert points at the positions specified.
        //auto itr = contours[i].begin();
        //for (j = insertat.size() - 1; j >= 0; j--)
        //    contours[i].insert(itr + insertat[j], branchpts[j]);
        
        //// Clear the lists
        //branchpts.clear();
        //insertat.clear();
    }
}

static bool hasbranchpts(Point curr, Point next, map<int, map<int, int>>& bpts, int i, int j)
{
    int x, y;

    //int kk = i + j;

    int xinc = ((next.x - curr.x) == 0) ? 0 : abs(curr.x - next.x) / (next.x - curr.x);
    int yinc = ((next.y - curr.y) == 0) ? 0 : abs(curr.y - next.y) / (next.y - curr.y);

    /*if (abs(curr.x - next.x) != abs(curr.y - next.y) && (curr.x - next.x) != 0 && (curr.y - next.y) != 0)
        qInfo() << "Control";*/

    for (x = curr.x + xinc, y = curr.y + yinc; (x != next.x || y != next.y); x += xinc, y += yinc)
        if (bpts[x][y])
            return true;
    
    return false;
}

void drawskeleton(Mat &_skeleton, ListofListsRef<Point> rootsegments)
{

}

void getroottopology(Mat &_skeleton, Mat dist, 
    PointList branchpts,
    PointList endpts,
    PointList overlappts,
    ListofListsRef<Point> rootsegments, bool pruneroots, int rootPruningThreshold)
{
    //static int aa = 0;
    int i, j, _j;
    int N1, N2, N3, N4, N5, N6, N7, N8, nsum, nsum1, nsum2, ispresent, skncomp, nsize;
    float *dtptr = dist.ptr<float>();
    rootsegment *segment = nullptr;
    ListofLists<Point> branch, end, over;
    map<int, map<int, int>> bpts, ptsdelete;
    
    // Maintain segments for each branch or overlapped point
    //map<int, map<int, vector<pair<unsigned long long, int>>>> segidx;

    Mat ebpts;
    uchar *ebptsptr = nullptr;

    Dictionary<unsigned long long, vector<rootsegment *>> segments;

    // The variable below stores the indices (in segments list) for every branch/end/overlapped
    // point connected to it.
    map<int, map<int, vector<pair<unsigned long long, int>>>> connIndices;

    ListofLists<Point> contours;
    Indices ptsizes, conmap;
    Mat sklabels;
    size_t sz;
    int lj = 0;

    //aa++;

    Mat skeleton = _skeleton.clone();
    hconcat(Mat::zeros(skeleton.rows, 1, skeleton.type()), skeleton, skeleton);
    hconcat(skeleton, Mat::zeros(skeleton.rows, 1, skeleton.type()), skeleton);

    vconcat(Mat::zeros(1, skeleton.cols, skeleton.type()), skeleton, skeleton);
    vconcat(skeleton, Mat::zeros(1, skeleton.cols, skeleton.type()), skeleton);

    skncomp = connectedComponents(skeleton, sklabels) - 1;
    
    unsigned char *skel = skeleton.ptr<unsigned char>();
    int *skco = sklabels.ptr<int>();
    int ncols = skeleton.cols;

    findContours(skeleton.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    modifycontours(skeleton, contours);

    branch.resize(skncomp);
    end.resize(skncomp);
    over.resize(skncomp);
    ptsizes.resize(skncomp);

    segments.reserve(20000);

    conmap.resize(contours.size());
    
    for (i = 0, nsize = 0; i < contours.size(); i++)
    {
        conmap[i] = skco[contours[i][0].y * ncols + contours[i][0].x] - 1;
        ptsizes[conmap[i]] += static_cast<int>(contours[i].size());
        nsize += static_cast<int>(contours[i].size());
    }

    for (i = 0; i < skncomp; i++)
    {
        branch[i].reserve(ptsizes[i]);
        end[i].reserve(ptsizes[i]);
        over[i].reserve(ptsizes[i]);
    }

    //qDebug() << "Image size: rows = " << skeleton.rows << " columns = " << skeleton.cols;

    //window("new", skeleton);

    // To identify the branch, end and overlapped points first before 
    // generating the list of root segments. This helps us to check 
    // if a potential root segment contains any of these points in the 
    // middle of the root segment, which is not desirable.
    for (i = 0; i < contours.size(); i++)
    {
        for (j = 0; j < contours[i].size(); j++)
        {
            N1 = int(skel[contours[i][j].y * ncols + contours[i][j].x + 1] > 0);
            N2 = int(skel[(contours[i][j].y - 1) * ncols + contours[i][j].x + 1] > 0);
            N3 = int(skel[(contours[i][j].y - 1) * ncols + contours[i][j].x] > 0);
            N4 = int(skel[(contours[i][j].y - 1) * ncols + contours[i][j].x - 1] > 0);
            N5 = int(skel[contours[i][j].y * ncols + contours[i][j].x - 1] > 0);
            N6 = int(skel[(contours[i][j].y + 1) * ncols + contours[i][j].x - 1] > 0);
            N7 = int(skel[(contours[i][j].y + 1) * ncols + contours[i][j].x] > 0);
            N8 = int(skel[(contours[i][j].y + 1) * ncols + contours[i][j].x + 1] > 0);

            nsum = N1 + N2 + N3 + N4 + N5 + N6 + N7 + N8;

            if (nsum == 1 || (nsum == 2 &&
                (((N1 + N2) == 2) || ((N3 + N4) == 2) || ((N5 + N6) == 2) || ((N7 + N8) == 2) ||
                ((N1 + N8) == 2) || ((N7 + N6) == 2) || ((N5 + N4) == 2) || ((N3 + N2) == 2)))) // End-point
            {
                end[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                bpts[contours[i][j].x - 1][contours[i][j].y - 1] = 1;
            }
            else
            {
                nsum1 = int(N1 > N2) + int(N2 > N3) + int(N3 > N4) + int(N4 > N5) +
                    int(N5 > N6) + int(N6 > N7) + int(N7 > N8) + int(N8 > N1);

                nsum2 = int(N1 < N2) + int(N2 < N3) + int(N3 < N4) + int(N4 < N5) +
                    int(N5 < N6) + int(N6 < N7) + int(N7 < N8) + int(N8 < N1);

                if ((nsum1 == 3 && nsum2 == 3) ||
                    (nsum >= 4 && nsum < 7 && ((N1 + N2 + N3) == 3 || (N3 + N4 + N5) == 3 || (N5 + N6 + N7) == 3 || (N7 + N8 + N1) == 3) &&
                    (!(nsum == 6 && ((N1 + N5) == 0 || (N3 + N7) == 0 ||
                        (N1 + N2) == 0 || (N1 + N8) == 0 || (N3 + N2) == 0 || (N7 + N8) == 0 ||
                        (N3 + N4) == 0 || (N7 + N6) == 0 || (N5 + N4) == 0 || (N5 + N6) == 0)))) || // branch point
                        (nsum == 4 && ((N1 + N3 + N5 + N7) == 4 || (N2 + N4 + N6 + N8) == 4))) // overlapping-point
                {
                    if (nsum == 4 && ((N1 + N3 + N5 + N7) == 4 || (N2 + N4 + N6 + N8) == 4))
                    {
                        over[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        branch[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        bpts[contours[i][j].x - 1][contours[i][j].y - 1] = 3;
                    }
                    else
                    {
                        branch[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        bpts[contours[i][j].x - 1][contours[i][j].y - 1] = 2;
                    }
                }
                else
                    bpts[contours[i][j].x - 1][contours[i][j].y - 1] = 0;
            }
        }
    }

    // To generate all the topological information using 
    // the contours we got earlier.
    for (i = 0; i < contours.size(); i++)
    {
        sz = contours[i].size();

        if (sz == 1)
            continue;

        if (segment != nullptr)
        {
            delete segment;
            segment = nullptr;
        }

        for (_j = 0, lj = 0; lj <= (2 * sz + 2); _j++, lj++)
        {
            j = (_j % sz);
            
            if (bpts[contours[i][j].x - 1][contours[i][j].y - 1] == 1) // End point
            {
                if (segment == nullptr)
                    segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, 1);
                else
                {
                    if (hasbranchpts(segment->pts.back(), { contours[i][j].x - 1, contours[i][j].y - 1 }, bpts, i, j))
                    {
                        delete segment;
                        segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, 1);
                    }
                    else
                    {
                        segment->setendpoint({ contours[i][j].x - 1, contours[i][j].y - 1 }, 1);

                        if (pruneroots && (bpts[segment->pts[0].x][segment->pts[0].y] == 2 || bpts[segment->pts[0].x][segment->pts[0].y] == 3) &&
                            segment->getLength() <= (dtptr[segment->pts[0].y * dist.cols + segment->pts[0].x] + rootPruningThreshold))
                        {
                            /*ptsdelete[contours[i][j].x - 1][contours[i][j].y - 1] = 1;
                            
                            if (bpts[segment->pts[0].x][segment->pts[0].y] == 2)
                                ptsdelete[segment->pts[0].x][segment->pts[0].y] = 1;
                            else
                                ptsdelete[segment->pts[0].x][segment->pts[0].y]++;*/

                            segment->prunedelete = true;
                        }

                        // The root segment is completed now.
                        // We will now check if the segment is 
                        // already added to the list of segments.
                        // If it is not present, then we add the
                        // new segment.
                        ispresent = 0;
                        for (auto& seg : segments[segment->key])
                        {
                            if ((*seg) == (*segment))
                            {
                                ispresent = 1;
                                break;
                            }
                        }

                        if (ispresent)
                            delete segment;
                        else
                        {
                            segments[segment->key].push_back(segment);
                            
                            if (segment->pts.front().x != segment->pts.back().x || segment->pts.front().y != segment->pts.back().y)
                            {
                                connIndices[segment->pts.front().x][segment->pts.front().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));
                                connIndices[segment->pts.back().x][segment->pts.back().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));
                            }
                            else
                                connIndices[segment->pts.front().x][segment->pts.front().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));

                            //segidx[segment->pts[0].x][segment->pts[0].y].push_back({ segment->key, segments[segment->key].size() - 1 });
                        }

                        segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, 1);
                        
                        
                        if (_j > (sz + 1))
                            break;
                    }
                }
            }
            else // Not an end-point
            {
                if (bpts[contours[i][j].x - 1][contours[i][j].y - 1] == 2 || // branch point
                    bpts[contours[i][j].x - 1][contours[i][j].y - 1] == 3)   // overlapped points
                {
                    if (segment == nullptr)
                        segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, bpts[contours[i][j].x - 1][contours[i][j].y - 1]);
                    else
                    {
                        if (hasbranchpts(segment->pts.back(), { contours[i][j].x - 1, contours[i][j].y - 1 }, bpts, i, j))
                        {
                            delete segment;
                            segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, bpts[contours[i][j].x - 1][contours[i][j].y - 1]);
                        }
                        else
                        {
                            segment->setendpoint({ contours[i][j].x - 1, contours[i][j].y - 1 }, bpts[contours[i][j].x - 1][contours[i][j].y - 1]);

                            if (pruneroots && bpts[segment->pts[0].x][segment->pts[0].y] == 1 &&
                                segment->getLength() <= (dtptr[(contours[i][j].y - 1) * dist.cols + contours[i][j].x - 1] + rootPruningThreshold))
                            {
                                /*ptsdelete[segment->pts[0].x][segment->pts[0].y] = 1;

                                if (bpts[contours[i][j].x - 1][contours[i][j].y - 1] == 2)
                                    ptsdelete[contours[i][j].x - 1][contours[i][j].y - 1] = 1;
                                else
                                    ptsdelete[contours[i][j].x - 1][contours[i][j].y - 1]++;*/

                                segment->prunedelete = true;
                            }
                            
                            // The root segment is completed now.
                            // We will now check if the segment is 
                            // already added to the list of segments.
                            // If it is not present, then we add the
                            // new segment.
                            ispresent = 0;
                            for (auto& seg : segments[segment->key])
                            {
                                if ((*seg) == (*segment))
                                {
                                    ispresent = 1;
                                    break;
                                }
                            }

                            if (ispresent)
                                delete segment;
                            else
                            {
                                segments[segment->key].push_back(segment);

                                if (segment->pts.front().x != segment->pts.back().x || segment->pts.front().y != segment->pts.back().y)
                                {
                                    connIndices[segment->pts.front().x][segment->pts.front().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));
                                    connIndices[segment->pts.back().x][segment->pts.back().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));
                                }
                                else
                                    connIndices[segment->pts.front().x][segment->pts.front().y].push_back(pair<unsigned long long, int>(segment->key, static_cast<int>(segments[segment->key].size()) - 1));

                                /*if (bpts[segment->pts[0].x][segment->pts[0].y] != 1)
                                    segidx[segment->pts[0].x][segment->pts[0].y].push_back({ segment->key, segments[segment->key].size() - 1 });
                                segidx[contours[i][j].x - 1][contours[i][j].y - 1].push_back({ segment->key, segments[segment->key].size() - 1 });*/
                            }

                            segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 }, bpts[contours[i][j].x - 1][contours[i][j].y - 1]);
                            
                            if (_j > (sz + 1))
                            {
                                delete segment;
                                segment = nullptr;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // Continue if the first set of points on the contour are
                    // a continuation points.
                    if (segment != nullptr)
                    {
                        if (hasbranchpts(segment->pts.back(), { contours[i][j].x - 1, contours[i][j].y - 1 }, bpts, i, j))
                        {
                            delete segment;
                            segment = nullptr;
                            //segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        }
                        else
                        {
                            segment->pts.push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        }
                    }
                }
            }
        }
    }
    
    //window("new1", skeleton.rowRange(3729 - 50, 3729 + 50).colRange(3400 - 50, 3400 + 50));

    if (pruneroots)
    {
        int x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt;
        vector<pair<unsigned long long, int>> segidx;
        vector<Point> branchPts;
        bool pruningNeeded = false;

        uchar* skelptr = _skeleton.ptr<uchar>();
        
        do
        {
            branchPts.clear();
            for (auto &kv : segments)
            {
                for (int i = 0; i < kv.second.size(); i++)
                {
                    if (kv.second[i] == nullptr || (!kv.second[i]->prunedelete))
                        continue;

                    for (int j = 0; j < kv.second[i]->pts.size() - 1; j++)
                    {
                        nnextpt = j + 1;

                        currx = kv.second[i]->pts[j].x;
                        curry = kv.second[i]->pts[j].y;
                        nextx = kv.second[i]->pts[nnextpt].x;
                        nexty = kv.second[i]->pts[nnextpt].y;

                        if (nextx != currx)
                            xinc = abs(currx - nextx) / (nextx - currx);
                        else
                            xinc = 0;

                        if (nexty != curry)
                            yinc = abs(curry - nexty) / (nexty - curry);
                        else
                            yinc = 0;

                        for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
                            skelptr[y * _skeleton.cols + x] = 0;
                        skelptr[y * _skeleton.cols + x] = 0;
                    }

                    segidx.clear();
                    Point ptt;

                    switch (kv.second[i]->startpointtype)
                    {
                    case 1:
                        bpts[kv.second[i]->pts[0].x][kv.second[i]->pts[0].y] = 0;
                        ptsdelete[kv.second[i]->pts.back().x][kv.second[i]->pts.back().y]++;
                        ptt = kv.second[i]->pts.back();
                        branchPts.push_back({ kv.second[i]->pts.back().x, kv.second[i]->pts.back().y });
                        skelptr[kv.second[i]->pts.back().y * _skeleton.cols + kv.second[i]->pts.back().x] = 255;
                        connIndices[kv.second[i]->pts[0].x][kv.second[i]->pts[0].y].clear();
                        for (auto &p : connIndices[kv.second[i]->pts.back().x][kv.second[i]->pts.back().y])
                            if (p.first != kv.second[i]->key || p.second != i)
                                segidx.push_back(pair<unsigned long long, int>(p.first, p.second));
                        connIndices[kv.second[i]->pts.back().x][kv.second[i]->pts.back().y] = segidx;
                        break;
                    case 2:
                    case 3:
                        bpts[kv.second[i]->pts.back().x][kv.second[i]->pts.back().y] = 0;
                        ptsdelete[kv.second[i]->pts[0].x][kv.second[i]->pts[0].y]++;
                        ptt = kv.second[i]->pts[0];
                        branchPts.push_back({ kv.second[i]->pts[0].x, kv.second[i]->pts[0].y });
                        skelptr[kv.second[i]->pts[0].y * _skeleton.cols + kv.second[i]->pts[0].x] = 255;
                        connIndices[kv.second[i]->pts.back().x][kv.second[i]->pts.back().x].clear();
                        for (auto &p : connIndices[kv.second[i]->pts[0].x][kv.second[i]->pts[0].y])
                            if (p.first != kv.second[i]->key || p.second != i)
                                segidx.push_back(pair<unsigned long long, int>(p.first, p.second));
                        connIndices[kv.second[i]->pts[0].x][kv.second[i]->pts[0].y] = segidx;
                        break;
                    }

                    // To prune the segment.
                    delete kv.second[i];
                    kv.second[i] = nullptr;
                }
            }

            // After pruning is complete, some branch points may be connected
            // to only two root segments. In such a case, these points are no
            // longer branch points. Similarly, some branch points may be
            // connected to only one root segment as a result of root pruning.
            // These points should be identified as end points. All the branch
            // points where a pruned root is connected is stored in branchPts
            // list. So, we iterate through each point in branchPts, check the 
            // number of root segments connected. If the point is previously 
            // marked as branch point, but only two root segments are connected,
            // then the root segments should be merged. If only one root segment 
            // is connected, then the point should be marked as end point (root
            // tip). If three root segments are found, then the point must be 
            // an overlapped point and we just have to update the point to a 
            // branch point. After all the merging is complete, we should get 
            // all the points in the branchPts, that are marked as end points 
            // and check their connected root segments for pruning criterion. 
            // If the criterion is satisfied, these root segments should be 
            // marked for pruning in subsequent iteration.
            for (Point &pt : branchPts)
            {
                vector<pair<unsigned long long, int>> ci;
                int ibpts = bpts[pt.x][pt.y];

                for (int idx = 0; idx < connIndices[pt.x][pt.y].size(); idx++)
                {
                    if (segments[connIndices[pt.x][pt.y][idx].first][connIndices[pt.x][pt.y][idx].second] != nullptr)
                        ci.push_back(connIndices[pt.x][pt.y][idx]);
                }

                /*if (pt.x == 3400 && pt.y == 3729)
                {
                    window("new2", skeleton.rowRange(pt.y - 5, pt.y + 5).colRange(pt.x - 5, pt.x + 5));

                }*/

                if (ci.size() == 3 && bpts[pt.x][pt.y] == 3) // Overlapped point
                {
                    bpts[pt.x][pt.y] = 2;
                }
                else if (ci.size() == 2 && (bpts[pt.x][pt.y] == 2 || bpts[pt.x][pt.y] == 3)) // Branch point
                {
                    rootsegment *rs1 = segments[ci[0].first][ci[0].second];
                    rootsegment *rs2 = segments[ci[1].first][ci[1].second];

                    if (rs1 != rs2)
                    {
                        Point conEndPt1 = (rs1->pts[0] == pt) ? rs1->pts.back() : rs1->pts[0];
                        Point conEndPt2 = (rs2->pts[0] == pt) ? rs2->pts.back() : rs2->pts[0];

                        rs1->merge(rs2);
                        rs1->prunedelete = false;

                        segments[ci[0].first][ci[0].second] = nullptr;
                        segments[rs1->key].push_back(rs1);

                        vector<pair<unsigned long long, int>> sids;
                        for (auto& pair : connIndices[conEndPt2.x][conEndPt2.y])
                        {
                            if ((pair.first != ci[1].first || pair.second != ci[1].second) &&
                                (segments[pair.first][pair.second] != nullptr))
                                sids.push_back(pair);
                        }

                        sids.push_back(pair<unsigned long long, int>(rs1->key, static_cast<int>(segments[rs1->key].size()) - 1));
                        connIndices[conEndPt2.x][conEndPt2.y] = sids;

                        sids.clear();
                        for (auto& pair : connIndices[conEndPt1.x][conEndPt1.y])
                        {
                            if ((pair.first != ci[0].first || pair.second != ci[0].second) && 
                                (segments[pair.first][pair.second] != nullptr))
                                sids.push_back(pair);
                        }

                        sids.push_back(pair<unsigned long long, int>(rs1->key, static_cast<int>(segments[rs1->key].size()) - 1));
                        connIndices[conEndPt1.x][conEndPt1.y] = sids;

                        delete segments[ci[1].first][ci[1].second];
                        segments[ci[1].first][ci[1].second] = nullptr;

                        connIndices[pt.x][pt.y].clear();
                        bpts[pt.x][pt.y] = 0;
                    }
                    else
                        bpts[pt.x][pt.y] = 0;
                }
                else if (ci.size() == 1 && (bpts[pt.x][pt.y] == 2 || bpts[pt.x][pt.y] == 3))
                {
                    bpts[pt.x][pt.y] = 1;
                }
                else if (ci.size() == 0 && (bpts[pt.x][pt.y] == 2 || bpts[pt.x][pt.y] == 3))
                {
                    bpts[pt.x][pt.y] = 0;
                }
            }

            // Now that the pruning is complete, we now check if any of the merged 
            // segments fail the pruning criterion.
            pruningNeeded = false;
            for (Point &pt : branchPts)
            {
                if (bpts[pt.x][pt.y] == 1 && connIndices[pt.x][pt.y].size() == 1)
                {
                    rootsegment *rs = segments[connIndices[pt.x][pt.y][0].first][connIndices[pt.x][pt.y][0].second];
                    if (rs != nullptr)
                    {
                        Point pt = (rs->pts[0] == pt) ? rs->pts.back() : rs->pts[0];

                        if (rs->getLength() <= dtptr[pt.y * dist.cols + pt.x] + rootPruningThreshold)
                        {
                            rs->prunedelete = true;
                            pruningNeeded = true;
                        }
                    }
                }
            }
        } while (pruningNeeded == true);

        // After all the merging is complete, we should get 
        // all the points in the branchPts, that are marked as end points 
        // and check their connected root segments for pruning criterion. 
        // If the criterion is satisfied, these root segments should be 
        // marked for pruning in subsequent iteration.

        //for (auto &kv1 : ptsdelete)
        //{
        //    for (auto &kv2 : kv1.second)
        //    {
        //        if (kv2.second == 0)
        //            continue;

        //        //seg.clear();

        //        switch (kv2.second)
        //        {
        //        case 1:
        //            if (bpts[kv1.first][kv2.first] == 2)
        //            {
        //                bpts[kv1.first][kv2.first] = 0;

        //                //// Correct the segments
        //                //for (auto &kv3 : segidx[kv1.first][kv2.first])
        //                //{
        //                //    if (!segments[kv3.first][kv3.second]->prunedelete)
        //                //        seg.push_back(segments[kv3.first][kv3.second]);

        //                //    segment = new rootsegment(seg[0]->pts[0], );
        //                //}
        //            }
        //            else
        //                bpts[kv1.first][kv2.first] = 2;
        //            break;
        //        case 2:
        //            if (bpts[kv1.first][kv2.first] == 2)
        //            {
        //                bpts[kv1.first][kv2.first] = 1;

        //                // Add the new end points
        //                end.back().push_back({ kv1.first, kv2.first });
        //            }
        //            else
        //            {
        //                bpts[kv1.first][kv2.first] = 0;

        //                // Correct the segments

        //            }
        //            break;
        //        case 3:
        //            if (bpts[kv1.first][kv2.first] == 2)
        //                bpts[kv1.first][kv2.first] = 0;
        //            else
        //            {
        //                bpts[kv1.first][kv2.first] = 1;

        //                // Add the new end points
        //                end.back().push_back({ kv1.first, kv2.first });
        //            }
        //            break;
        //        case 4:
        //            bpts[kv1.first][kv2.first] = 0;
        //            break;
        //        }
        //    }
        //}
    }

    for (auto &kv1 : bpts)
    {
        for (auto &kv2 : kv1.second)
        {
            switch (kv2.second)
            {
            case 1: // End point
                endpts.push_back({kv1.first, kv2.first});
                break;
            case 2:
                branchpts.push_back({ kv1.first, kv2.first });
                break;
            case 3:
                overlappts.push_back({ kv1.first, kv2.first });
                break;
            }
        }
    }

    for (auto &kv : segments)
    {
        for (auto &seg : kv.second)
        {
            if (seg == nullptr)
                continue;

            rootsegments.push_back(seg->pts);
        }
    }

    //if (!pruneroots)
    //{
        //removeduplicates(contours, conmap, branch, branchpts);
        //removeduplicates(contours, conmap, end, endpts);
        //removeduplicates(contours, conmap, over, overlappts);
        //getrootsegmentlist(segments, rootsegments);
    //}
    //else
    //{
        //qInfo() << "1 end :" << end[0].size() << " endpts :" << endpts.size();
        //removeduplicateswithpruning(contours, conmap, branch, bpts, branchpts, 2);
        //qInfo() << "1.1 end :" << end[0].size() << " endpts :" << endpts.size();
        //removeduplicateswithpruning(contours, conmap, end, bpts, endpts, 1);
        //qInfo() << "1.2 end :" << end[0].size() << " endpts :" << endpts.size();
        //removeduplicateswithpruning(contours, conmap, over, bpts, overlappts, 3);
        //qInfo() << "1.3 end :" << end[0].size() << " endpts :" << endpts.size();
        //getrootsegmentlistwithpruning(segments, rootsegments);
        //qInfo() << "1.4 end :" << end[0].size() << " endpts :" << endpts.size();
        
        //qInfo() << "2 end :" << end[0].size() << " endpts :" << endpts.size();

    //}

    bpts.clear();
    for (auto kv : segments)
    {
        for (auto pt : kv.second)
            if (pt != nullptr)
                delete pt;
    }

    segments.clear();
}

Mat drawtopology(Mat segmented, Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts)
{
    Mat skeleton;
    /*Mat se = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(_skeleton, skeleton, se);*/
    skeleton = _skeleton;
    Mat skdiff = segmented - skeleton;
    Mat colorimage;
    vector<Mat> channels;

    channels.push_back(skdiff + ((skeleton / 255) * 28));
    channels.push_back(skdiff + ((skeleton / 255) * 26));
    channels.push_back(skdiff + ((skeleton / 255) * 228));

    merge(channels, colorimage);

    for (auto& pt : branchpts)
        circle(colorimage, Point(pt.x - 1, pt.y - 1), 0, Scalar(184, 126, 55), -1, LineTypes::LINE_8);
    for (auto& pt : endpts)
        circle(colorimage, Point(pt.x - 1, pt.y - 1), 0, Scalar(74, 175, 77), -1, LineTypes::LINE_8);
    for (auto& pt : overlappts)
        circle(colorimage, Point(pt.x - 1, pt.y - 1), 0, Scalar(163, 78, 152), -1, LineTypes::LINE_8);

    return colorimage;
}

Mat drawtopology2(Mat segmented, Scalar_<int> ranges, Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts)
{
    Mat skeleton;
    /*Mat se = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(_skeleton, skeleton, se);*/
    skeleton = _skeleton;
    Mat skdiff = segmented - skeleton;
    Mat colorimage;
    vector<Mat> channels;
    ListofLists<Point> contours;

    Mat skeleton2 = _skeleton.clone();
    hconcat(Mat::zeros(skeleton2.rows, 1, skeleton2.type()), skeleton2, skeleton2);
    hconcat(skeleton2, Mat::zeros(skeleton2.rows, 1, skeleton2.type()), skeleton2);

    vconcat(Mat::zeros(1, skeleton2.cols, skeleton2.type()), skeleton2, skeleton2);
    vconcat(skeleton2, Mat::zeros(1, skeleton2.cols, skeleton2.type()), skeleton2);

    findContours(skeleton2.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    modifycontours(skeleton2, contours);

    channels.push_back(skdiff + ((skeleton / 255) * 28));
    channels.push_back(skdiff + ((skeleton / 255) * 26));
    channels.push_back(skdiff + ((skeleton / 255) * 228));

    merge(channels, colorimage);
    
    for (int i = 0; i < contours.size(); i++)
    {
        for (auto& pt : contours[i])
            circle(colorimage, Point(pt.x - 1, pt.y - 1), 0, Scalar(0, 127, 255), -1, LineTypes::LINE_8);
    }
    for (auto& pt : branchpts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(184, 126, 55), -1, LineTypes::LINE_8);
    for (auto& pt : endpts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(74, 175, 77), -1, LineTypes::LINE_8);
    for (auto& pt : overlappts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(163, 78, 152), -1, LineTypes::LINE_8);

    return colorimage;
}

cv::Mat drawtopology_wholeroot(cv::Mat img, cv::Scalar_<int> ranges, std::vector<std::vector<int>> cc, std::vector<int> colorcodes, std::vector<std::vector<int>> colors)
{
    int c = ranges[2] - ranges[3] + 2; // img.cols + 2;
    cv::Mat m = img.rowRange(ranges[1], ranges[0]).colRange(ranges[3], ranges[2]).clone();

    for (int i = 0; i < cc.size(); i++)
    {
        for (int j = 0; j < cc[i].size(); j++)
        {
            circle(m, { ((cc[i][j] % c) - 1), ((cc[i][j] / c) - 1) }, 0, cv::Scalar(colors[colorcodes[i] - 1][2], colors[colorcodes[i] - 1][1], colors[colorcodes[i] - 1][0]));
        }
    }

    cv::Mat result = img.clone();
    m.copyTo(result.rowRange(ranges[1], ranges[0]).colRange(ranges[3], ranges[2]));

    return result;
}

Mat drawtopology_disconnectedroot(Mat segmented, cv::Mat dist, Scalar_<int> ranges, Mat _skeleton, PointList branchpts, PointList endpts, PointList overlappts, int width)
{
    Mat skeleton;
    Mat se = getStructuringElement(MORPH_RECT, Size(width, width));
    dilate(_skeleton, skeleton, se);
    //skeleton = _skeleton;
    Mat skdiff;
    
    skdiff = segmented - skeleton;
    Mat colorimage;
    vector<Mat> channels;
    ListofLists<Point> contours;

    /*Mat skeleton2 = _skeleton.clone();
    hconcat(Mat::zeros(skeleton2.rows, 1, skeleton2.type()), skeleton2, skeleton2);
    hconcat(skeleton2, Mat::zeros(skeleton2.rows, 1, skeleton2.type()), skeleton2);

    vconcat(Mat::zeros(1, skeleton2.cols, skeleton2.type()), skeleton2, skeleton2);
    vconcat(skeleton2, Mat::zeros(1, skeleton2.cols, skeleton2.type()), skeleton2);

    findContours(skeleton2.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    modifycontours(skeleton2, contours);*/

    if (!skeleton.empty())
    {
        channels.push_back(skdiff + ((skeleton / 255) * 28));
        channels.push_back(skdiff + ((skeleton / 255) * 26) + dist);
        channels.push_back(skdiff + ((skeleton / 255) * 228));
    }
    else
    {
        channels.push_back(skdiff);
        channels.push_back(skdiff + dist);
        channels.push_back(skdiff);
    }

    merge(channels, colorimage);

    /*for (int i = 0; i < contours.size(); i++)
    {
        for (auto& pt : contours[i])
            circle(colorimage, Point(pt.x - 1, pt.y - 1), 0, Scalar(0, 127, 255), -1, LineTypes::LINE_8);
    }*/
    for (auto& pt : branchpts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(184, 126, 55), width, LineTypes::LINE_8);
    for (auto& pt : endpts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(74, 175, 77), width, LineTypes::LINE_8);
    for (auto& pt : overlappts)
        circle(colorimage, Point(pt.x + ranges[3], pt.y + ranges[1]), 0, Scalar(163, 78, 152), width, LineTypes::LINE_8);

    return colorimage;
}

Mat drawmedialaxis(Mat dist, Mat skel, vector<double> rranges, int width)
{
    float *distptr = dist.ptr<float>();
    size_t rsize = rranges.size();
    
    Mat fresult = find(skel, FindType::Subscripts).first;
    int *fresultptr = fresult.ptr<int>();
    int k = 0, idx = 0;
    bool kfound = false;

    Mat result = Mat::zeros(skel.size(), CV_8UC3);
    
    /*vector<Vec<uchar, 3>> colorpalette = {
        { 227,206,166 },
        { 180,120,31 },
        { 138,223,178 },
        { 153,154,251 },
        { 111,191,253 },
        { 0,127,255 },
        { 214,178,202 },
        { 154,61,106 },
        { 153,255,255 },
        { 40,89,177 } };*/

    /*vector<vector<Scalar>> colorranges = {
        { { 28, 25, 215 } },
        { { 28, 25, 215 },{ 65, 150, 26 } },
        { { 28, 25, 215 },{ 191, 255, 255 },{ 65, 150, 26 } },
        { { 28, 25, 215 },{ 253, 174, 97 },{ 191, 255, 255 },{ 65, 150, 26 } },

        { { 28,25,215 },{ 97,174,253 },{ 191,255,255 },{ 106,217,166 },{ 65,150,26 } },
        { { 39,48,215 },{ 89,141,252 },{ 139,224,254 },{ 139,239,217 },{ 96,207,145 },{ 80,152,26 } },
        { { 39,48,215 },{ 89,141,252 },{ 139,224,254 },{ 191,255,255 },{ 139,239,217 },{ 96,207,145 },{ 80,152,26 } },
        { { 39,48,215 },{ 67,109,244 },{ 97,174,253 },{ 139,224,254 },{ 139,239,217 },{ 106,217,166 },{ 99,189,102 },{ 80,152,26 } },

        { { 39,48,215 },{ 67,109,244 },{ 97,174,253 },{ 139,224,254 },{ 191,255,255 },{ 139,239,217 },{ 106,217,166 },{ 99,189,102 },{ 80,152,26 } },
        { { 38,0,165 },{ 39,48,215 },{ 67,109,244 },{ 97,174,253 },{ 139,224,254 },{ 139,239,217 },{ 106,217,166 },{ 99,189,102 },{ 80,152,26 },{ 55,104,0 } },
        { { 38,0,165 },{ 39,48,215 },{ 67,109,244 },{ 97,174,253 },{ 139,224,254 },{ 191,255,255 },{ 139,239,217 },{ 106,217,166 },{ 99,189,102 },{ 80,152,26 },{ 55,104,0 } }
    };*/

    vector<Scalar> colorranges = {
        { 12, 5, 220 },{ 176, 101, 25 },{ 101, 178, 78 },{ 54, 167, 244 },{ 114, 46, 136 },{ 28, 96, 232 },{ 222, 175, 123 },{ 86, 240, 247 },{ 171, 224, 202 },{ 14, 25, 114 },{ 199, 137, 82 }
    };

    //vector<Scalar> colorpalette = colorranges[rsize];

    for (int i = 0; i < fresult.rows; i++)
    {
        idx = fresultptr[2 * i] * dist.cols + fresultptr[2 * i + 1];

        for (k = 0; k < rsize; k++)
            if (distptr[idx] <= rranges[k])
                break;
        
        circle(result, Point(fresultptr[2 * i + 1], fresultptr[2 * i]), 0, colorranges[k % colorranges.size()], width);
    }

    return result;
}
