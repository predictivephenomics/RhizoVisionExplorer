/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: FeatureExtractorThread.h

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

#ifndef FEATUREEXTRACTORTHREAD_H
#define FEATUREEXTRACTORTHREAD_H

#include <cvutil.h>

#include <QtWidgets/QDialog>
#include <QtCore/QQueue>
#include <QtCore/QThread>

//QT_BEGIN_NAMESPACE
//class QThread;
//QT_END_NAMESPACE

typedef struct
{
    std::string filename;
    std::string savefolder;
    bool invertimage;
    bool saveseg;
    bool saveimages;
    bool pixelconv;
    double conversion;
    double rootlo;
    double roothi;
    int threshold;
} feature_config;

class FeatureExtractorThread : public QThread
{
    Q_OBJECT;

    typedef void(*processfunc)(feature_config* config, std::vector<double>& featurevals);
    //typedef void(*processfunc)(std::string filename, std::string savefolder, std::vector<double>& featurevals, bool invertimage, bool saveseg, bool saveimages, bool pixelconv, double conversion, double rootlo, double roothi);

    // Location information
    std::vector<std::string> filelist;
    int fileidx;
    std::string saveloc;

    // Feature extractor function to be called
    processfunc pfunc;
    
    // Options from main GUI
    bool invertimage = false;
    bool savesegmented = false;
    bool saveprocessed = false;
    bool convertphysical = false;
    double pixel2mm = -1;
    double rootlo = 5, roothi = 10;
    int threshold = 128;

    bool workfinished = false;
public:
    FeatureExtractorThread(QObject *parent = 0);
    void reset();
    void setfilelist(std::vector<std::string> flist);
    void setsavelocation(std::string location);
    void setprocessfunction(processfunc pfunction);
    void setinvertimage(bool invert);
    void setsavesegmented(bool savsegmented);
    void setsaveprocessed(bool savprocessed);
    void setconvertphysical(bool convphysical, double conv);
    void setdiameterrange(double lo, double hi);
    void setthreshold(int k);

    bool isfinished();

    void run();

signals:
    void ReportProgress(QString filename, int fileno);
};


#endif
