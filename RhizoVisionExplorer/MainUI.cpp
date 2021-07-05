/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: MainUI.cpp

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

#include "MainUI.h"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarCategoryAxis>

using namespace std;
using namespace cv;

CheckEnabler::CheckEnabler(QString text, QString lbltext, bool integervalue, QWidget *parent) : QWidget(parent)
{
    cb = new QCheckBox(text);

    lbl = new QLabel(lbltext);
    edt = new QLineEdit();

    if (lbltext.length() == 0)
        lbl->setFixedWidth(0);
    else
        lbl->setFixedWidth(130);

    if (integervalue)
        edt->setValidator(new QIntValidator(1, INT_MAX));
    else
        edt->setValidator(new QDoubleValidator(1.0, DBL_MAX, 5));
    //edt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    //edt->setMinimumWidth(40);
    edt->setFixedWidth(60);

    lbl->setDisabled(true);
    edt->setDisabled(true);

    widgets.push_back(lbl);
    widgets.push_back(edt);

    layout = new QHBoxLayout();
    layout->addSpacing(20);
    layout->addWidget(lbl);
    layout->addWidget(edt, 1);// , Qt::AlignLeft);
    //layout->addStretch();
    
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(cb);
    vlayout->addLayout(layout);
    vlayout->setContentsMargins(0, 0, 0, 0);

    connect(cb, &QCheckBox::stateChanged, [&]() 
    {
        if (cb->checkState() == Qt::CheckState::Checked)
        {
            //lbl->setEnabled(1);
            //edt->setEnabled(1);
            for (auto& w : widgets)
                w->setEnabled(1);
        }
        else
        {
            //lbl->setDisabled(1);
            //edt->setDisabled(1);
            for (auto& w : widgets)
                w->setDisabled(1);
        }

        emit stateChanged();
    });

    connect(edt, &QLineEdit::textChanged, [&]() 
    {
        emit textChanged();
    });

    this->setLayout(vlayout);
}

StringCheckEnabler::StringCheckEnabler(QString text, QString lbltext, QWidget *parent) : QWidget(parent)
{
    cb = new QCheckBox(text);

    lbl = new QLabel(lbltext);
    edt = new QLineEdit();
    //edt->setValidator(new QDoubleValidator(1.0, DBL_MAX, 5));
    edt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    edt->setMinimumWidth(40);

    lbl->setDisabled(true);
    edt->setDisabled(true);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addSpacing(20);
    layout->addWidget(lbl);
    layout->addWidget(edt);// , 1, Qt::AlignLeft);
                           //layout->addStretch();

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(cb);
    vlayout->addLayout(layout);
    vlayout->setContentsMargins(0, 0, 0, 0);

    connect(cb, &QCheckBox::stateChanged, [&]()
    {
        if (cb->checkState() == Qt::CheckState::Checked)
        {
            lbl->setEnabled(1);
            edt->setEnabled(1);
        }
        else
        {
            lbl->setDisabled(1);
            edt->setDisabled(1);
        }

        emit stateChanged();
    });

    connect(edt, &QLineEdit::textChanged, [&]()
    {
        emit textChanged();
    });

    this->setLayout(vlayout);
}

MainUI::MainUI()
{
    config = make_shared<feature_config>();
    config->ui = this;

    //this->setWindowIcon(QIcon(":/icons/rvanalyzernew"));
    pluginui = new QToolBox();

    createRangeControls();
    createOptions();
    connectSignalstoSlots();
}

//MainUI::MainUI(shared_ptr<feature_config> fconfig)
//{
//    config = fconfig;
//    //this->setWindowIcon(QIcon(":/icons/rvanalyzernew"));
//    pluginui = new QWidget();
//
//    createRangeControls();
//    createOptions();
//    connectSignalstoSlots();
//}

//MainUI::~MainUI()
//{
//    pluginui->deleteLater();
//    //config.reset();
//}

void MainUI::setTextWhatsThis()
{
    //imglocGroupBox->setWhatsThis("Location of the images containing JPG (*.jpg, *.jpeg, *.jpe, *.jp2), PNG (*.png), BMP (*.bmp, *.dib) or TIFF (*.tiff, *.tif) image files.");
    //imgsavGroupBox->setWhatsThis("Output location to store extracted features and optionally store thresholded and processed images.");

    thresh->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");
    thresholdslider->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");
    thresholdbox->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");

    //invertimage->setWhatsThis("When this option is checked, the pixels greater than the above threshold level are classified as plant root pixels. This option may be used to extract features from root images with darker backgrounds.");
    //savesegmented->setWhatsThis("Saves thresholded binary images in the output location. The file format will be PNG and the file name is the input file name appended with '_seg'.");
    //saveprocessed->setWhatsThis("Saves processed feature images in the output location. The file format will be PNG and the file name is the input file name appended with '_features'.");
    
    convertunits->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");
    //pixelsperinch->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");
    //pixeledt->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");

    ranges->setWhatsThis("Root diameter ranges are used to classify the roots based on the root diameter at every location of the plant root image. The program then makes a normalized histogram and stores the bins as fine, medium and coarse diameter frequencies.");
}

void MainUI::connectSignalstoSlots()
{
    //connect(invertimage, &QCheckBox::stateChanged, this, &MainUI::enableinvertimage);
    //connect(savesegmented, &QCheckBox::stateChanged, this, &MainUI::enablesavesegmented);
    //connect(saveprocessed, &QCheckBox::stateChanged, this, &MainUI::enablesaveprocessed);
    connect(convertunits, &CheckEnabler::stateChanged, this, &MainUI::enablepixelconversion);
    connect(convertunits, &CheckEnabler::textChanged, this, &MainUI::updateconversion);

    connect(linesmooth, &CheckEnabler::stateChanged, [&]() 
    {
        if (linesmooth->checked())
            config->smooththresh = linesmooth->getValue();
        else
            config->smooththresh = 0.0;
        config->enablesmooththresh = linesmooth->checked();
        //updatesegmentedoutput();
    });

    connect(linesmooth, &CheckEnabler::textChanged, [&]()
    {
        config->smooththresh = linesmooth->getValue();
        //updatesegmentedoutput();
    });

    connect(filterbknoisy, &CheckEnabler::stateChanged, [&]()
    {
        if (filterbknoisy->checked())
            config->maxcompsizebk = filterbknoisy->getValue();
        else
            config->maxcompsizebk = 0.0;
        config->filterbknoise = filterbknoisy->checked();
        //updatesegmentedoutput();
    });

    connect(filterbknoisy, &CheckEnabler::textChanged, [&]()
    {
        config->maxcompsizebk = filterbknoisy->getValue();
        //updatesegmentedoutput();
    });

    connect(filterfgnoisy, &CheckEnabler::stateChanged, [&]()
    {
        if (filterfgnoisy->checked())
            config->maxcompsizefg = filterfgnoisy->getValue();
        else
            config->maxcompsizefg = 0.0;
        config->filterfgnoise = filterfgnoisy->checked();
        //updatesegmentedoutput();
    });

    connect(filterfgnoisy, &CheckEnabler::textChanged, [&]()
    {
        config->maxcompsizefg = filterfgnoisy->getValue();
        //updatesegmentedoutput();
    });

    connect(showMedialAxis, &CheckEnabler::stateChanged, [&]()
    {
        if (showMedialAxis->checked())
        {
            config->medialaxiswidth = int(showMedialAxis->getValue());
            showMedialAxisDiameter->setEnabled(true);
            config->showMedialAxisDiameter = showMedialAxisDiameter->isChecked();
            //config->showMedialAxisDiameter = true;
        }
        else
        {
            config->medialaxiswidth = 0;
            showMedialAxisDiameter->setEnabled(false);
            config->showMedialAxisDiameter = false;
            //config->showMedialAxisDiameter = false;
        }
        config->showMedialAxis = showMedialAxis->checked();
    });

    connect(showMedialAxis, &CheckEnabler::textChanged, [&]()
    {
        config->medialaxiswidth = int(showMedialAxis->getValue());
    });

    connect(showContours, &CheckEnabler::stateChanged, [&]()
    {
        if (showContours->checked())
            config->contourwidth = showContours->getValue();
        else
            config->contourwidth = 0.0;
        config->showContours = showContours->checked();
    });

    connect(showContours, &CheckEnabler::textChanged, [&]()
    {
        config->contourwidth = showContours->getValue();
    });

    connect(roottype, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int idx)
    {
        //this->roottypeval = idx;
        config->roottype = idx;

        // Disable noise filtering when the root type is whole root.
        filterbknoisy->setEnabled((idx != 0) || ((idx == 0) && (!keepLargest->isChecked())));
        filterfgnoisy->setEnabled((idx != 0) || ((idx == 0) && (!keepLargest->isChecked())));
        keepLargest->setEnabled(idx == 0);

        if (idx == 0)
        {
            showConvexHull->setEnabled(true);
            showHoles->setEnabled(true);
            showContours->setEnabled(true);

            config->showConvexHull = showConvexHull->isChecked();
            config->showHoles = showHoles->isChecked();
            config->showContours = showHoles->isChecked();
            
            config->keepLargest = keepLargest->isChecked();
        }
        else
        {
            showConvexHull->setEnabled(false);
            showHoles->setEnabled(false);
            showContours->setEnabled(false);

            config->showConvexHull = false;
            config->showHoles = false;
            config->showContours = false;

            config->keepLargest = false;
        }

        if (filterbknoisy->isEnabled())
        {
            if (filterbknoisy->checked())
                config->maxcompsizebk = filterbknoisy->getValue();
            else
                config->maxcompsizebk = 0.0;
            config->filterbknoise = filterbknoisy->checked();
        }
        else
        {
            config->maxcompsizebk = 0.0;
            config->filterbknoise = false;
        }

        if (filterfgnoisy->isEnabled())
        {
            if (filterfgnoisy->checked())
                config->maxcompsizefg = filterfgnoisy->getValue();
            else
                config->maxcompsizefg = 0.0;
            config->filterfgnoise = filterfgnoisy->checked();
        }
        else
        {
            config->maxcompsizefg = 0.0;
            config->filterfgnoise = false;
        }
    });

    connect(keepLargest, &QCheckBox::stateChanged, [&]()
    {
        int idx = roottype->currentIndex();
        config->keepLargest = keepLargest->isChecked();

        filterbknoisy->setEnabled((idx != 0) || ((idx == 0) && (!keepLargest->isChecked())));
        filterfgnoisy->setEnabled((idx != 0) || ((idx == 0) && (!keepLargest->isChecked())));

        if (filterbknoisy->isEnabled())
        {
            if (filterbknoisy->checked())
                config->maxcompsizebk = filterbknoisy->getValue();
            else
                config->maxcompsizebk = 0.0;
            config->filterbknoise = filterbknoisy->checked();
        }
        else
        {
            config->maxcompsizebk = 0.0;
            config->filterbknoise = false;
        }

        if (filterfgnoisy->isEnabled())
        {
            if (filterfgnoisy->checked())
                config->maxcompsizefg = filterfgnoisy->getValue();
            else
                config->maxcompsizefg = 0.0;
            config->filterfgnoise = filterfgnoisy->checked();
        }
        else
        {
            config->maxcompsizefg = 0.0;
            config->filterfgnoise = false;
        }
    });

    connect(showConvexHull, &QCheckBox::stateChanged, [&](int k)
    {
        config->showConvexHull = showConvexHull->isChecked();
    });

    connect(showHoles, &QCheckBox::stateChanged, [&](int k)
    {
        config->showHoles = showHoles->isChecked();
    });

    connect(showDistMap, &QCheckBox::stateChanged, [&](int k)
    {
        config->showDistMap = showDistMap->isChecked();
    });

    connect(showMedialAxisDiameter, &QCheckBox::stateChanged, [&](int k)
    {
        config->showMedialAxisDiameter = showMedialAxisDiameter->isChecked();
    });

    connect(enableRootPruning, &CheckEnabler::stateChanged, [&]()
    {
        if (enableRootPruning->checked())
            config->rootPruningThreshold = enableRootPruning->getValue();
        else
            config->rootPruningThreshold = 0.0;
        config->enableRootPruning = enableRootPruning->checked();
    });

    connect(enableRootPruning, &CheckEnabler::textChanged, [&]()
    {
        config->rootPruningThreshold = enableRootPruning->getValue();
    });

    /*connect(pixelspermm, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int idx)
    {
        config->pixelspermm = idx;
    });*/

    connect(dispoutput, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int k)
    {
        if (config->displayOutputIndex != k && dispoutputupdating == false)
        {
            config->displayOutputIndex = k;

            if (hostWindow == nullptr)
                return;

            if (!executing)
                hostWindow->setDisplayOutputIndex(k);
            /*Mat m;

            switch (k)
            {
            case 0:
                m = hostWindow->getLoadedImage();
                break;
            case 1:
            case 2:
                m = hostWindow->getOutputImage();
                break;
            }

            if (!m.empty())
                emit updateVisualOutput(m);*/
        }
    });

    connect(thresholdslider, &QSlider::valueChanged, [&](int k)
    {
        if (thresholdbox->value() != k)
        {
            thresholdbox->setValue(k);
            config->threshold = k;

            thresholdbar->setValue(k);
            //updatesegmentedoutput();
        }
    });

    connect(thresholdbar, &QProgressBar::valueChanged, thresholdbar, static_cast<void (QProgressBar::*)()>(&QProgressBar::repaint));

    connect(thresholdbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int k)
    {
        if (thresholdslider->value() != k)
        {
            thresholdslider->setValue(k);
            config->threshold = k;

            thresholdbar->setValue(k);
            //updatesegmentedoutput();
        }
    });

    for (int i = 0; i < spboxes.size(); i++)
    {
        connect(spboxes[i], static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](double k) 
        {
            spboxvaluechanged(static_cast<QDoubleSpinBox *>(sender()), k);
        });
    }

    connect(addrangerow, &QPushButton::clicked, [&]() { MainUI::addrangerowclicked(); });
    connect(removerangerow, &QPushButton::clicked, this, &MainUI::removerangerowclicked);
    connect(pbpreview, &QPushButton::clicked, this, &MainUI::updatesegmentedoutput);

    connect(pluginui, &QToolBox::currentChanged, [&](int idx)
    {
        pluginui->setItemIcon(0, rightarrow);
        pluginui->setItemIcon(1, rightarrow);
        pluginui->setItemIcon(2, rightarrow);

        pluginui->setItemIcon(idx, downarrow);
    });
}

void MainUI::setprocessfunction(processfunc pfunc)
{
    //workthread->setprocessfunction(pfunc);
    this->pfunc = pfunc;
}

void MainUI::setHostWindow(cvutil::cvutilWindow *wnd)
{
    hostWindow = wnd;

    QToolBar *tbar = new QToolBar();

    /*QHBoxLayout *rtype = new QHBoxLayout;
    rtype->addWidget(lbdispoutput);
    rtype->addWidget(dispoutput);*/
    tbar->addWidget(lbdispoutput);
    tbar->addWidget(dispoutput);
    hostWindow->addToolBar(tbar);

    hostWindow->setAboutText(
        "<b>RhizoVision Explorer</b> (version " RHIZOVISION_EXPLORER_VERSION ")<br/>"
        "<b>Copyright (C) 2021, Anand Seethepalli and Larry York<br/>"
        "Copyright(C) 2020, Courtesy of Noble Research Institute, LLC</b><br/><br/>"

        "This program extracts phenotypic traits from plant root images acquired "
        "from flatbed scanners or digital cameras by performing image processing "
        "operations. Additionally, the program can output segmented images or feature "
        "images overlaid with trait descriptors. All operations may be performed in "
        "batch mode on directories of images once the user has selected appropriate "
        "analysis options. <br/><br/>"

        "RhizoVision Explorer is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by the Free "
        "Software Foundation, either version 3 of the License, or (at your option) any "
        "later version. <br/><br/>"

        "RhizoVision Explorer is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General "
        "Public License for more details. <br/><br/>"

        "You should have received a copy of the GNU General Public License along "
        R"(with RhizoVision Explorer; see the file COPYING.  If not, see <a href = "https://www.gnu.org/licenses/">https://www.gnu.org/licenses/</a>.<br/><br/>)"

        "Please send any comments/suggestions/improvements for the program to the authors:<br/>"
        "<pre>Anand Seethepalli<br/>"
        "email: <br/>anand_seethepalli@yahoo.co.in<br/><br/>"
        "Larry York<br/>"
        "email: <br/>larry.york@gmail.com<br/><br/></pre>",
        QPixmap(":/icons/RVElogoclearback.png"), QIcon(":/icons/RVElogoclearback.png"));
}

void MainUI::createOptions()
{
    QWidget *page = new QWidget();
    //QHBoxLayout *pc = new QHBoxLayout(page);
    QVBoxLayout *mainvl = new QVBoxLayout(page);
    mainvl->setContentsMargins(5, 5, 5, 3);

    lbroottype = new QLabel("Analysis mode");
    roottype = new QComboBox();
    roottype->insertItem(0, "Whole root");
    roottype->insertItem(1, "Broken roots");
    
    roottype->setCurrentIndex(config->roottype);

    lbdispoutput = new QLabel("Display output :  ");
    dispoutput = new QComboBox();
    dispoutput->insertItem(0, "Input image");
    //dispoutput->insertItem(1, "Segmented image");
    //dispoutput->insertItem(2, "Feature image");
    
    dispoutput->setCurrentIndex(config->displayOutputIndex);
    dispoutput->setFixedWidth(150);

    enableRootPruning = new CheckEnabler("Enable root pruning", "Root pruning threshold", true);
    enableRootPruning->setChecked(config->enableRootPruning);
    enableRootPruning->setValue(config->rootPruningThreshold);

    pixelspermm = new QComboBox();
    pixelspermm->insertItem(0, "DPI");
    pixelspermm->insertItem(1, "Pixels per mm");
    pixelspermm->setFixedWidth(90);
    pixelspermm->setDisabled(1);
    pixelspermm->setCurrentIndex(config->pixelspermm);

    //invertimage = new QCheckBox("Invert images before processing");
    //invertimage->setChecked(config->invertimage);
    
    thresh = new QLabel("Image thresholding level");
    thresholdslider = new QSlider(Qt::Orientation::Horizontal);
    thresholdslider->setMinimum(0);
    thresholdslider->setMaximum(255);
    //thresholdslider->setFixedHeight(20);
    thresholdslider->setContentsMargins(0, 0, 0, 0);
    thresholdslider->setValue(config->threshold);
    thresholdslider->setStyleSheet(
        "QSlider::groove:horizontal { border: 1px solid #999999; background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 0, stop : 0 #000000, stop:1 #ffffff); margin: 0px 0; } "
        "QSlider::handle:horizontal { height: 10px; width: 7px; background: #ffa100; margin: 0; /* expand outside the groove */ } "
    );

    thresholdbar = new QProgressBar();
    thresholdbar->setTextVisible(false);
    thresholdbar->setMinimum(0);
    thresholdbar->setMaximum(255);
    thresholdbar->setFixedHeight(7);
    thresholdbar->setContentsMargins(0, 0, 0, 0);
    thresholdbar->setValue(config->threshold);
    thresholdbar->setStyleSheet(
        "QProgressBar:horizontal{ border: 1px solid gray; background: white; } "
        "QProgressBar::chunk:horizontal{ background: #000000 } "
    );

    thresholdbox = new QSpinBox();
    thresholdbox->setMinimum(0);
    thresholdbox->setMaximum(255);
    thresholdbox->setValue(config->threshold);
    thresholdbox->setFixedWidth(70);

    pbpreview = new QPushButton("Preview segmented image");

    //savesegmented = new QCheckBox("Save segmented images");
    //saveprocessed = new QCheckBox("Save processed images");
    convertunits = new CheckEnabler("Convert pixels to physical units", "");
    
    convertunits->setChecked(config->pixelconv);
    convertunits->setValue(config->conversion);
    //savesegmented->setChecked(config->saveseg);
    //saveprocessed->setChecked(config->saveimages);
    convertunits->addToLayout(pixelspermm);

    linesmooth = new CheckEnabler("Enable edge smoothing", "Edge smoothing threshold :");
    linesmooth->setChecked(config->enablesmooththresh);
    linesmooth->setValue(config->smooththresh);
    
    keepLargest = new QCheckBox("Keep largest component");
    keepLargest->setChecked(config->keepLargest);

    filterbknoisy = new CheckEnabler("Filter non-root objects", "Maximum size (pixels) :");
    filterbknoisy->setChecked(config->filterbknoise);
    filterbknoisy->setValue(config->maxcompsizebk);
    filterbknoisy->setEnabled(config->roottype == 1);

    filterfgnoisy = new CheckEnabler("Fill holes in root objects", "Maximum size (pixels) :");
    filterfgnoisy->setChecked(config->filterfgnoise);
    filterfgnoisy->setValue(config->maxcompsizefg);
    filterfgnoisy->setEnabled(config->roottype == 1);

    keepLargest->setEnabled(config->roottype == 0); 
    filterbknoisy->setEnabled((config->roottype != 0) || ((config->roottype == 0) && (!keepLargest->isChecked())));
    filterfgnoisy->setEnabled((config->roottype != 0) || ((config->roottype == 0) && (!keepLargest->isChecked())));
    
    QHBoxLayout *hlayout = new QHBoxLayout;
    
    QHBoxLayout *rtype = new QHBoxLayout;
    rtype->addWidget(lbroottype);
    rtype->addWidget(roottype);
    mainvl->addLayout(rtype);
    mainvl->addWidget(convertunits);
    mainvl->addWidget(thresh);

    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(thresholdslider);
    vl->addWidget(thresholdbar);
    vl->setSpacing(3);

    hlayout->addLayout(vl);
    hlayout->addWidget(thresholdbox);
    mainvl->addLayout(hlayout);
    //mainvl->addWidget(invertimage);

    mainvl->addWidget(keepLargest);
    mainvl->addWidget(filterbknoisy);
    mainvl->addWidget(filterfgnoisy);
    mainvl->addWidget(linesmooth);
    mainvl->addWidget(pbpreview);
    mainvl->addStretch();
    //pc->addLayout(mainvl);

    QPixmap arr = pluginui->style()->standardPixmap(QStyle::StandardPixmap::SP_TitleBarShadeButton);
    QMatrix rm;
    rm.rotate(90);
    rightarrow = QIcon(arr.transformed(rm));
    rm.rotate(-135);
    downarrow = pluginui->style()->standardPixmap(QStyle::StandardPixmap::SP_TitleBarUnshadeButton).transformed(rm);
    pluginui->addItem(page, downarrow, tr("Image pre-processing"));

    page = new QWidget();
    //mainvl = new QVBoxLayout(page);
    //vlayout->addWidget(savesegmented);
    //vlayout->addWidget(saveprocessed);
    vlayout = new QVBoxLayout(page);
    vlayout->setContentsMargins(5, 5, 5, 3);

    vlayout->addWidget(enableRootPruning);
    //vlayout->addWidget(convertunits);
    
    vlayout->addWidget(ranges);
    QLabel *lbl = nullptr;

    for (int i = 0; i < dranges.size(); i++)
    {
        hlayout = new QHBoxLayout;

        hlayout->addWidget(dranges[i]);
        hlayout->addWidget(editmins[i]);
        
        if (i != (dranges.size() - 1))
        {
            lbl = new QLabel("to");
            lbl->setFixedWidth(10);
            hlayout->addWidget(lbl);
            hlayout->addWidget(spboxes[i]);
        }
        else
        {
            lbl = new QLabel("above.");
            lbl->setFixedWidth(40);
            hlayout->addWidget(lbl, 0, Qt::AlignLeft);
        }

        vlayout->addLayout(hlayout);
    }

    hlayout = new QHBoxLayout;
    hlayout->addWidget(addrangerow, 0, Qt::AlignLeft);
    hlayout->addWidget(removerangerow, 0, Qt::AlignRight);
    vlayout->addLayout(hlayout);
    vlayout->addStretch();
    pluginui->addItem(page, rightarrow, tr("Feature extraction"));

    //vlayout->addStretch();
    setTextWhatsThis();
    
    page = new QWidget();
    mainvl = new QVBoxLayout(page);
    mainvl->setContentsMargins(5,5,5,5);

    showConvexHull = new QCheckBox("Show convex hull");
    showHoles = new QCheckBox("Show holes");
    showDistMap = new QCheckBox("Show distance map");
    showMedialAxis = new CheckEnabler("Show medial axis", "Medial axis width", true);
    showContours = new CheckEnabler("Show root contour", "Contour width", true);
    showMedialAxisDiameter = new QCheckBox("Color medial axis based on diameter range");

    showConvexHull->setEnabled(config->roottype == 0);
    showHoles->setEnabled(config->roottype == 0);
    showContours->setEnabled(config->roottype == 0);

    showConvexHull->setChecked(config->showConvexHull);
    showHoles->setChecked(config->showHoles);
    showDistMap->setChecked(config->showDistMap);
    showMedialAxis->setChecked(config->showMedialAxis);
    showMedialAxis->setValue(config->medialaxiswidth);
    showContours->setChecked(config->showContours);
    showContours->setValue(config->contourwidth);
    showMedialAxisDiameter->setChecked(config->showMedialAxisDiameter);

    mainvl->addWidget(showConvexHull);
    mainvl->addWidget(showHoles);
    mainvl->addWidget(showDistMap);
    mainvl->addWidget(showMedialAxis);
    mainvl->addWidget(showMedialAxisDiameter);
    mainvl->addWidget(showContours);

    /*rtype = new QHBoxLayout;
    rtype->addWidget(lbdispoutput);
    rtype->addWidget(dispoutput);
    mainvl->addLayout(rtype);*/
    mainvl->addStretch();
    pluginui->addItem(page, rightarrow, tr("Output display"));

    pluginuiarea = new QScrollArea();
    /*mainvl = new QVBoxLayout();
    QWidget *w = new QWidget();
    w->setLayout(vlayout);*/
    pluginuiarea->setWidget(pluginui);
    pluginuiarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pluginuiarea->setWidgetResizable(true);

    /*QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addWidget(pluginuiarea);
    pluginui->setLayout(vl);*/

    QRectF rec = pluginuiarea->geometry();
    pluginuiarea->setGeometry(rec.x(), rec.y(), 100, rec.height());

    pluginuiarea->setMinimumWidth(100);
}

void MainUI::updateRangeControls()
{
    vector<double> val;
    
    bool prevmode = ranges->text() == "Root diameter ranges (in mm)";

    if (convertunits->checked())
    {
        ranges->setText("Root diameter ranges (in mm)");

        //if (!prevmode) // This function is triggered by check box
        //{
        //    for (int boxid = 0; boxid < spboxes.size(); boxid++)
        //        val.push_back(spboxes[boxid]->value() / convertunits->getValue());
        //}
        //else // This function is triggered by change in conversion units
        //{
        //    for (int boxid = 0; boxid < spboxes.size(); boxid++)
        //        val.push_back((spboxes[boxid]->value() * config->prevconversion) / convertunits->getValue());
        //}

        //for (int boxid = 0; boxid < spboxes.size(); boxid++)
        //{
        //    spboxes[boxid]->setMaximum(100);
        //    spboxes[boxid]->setSingleStep(1.0);
        //    spboxes[boxid]->setValue(val[boxid]);
        //}
    }
    else
    {
        ranges->setText("Root diameter ranges (in pixel units)");

        //if (prevmode) // This function is triggered by check box
        //{
        //    for (int boxid = 0; boxid < spboxes.size(); boxid++)
        //        val.push_back(round(spboxes[boxid]->value() * convertunits->getValue()) + 0.0);
        //}
        //
        //for (int boxid = 0; boxid < spboxes.size(); boxid++)
        //{
        //    spboxes[boxid]->setMaximum(5000);
        //    spboxes[boxid]->setSingleStep(1.0);
        //    spboxes[boxid]->setValue(val[boxid]);
        //}
    }
}

void MainUI::createRangeControls()
{
    // Initialize
    ranges = new QLabel("Root diameter ranges (in pixel units)");
    /*fdrange = new QLabel("Fine roots:");
    mdrange = new QLabel("Medium roots:");
    cdrange = new QLabel("Coarse roots:");*/

    for (int i = 0; i <= config->dranges.size(); i++)
    {
        dranges.push_back(new QLabel("Range #" + QString::number(i + 1) + ":"));
        dranges[i]->setFixedWidth(70);

        editmins.push_back(new QLineEdit());
        if (i == 0)
            editmins[i]->setText("0");
        else
            editmins[i]->setText(QString::number(config->dranges[i - 1], 'g'));
        editmins[i]->setReadOnly(true);
        QPalette readOnlyPalette = editmins[i]->palette();
        QColor mainWindowBgColor = pluginui->palette().color(QPalette::Window);
        readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
        editmins[i]->setPalette(readOnlyPalette);
        editmins[i]->setFixedWidth(50);

        if (i != config->dranges.size())
        {
            spboxes.push_back(new QDoubleSpinBox());
            if (i == 0)
                spboxes[i]->setMinimum(0.0);
            else
                spboxes[i]->setMinimum(config->dranges[i - 1]);
            
            spboxes[i]->setValue(config->dranges[i]);
            spboxes[i]->setSingleStep(1.0);
            spboxes[i]->setMaximum(5000.0);
        }
    }

    addrangerow = new QPushButton();
    addrangerow->setIcon(QIcon(":/icons/zoomin"));
    removerangerow = new QPushButton();
    removerangerow->setIcon(QIcon(":/icons/zoomout"));
}

void MainUI::addrangerowclicked(double lastdrange)
{
    if (spboxes.size() == 10)
        return;

    double ldrange = lastdrange;

    if (ldrange == -1.0)
    {
        if (dranges.size() > 1)
            //config->dranges.push_back(config->dranges[config->dranges.size() - 1] + 1.0);
            ldrange = spboxes[spboxes.size() - 1]->value() + 1.0;
        else
            //config->dranges.push_back(1.0);
            ldrange = 1.0;
    }
    //else
    //    config->dranges.push_back(lastdrange);

    dranges.insert(dranges.end() - 1, new QLabel("Range #" + QString::number(dranges.size()) + ":"));
    dranges[dranges.size() - 2]->setFixedWidth(70);
    dranges[dranges.size() - 1]->setText("Range #" + QString::number(dranges.size()) + ":");

    editmins.insert(editmins.end() - 1, new QLineEdit());
    if(spboxes.size() == 0)
        editmins[editmins.size() - 2]->setText(QString::number(0.0, 'g'));
    else
        //editmins[editmins.size() - 2]->setText(QString::number(config->dranges[config->dranges.size() - 2], 'g'));
        editmins[editmins.size() - 2]->setText(QString::number(spboxes[spboxes.size() - 1]->value(), 'g'));

    editmins[editmins.size() - 2]->setReadOnly(true);
    QPalette readOnlyPalette = editmins[editmins.size() - 2]->palette();
    QColor mainWindowBgColor = pluginui->palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    editmins[editmins.size() - 2]->setPalette(readOnlyPalette);
    editmins[editmins.size() - 2]->setFixedWidth(50);

    //editmins[editmins.size() - 1]->setText(QString::number(config->dranges[config->dranges.size() - 1], 'g'));
    editmins[editmins.size() - 1]->setText(QString::number(ldrange, 'g'));

    spboxes.push_back(new QDoubleSpinBox());
    if (dranges.size() == 2)
        spboxes[0]->setMinimum(0.0);
    else
        //spboxes[spboxes.size() - 1]->setMinimum(config->dranges[config->dranges.size() - 2]);
        spboxes[spboxes.size() - 1]->setMinimum(spboxes[spboxes.size() - 2]->value());

    //spboxes[spboxes.size() - 1]->setValue(config->dranges[config->dranges.size() - 1]);
    spboxes[spboxes.size() - 1]->setValue(ldrange);
    spboxes[spboxes.size() - 1]->setSingleStep(1.0);
    spboxes[spboxes.size() - 1]->setMaximum(5000.0);
    
    connect(spboxes[spboxes.size() - 1], static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [&](double k)
    {
        spboxvaluechanged(static_cast<QDoubleSpinBox *>(sender()), k);
    });

    QHBoxLayout * hlayout = new QHBoxLayout;

    hlayout->addWidget(dranges[dranges.size() - 2]);
    hlayout->addWidget(editmins[editmins.size() - 2]);

    QLabel *lbl = new QLabel("to");
    lbl->setFixedWidth(10);
    hlayout->addWidget(lbl);
    hlayout->addWidget(spboxes[spboxes.size() - 1]);

    if (spboxes.size() == 10)
        addrangerow->setEnabled(false);
    else
        addrangerow->setEnabled(true);
    removerangerow->setEnabled(true);

    vlayout->insertLayout(vlayout->count() - 3, hlayout);
}

void MainUI::removerangerowclicked()
{
    if (dranges.size() == 1)
        return;

    QLayoutItem *item = vlayout->takeAt(vlayout->count() - 4);
    QLayout *lay = item->layout();

    QLayoutItem *witem;
    while ((witem = lay->takeAt(0)) != 0)
    {
        delete witem->widget();
        delete witem;
    }
    
    delete item;
    
    dranges.erase(dranges.end() - 2);
    editmins.erase(editmins.end() - 2);
    spboxes.pop_back();
    //config->dranges.pop_back();

    dranges[dranges.size() - 1]->setText("Range #" + QString::number(dranges.size()) + ":");

    if (dranges.size() == 1)
        editmins[0]->setText("0");
    else
        //editmins[editmins.size() - 1]->setText(QString::number(config->dranges[config->dranges.size() - 1], 'g'));
        editmins[editmins.size() - 1]->setText(QString::number(spboxes[spboxes.size() - 1]->value(), 'g'));

    if (dranges.size() == 1)
        removerangerow->setEnabled(false);
    else
        removerangerow->setEnabled(true);
    addrangerow->setEnabled(true);

    vlayout->update();
}

void MainUI::updatesegmentedoutput()
{
    if (hostWindow != nullptr)
    {
        RoiManager *mgr = RoiManager::GetInstance();
        int roicount = mgr->getROICount();
        
        Mat s;
        double pixel2mm = 1.0;

        if (convertunits->checked())
        {
            if (pixelspermm->currentIndex() == 0) // For DPI conversion
                pixel2mm /= (convertunits->getValue() / 25.4);
            else
                pixel2mm /= convertunits->getValue();
        }

        Mat m = hostWindow->getLoadedImage();
        if (m.empty())
            return;

        if (roicount > 0)
        {
            auto rois = mgr->getROIs();
            Mat segs = Mat::zeros(m.size(), CV_8UC1);
            
            for (int i = 0; i < roicount; i++)
            {
                auto rect = rois[i]->rect();
                Mat ip = m(Rect(rect.x(), rect.y(), rect.width(), rect.height()));

                //if (config->invertimage)
                if (hostWindow->colorInversionEnabled())
                    ip = Scalar::all(255) - ip;
                
                s = getSingleChannel(ip < config->threshold);

                if (config->roottype == 0 && config->keepLargest == true)
                    s = getLargestConnectedComponent(s);
                else
                {
                    if (config->filterbknoise)
                        s = getImageFromComponents(s.size(), getConnectedComponents(s, (config->maxcompsizebk / pixel2mm) / pixel2mm));
                    if (config->filterfgnoise)
                        s = 255 - getImageFromComponents(s.size(), getConnectedComponents(255 - s, (config->maxcompsizefg / pixel2mm) / pixel2mm));
                }

                if (config->enablesmooththresh && config->smooththresh > 0.0)
                    s = linesim(s, LineSimplificationType::DouglasPeucker, config->smooththresh);

                //if (config->invertimage)
                //    s = 255 - s;

                Mat op = segs(Rect(rect.x(), rect.y(), rect.width(), rect.height()));
                s.copyTo(op);
            }

            s = segs;
        }
        else
        {
            //if (config->invertimage)
            if (hostWindow->colorInversionEnabled())
                m = Scalar::all(255) - m;
            
            s = getSingleChannel(m < config->threshold);

            //s = getSingleChannel(m < config->threshold);
            if (config->roottype == 0 && config->keepLargest == true)
                s = getLargestConnectedComponent(s);
            else
            {
                if (config->filterbknoise)
                    s = getImageFromComponents(s.size(), getConnectedComponents(s, (config->maxcompsizebk / pixel2mm) / pixel2mm));
                if (config->filterfgnoise)
                    s = 255 - getImageFromComponents(s.size(), getConnectedComponents(255 - s, (config->maxcompsizefg / pixel2mm) / pixel2mm));
            }

            if (config->enablesmooththresh && config->smooththresh > 0.0)
                s = linesim(s, LineSimplificationType::DouglasPeucker, config->smooththresh);
            
            //emit updateVisualOutput(config->seg);
        }

        config->seg = 255 - s;
        config->processed = Mat();
        config->displayOutputIndex = 1;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutput->insertItem(1, "Segmented image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(1);
        hostWindow->setDisplayOutputIndex(1);
    }
}

//void MainUI::enableinvertimage()
//{
//    config->invertimage = (invertimage->checkState() == Qt::CheckState::Checked);
//    //updatesegmentedoutput();
//    //if (invertimage->checkState() == Qt::CheckState::Checked)
//        //workthread->setinvertimage(true);
//    //else
//        //workthread->setinvertimage(false);
//}

//void MainUI::enablesavesegmented()
//{
//    config->saveseg = (savesegmented->checkState() == Qt::CheckState::Checked);
//    /*if (savesegmented->checkState() == Qt::CheckState::Checked)
//        workthread->setsavesegmented(true);
//    else
//        workthread->setsavesegmented(false);*/
//}

//void MainUI::enablesaveprocessed()
//{
//    config->saveimages = (saveprocessed->checkState() == Qt::CheckState::Checked);
//    /*if (saveprocessed->checkState() == Qt::CheckState::Checked)
//        workthread->setsaveprocessed(true);
//    else
//        workthread->setsaveprocessed(false);*/
//}

void MainUI::enablepixelconversion()
{
    //config->pixelconv = convertunits->checked();
    if (convertunits->checked())
    {
        //config->conversion = convertunits->getValue();
        filterbknoisy->setLabelText("Maximum size (mm sq.) :");
        filterfgnoisy->setLabelText("Maximum size (mm sq.) :");
    }
    else
    {
        //config->conversion = 1.0;
        filterbknoisy->setLabelText("Maximum size (pixels) :");
        filterfgnoisy->setLabelText("Maximum size (pixels) :");

        filterbknoisy->setValue(floor(filterbknoisy->getValue()));
        filterfgnoisy->setValue(floor(filterfgnoisy->getValue()));
    }
    updateRangeControls();
}

void MainUI::updateconversion()
{
    //config->pixelconv = true;
    
    if (convertunits->getValue() != 0.0)
    {
        //config->prevconversion = config->conversion;
        //config->conversion = convertunits->getValue();
        updateRangeControls();
    }
}

void MainUI::spboxvaluechanged(QDoubleSpinBox *s, double k)
{
    int i = 0;

    for (i = 0; i < spboxes.size(); i++)
        if (spboxes[i] == s)
            break;
    
    if (editmins[i + 1]->text().toDouble() != k)
    {
        if (i != (spboxes.size() - 1))
            spboxes[i + 1]->setMinimum(k);

        QString str;
        str.setNum(k, 'f', 2);
        editmins[i + 1]->setText(str);

        //config->dranges[i] = k;
    }
}

//void MainUI::timerEvent(QTimerEvent * ev)
//{
//    if (ev->timerId() != timerid)
//        return;
//
//    QTime t(0, 0);
//    t = t.addMSecs(elapsed);
//    if (timer.isValid())
//        t = t.addMSecs(timer.elapsed());
//    else
//    {
//        killTimer(timerid);
//        timerid = -1;
//    }
//
//    if (!workthread->isfinished())
//        timeel->setText(t.toString("hh:mm:ss"));
//
//    if (estcheck && nfiles > 0)
//    {
//        qint64 estimated = qint64(double(double(esttime) / double(nfiles)) *
//            double(filelist.size() - nfiles));
//        QTime e(0, 0);
//        e = e.addMSecs(estimated);
//        timeest->setText(e.toString("hh:mm:ss"));
//
//        estcheck = false;
//    }
//    else if (estcheck)
//        estcheck = false;
//
//}

string MainUI::getName()
{
    return "RhizoVision Explorer";
}

AppType MainUI::getApplicationType()
{
    return AppType::Custom;
}

OutputType MainUI::getOutputType()
{
    return OutputType::ImagesAndValues;
}

string MainUI::getAlgorithmName()
{
    return "RhizoVision Explorer";
}

vector<IParameterInfo *> MainUI::getParameters()
{
    return {};
}

QWidget *MainUI::getPluginUI()
{
    static int uicount = 1;

    if (uicount == 1)
    {
        uicount++;
        return pluginuiarea;
    }
    else // For batch processing
    {
        QWidget *w = new QWidget();
        QVBoxLayout *vl = new QVBoxLayout();
        saveseg = new StringCheckEnabler("Save segmented images", "File name suffix :");
        saveseg->setValue("_seg");
        savepro = new StringCheckEnabler("Save processed images", "File name suffix :");
        savepro->setValue("_features");

        saveseg->setChecked(config->savesegmented);
        savepro->setChecked(config->saveprocessed);

        QRegExp re("^(?!(?:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\\.[^.]*)?$)[^<>:\"/\\\\|?*\\x00-\\x1F]*[^<>:\"/\\\\|?*\\x00-\\x1F\\ .]$");
        QLabel *flbl = new QLabel("Features output file :");
        featurefileedt = new QLineEdit();
        featurefileedt->setValidator(new QRegExpValidator(re));
        featurefileedt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        featurefileedt->setMinimumWidth(40);
        featurefileedt->setText(config->featurecsvfile);

        QLabel *mlbl = new QLabel("Metadata file :");
        metadatafileedt = new QLineEdit();
        metadatafileedt->setValidator(new QRegExpValidator(re));
        metadatafileedt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        metadatafileedt->setMinimumWidth(40);
        metadatafileedt->setText(config->metadatacsvfile);

        QHBoxLayout *flayout = new QHBoxLayout();
        //flayout->addSpacing(20);
        flayout->addWidget(flbl);
        flayout->addWidget(featurefileedt);
        flayout->setContentsMargins(0, 0, 0, 0);

        QHBoxLayout *mlayout = new QHBoxLayout();
        //mlayout->addSpacing(20);
        mlayout->addWidget(mlbl);
        mlayout->addWidget(metadatafileedt);
        mlayout->setContentsMargins(0, 0, 0, 0);

        connect(saveseg, &StringCheckEnabler::stateChanged, [&]()
        {
            if (saveseg->checked())
                config->segsuffix = saveseg->getValue();
            else
                config->segsuffix = "";
            config->savesegmented = saveseg->checked();
        });

        connect(saveseg, &StringCheckEnabler::textChanged, [&]()
        {
            config->segsuffix = saveseg->getValue();
        });

        connect(savepro, &StringCheckEnabler::stateChanged, [&]()
        {
            if (savepro->checked())
                config->prosuffix = savepro->getValue();
            else
                config->prosuffix = "";
            config->saveprocessed = savepro->checked();
        });

        connect(savepro, &StringCheckEnabler::textChanged, [&]()
        {
            config->prosuffix = savepro->getValue();
        });

        connect(featurefileedt, &QLineEdit::textChanged, [&]()
        {
            config->featurecsvfile = featurefileedt->text();
        });

        connect(metadatafileedt, &QLineEdit::textChanged, [&]()
        {
            config->metadatacsvfile = metadatafileedt->text();
        });

        vl->addLayout(flayout);
        vl->addLayout(mlayout);
        vl->addWidget(saveseg);
        vl->addWidget(savepro);
        vl->setContentsMargins(0, 0, 0, 0);
        w->setLayout(vl);
        
        return w;
        /*auto ui = new MainUI(config);
        ui->setprocessfunction(pfunc);
        return ui;*/
    }
}

void MainUI::setImage(Mat image, QString imagename)
{
    if (!config->batchmode) // && config->displayOutputIndex != 0)
    {
        config->displayOutputIndex = 0;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(0);
    }

    config->input = image;
    config->imagename = imagename;
}

void MainUI::colorizechart(QChart *chart, int size)
{
    /*vector<vector<Scalar>> colorranges = {
        { { 215, 25, 28 } },
        { { 215, 25, 28 },{ 26 , 150, 65 } },
        { { 215, 25, 28 },{ 255, 255, 191 },{ 26 , 150, 65 } },
        { { 215, 25, 28 },{ 97 , 174, 253 },{ 255, 255, 191 },{ 26,150,65 } },
        { { 215, 25, 28 },{ 253, 174, 97 },{ 255, 255, 191 },{ 166,217,106 },{ 26 ,150,65 } },
        { { 215, 48, 39 },{ 252, 141, 89 },{ 254, 224, 139 },{ 217,239,139 },{ 145,207,96 },{ 26 ,152,80 } },
        { { 215, 48, 39 },{ 252, 141, 89 },{ 254, 224, 139 },{ 255,255,191 },{ 217,239,139 },{ 145,207,96 },{ 26 ,152,80 } },
        { { 215, 48, 39 },{ 244, 109, 67 },{ 253, 174, 97 },{ 254,224,139 },{ 217,239,139 },{ 166,217,106 },{ 102,189,99 },{ 26 ,152,80 } },
        { { 215, 48, 39 },{ 244, 109, 67 },{ 253, 174, 97 },{ 254,224,139 },{ 255,255,191 },{ 217,239,139 },{ 166,217,106 },{ 102,189,99 },{ 26,152,80 } },
        { { 165, 0,  38 },{ 215, 48,  39 },{ 244, 109, 67 },{ 253,174,97 },{ 254,224,139 },{ 217,239,139 },{ 166,217,106 },{ 102,189,99 },{ 26,152,80 },{ 0,104,55 } },
        { { 165, 0,  38 },{ 215, 48,  39 },{ 244, 109, 67 },{ 253,174,97 },{ 254,224,139 },{ 255,255,191 },{ 217,239,139 },{ 166,217,106 },{ 102,189,99 },{ 26,152,80 },{ 0,104,55 } }
    };*/

    vector<Scalar> colorranges = {
        { 220, 5, 12 },{ 25, 101, 176 },{ 78, 178, 101 },{ 244, 167, 54 },{ 136, 46, 114 },{ 232, 96, 28 },{ 123, 175, 222 },{ 247, 240, 86 },{ 202, 224, 171 },{ 114, 25, 14 },{82, 137, 199}
    };

    //vector<Scalar> color = colorranges[size - 1];
    auto series = chart->series();

    for (int i = 0; i < size; i++)
    {
        auto s = qobject_cast<QtCharts::QAbstractBarSeries *>(series[0])->barSets()[i];
        s->setColor(QColor(colorranges[i % colorranges.size()][0], colorranges[i % colorranges.size()][1], colorranges[i % colorranges.size()][2]));
    }
}

void MainUI::execute()
{
    executing = true;
    RoiManager *mgr = RoiManager::GetInstance();
    int roicount = mgr->getROICount();
    Mat inputimg = config->input;
    //vector<double> rhists;

    config->rotation = hostWindow->getRotation();
    config->invertimage = hostWindow->colorInversionEnabled();

    config->pixelconv = convertunits->checked();
    config->pixelspermm = pixelspermm->currentIndex();
    config->conversion = convertunits->getValue();

    config->dranges.clear();
    for (int i = 0; i < spboxes.size(); i++)
        config->dranges.push_back(spboxes[i]->value());

    if (roicount > 0)
    {
        auto rois = mgr->getROIs();
        Mat outputs, segs;
        vector<vector<double>> features, rhists;
        
        outputs = Mat::ones(inputimg.size(), CV_8UC3);
        outputs = Scalar(255, 255, 255);
        segs = Mat::ones(inputimg.size(), CV_8UC1) * 255;

        features.resize(roicount);
        rhists.resize(roicount);
        //rhists.resize(config->dranges.size() + 1);
        
        for (int i = 0; i < roicount; i++)
        {
            auto rect = rois[i]->rect();
            Mat ip;

            if ((rect.x() + rect.width() + 1) <= inputimg.cols && (rect.y() + rect.height() + 1) <= inputimg.rows)
                ip = inputimg(Rect(rect.x(), rect.y(), rect.width(), rect.height()));
            else
            {
                qWarning().noquote().nospace() << "Ignoring the region-of-interest '" << QString::fromStdString(mgr->getROIName(i)) << "', as it is out-of-bounds for the image " << config->imagename << ".";
                ip = inputimg;
            }

            config->input = ip;

            config->features.clear();
            config->rootlengthhist.clear();

            if (rect.height() > 0 && rect.width() > 0)
                pfunc(config.get());
            
            features[i] = config->features;
            rhists[i] = config->rootlengthhist;
            /*if (config->rootlengthhist.size() > 0)
            {
                for (int j = 0; j <= config->dranges.size(); j++)
                    rhists[j] += config->rootlengthhist[j];
            }*/

            //window("full output1", outputs);
            Mat roimat, roismat;

            if ((rect.x() + rect.width() + 1) <= inputimg.cols && (rect.y() + rect.height() + 1) <= inputimg.rows)
            {
                roimat = outputs(Rect(rect.x(), rect.y(), rect.width(), rect.height()));
                roismat = segs(Rect(rect.x(), rect.y(), rect.width(), rect.height()));
            }
            else
            {
                roimat = outputs;
                roismat = segs;
            }

            /*if (config->rotation != 0)
            {
                rotate(config->processed, roimat, revflag);
                rotate(config->seg, roismat, revflag);
            }
            else
            {*/
                config->processed.copyTo(roimat);
                config->seg.copyTo(roismat);
            //}

            /*window("processed", config->processed);
            window("full output2", outputs);*/
        }

        // To consolidate the output
        config->roifeatures.clear();
        config->roifeatures = features;
        config->roirootlengthhist.clear();
        config->roirootlengthhist = rhists;
        config->input = inputimg;
        config->seg = segs;
        config->processed = outputs;
    }
    else
    {
        config->features.clear();
        pfunc(config.get());
    }
    
    if (!config->batchmode && !config->abortprocess)
    {
        config->displayOutputIndex = 2;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutput->insertItem(1, "Segmented image");
        dispoutput->insertItem(2, "Processed image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(2);
    }

    config->abortprocess = false;
    executing = false;
}

void MainUI::abort()
{
    config->abortprocess = true;
}

std::vector<cv::Mat> MainUI::getOutputImages()
{
    //static int ii = 0;
    /*if ((!config->seg.empty()) && (!config->processed.empty()) && (config->displayOutputIndex != 2))
    {
        config->displayOutputIndex = 2;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutput->insertItem(1, "Segmented image");
        dispoutput->insertItem(2, "Processed image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(2);
    }
    else if ((!config->seg.empty()) && (config->processed.empty()) && (config->displayOutputIndex != 1))
    {
        config->displayOutputIndex = 1;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutput->insertItem(1, "Segmented image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(1);
    }
    else if (config->displayOutputIndex != 0)
    {
        config->displayOutputIndex = 0;
        dispoutputupdating = true;
        dispoutput->clear();
        dispoutput->insertItem(0, "Input image");
        dispoutputupdating = false;
        dispoutput->setCurrentIndex(0);
    }*/

    //std::cout << "control " << ii++ << "\n";
    return{ config->seg, config->processed };
}

QChart *MainUI::getChart()
{
    RoiManager *mgr = RoiManager::GetInstance();
    int roicount = mgr->getROICount();
    vector<double> rhists;

    config->displayOutputIndex = 2;
    dispoutputupdating = true;
    dispoutput->clear();
    dispoutput->insertItem(0, "Input image");
    dispoutput->insertItem(1, "Segmented image");
    dispoutput->insertItem(2, "Processed image");
    dispoutputupdating = false;
    dispoutput->setCurrentIndex(2);

    rhists.resize(config->dranges.size() + 1);

    
    if (roicount > 0)
    {
        for (int i = 0; i <= config->dranges.size(); i++)
            rhists[i] = 0;

        for (int i = 0; i < roicount; i++)
        {
            if (config->roirootlengthhist[i].size() > 0)
            {
                for (int j = 0; j <= config->dranges.size(); j++)
                    rhists[j] += config->roirootlengthhist[i][j];
            }
        }
    }
    else
        rhists = config->rootlengthhist;

    // To make plot in interactive mode.
    vector<string> categories;

    if (config->dranges.size() > 0)
    {
        categories.push_back("0-" + QString::number(config->dranges[0], 'f', 2).toStdString());
        for (int i = 1; i < config->dranges.size(); i++)
            categories.push_back(QString::number(config->dranges[i - 1], 'f', 2).toStdString() + "-" + QString::number(config->dranges[i], 'f', 2).toStdString());
        categories.push_back(QString::number(config->dranges[config->dranges.size() - 1], 'f', 2).toStdString() + " above");
    }
    else
        categories.push_back("0 above");

    f = createfigure();
    Mat yvalues = Mat::zeros(static_cast<int>(categories.size()), static_cast<int>(categories.size()), CV_64FC1);

    if (roicount == 0)
    {
        for (int i = 0; i < categories.size(); i++)
            yvalues.at<double>(i, i) = config->rootlengthhist[i];
    }
    else
    {
        for (int i = 0; i < categories.size(); i++)
            yvalues.at<double>(i, i) = rhists[i];
    }

    f->bar(categories, yvalues, {}, "stacked");

    if (config->pixelconv)
    {
        f->xlabel("Diameter ranges (mm)", 11, "bold");
        f->ylabel("Root Length (mm)", 11, "bold");
    }
    else
    {
        f->xlabel("Diameter ranges (pixels)", 11, "bold");
        f->ylabel("Root Length (pixels)", 11, "bold");
    }

    QChart *w = f->getChart();
    colorizechart(w, static_cast<int>(categories.size()));

    w->setMinimumHeight(200);
    w->layout()->setContentsMargins(0, 0, 0, 0);
    w->setBackgroundRoundness(0);
    //hostWindow->showChartWidget(w);
    return w;
}

int MainUI::getDisplayIndex()
{
    return config->displayOutputIndex;
}

void MainUI::setDisplayIndex(int idx)
{
    if (idx > dispoutput->count())
        return;

    config->displayOutputIndex = idx;
    dispoutput->setCurrentIndex(idx);
}

vector<double> MainUI::getFeatures()
{
    return config->features;
}

vector<double> MainUI::getFeatures(int roiidx)
{
    return config->roifeatures[roiidx];
}

void MainUI::setBatchMode(bool on)
{
    config->batchmode = on;
}

int MainUI::saveOutput(QString savloc, QString inputfilename)
{
    //ofstream f;
    QString saveloc = savloc;
    //string fefile = experimental::filesystem::path(inputfilename).filename().string();
    QFileInfo fefile(inputfilename);

    RoiManager * mgr = RoiManager::GetInstance();
    int roicount = mgr->getROICount();
    
    QFile outf(saveloc + config->featurecsvfile);
    //f.open(saveloc + config->featurecsvfile.toStdString(), ios::out | ios::app);
    
    if (outf.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QTextStream f(&outf);
        //f.setGenerateByteOrderMark(true);

        if (roicount == 0)
        {
            //f << experimental::filesystem::path(inputfilename).filename();
            f << fefile.fileName();
            f << "," << "Full";

            for (auto feat : config->features)
            {
                if (!(isnan(feat) || isinf(feat) || feat == 0.0))
                    f << "," << qSetRealNumberPrecision(ceil(log10(abs(feat))) + 6) << feat;
                else if (feat == 0.0)
                    f << "," << qSetRealNumberPrecision(6) << feat;
                else
                    f << "," << "NA";
            }

            f << endl;
        }
        else
        {
            for (int i = 0; i < roicount; i++)
            {
                //f << experimental::filesystem::path(inputfilename).filename();
                f << fefile.fileName();
                
                f << "," << QString::fromStdString(mgr->getROIName(i));

                for (auto feat : config->roifeatures[i])
                {
                    if (!(isnan(feat) || isinf(feat) || feat == 0.0))
                        f << "," << qSetRealNumberPrecision(ceil(log10(abs(feat))) + 6) << feat;
                    else if (feat == 0.0)
                        f << "," << qSetRealNumberPrecision(6) << feat; 
                    else
                        f << "," << "NA";
                }

                f << endl;
            }
        }
    }

    outf.close();
    
    if (config->savesegmented)
    {
        Mat m;
        cvtColor(config->seg, m, COLOR_RGB2BGR);
        imwrite(saveloc + fefile.fileName().left(fefile.fileName().length() - 4) + config->segsuffix + ".png", m);
    }
    if (config->saveprocessed)
    {
        Mat m;
        cvtColor(config->processed, m, COLOR_RGB2BGR);
        imwrite(saveloc + fefile.fileName().left(fefile.fileName().length() - 4) + config->prosuffix + ".png", m);
    }

    return 0;
}

void MainUI::saveMetadata(QString imgloc, QString savloc, QString filepath)
{
    QString outfilename;

    if (filepath.length() == 0)
        outfilename = savloc + config->metadatacsvfile; // , QIODevice::WriteOnly | QIODevice::Text);
    else
        outfilename = filepath; //, QIODevice::WriteOnly | QIODevice::Text);

    QFile outf(outfilename);

    if (!outf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open file to save metadata.";
        return;
    }

    QTextStream f(&outf);
    //f.setGenerateByteOrderMark(true);

    // We need to update the user setting here as this function is only called when 
    // executing the analysis in batch mode.
    config->rotation = hostWindow->getRotation();
    config->invertimage = hostWindow->colorInversionEnabled();

    config->pixelconv = convertunits->checked();
    config->pixelspermm = pixelspermm->currentIndex();
    config->conversion = convertunits->getValue();

    config->dranges.clear();
    for (int i = 0; i < spboxes.size(); i++)
        config->dranges.push_back(spboxes[i]->value());

    f << "RhizoVision Explorer Version, " << RHIZOVISION_EXPLORER_VERSION;
    if (filepath.length() > 0)
        f << "\nImage Path, " << imgloc << "\nOutput Path, " << savloc;
    f << "\nRoot type, " << ((config->roottype == 0) ? "Whole root" : "Broken roots");
    f << "\nImage Thresholding Level, " << config->threshold;
    f << "\nInvert images, " << ((hostWindow->colorInversionEnabled()) ? "true" : "false");
    f << "\nKeep largest component, " << ((config->keepLargest) ? "true" : "false");
    f << "\nFilter noisy components on background, " << ((config->filterbknoise) ? "true" : "false");
    f << "\nMaximum background noisy component size, " << config->maxcompsizebk;
    f << "\nFilter noisy components on foreground, " << ((config->filterfgnoise) ? "true" : "false");
    f << "\nMaximum foreground noisy component size, " << config->maxcompsizefg;
    f << "\nEnable edge smoothing, " << ((config->enablesmooththresh) ? "true" : "false");
    f << "\nEdge smoothing threshold, " << config->smooththresh;
    f << "\nEnable root pruning, " << ((config->enableRootPruning) ? "true" : "false");
    f << "\nRoot pruning threshold, " << config->rootPruningThreshold;
    f << "\nConvert pixels to physical units, " << ((config->pixelconv) ? "true" : "false");
    
    if (config->pixelspermm == 1) // for Pixels per mm
        f << "\nNumber of Pixels per mm, " << config->conversion;
    else if (config->pixelspermm == 0) // for DPI
        f << "\nDots per inch, " << config->conversion;

    double pixel2mm = 1.0;

    if (config->pixelconv)
    {
        if (config->pixelspermm == 0) // For DPI conversion
            pixel2mm /= (config->conversion / 25.4);
        else
            pixel2mm /= config->conversion;
    }

    f << "\nPixel to millimeter conversion factor, " << pixel2mm;

    for (int n = 0; n < config->dranges.size(); n++)
    {
        if (n == 0)
            f << "\nDiameter Range " << (n + 1) << ", 0, " << config->dranges[n];
        else
            f << "\nDiameter Range " << (n + 1) << ", " << config->dranges[n - 1] << ", " << config->dranges[n];
    }

    if (config->dranges.size() > 0)
        f << "\nDiameter Range " << (config->dranges.size() + 1) << ", " << config->dranges[config->dranges.size() - 1] << ", above";
    else
        f << "\nDiameter Range " << (config->dranges.size() + 1) << ", 0, above";

    f << "\nFeatures output file name, " << config->featurecsvfile;
    f << "\nSave segmented images, " << ((config->savesegmented) ? "true" : "false");
    f << "\nSegmented image file name suffix, " << config->segsuffix;
    f << "\nSave processed feature images, " << ((config->saveprocessed) ? "true" : "false");
    f << "\nProcessed image file name suffix, " << config->prosuffix;
    f << "\n";
    
    outf.close();
}

void MainUI::loadMetadata(QString filepath)
{
    QFile inputfile(filepath);

    if (!inputfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Cannot open file to load metadata.";
        return;
    }

    QTextStream fin(&inputfile);
    //fin.open(filepath, ios::in);

    vector<QString> row;
    QString line, word;
    int idx = 0;

    //vector<double> drange;
    while (dranges.size() != 1)
    {
        removerangerowclicked();
    }

    //while (getline(fin, line))
    while(!fin.atEnd())
    {
        line = fin.readLine();
        row.clear();
        idx = 0;

        //cout << line << endl;
        QStringList vals = line.split(",");
        //stringstream s(line);

//#define UPDATE_WORD                 \
//        if (!getline(s, word, ',')) \
//            continue;               \
//        word.erase(word.begin(), std::find_if(word.begin(), word.end(), [](int ch) { return !std::isspace(ch); })); \
//        word.erase(std::find_if(word.rbegin(), word.rend(), [](int ch) { return !std::isspace(ch); }).base(), word.end()); 
#define UPDATE_WORD                   \
        if (idx >= vals.size())       \
            continue;                 \
        word = vals[idx++].trimmed();
        

#define UPDATE_BOOL_SETTING(st, uicontrol) \
if (word == (st)) { \
    UPDATE_WORD; \
    if (word == "true") (uicontrol)->setChecked(true); else (uicontrol)->setChecked(false); }

#define UPDATE_VALUE_SETTING(st, uicontrol) \
if (word == (st)) { \
    UPDATE_WORD; \
    (uicontrol)->setValue(word.toDouble()); }

#define UPDATE_STRING_SETTING(st, uicontrol) \
if (word == (st)) { \
    UPDATE_WORD; \
    (uicontrol)->setValue(word); }

#define UPDATE_BOOL_CONFIG(st, uicontrol) \
if (word == (st)) { \
    UPDATE_WORD; \
    if (word == "true") (uicontrol) = (true); else (uicontrol) = (false); }

#define UPDATE_STRING_CONFIG(st, uicontrol) \
if (word == (st)) { \
    UPDATE_WORD; \
    (uicontrol) = word; }

        UPDATE_WORD;
        
        if (word == "Root type")
        {
            UPDATE_WORD;
            if (word == "Whole root") roottype->setCurrentIndex(0); else roottype->setCurrentIndex(1);
        }

        UPDATE_VALUE_SETTING("Image Thresholding Level", thresholdbox);
        
        //UPDATE_BOOL_SETTING("Invert images", invertimage);
        if (word == "Invert images")
        {
                UPDATE_WORD; \
                if (word == "true")
                    hostWindow->setColorInversionEnabled(true);
                else
                    hostWindow->setColorInversionEnabled(false);
        }

        UPDATE_BOOL_SETTING("Keep largest component", keepLargest);
        UPDATE_BOOL_SETTING("Filter noisy components on background", filterbknoisy);
        UPDATE_VALUE_SETTING("Maximum background noisy component size", filterbknoisy);
        UPDATE_BOOL_SETTING("Filter noisy components on foreground", filterfgnoisy);
        UPDATE_VALUE_SETTING("Maximum foreground noisy component size", filterfgnoisy);
        UPDATE_BOOL_SETTING("Enable edge smoothing", linesmooth);
        UPDATE_VALUE_SETTING("Edge smoothing threshold", linesmooth);
        UPDATE_BOOL_SETTING("Enable root pruning", enableRootPruning);
        UPDATE_VALUE_SETTING("Root pruning threshold", enableRootPruning);
        UPDATE_BOOL_SETTING("Convert pixels to physical units", convertunits);
        //UPDATE_VALUE_SETTING("Number of Pixels per mm", convertunits);
        
        if (word == ("Number of Pixels per mm"))
        {
            UPDATE_WORD;
            (convertunits)->setValue(word.toDouble());
            pixelspermm->setCurrentIndex(1); // for Pixels per mm
            config->pixelspermm = 1;
        }

        if (word == ("Dots per inch"))
        {
            UPDATE_WORD;
            (convertunits)->setValue(word.toDouble());
            pixelspermm->setCurrentIndex(0); // for DPI
            config->pixelspermm = 0;
        }

        if (word.indexOf("Diameter Range ") != string::npos)
        {
            //getline(s, word, ','); 
            // ignore first value
            idx++;
            UPDATE_WORD;

            if (word != "above")
                addrangerowclicked(word.toDouble());
        }

        UPDATE_STRING_CONFIG("Features output file name", config->featurecsvfile);
        UPDATE_BOOL_CONFIG("Save segmented images", config->savesegmented);
        UPDATE_STRING_CONFIG("Segmented image file name suffix", config->segsuffix);
        UPDATE_BOOL_CONFIG("Save processed feature images", config->saveprocessed);
        UPDATE_STRING_CONFIG("Processed image file name suffix", config->prosuffix);

#undef UPDATE_STRING_CONFIG
#undef UPDATE_BOOL_CONFIG
#undef UPDATE_STRING_SETTING
#undef UPDATE_VALUE_SETTING
#undef UPDATE_BOOL_SETTING
#undef UPDATE_WORD
    }

    inputfile.close();
    qInfo() << "Settings loaded from file " << filepath;
}

int MainUI::writeHeader(QString savloc, QString filename)
{
    QString outputfile;
    
    if (filename.length() == 0)
        //f.open(savloc.toStdString() + config->featurecsvfile.toStdString(), ios::out);
        outputfile = savloc + config->featurecsvfile;
    else
        //f.open(savloc.toStdString() + filename, ios::out);
        outputfile = savloc + filename;

    QFile outf(outputfile);

    if (!outf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Cannot save to output file.";
        return -1;
    }

    QTextStream f(&outf);
    //f.setGenerateByteOrderMark(true);

    // We need to update the user setting here as this function is only called when 
    // executing the analysis in batch mode.
    config->pixelconv = convertunits->checked();
    
    QString unitlength = (config->pixelconv) ? ".mm" : ".px";
    QString unitarea = (config->pixelconv) ? ".mm2" : ".px2";
    QString unitvolume = (config->pixelconv) ? ".mm3" : ".px3";
    QString perunitlength = (config->pixelconv) ? ".per.mm" : ".per.px";

    if (config->roottype == 0) // whole root
    {
        f << "File.Name,Region.of.Interest,Median.Number.of.Roots,Maximum.Number.of.Roots,Number.of.Root.Tips,"
            << "Total.Root.Length" << unitlength << ",Depth" << unitlength << ",Maximum.Width" << unitlength << ",Width-to-Depth.Ratio,Network.Area" << unitarea << ","
            << "Convex.Area" << unitarea << ",Solidity,Lower.Root.Area" << unitarea << ",Average.Diameter" << unitlength << ",Median.Diameter" << unitlength << ","
            << "Maximum.Diameter" << unitlength << ",Perimeter" << unitlength << ",Volume" << unitvolume << ",Surface.Area" << unitarea << ",Holes,Average.Hole.Size" << unitarea << ","
            << "Computation.Time.s,Average.Root.Orientation.deg,Shallow.Angle.Frequency,Medium.Angle.Frequency,"
            << "Steep.Angle.Frequency"; // , Fine Diameter Frequency, Medium Diameter Frequency, "
            //<< "Coarse Diameter Frequency\n";
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Root.Length.Diameter.Range." << (k + 1) << unitlength;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Projected.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Surface.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Volume.Diameter.Range." << (k + 1) << unitvolume;
        f << "\n";
    }
    else // broken roots
    {
        f << "File.Name,Region.of.Interest,Number.of.Root.Tips,Number.of.Branch.Points,"
            << "Total.Root.Length" << unitlength << ",Branching.frequency" << perunitlength << ",Network.Area" << unitarea << ","
            << "Average.Diameter" << unitlength << ",Median.Diameter" << unitlength << ","
            << "Maximum.Diameter" << unitlength << ",Perimeter" << unitlength << ",Volume" << unitvolume << ",Surface.Area" << unitarea << ","
            << "Computation.Time.s";
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Root.Length.Diameter.Range." << (k + 1) << unitlength;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Projected.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Surface.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config->dranges.size(); k++)
            f << ",Volume.Diameter.Range." << (k + 1) << unitvolume;
        f << "\n";
    }

    outf.close();
    return 0;
}

std::vector<QString> MainUI::getCSVColumnNames()
{
    QChar unitChar(185), squareChar(178), cubeChar(179), minusChar(8315);
    QString squareString = QString("%1").arg(squareChar);
    QString cubeString = QString("%1").arg(cubeChar);
    QString invString = QString("%1%2").arg(minusChar).arg(unitChar);

    QString unitlength = (config->pixelconv) ? " (mm)" : " (px)";
    QString unitarea = (config->pixelconv) ? " (mm" : " (px";
    QString unitvolume = (config->pixelconv) ? " (mm" : " (px";
    QString perunitlength = (config->pixelconv) ? " (mm" : " (px";

    unitarea += squareString;
    unitvolume += cubeString;
    perunitlength += invString;

    unitarea += ")";
    unitvolume += ")";
    perunitlength += ")";

    if (config->roottype == 0) // whole root
    {
        vector<QString> names = { "File Name", "Region of Interest", "Median Number of Roots",
            "Maximum Number of Roots", "Number of Root Tips",
            "Total Root Length" + unitlength, "Depth" + unitlength, "Maximum Width" + unitlength,
            "Width-to-Depth Ratio", "Network Area" + unitarea, "Convex Area" + unitarea,
            "Solidity", "Lower Root Area" + unitarea, "Average Diameter" + unitlength,
            "Median Diameter" + unitlength, "Maximum Diameter" + unitlength, "Perimeter" + unitlength,
            "Volume" + unitvolume, "Surface Area" + unitarea, "Holes", "Average Hole Size" + unitarea,
            "Computation Time (s)", "Average Root Orientation (deg)",
            "Shallow Angle Frequency", "Medium Angle Frequency",
            "Steep Angle Frequency" }; //, "Fine Diameter Frequency",
            //"Medium Diameter Frequency", "Coarse Diameter Frequency" };

        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Root Length Diameter Range " + to_string(k+1)) + unitlength);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Projected Area Diameter Range " + to_string(k + 1)) + unitarea);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Surface Area Diameter Range " + to_string(k + 1)) + unitarea);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Volume Diameter Range " + to_string(k + 1)) + unitvolume);

        return names;
        
    }
    else // broken roots
    {
        vector<QString> names = {"File Name", "Region of Interest", "Number of Root Tips", 
            "Number of Branch Points", "Total Root Length" + unitlength,
            "Branching frequency" + perunitlength, "Network Area" + unitarea,
            "Average Diameter" + unitlength, "Median Diameter" + unitlength,
            "Maximum Diameter" + unitlength, "Perimeter" + unitlength, "Volume" + unitvolume,
            "Surface Area" + unitarea, "Computation Time (s)"};

        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Root Length Diameter Range " + to_string(k + 1)) + unitlength);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Projected Area Diameter Range " + to_string(k + 1)) + unitarea);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Surface Area Diameter Range " + to_string(k + 1)) + unitarea);
        for (int k = 0; k <= config->dranges.size(); k++)
            names.push_back(QString::fromStdString("Volume Diameter Range " + to_string(k + 1)) + unitvolume);

        return names;
    }
}

int MainUI::getProgressSteps()
{
    // The plugin performs the following steps:
    // 1. Thresholding the input image.
    // 2. Filtering the image and retreiving the region containing plant root.
    // 3. Simplifying contours.
    // 4. Determining distance map.
    // 5. Skeletonizing the image.
    // 6. Finding root contours.
    //
    // If the root is crown root, 
    //      7. Finding the convex hull.
    // 
    // 8. Find the root topology.
    // 9. Extracting features.
    // 10. Drawing output image.
    RoiManager *mgr = RoiManager::GetInstance();

    if (mgr->getROICount() == 0)
        return (config->roottype == 0) ? 10 : 9;
    else
        return ((config->roottype == 0) ? 10 : 9) * mgr->getROICount();
}

