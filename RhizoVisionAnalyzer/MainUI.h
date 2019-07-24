/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: MainUI.h

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

#pragma once

#ifndef MAINUI_H
#define MAINUI_H

#include <string>
#include <filesystem>

#include <QtWidgets/QDialog>
#include <QtCore/QQueue>
#include <QtCore/QElapsedTimer>

#include <cvutil.h>

#include "FeatureExtractorThread.h"

#define RHIZOVISION_ANALYZER_VERSION "1.0.3"

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QCheckBox;
class QProgressBar;
class QVBoxLayout;
class QStatusBar;
class QSpinBox;
class QDoubleSpinBox;
class QSlider;
QT_END_NAMESPACE

class MainUI : public QDialog
{
    Q_OBJECT;

    
public:
    typedef void(*processfunc)(feature_config* config, std::vector<double>& featurevals);
    //typedef void(*processfunc)(std::string filename, std::string savefolder, std::vector<double>& featurevals, bool invertimage, bool saveseg, bool saveimages, bool pixelconv, double conversion, double rootlo, double roothi);

    MainUI();
    void setprocessfunction(processfunc pfunc);

public slots:
    void UpdateProgress(QString filename, int fileno);
    void workfinished();

protected:
    void timerEvent(QTimerEvent * ev);
    void closeEvent(QCloseEvent * ev);
    void keyPressEvent(QKeyEvent *ev);

private:
    //void createMenu();
    QGroupBox *createHorizontalGroupBox(QString grouptext, QPushButton **btn, QString btntext, QLineEdit **edt, QLabel **lab);
    void updateLabels();
    void browse();
    bool CanStartProcessing();
    void createOptionsGroupBox();
    void createRangeControls();
    void updateRangeControls();
    void enablepixelconversion();
    void createProgressGroupBox();
    void connectSignalstoSlots();
    void startclicked();
    void pauseclicked();
    void enableinvertimage();
    void enablesavesegmented();
    void enablesaveprocessed();
    void updateconversion();
    bool realstart();
    void realstop();
    void realpause();
    void realresume();
    void about();

    void setTextWhatsThis();
    void getfilelist(std::string imgpath);
    
    //QMenuBar *menuBar;
    QGroupBox *imglocGroupBox;
    QGroupBox *imgsavGroupBox;
    QGroupBox *imgoptGroupBox;
    QGroupBox *imgproGroupBox;

    QPushButton *imgbtn, *savbtn, *processbtn;
    QLineEdit *imgline, *savline;
    QLabel *imglabel, *savlabel;

    QVBoxLayout *vlayout;
    QCheckBox *invertimage, *savesegmented, *saveprocessed, *convertunits;
    QLabel *pixelsperinch;
    QLineEdit *pixeledt;
    double currconv = 0.0;

    QLabel *ranges, *fdrange, *mdrange, *cdrange;
    QLineEdit *feditmin, *meditmin, *ceditmin;
    QDoubleSpinBox *fspbox, *mspbox;

    QLabel *currfilename, *timeremaining, *timeelapsed;
    QLineEdit *currfile, *timeel, *timeest;
    QProgressBar *progress;

    QPushButton *aboutbtn, *start, *pause, *quit;
    QStatusBar *sbar;

    QLabel* thresh;
    QSlider *thresholdslider;
    QSpinBox *thresholdbox;

    // Timer functionality
    QElapsedTimer timer;
    int timerid = -1;
    qint64 elapsed = 0;
    qint64 esttime = 0;
    int nfiles = 0;
    bool estcheck = false;

    bool IsValidImageLoc = false, IsValidSaveLoc = false;
    std::string imgloc, savloc;
    std::vector<std::string> filelist;
    
    // Threading functionality
    FeatureExtractorThread *workthread;
    
    const int statusbartimeout = 0;

    enum class CurrentMode { Stopped, Started, Paused, Resumed };
    CurrentMode mode;
};

#endif // DIALOG_H

