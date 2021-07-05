/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: rootsegmentprop.cpp

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

// This file contains implementations of functions that extract
// phenotypic traits from root segments such as total root length
// as well as topological information such as first order and 
// second order roots, identification of main root, branching 
// frequency and average length before next branch.

#include "roottopology.h"
#include "rootsegmentprop.h"

using namespace std;
using namespace cv;

void getrootlength(Mat skeleton, ListofListsRef<Point> segments, PointList overlap, double &rootlen)
{
    rootlen = 0.0;
    Point diff;
    uchar *skel = skeleton.ptr<uchar>();
    double a = 0, b = 0;

    // Compute the sum of distances of root segments using 
    // euclidean distance metric.
    for (int i = 0; i < segments.size(); i++)
    {
        for (int j = 0; j < segments[i].size() - 1; j++)
        {
            diff = segments[i][j] - segments[i][j + 1];
            a = fabs(double(diff.x));
            b = fabs(double(diff.y));

            if (a == b)
                rootlen += a * CVUTIL_SQRT2;
            else if (a > b)
                rootlen += a;
            else
                rootlen += b;
            //rootlen += sqrt(diff.ddot(diff));
        }
    }

    for (auto &overlappt : overlap)
    {
        // To check if the overlappt is a diagonal or orthogonal
        if (skel[overlappt.y * skeleton.cols + overlappt.x + 1] > 0)
        {
            // Orthogonal
            rootlen++;
        }
        else
        {
            // Diagonal
            rootlen += CVUTIL_SQRT2;
            //rootlen += sqrt(2);
        }
    }
}

//double getrootlength(Mat skeleton)
//{
//    double sum = 0.0;
//    Mat branchpt, endpt;
//    Points branchpts, endpts, overlap;
//    ListofLists<Point> segments;
//
//    // Identify branch points and end points in the skeleton image.
//    getroottopology(skeleton, branchpts, endpts, overlap, segments);
//
//    // Compute the sum of distances of root segments using 
//    // euclidean distance metric.
//    getrootlength(skeleton, segments, overlap, sum);
//
//    for (int i = 0; i < segments.size(); i++)
//        segments[i].resize(0);
//
//    segments.resize(0);
//    overlap.resize(0);
//    endpts.resize(0);
//    branchpts.resize(0);
//
//    return sum;
//}

void getrootradii(Mat skeleton, Mat segment, Mat dist, ListofListsRef<Point> segments, ListofListsRef<double> radii, double& maxradius, int& maxradrowidx)
{
    int i, j, x, y, xinc, yinc, currx, curry, nextx, nexty, nnextpt, ptcount;
    size_t cisz;

    Mat dskeleton;
    skeleton.convertTo(dskeleton, CV_64FC1);
    dskeleton = dskeleton / 255.0;

    Mat dt;
    dist.convertTo(dt, CV_64FC1);
    dt = dt.mul(dskeleton);
    double *dtptr = dt.ptr<double>();

    maxradius = -1.0;
    maxradrowidx = INT_MAX;

    // We now get the root radii at every skeletal pixel and for each segment.
    // Since, each segment contains points that describe the shape of the segment
    // in the simplest way possible, some pixels on the root segment may not be 
    // present in the root segment. Hence, we traverse pixel-by-pixel in each 
    // root segment and get list of radii for all the pixels for that root 
    // segment. This lists of radii can further be used to perform statistics,
    // identify main root or even cluster the roots based on root radii.
    
    // First, to allocate sufficient memory.
    radii.resize(segments.size());
    for (i = 0; i < segments.size(); i++)
    {
        cisz = segments[i].size() - 1;
        
        for (j = 0, ptcount = 0; j < cisz; j++)
        {
            nnextpt = j + 1;

            currx = segments[i][j].x;
            curry = segments[i][j].y;
            nextx = segments[i][nnextpt].x;
            nexty = segments[i][nnextpt].y;

            ptcount += max(abs(curry - nexty), abs(currx - nextx));
        }

        radii[i].resize(ptcount + 1);
    }

    // Each root segment may start from a branch or an end point and
    // end at a branch or an end point. In case of an end point, the
    // radius is considered only once. But in case of branch point,
    // since it is connected to two other root segments, the radius 
    // at the branch point is considered thrice in total. Hence,
    // when calculating average root radius across plant root, we need
    // to subtract twice the root radius at every branch point, to get
    // a better estimate. But, this function just makes list of root
    // radii of all the pixels that are present in each segment.
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

            for (x = currx, y = curry; !(x == nextx && y == nexty); x += xinc, y += yinc)
            {
                radii[i][ptcount++] = dtptr[y * dt.cols + x];
                if (maxradius < dtptr[y * dt.cols + x])
                {
                    maxradius = dtptr[y * dt.cols + x];
                    maxradrowidx = y;
                }
                else if (maxradius == dtptr[y * dt.cols + x] && maxradrowidx > y)
                {
                    maxradrowidx = y;
                }
            }
        }

        radii[i][ptcount] = dtptr[segments[i][j].y * dt.cols + segments[i][j].x];

        if (maxradius < dtptr[segments[i][j].y * dt.cols + segments[i][j].x])
        {
            maxradius = dtptr[segments[i][j].y * dt.cols + segments[i][j].x];
            if (maxradrowidx > segments[i][j].y)
                maxradrowidx = segments[i][j].y;
        }
    }
}

void getrootradiiforsegments(ListofListsRef<double> radii, ValueList avgradii)
{
    double sum = 0.0;

    avgradii.resize(radii.size());

    for (int i = 0; i < radii.size(); i++)
    {
        sum = 0.0;

        for (int j = 0; j < radii[i].size(); j++)
            sum += radii[i][j];

        avgradii[i] = sum / radii[i].size();
    }
}

void gethistogramofrootradii(Mat skeleton, Mat segment, ValueList x, IndexList y)
{
    Mat dskeleton;
    int sum; 
    
    skeleton.convertTo(dskeleton, CV_64FC1);
    dskeleton = dskeleton / 255.0;

    Mat dt = bwdist(segment);
    dt.convertTo(dt, CV_64FC1);
    dt = dt.mul(dskeleton);

    pair<Mat, Mat> nonzeros = find(dt, FindType::IndicesAndValues);
    double *nzptr = nonzeros.second.ptr<double>();
    
    x = unique<double>(nonzeros.second);
    int xsz = static_cast<int>(x.size());
    y.resize(xsz);

    for (int i = 0; i < xsz; i++)
    {
        sum = 0;
        for (int j = 0; j < nonzeros.second.rows; j++)
            if (x[i] == nzptr[j])
                sum++;

        y[i] = sum;
    }
}

/// validatesegments () - Checks root segments for errors
/// Validation rules:
/// ----------------
/// 1. The internal points of root segments should not be shared with
/// other root segments.
/// 
/// 2. All the branch points should be used 3 times either as start points
/// or end points by the root segments.
///
/// 3. If a branch point is an overlapped point, it should be used 4
/// times. All overlapped points must be used 4 times.
///
/// 4. All end points must be used only once.
///
/// 5. If an image is reconstructed with the root segments, it must 
/// form the complete skeleton.
bool validatesegments(Mat skeleton, PointList branch, PointList end, PointList overlap, ListofListsRef<Point> rootsegments)
{
    // The internal points of root segments should not be shared with
    // other root segments.
    size_t nsize = rootsegments.size();

    vector<size_t> sizesdata;
    sizesdata.resize(nsize);
    size_t *sizes = sizesdata.data();

    for (size_t i = 0; i < rootsegments.size(); i++)
        sizes[i] = rootsegments[i].size();

    for (int i = 0; i < nsize; i++)
    {
        //cout << "i = " << i << endl;

        for (int j = 1; j < (sizes[i] - 1); j++)
        {
            for (int k = i + 1; k < nsize; k++)
            {
                if (k == i)
                    continue;

                for (int l = 1; l < (sizes[k] - 1); l++)
                {
                    if (rootsegments[i][j].x == rootsegments[k][l].x &&
                        rootsegments[i][j].y == rootsegments[k][l].y)
                    {
#ifdef DEBUG_MESSAGES
                        cout << "[" << __LINE__ << "]: x = " << rootsegments[i][j].x << " - y = " << rootsegments[i][j].y << endl;
#endif
                        return false;
                    }
                }
            }
        }
    }

    /* All the branch points should be used 3 times either as start points
    or end points by the root segments.

    If a branch point is an overlapped point, it should be used 4
    times. All overlapped points must be used 4 times.

    All end points must be used only once. */
    Dictionary<int, int> bids, eids, oids;
    Point pt;
    bids.reserve(branch.size());
    eids.reserve(end.size());
    oids.reserve(overlap.size());

    for (Point &p : branch)
        bids[p.x * 10000 + p.y] = -1;
    for (Point &p : end)
        eids[p.x * 10000 + p.y] = -1;
    for (Point &p : overlap)
        oids[p.x * 10000 + p.y] = -1;

    for (int i = 0; i < rootsegments.size(); i++)
    {
        pt = rootsegments[i][0];

        if (bids[pt.x * 10000 + pt.y] != 0)
        {
            if (bids[pt.x * 10000 + pt.y] < 0)
                bids[pt.x * 10000 + pt.y] = 1;
            else
                bids[pt.x * 10000 + pt.y]++;
        }

        if (eids[pt.x * 10000 + pt.y] != 0)
        {
            if (eids[pt.x * 10000 + pt.y] < 0)
                eids[pt.x * 10000 + pt.y] = 1;
            else
                eids[pt.x * 10000 + pt.y]++;
        }

        if (oids[pt.x * 10000 + pt.y] != 0)
        {
            if (oids[pt.x * 10000 + pt.y] < 0)
                oids[pt.x * 10000 + pt.y] = 1;
            else
                oids[pt.x * 10000 + pt.y]++;
        }

        pt = rootsegments[i][rootsegments[i].size() - 1];

        if (bids[pt.x * 10000 + pt.y] != 0)
        {
            if (bids[pt.x * 10000 + pt.y] < 0)
                bids[pt.x * 10000 + pt.y] = 1;
            else
                bids[pt.x * 10000 + pt.y]++;
        }

        if (eids[pt.x * 10000 + pt.y] != 0)
        {
            if (eids[pt.x * 10000 + pt.y] < 0)
                eids[pt.x * 10000 + pt.y] = 1;
            else
                eids[pt.x * 10000 + pt.y]++;
        }

        if (oids[pt.x * 10000 + pt.y] != 0)
        {
            if (oids[pt.x * 10000 + pt.y] < 0)
                oids[pt.x * 10000 + pt.y] = 1;
            else
                oids[pt.x * 10000 + pt.y]++;
        }
    }

    for (Point &p : branch)
    {
        if (bids[p.x * 10000 + p.y] != 3)
        {
            if (bids[p.x * 10000 + p.y] != 4 && bids[p.x * 10000 + p.y] != oids[p.x * 10000 + p.y])
            {
#ifdef DEBUG_MESSAGES
                cout << "[" << __LINE__ << "]: x = " << p.x << " - y = " << p.y << " bids = " << bids[p.x * 10000 + p.y] << " oids = " << oids[p.x * 10000 + p.y] << endl;
                //window("b1", skeleton.rowRange(p.y - 3, p.y + 4).colRange(p.x - 3, p.x + 4));
                //window("b2", skeleton.rowRange(p.y - 103, p.y + 104).colRange(p.x - 103, p.x + 104));
#endif
                return false;
            }
        }
    }
    for (Point &p : end)
    {
        if (eids[p.x * 10000 + p.y] != 1)
        {
#ifdef DEBUG_MESSAGES
            cout << "[" << __LINE__ << "]: x = " << p.x << " - y = " << p.y << endl;
#endif
            return false;
        }
    }
    for (Point &p : overlap)
    {
        if (oids[p.x * 10000 + p.y] != 4)
        {
#ifdef DEBUG_MESSAGES
            cout << "[" << __LINE__ << "]: x = " << p.x << " - y = " << p.y << endl;
#endif
            return false;
        }
    }

    // If an image is reconstructed with the root segments, it must 
    // form the complete skeleton.
    Mat rec = Mat::zeros(skeleton.size(), CV_8UC1);
    unsigned char *recptr = rec.ptr<unsigned char>();
    int currx, curry, nextx, nexty, xinc, yinc, x, y;
    const unsigned char white = static_cast<unsigned char>(255);

    for (int i = 0; i < rootsegments.size(); i++)
    {
        for (int j = 0; j < rootsegments[i].size() - 1; j++)
        {
            currx = rootsegments[i][j].x - 1;
            curry = rootsegments[i][j].y - 1;
            nextx = rootsegments[i][j + 1].x - 1;
            nexty = rootsegments[i][j + 1].y - 1;

            if (!((currx == nextx) && (curry == nexty)))
            {
                if (currx == nextx)
                {
                    yinc = abs(curry - nexty) / (nexty - curry);

                    for (y = curry; y != nexty; y += yinc)
                        recptr[y * rec.cols + currx] = white;
                }
                else if (curry == nexty)
                {
                    xinc = abs(currx - nextx) / (nextx - currx);

                    for (x = currx; x != nextx; x += xinc)
                        recptr[curry * rec.cols + x] = white;
                }
                else
                {
                    xinc = abs(currx - nextx) / (nextx - currx);
                    yinc = abs(curry - nexty) / (nexty - curry);

                    for (x = currx, y = curry; x != nextx; x += xinc, y += yinc)
                        recptr[y * rec.cols + x] = white;
                }
            }

            recptr[nexty * rec.cols + nextx] = white;
        }
    }

    Mat diff = skeleton - rec;

    if (countNonZero(diff) > 0)
    {
        /*Mat mm;
        vector<Mat> ch;

        ch.push_back(Mat::zeros(rec.size(), CV_8UC1));
        ch.push_back(rec);
        ch.push_back(skeleton);

        merge(ch, mm);
        imwrite("C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\bb.png", mm);
        window("b", mm.rowRange(5001 - 10, 5001 + 11).colRange(5250 - 10, 5250 + 11));*/
        return false;
    }

    // We return true if all the conditions mentioned above are true.
    return true;
}
