/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: roottopology.cpp

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

#include "roottopology.h"

using namespace std;
using namespace cv;

class rootsegment
{
    bool endptset = false;

    void removeunnecesary();
    
    unsigned long long generatekey(int x1, int y1, int x2, int y2);
public:
    rootsegment(Point startpoint);
    ~rootsegment();

    void setendpoint(Point endpoint);

    bool isendpointset();

    bool operator ==(rootsegment r);

    Points getpoints();
    
    //vector<int> points;
    unsigned long long key;
    Points pts;
};

rootsegment::rootsegment(Point startpoint)
{
    pts.reserve(100);
    pts.push_back(startpoint);
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

void rootsegment::setendpoint(Point endpoint)
{
    int lo, hi;

    endptset = true;
    pts.push_back(endpoint);
    removeunnecesary();
    
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

Points rootsegment::getpoints()
{
    return pts;
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

void removeduplicates(ListofListsRef<Point> contours, IndexList conmap, ListofListsRef<Point> connpts, PointList pts)
{
    int i = 0, nsum = 0;
    int nconnpts = connpts.size();
    
    for (i = 0, nsum = 0; i < nconnpts; i++)
    {
        unordered_set<Point> s;
        for (Point& j : connpts[i])
            s.insert(j);
        connpts[i].assign(s.begin(), s.end());
        nsum += connpts[i].size();
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

void getrootsegmentlist(DictionaryRef<int, vector<rootsegment *>> segments, ListofListsRef<Point> rootsegments)
{
    int j = 0, nsum = 0;
    int nsegments = segments.size();

    // for (i = 0, nsum = 0; i < nsegments; i++)
    for (auto &s : segments)
    {
        nsum += s.second.size();
    }

    rootsegments.resize(nsum);

    nsum = 0;
    for (auto &s : segments)
    {
        for (j = 0; j < s.second.size(); j++)
            rootsegments[nsum++] = s.second[j]->getpoints();
    }
}

void modifycontours(Mat skeleton, ListofListsRef<Point> contours)
{
    int i, j, nsum, x, y, nnextpt, currx, curry, nextx, nexty, xinc, yinc, npoints;
    unsigned char *skel = skeleton.ptr<unsigned char>();
    int ncols = skeleton.cols;
    int N1, N2, N3, N4, N5, N6, N7, N8;
    Points branchpts;
    Indices insertat;
    size_t csz = contours.size(), cisz;
    unsigned long long key;

    branchpts.reserve(2000);
    insertat.reserve(2000);
    
    for (i = 0, npoints = 0; i < csz; i++)
    {
        npoints += contours[i].size();
    }

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
            
            if (currx < nextx)
                key = ((currx * 10000 + curry) * 10000 + nextx) * 10000 + nexty;
            else if (currx > nextx)
                key = ((nextx * 10000 + nexty) * 10000 + currx) * 10000 + curry;
            else // If x coordinates are equal, we base our logic on y coordinates.
            {
                if (curry < nexty)
                    key = ((currx * 10000 + curry) * 10000 + nextx) * 10000 + nexty;
                else
                    key = ((nextx * 10000 + nexty) * 10000 + currx) * 10000 + curry;
            }

            // If the current point to next point is a
            // horizontal line.
            if (curry == nexty)
            {
                if (currx < nextx)
                {
                    for (x = currx + 1; x < nextx; x++)
                    {
                        N3 = int(skel[(curry - 1) * ncols + x] > 0);
                        N7 = int(skel[(curry + 1) * ncols + x] > 0);
                        
                        if (N3 == 1 || N7 == 1) // T found
                        {
                            branchpts.push_back(Point(x, curry));
                            insertat.push_back(j + 1);
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
                            branchpts.push_back(Point(x, curry));
                            insertat.push_back(j + 1);
                        }
                    }
                }
            }
            else // If the contour comes to horizontal T from the perpendicular side.
            {
                if ((currx + 1) == nextx && (curry + 1) == nexty)
                {
                    if (skel[(curry) * ncols + currx + 1] > 0)
                    {
                        branchpts.push_back(Point(currx + 1, curry));
                        insertat.push_back(j + 1);
                    }
                    else if (skel[(curry + 1) * ncols + currx] > 0)
                    {
                        branchpts.push_back(Point(currx, curry + 1));
                        insertat.push_back(j + 1);
                    }
                }
                else if ((currx + 1) == nextx && (curry - 1) == nexty)
                {
                    if (skel[(curry)* ncols + currx + 1] > 0)
                    {
                        branchpts.push_back(Point(currx + 1, curry));
                        insertat.push_back(j + 1);
                    }
                    else if (skel[(curry - 1) * ncols + currx] > 0)
                    {
                        branchpts.push_back(Point(currx, curry - 1));
                        insertat.push_back(j + 1);
                    }
                }
                else if ((currx - 1) == nextx && (curry + 1) == nexty)
                {
                    if (skel[(curry)* ncols + currx - 1] > 0)
                    {
                        branchpts.push_back(Point(currx - 1, curry));
                        insertat.push_back(j + 1);
                    }
                    else if (skel[(curry + 1) * ncols + currx] > 0)
                    {
                        branchpts.push_back(Point(currx, curry + 1));
                        insertat.push_back(j + 1);
                    }
                }
                else if ((currx - 1) == nextx && (curry - 1) == nexty)
                {
                    if (skel[(curry)* ncols + currx - 1] > 0)
                    {
                        branchpts.push_back(Point(currx - 1, curry));
                        insertat.push_back(j + 1);
                    }
                    else if (skel[(curry - 1) * ncols + currx] > 0)
                    {
                        branchpts.push_back(Point(currx, curry - 1));
                        insertat.push_back(j + 1);
                    }
                }
                else if (abs(currx - nextx) == abs(curry - nexty)) // To analyze diagonal paths
                {
                    xinc = abs(currx - nextx) / (nextx - currx);
                    yinc = abs(curry - nexty) / (nexty - curry);

                    for (x = currx, y = curry; x != nextx; x += xinc, y += yinc)
                    {
                        if (skel[(y) * ncols + x + xinc] > 0)
                        {
                            if (x != currx)
                            {
                                branchpts.push_back(Point(x, y));
                                insertat.push_back(j + 1);
                            }

                            branchpts.push_back(Point(x + xinc, y));
                            insertat.push_back(j + 1);

                            if ((x + xinc) != nextx)
                            {
                                branchpts.push_back(Point(x + xinc, y + yinc));
                                insertat.push_back(j + 1);
                            }
                        }
                        else if (skel[(y + yinc) * ncols + x] > 0)
                        {
                            if (x != currx)
                            {
                                branchpts.push_back(Point(x, y));
                                insertat.push_back(j + 1);
                            }

                            branchpts.push_back(Point(x, y + yinc));
                            insertat.push_back(j + 1);

                            if ((x + xinc) != nextx)
                            {
                                branchpts.push_back(Point(x + xinc, y + yinc));
                                insertat.push_back(j + 1);
                            }
                        }
                        else if (skel[(y + yinc) * ncols + x - xinc] > 0 && skel[(y + yinc) * ncols + x] == 0 && skel[(y)* ncols + x - xinc] == 0)
                        {
                            if (x != currx)
                            {
                                branchpts.push_back(Point(x, y));
                                insertat.push_back(j + 1);
                            }
                        }
                        else if (skel[(y - yinc) * ncols + x + xinc] > 0 && skel[(y - yinc) * ncols + x] == 0 && skel[(y) * ncols + x + xinc] == 0)
                        {
                            if (x != currx)
                            {
                                branchpts.push_back(Point(x, y));
                                insertat.push_back(j + 1);
                            }
                        }
                    }
                }
            }
        }

        // Reserve the space for additional pixels.
        contours[i].reserve(contours[i].size() + insertat.size());

        // Insert points at the positions specified.
        auto itr = contours[i].begin();
        for (j = insertat.size() - 1; j >= 0; j--)
            contours[i].insert(itr + insertat[j], branchpts[j]);
        
        // Clear the lists
        branchpts.clear();
        insertat.clear();
    }
}

void getroottopology(Mat _skeleton,
    PointList branchpts,
    PointList endpts,
    PointList overlappts,
    ListofListsRef<Point> rootsegments)
{
    int i, j, _j, temp;
    int N1, N2, N3, N4, N5, N6, N7, N8, nsum, nsum1, nsum2, ispresent, skncomp;
    rootsegment *segment = nullptr;
    ListofLists<Point> branch, end, over;
    Dictionary<int, vector<rootsegment *>> segments;
    ListofLists<Point> contours;
    Indices ptsizes, conmap;
    Mat sklabels;
    size_t sz;

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

    for (i = 0; i < contours.size(); i++)
    {
        conmap[i] = skco[contours[i][0].y * ncols + contours[i][0].x] - 1;
        ptsizes[conmap[i]] += contours[i].size();
    }

    for (i = 0; i < skncomp; i++)
    {
        branch[i].reserve(ptsizes[i]);
        end[i].reserve(ptsizes[i]);
        over[i].reserve(ptsizes[i]);
        //segments[i].reserve(ptsizes[i]);
    }

    // We generate all the topological information using 
    // the indices first. Later we convert these indices
    // Points.
    for (i = 0; i < contours.size(); i++)
    {
        sz = contours[i].size();

        /*cout << "i = " << i << " sz = " << sz << " conmap[i] = " << conmap[i] << endl;
        cout << "branch[conmap[i]].size() = " << branch[conmap[i]].size() << " end[conmap[i]].size() = " << end[conmap[i]].size() << " over[conmap[i]].size() = " << over[conmap[i]].size() << endl;*/
        //cout << "i = " << i << " sz = " << sz << endl;

        //toc();
        if (sz == 1)
            continue;

        for (_j = 0; ; _j++)
        {
            j = (_j % sz);

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
                if (segment == nullptr)
                    segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 });
                else
                {
                    segment->setendpoint({ contours[i][j].x - 1, contours[i][j].y - 1 });
                    
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
                        segments[segment->key].push_back(segment);
                    
                    segment = nullptr;

                    if (_j > (sz + 1))
                        break;
                }
            }
            else // Not an end-point
            {
                nsum1 = int(N1 > N2) + int(N2 > N3) + int(N3 > N4) + int(N4 > N5) +
                    int(N5 > N6) + int(N6 > N7) + int(N7 > N8) + int(N8 > N1);

                nsum2 = int(N1 < N2) + int(N2 < N3) + int(N3 < N4) + int(N4 < N5) +
                    int(N5 < N6) + int(N6 < N7) + int(N7 < N8) + int(N8 < N1);

                //if (nsum1 == 2 && nsum2 == 2) // continuation point
                //{
                //    // Continue if the first set of points on the contour are
                //    // a continuation points.
                //    if (segment != nullptr)
                //        segment->pts.push_back(contours[i][j]);
                //}
                
                //if ((nsum1 == 3 && nsum2 == 3) || (nsum >= 4 && (nsum1 != 4 || nsum2 != 4)) || // branch point
                //    (nsum1 == 4 && nsum2 == 4)) // overlapping-point
                if ((nsum1 == 3 && nsum2 == 3) || 
                    (nsum >= 4 && nsum < 7 && ((N1 + N2 + N3) == 3 || (N3 + N4 + N5) == 3 || (N5 + N6 + N7) == 3 || (N7 + N8 + N1) == 3) && 
                        (!(nsum == 6 && ((N1 + N5) == 0 || (N3 + N7) == 0 || 
                        (N1 + N2) == 0 || (N1 + N8) == 0 || (N3 + N2) == 0 || (N7 + N8) == 0 || 
                            (N3 + N4) == 0 || (N7 + N6) == 0 || (N5 + N4) == 0 || (N5 + N6) == 0)))) || // branch point
                        (nsum == 4 && ((N1 + N3 + N5 + N7) == 4 || (N2 + N4 + N6 + N8) == 4))) // overlapping-point
                {
                    //if (nsum1 == 4 && nsum2 == 4)
                    if (nsum == 4 && ((N1 + N3 + N5 + N7) == 4 || (N2 + N4 + N6 + N8) == 4))
                        over[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });

                    branch[conmap[i]].push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                    if (segment == nullptr)
                        segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 });
                    else
                    {
                        segment->setendpoint({ contours[i][j].x - 1, contours[i][j].y - 1 });
                        
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
                            segments[segment->key].push_back(segment);
                            
                        segment = new rootsegment({ contours[i][j].x - 1, contours[i][j].y - 1 });

                        if (_j > (sz + 1))
                        {
                            delete segment;
                            segment = nullptr;
                            break;
                        }
                    }
                }
                else
                {
                    // Continue if the first set of points on the contour are
                    // a continuation points.
                    if (segment != nullptr)
                        segment->pts.push_back({ contours[i][j].x - 1, contours[i][j].y - 1 });
                }
            }
        }

        // The segment may not be completed if the
        // contour is circular (starts and ends at 
        // the same point). Hence, we check if the
        // segment is completed before we proceed 
        // towards analyzing next contour.

        //if ((segment != nullptr) && (!segment->isendpointset()))
        //{
        //    //for (j = 0; j < contours[i].size(); j++)
        //    segment->setendpoint(0);
        //    segment->updatepointlist(contours);

        //    // The root segment is completed now.
        //    // We will now check if the segment is 
        //    // already added to the list of segments.
        //    // If it is not present, then we add the
        //    // new segment.
        //    ispresent = 0;
        //    for (auto& seg : segments[conmap[i]])
        //    {
        //        if (seg == (*segment))
        //        {
        //            ispresent = 1;
        //            break;
        //        }
        //    }

        //    if (ispresent)
        //        delete segment;
        //    else
        //        segments[conmap[i]].push_back(move(*segment));

        //    segment = nullptr;
        //}
    }

    removeduplicates(contours, conmap, branch, branchpts);
    removeduplicates(contours, conmap, end, endpts);
    removeduplicates(contours, conmap, over, overlappts);
    getrootsegmentlist(segments, rootsegments);

    for (auto kv : segments)
    {
        for (auto pt : kv.second)
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
