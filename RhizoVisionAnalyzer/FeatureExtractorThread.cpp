/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: FeatureExtractorThread.cpp

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

#include "MainUI.h"
#include "FeatureExtractorThread.h"

using namespace std;

FeatureExtractorThread::FeatureExtractorThread(QObject *parent) : QThread(parent)
{
    moveToThread(this);
    reset();
}

void FeatureExtractorThread::reset()
{
    fileidx = 0;
}

void FeatureExtractorThread::setfilelist(vector<string> flist)
{
    filelist = flist;
}

void FeatureExtractorThread::setsavelocation(string location)
{
    saveloc = location;
}

void FeatureExtractorThread::setinvertimage(bool invert)
{
    invertimage = invert;
}

void FeatureExtractorThread::setsavesegmented(bool savsegmented)
{
    savesegmented = savsegmented;
}

void FeatureExtractorThread::setsaveprocessed(bool savprocessed)
{
    saveprocessed = savprocessed;
}

void FeatureExtractorThread::setconvertphysical(bool convphysical, double conv)
{
    convertphysical = convphysical;
    pixel2mm = conv;
}

void FeatureExtractorThread::setdiameterrange(double lo, double hi)
{
    rootlo = lo;
    roothi = hi;
}

void FeatureExtractorThread::setprocessfunction(processfunc pfunction)
{
    pfunc = pfunction;
}

void FeatureExtractorThread::setthreshold(int k)
{
    threshold = k;
}

// We wanted to implement our own "finished"
// functionality than using the isFinished()
// because the thread we implemented is 
// pausable. The function below returns false
// when the thread is stopped due to a pause
// action.
bool FeatureExtractorThread::isfinished()
{
    return workfinished;
}

void FeatureExtractorThread::run()
{
    vector<double> features;
    workfinished = false;
    feature_config config;

    config.savefolder = saveloc;
    config.invertimage = invertimage;
    config.saveimages = saveprocessed;
    config.saveseg = savesegmented;
    config.conversion = pixel2mm;
    config.pixelconv = convertphysical;
    config.rootlo = rootlo;
    config.roothi = roothi;
    config.threshold = threshold;

    for (; fileidx < filelist.size(); fileidx++)
    {
        if (isInterruptionRequested())
            return;
        
        // emit the signal for the count label
        emit ReportProgress(tr(filelist[fileidx].c_str()), fileidx);

        features.clear();
        config.filename = filelist[fileidx];

        pfunc(&config, features);
        
        ofstream f;
        f.open(saveloc + "features.csv", ios::out | ios::app);

        f << experimental::filesystem::path(filelist[fileidx]).filename();

        for (auto feat : features)
        {
            if (!(isnan(feat) || isinf(feat)))
                f << "," << feat;
            else
                f << "," << "NA";
        }

        f << endl;
        f.close();
    }

    workfinished = true;
}

