/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: MainUI.cpp

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

#include <QtWidgets/QtWidgets>

#include "MainUI.h"

using namespace std;
using namespace cv;

MainUI::MainUI()
{
    this->setWindowIcon(QIcon(":/icons/rvanalyzernew"));

    //createMenu();
    imglocGroupBox = createHorizontalGroupBox(tr("Image(s) location"), &imgbtn, "Browse", &imgline, &imglabel);
    imgsavGroupBox = createHorizontalGroupBox(tr("Output location"), &savbtn, "Browse", &savline, &savlabel);

    createRangeControls();
    createOptionsGroupBox();
    createProgressGroupBox();

    QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_FileDialogInfoView);
    aboutbtn = new QPushButton();
    aboutbtn->setIcon(icn);
    start = new QPushButton("Start");
    pause = new QPushButton("Pause");
    pause->setDisabled(true);
    quit = new QPushButton("Quit");
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->setAlignment(Qt::AlignRight);
    hlay->addWidget(aboutbtn, 0, Qt::AlignLeft);
    hlay->addStretch();
    hlay->addWidget(start, 0, Qt::AlignRight);
    hlay->addWidget(pause, 0, Qt::AlignRight);
    hlay->addWidget(quit, 0, Qt::AlignRight);

    sbar = new QStatusBar();
    sbar->setSizeGripEnabled(false);

    workthread = new FeatureExtractorThread();
    connectSignalstoSlots();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 3); 
    mainLayout->addWidget(imglocGroupBox);
    mainLayout->addWidget(imgsavGroupBox);
    mainLayout->addWidget(imgoptGroupBox);
    mainLayout->addWidget(imgproGroupBox);
    mainLayout->addLayout(hlay);
    mainLayout->addWidget(sbar);
    
    setTextWhatsThis();
    setLayout(mainLayout);

    //setWindowTitle(tr("RhizoVision Crown"));
    /*imgline->setText("C:\\Users\\aseethepalli\\Documents\\projects\\rd\\chaks");
    savline->setText("C:\\Users\\aseethepalli\\Documents\\projects\\rd\\chaks_output11");*/

    /*imgline->setText("C:\\Users\\aseethepalli\\Documents\\projects\\rd\\new");
    savline->setText("C:\\Users\\aseethepalli\\Documents\\projects\\rd\\newout");*/

    /*imgline->setText("C:\\Users\\aseethepalli\\Desktop\\a");
    savline->setText("C:\\Users\\aseethepalli\\Desktop\\b");*/

    /*imgline->setText("C:/Users/aseethepalli/Documents/projects/rd/redriverwheat_05242018");
    savline->setText("C:/Users/aseethepalli/Documents/projects/rd/redriverwheat_05242018_output8");*/
    
    updateLabels();

    
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    
    sbar->showMessage("Ready.", statusbartimeout);
    mode = CurrentMode::Stopped;
}

void MainUI::setTextWhatsThis()
{
    imglocGroupBox->setWhatsThis("Location of the images containing JPG (*.jpg, *.jpeg, *.jpe, *.jp2), PNG (*.png), BMP (*.bmp, *.dib) or TIFF (*.tiff, *.tif) image files.");
    imgsavGroupBox->setWhatsThis("Output location to store extracted features and optionally store thresholded and processed images.");

    thresh->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");
    thresholdslider->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");
    thresholdbox->setWhatsThis("The pixel values of a monochromatic image ranges from 0 to 255. Pixels less than this threshold level are classified as plant root pixels and the rest are background pixels.");

    invertimage->setWhatsThis("When this option is checked, the pixels greater than the above threshold level are classified as plant root pixels. This option may be used to extract features from root images with darker backgrounds.");
    savesegmented->setWhatsThis("Saves thresholded binary images in the output location. The file format will be PNG and the file name is the input file name appended with '_seg'.");
    saveprocessed->setWhatsThis("Saves processed feature images in the output location. The file format will be PNG and the file name is the input file name appended with '_features'.");
    
    convertunits->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");
    pixelsperinch->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");
    pixeledt->setWhatsThis("Converts extracted lengths from pixel units to millimeters, areas to mm<sup>2</sup> and volume to mm<sup>3</sup>. This is done by dividing the lengths by pixels-by-mm conversion factor. For areas and volume, the features are divided by the conversion factor twice and thrice respectively.");

    ranges->setWhatsThis("Root diameter ranges are used to classify the roots based on the root diameter at every location of the plant root image. The program then makes a normalized histogram and stores the bins as fine, medium and coarse diameter frequencies.");

    fdrange->setWhatsThis("Diameter range for fine roots.");
    feditmin->setWhatsThis("Diameter range for fine roots.");
    fspbox->setWhatsThis("Diameter range for fine roots.");

    mdrange->setWhatsThis("Diameter range for medium roots.");
    meditmin->setWhatsThis("Diameter range for medium roots.");
    mspbox->setWhatsThis("Diameter range for medium roots.");

    cdrange->setWhatsThis("Diameter range for coarse roots.");
    ceditmin->setWhatsThis("Diameter range for coarse roots.");
}

void MainUI::connectSignalstoSlots()
{
    connect(imgline, &QLineEdit::textChanged, this, &MainUI::updateLabels);
    connect(savline, &QLineEdit::textChanged, this, &MainUI::updateLabels);

    connect(imgbtn, &QPushButton::released, this, &MainUI::browse);
    connect(savbtn, &QPushButton::released, this, &MainUI::browse);

    connect(invertimage, &QCheckBox::stateChanged, this, &MainUI::enableinvertimage);
    connect(savesegmented, &QCheckBox::stateChanged, this, &MainUI::enablesavesegmented);
    connect(saveprocessed, &QCheckBox::stateChanged, this, &MainUI::enablesaveprocessed);
    connect(convertunits, &QCheckBox::stateChanged, this, &MainUI::enablepixelconversion);
    connect(pixeledt, &QLineEdit::textChanged, this, &MainUI::updateconversion);

    connect(aboutbtn, &QPushButton::clicked, this, &MainUI::about);
    connect(start, &QPushButton::clicked, this, &MainUI::startclicked);
    connect(pause, &QPushButton::clicked, this, &MainUI::pauseclicked);
    connect(quit, &QPushButton::clicked, this, &MainUI::close);

    connect(workthread, &FeatureExtractorThread::ReportProgress, this, &MainUI::UpdateProgress);
    connect(workthread, &FeatureExtractorThread::finished, this, &MainUI::workfinished);

    connect(thresholdslider, &QSlider::valueChanged, [&](int k)
    {
        if (thresholdbox->value() != k)
        {
            thresholdbox->setValue(k);
            workthread->setthreshold(k);
        }
    });

    connect(thresholdbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int k)
    {
        if (thresholdslider->value() != k)
        {
            thresholdslider->setValue(k);
            workthread->setthreshold(k);
        }
    });

    connect(fspbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double k)
    {
        if (meditmin->text().toDouble() != k)
        {
            mspbox->setMinimum(k);

            QString str;
            str.setNum(k, 'f', 2);
            meditmin->setText(str);

            workthread->setdiameterrange(fspbox->value(), mspbox->value());
        }
    });

    connect(mspbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double k)
    {
        if (ceditmin->text().toDouble() != k)
        {
            QString str;
            str.setNum(k, 'f', 2);
            ceditmin->setText(str);

            workthread->setdiameterrange(fspbox->value(), mspbox->value());
        }
    });
}

void MainUI::about()
{
    QDialog *about = new QDialog(this);
    about->setWindowFlags(about->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    about->setWindowIcon(QIcon(":/icons/rvanalyzernew"));
    about->setWindowTitle(tr("About RhizoVision Analyzer"));

    QVBoxLayout *vl = new QVBoxLayout();
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *lb = new QLabel();
    lb->setPixmap(QPixmap(":/icons/rvanalyzernew"));
    lb->setMaximumWidth(100);
    lb->setMaximumHeight(100);
    lb->setScaledContents(true);

    layout->addWidget(lb, 0);

    QTextBrowser  *tlb = new QTextBrowser();
    tlb->setHtml(tr(
    "<b>RhizoVision Analyzer</b> (version " RHIZOVISION_ANALYZER_VERSION ")<br/>"
    "<b>Copyright (C) 2018-2019 Noble Research Institute, LLC</b><br/><br/>"

    "This program extracts phenotypic traits from plant root images by "
    "performing image processing operations. This program can optionally "
    "save the processed and thresholded images. Also, it can optionally "
    "convert the extracted traits to physical units.<br/><br/>"
    
    "RhizoVision Analyzer is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version.<br/><br/>"
    "RhizoVision Analyzer is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.<br/><br/>"
    "You should have received a copy of the GNU General Public License "
    R"(along with RhizoVision Analyzer.  If not, see <a href = "https://www.gnu.org/licenses/">https://www.gnu.org/licenses/.</a><br/><br/>)"
    
    "Please send any comments/suggestions/improvements for the program to the authors:<br/>"
    "<pre>Anand Seethepalli,<br/>"
    "Computer Vision Specialist,<br/>"
    "Root Phenomics Lab,<br/>"
    "Noble Research Institute, LLC<br/><br/>"
    "email: <br/>aseethepalli@noble.org<br/>"
    "anand_seethepalli@yahoo.co.in<br/><br/>"
    "Larry York,<br/>"
    "Principal Investigator<br/>"
    "Root Phenomics Lab,<br/>"
    "Noble Research Institute, LLC<br/><br/>"
    "email: <br/>lmyork@noble.org<br/><br/></pre>"
    ));
    tlb->setReadOnly(true);
    tlb->setOpenExternalLinks(true);
    layout->addWidget(tlb, 0);

    vl->addLayout(layout);

    QPushButton *btn = new QPushButton("Ok");
    connect(btn, &QPushButton::clicked, about, &QDialog::close);

    vl->addWidget(btn, 0, Qt::AlignRight);
    about->setLayout(vl);
    about->layout()->setSizeConstraint(QLayout::SetFixedSize);

    about->open();

    about->exec();

    about->deleteLater();
}

void MainUI::closeEvent(QCloseEvent * ev)
{
    if (mode == CurrentMode::Started || mode == CurrentMode::Resumed || mode == CurrentMode::Paused)
    {
        if (QMessageBox::question(nullptr, "RhizoVision Analyzer", "Do you wish to cancel the progress?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
            ev->accept();
        else
            ev->ignore();
    }
    else
        ev->accept();
}

void MainUI::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key::Key_Escape)
    {
        if (mode == CurrentMode::Started || mode == CurrentMode::Resumed || mode == CurrentMode::Paused)
        {
            if (QMessageBox::question(nullptr, "RhizoVision Analyzer", "Do you wish to cancel the progress?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
            {
                ev->accept();
                reject();
            }
            else
                ev->ignore();
        }
        else
        {
            ev->accept();
            reject();
        }
    }
}

void MainUI::getfilelist(string imgpath)
{
    filelist.clear();

    for (auto & p : experimental::filesystem::directory_iterator(imgpath))
    {
        string f = p.path().extension().string();
        transform(f.begin(), f.end(), f.begin(), toupper);

        if (f == ".PNG"  || f == ".JPG"  || f == ".BMP" || 
            f == ".JPEG" || f == ".JPE"  || f == ".DIB" ||
            f == ".JP2"  || f == ".TIFF" || f == ".TIF")
        {
            /*Mat m = imread(p.path().string());

            if (!m.empty())
            {*/
                filelist.push_back(p.path().string());
            //}
        }
    }
}

void MainUI::UpdateProgress(QString filename, int fileno)
{
    string basename = experimental::filesystem::path(filename.toStdString()).filename().string();
    currfile->setText(tr((basename + " (" +
        to_string(fileno + 1) + " of " + to_string(filelist.size()) + ")").c_str()));
    progress->setValue(fileno);
    esttime = elapsed + timer.elapsed();
    nfiles = fileno;
    estcheck = true;
}

void MainUI::workfinished()
{
    // UI changes
    if (workthread->isfinished())
    {
        sbar->showMessage("Process complete", statusbartimeout);
        start->setText("Start");
        imglocGroupBox->setEnabled(true);
        imgsavGroupBox->setEnabled(true);
        imgoptGroupBox->setEnabled(true);
        pause->setText("Pause");
        pause->setDisabled(true);

        progress->setValue(filelist.size());
        currfile->setText(tr(""));

        workthread->reset();

        // Stop the timer
        elapsed = 0;
        timer.invalidate();

        QTime e(0, 0);
        timeest->setText(e.toString("hh:mm:ss"));

        mode = CurrentMode::Stopped;
    }
}

void MainUI::setprocessfunction(processfunc pfunc)
{
    workthread->setprocessfunction(pfunc);
}

bool ncompare(string left, string right)
{
    bool pcont = false;
    char *cleft = (char *)left.c_str();
    char *cright = (char *)right.c_str();

    while (1)
    {
        pcont = false;

        if (strlen(cleft) == 0 || strlen(cright) == 0)
            return (strlen(cleft) != 0);
        
        if (isdigit(*cleft) && isdigit(*cright))
        {
            char *rleft;
            char *rright;
            long long valA = strtoll(cleft, &rleft, 10);
            long long valB = strtoll(cright, &rright, 10);

            if (valA != valB)
                return ((valA - valB) < 0);
            else if (int(rright - cright) != int(rleft - cleft))
                return (int(rright - cright) - int(rleft - cleft)) < 0;
            else
            {
                cleft = rleft;
                cright = rright;
                continue;
            }
        }
        if (isdigit(*cleft) || isdigit(*cright))
            return (isdigit(*cleft) != 0);
        
        while (*cleft && *cright)
        {
            if (isdigit(*cleft) || isdigit(*cright))
            {
                pcont = true;
                break;
            }
            if (tolower(*cleft) != tolower(*cright))
                return (tolower(*cleft) - tolower(*cright)) < 0;
            
            cleft++;
            cright++;
        }

        if (pcont)
            continue;

        return (*cleft == '\0');
    }
}

bool MainUI::realstart()
{
    string imgpath = imgloc;
    std::replace(imgpath.begin(), imgpath.end(), '/', '\\');
    string savpath = imgloc;
    std::replace(savpath.begin(), savpath.end(), '/', '\\');

    // Check for valid paths.
    if (!CanStartProcessing())
    {
        if (!IsValidImageLoc)
            sbar->showMessage("Invalid image location.", statusbartimeout);
        else if (!IsValidSaveLoc)
            sbar->showMessage("Invalid output location.", statusbartimeout);

        return false;
    }

    // Verify if at least one image file exists.
    getfilelist(imgpath);

    if (filelist.size() == 0)
    {
        sbar->showMessage("No image found.", statusbartimeout);
        return false;
    }
    
    // UI changes.
    sbar->showMessage("Processing", statusbartimeout);
    start->setText("Stop");
    pause->setEnabled(true);
    imglocGroupBox->setDisabled(true);
    imgsavGroupBox->setDisabled(true);
    imgoptGroupBox->setDisabled(true);
    progress->setMaximum(filelist.size());

    sort(filelist.begin(), filelist.end(), ncompare);
    workthread->setfilelist(filelist);
    
    // Write features header in features text 
    // file in save location.
    ofstream f;
    f.open(savloc + "features.csv", ios::out);
    f << "File Name,Median Number of Roots,Maximum Number of Roots,Number of Root Tips,"
        << "Total Root Length,Depth,Maximum Width,Width-to-Depth Ratio,Network Area,"
        << "Convex Area,Solidity,Lower Root Area,Average Diameter,Median Diameter,"
        << "Maximum Diameter,Perimeter,Volume,Surface Area,Holes,Average Hole Size,"
        << "Computation Time,Average Root Orientation,Shallow Angle Frequency,Medium Angle Frequency,"
        << "Steep Angle Frequency,Fine Diameter Frequency,Medium Diameter Frequency,"
        << "Coarse Diameter Frequency\n";
    /*f << "File Name,Median Number of Roots,Maximum Number of Roots,Number of Root Tips,"
        << "Total Root Length,Depth,Maximum Width,Width-to-Depth Ratio,Network Area,"
        << "Convex Area,Solidity,Lower Root Area,Average Diameter,Median Diameter,"
        << "Maximum Diameter,Perimeter,Volume,Surface Area,Holes,Average Hole Size,"
        << "Computation Time,Average Root Orientation,Shallow Angle Frequency,Medium Angle Frequency,"
        << "Steep Angle Frequency,Shallowness Index,Fine Diameter Frequency,Medium Diameter Frequency,"
        << "Coarse Diameter Frequency,Fineness Index\n";*/
    f.close();

    // Save the settings in a metadata file in the output directory.
    f.open(savloc + "metadata.csv", ios::out);
    f << "RhizoVision Analyzer Version, " << RHIZOVISION_ANALYZER_VERSION;
    f << "\nImage Path, " << imgpath << "\nOutput Path, " << savpath;
    f << "\nImage Thresholding Level, " << thresholdbox->value();
    
    if (invertimage->checkState() == Qt::CheckState::Checked)
        f << "\nInvert Images, true, ";
    else
        f << "\nInvert Images, false, ";

    if (savesegmented->checkState() == Qt::CheckState::Checked)
        f << "\nSave Segmented Images, true, ";
    else
        f << "\nSave Segmented Images, false, ";

    if (saveprocessed->checkState() == Qt::CheckState::Checked)
        f << "\nSave Segmented Images, true, ";
    else
        f << "\nSave Segmented Images, false, ";

    if (convertunits->checkState() == Qt::CheckState::Checked)
        f << "\nConvert Pixels to Physical Units, true, ";
    else
        f << "\nConvert Pixels to Physical Units, false, ";

    f << "\nNumber of Pixels per mm, " << pixeledt->text().toDouble();
    f << "\nFine Roots Upper Range, " << fspbox->value();
    f << "\nMedium Roots Upper Range, " << mspbox->value() << "\n";

    workthread->start();
    
    // Start the timer
    elapsed = 0;
    timer.restart();
    if (timerid == -1)
        timerid = startTimer(100);
    QTime est(0, 0);
    est = est.addSecs(filelist.size() * 3);
    timeest->setText(est.toString("hh:mm:ss"));

    return true;
}

void MainUI::realstop()
{
    // UI changes
    start->setText("Start");
    imglocGroupBox->setEnabled(true);
    imgsavGroupBox->setEnabled(true);
    imgoptGroupBox->setEnabled(true);
    pause->setText("Pause");
    pause->setDisabled(true);
    
    workthread->requestInterruption();
    
    // Wait till thread stopped
    while (!workthread->isFinished())
        Sleep(500);

    Sleep(500);

    // Stop the timer
    elapsed = 0;
    timer.invalidate();
    
    progress->setValue(0);
    workthread->reset();
    currfile->setText(tr(""));
    QTime e(0, 0);
    timeest->setText(e.toString("hh:mm:ss"));
    sbar->showMessage("Stopped", statusbartimeout);
}

void MainUI::realpause()
{
    // UI changes
    pause->setText("Resume");
    imgoptGroupBox->setEnabled(true);
    
    workthread->requestInterruption();
    
    // Proceed to event handling only 
    // after thread is stopped.
    while (!workthread->isFinished())
        Sleep(500);

    Sleep(500);

    // Timer updating...
    // Pause is similar to stopping except that
    // we store the elapsed time so far.
    elapsed += timer.elapsed();
    timer.invalidate();

    sbar->showMessage("Paused", statusbartimeout);
}

void MainUI::realresume()
{
    // UI changes
    sbar->showMessage("Processing", statusbartimeout);
    pause->setText("Pause");
    imgoptGroupBox->setDisabled(true);

    workthread->start();

    // Timer update...
    timer.restart();
    if (timerid == -1)
        timerid = startTimer(100);
}

// The same button handler does Start and Stop handling.
// We just dispatch to real event handlers here based on
// current mode. We also use the modes in the real handlers
// to perform additional tasks.
void MainUI::startclicked()
{
    if (mode == CurrentMode::Stopped)
    {
        if (realstart())
            mode = CurrentMode::Started;
    }
    else
    {
        realstop();
        mode = CurrentMode::Stopped;
    }
}

void MainUI::pauseclicked()
{
    if (mode == CurrentMode::Started || mode == CurrentMode::Resumed)
    {
        realpause();
        mode = CurrentMode::Paused;
    }
    else if (mode == CurrentMode::Paused)
    {
        realresume();
        mode = CurrentMode::Resumed;
    }
}

void MainUI::browse()
{
    QPushButton *s = qobject_cast<QPushButton *>(sender());
    /*QFileDialog dlg;
    dlg.setFileMode(QFileDialog::FileMode::DirectoryOnly);*/
    string fileloc;

    if (s == imgbtn)
    {
        string filepath = imgline->text().toStdString();
        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        experimental::filesystem::path p(filepath);
        
        if (experimental::filesystem::exists(p) &&
            experimental::filesystem::is_directory(p))
        {
            
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Image(s) Location"), QDir(tr(filepath.c_str())).path(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks).toStdString();
        }
        else
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Image(s) Location"), QString(), 
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks).toStdString();
        
        if (fileloc.length() > 0)
            imgline->setText(tr(fileloc.c_str()));
    }
    else if (s == savbtn)
    {
        string filepath = savline->text().toStdString();
        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        experimental::filesystem::path p(filepath);

        if (experimental::filesystem::exists(p) &&
            experimental::filesystem::is_directory(p))
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Output Location"), tr(filepath.c_str()),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks).toStdString();
        else
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Output Location"), QString(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks).toStdString();
        
        if (fileloc.length() > 0)
            savline->setText(tr(fileloc.c_str()));
    }
}

bool MainUI::CanStartProcessing()
{
    return (IsValidImageLoc && IsValidSaveLoc);
}

void MainUI::createProgressGroupBox()
{
    int fixedwidth = 180;
    imgproGroupBox = new QGroupBox("Progress");
    vlayout = new QVBoxLayout;

    QGridLayout * glayout = new QGridLayout();
    currfilename = new QLabel("Current file : ");
    currfile = new QLineEdit("");
    currfile->setReadOnly(true);
    QPalette readOnlyPalette = currfile->palette();
    QColor mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    currfile->setPalette(readOnlyPalette);
    currfile->setFixedWidth(180);

    timeelapsed = new QLabel("Time elapsed : ");
    timeel = new QLineEdit();
    timeel->setReadOnly(true);
    readOnlyPalette = timeel->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    timeel->setPalette(readOnlyPalette);
    timeel->setFixedWidth(180);

    timeremaining = new QLabel("Est. time remaining : ");
    timeest = new QLineEdit();
    timeest->setReadOnly(true);
    readOnlyPalette = timeest->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    timeest->setPalette(readOnlyPalette);
    timeest->setFixedWidth(180);
    
    glayout->addWidget(currfilename, 0, 0);
    glayout->addWidget(currfile, 0, 1, 1, 2, Qt::AlignRight);
    glayout->addWidget(timeelapsed, 1, 0);
    glayout->addWidget(timeel, 1, 1, 1, 2, Qt::AlignRight);
    glayout->addWidget(timeremaining, 2, 0);
    glayout->addWidget(timeest, 2, 1, 1, 2, Qt::AlignRight);

    progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setValue(0);
    progress->setTextVisible(true);
    QHBoxLayout *hlayoutprog = new QHBoxLayout;
    hlayoutprog->addStretch();
    hlayoutprog->addWidget(progress, 1);
    
    vlayout->addLayout(glayout);
    vlayout->addLayout(hlayoutprog, 1);
    
    imgproGroupBox->setLayout(vlayout);
}

void MainUI::updateLabels()
{
    string filepath = imgline->text().toStdString();
    experimental::filesystem::path p(filepath);
    int iconheight = 20;

    if (experimental::filesystem::exists(p) &&
        experimental::filesystem::is_directory(p))
    {
        QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        imglabel->setPixmap(map);
        IsValidImageLoc = true;

        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        imgloc = filepath;
    }
    else
    {
        /*QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogCancelButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        imglabel->setPixmap(map);*/
        imglabel->setPixmap(QPixmap(0, 0));
        IsValidImageLoc = false;
    }

    filepath = savline->text().toStdString();
    p = experimental::filesystem::path(filepath);

    if (experimental::filesystem::exists(p) &&
        experimental::filesystem::is_directory(p))
    {
        QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        savlabel->setPixmap(map);
        IsValidSaveLoc = true;

        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        savloc = filepath;
        workthread->setsavelocation(savloc);
    }
    else
    {
        /*QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogCancelButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        savlabel->setPixmap(map);*/
        savlabel->setPixmap(QPixmap(0, 0));
        IsValidSaveLoc = false;
    }
}

QGroupBox *MainUI::createHorizontalGroupBox(QString grouptext, QPushButton **btn, QString btntext, QLineEdit **edt, QLabel **lab)
{
    QGroupBox *result = new QGroupBox(grouptext);
    QHBoxLayout *layout = new QHBoxLayout;

    (*edt) = new QLineEdit();
    (*btn) = new QPushButton(btntext);
    (*lab) = new QLabel();
    layout->addWidget(*edt);
    layout->addWidget(*btn);
    layout->addWidget(*lab);
    //layout->addStretch();
    result->setLayout(layout);

    return result;
}

void MainUI::createOptionsGroupBox()
{
    imgoptGroupBox = new QGroupBox("Options");
    vlayout = new QVBoxLayout;
    
    invertimage = new QCheckBox("Invert images before processing");
    
    thresh = new QLabel("Image thresholding level");
    thresholdslider = new QSlider(Qt::Orientation::Horizontal);
    thresholdslider->setMinimum(0);
    thresholdslider->setMaximum(255);
    thresholdslider->setValue(128);
    thresholdslider->setStyleSheet(
        "QSlider::groove:horizontal { border: 1px solid #999999; background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 0, stop : 0 #000000, stop:1 #ffffff); margin: 2px 0; } "
        "QSlider::handle:horizontal { height: 10px; width: 7px; background: #ffa100; margin: 0; /* expand outside the groove */ } "
    );

    thresholdbox = new QSpinBox();
    thresholdbox->setMinimum(0);
    thresholdbox->setMaximum(255);
    thresholdbox->setValue(128);
    thresholdbox->setFixedWidth(70);

    savesegmented = new QCheckBox("Save segmented images");
    saveprocessed = new QCheckBox("Save processed images");
    convertunits = new QCheckBox("Convert pixels to physical units");
    QHBoxLayout *hlayout = new QHBoxLayout;
    pixelsperinch = new QLabel("Number of pixels per mm :");
    pixeledt = new QLineEdit();
    pixeledt->setValidator(new QDoubleValidator(1.0, DBL_MAX, 5));
    pixeledt->setText("1.0");
    //pixeledt->setFixedWidth(65);
    currconv = 1.0;
    
    vlayout->addWidget(thresh);

    hlayout->addWidget(thresholdslider);
    hlayout->addWidget(thresholdbox);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(invertimage);

    hlayout = new QHBoxLayout;
    hlayout->addSpacing(20);
    hlayout->addWidget(pixelsperinch);
    //hlayout->addSpacing(40);
    hlayout->addWidget(pixeledt);
    hlayout->addStretch();

    vlayout->addWidget(savesegmented);
    vlayout->addWidget(saveprocessed);
    vlayout->addWidget(convertunits);
    vlayout->addLayout(hlayout);

    vlayout->addWidget(ranges);
    QLabel *lbl = new QLabel("to");
    lbl->setFixedWidth(10);
    int rangespacing = 48;

    hlayout = new QHBoxLayout;
    hlayout->addWidget(fdrange);
    hlayout->addSpacing(rangespacing);
    hlayout->addWidget(feditmin);
    hlayout->addWidget(lbl);
    hlayout->addWidget(fspbox, 0, Qt::AlignLeft);

    vlayout->addLayout(hlayout);

    lbl = new QLabel("to");
    lbl->setFixedWidth(10);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(mdrange);
    hlayout->addSpacing(rangespacing);
    hlayout->addWidget(meditmin);
    hlayout->addWidget(lbl);
    hlayout->addWidget(mspbox, 0, Qt::AlignLeft);

    vlayout->addLayout(hlayout);

    lbl = new QLabel("above.");
    lbl->setFixedWidth(40);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(cdrange);
    hlayout->addSpacing(rangespacing);
    hlayout->addWidget(ceditmin);
    hlayout->addWidget(lbl, 0, Qt::AlignLeft);

    vlayout->addLayout(hlayout);

    pixelsperinch->setDisabled(1);
    pixeledt->setDisabled(1);

    imgoptGroupBox->setLayout(vlayout);
}

void MainUI::updateRangeControls()
{
    vector<double> val;
    vector<QDoubleSpinBox *> boxes;

    boxes.push_back(fspbox);
    boxes.push_back(mspbox);
    
    bool prevmode = ranges->text() == "Root diameter ranges (in mm)";

    if (convertunits->checkState() == Qt::CheckState::Checked)
    {
        ranges->setText("Root diameter ranges (in mm)");

        if (!prevmode) // This function is triggered by check box
        {
            for (int boxid = 0; boxid < boxes.size(); boxid++)
                val.push_back(boxes[boxid]->value() / pixeledt->text().toDouble());
        }
        else // This function is triggered by change in conversion units
        {
            for (int boxid = 0; boxid < boxes.size(); boxid++)
                val.push_back((boxes[boxid]->value() * currconv) / pixeledt->text().toDouble());
        }

        for (int boxid = 0; boxid < boxes.size(); boxid++)
        {
            boxes[boxid]->setMaximum(100);
            boxes[boxid]->setSingleStep(0.1);
            boxes[boxid]->setValue(val[boxid]);
        }
    }
    else
    {
        ranges->setText("Root diameter ranges (in pixel units)");

        if (prevmode) // This function is triggered by check box
        {
            for (int boxid = 0; boxid < boxes.size(); boxid++)
                val.push_back(round(boxes[boxid]->value() * pixeledt->text().toDouble()) + 0.0);
        }
        
        for (int boxid = 0; boxid < boxes.size(); boxid++)
        {
            boxes[boxid]->setMaximum(5000);
            boxes[boxid]->setSingleStep(1.0);
            boxes[boxid]->setValue(val[boxid]);
        }
    }
}

void MainUI::createRangeControls()
{
    // Initialize
    ranges = new QLabel("Root diameter ranges (in pixel units)");
    fdrange = new QLabel("Fine roots:");
    mdrange = new QLabel("Medium roots:");
    cdrange = new QLabel("Coarse roots:");

    fdrange->setFixedWidth(80);
    mdrange->setFixedWidth(80);
    cdrange->setFixedWidth(80);

    feditmin = new QLineEdit();
    feditmin->setText("0");
    feditmin->setReadOnly(true);
    QPalette readOnlyPalette = feditmin->palette();
    QColor mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    feditmin->setPalette(readOnlyPalette);
    
    meditmin = new QLineEdit();
    meditmin->setText("5.0");
    meditmin->setReadOnly(true);
    readOnlyPalette = meditmin->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    meditmin->setPalette(readOnlyPalette);
    //meditmin->setValidator(new QDoubleValidator(0, 5000.0, 2));

    ceditmin = new QLineEdit();
    ceditmin->setText("10.0");
    ceditmin->setReadOnly(true);
    readOnlyPalette = ceditmin->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    ceditmin->setPalette(readOnlyPalette);
    //ceditmin->setValidator(new QDoubleValidator(0, 5000.0, 2));

    fspbox = new QDoubleSpinBox();
    mspbox = new QDoubleSpinBox();
    
    // Set basic properties
    //fspbox->setMaximum(10.0);
    fspbox->setMinimum(0.0);
    fspbox->setValue(5.0);
    fspbox->setSingleStep(1.0);

    mspbox->setMaximum(5000.0);
    mspbox->setMinimum(5.0);
    mspbox->setValue(10.0);
    mspbox->setSingleStep(1.0);

    fspbox->setFixedWidth(70);
    mspbox->setFixedWidth(70);

    feditmin->setFixedWidth(60);
    meditmin->setFixedWidth(60);
    ceditmin->setFixedWidth(60);
}

void MainUI::enableinvertimage()
{
    if (invertimage->checkState() == Qt::CheckState::Checked)
        workthread->setinvertimage(true);
    else
        workthread->setinvertimage(false);
}

void MainUI::enablesavesegmented()
{
    if (savesegmented->checkState() == Qt::CheckState::Checked)
        workthread->setsavesegmented(true);
    else
        workthread->setsavesegmented(false);
}

void MainUI::enablesaveprocessed()
{
    if (saveprocessed->checkState() == Qt::CheckState::Checked)
        workthread->setsaveprocessed(true);
    else
        workthread->setsaveprocessed(false);
}

void MainUI::enablepixelconversion()
{
    if (convertunits->checkState() == Qt::CheckState::Checked)
    {
        pixelsperinch->setEnabled(1);
        pixeledt->setEnabled(1);

        workthread->setconvertphysical(true, pixeledt->text().toDouble());
        updateRangeControls();
    }
    else
    {
        pixelsperinch->setDisabled(1);
        pixeledt->setDisabled(1);

        workthread->setconvertphysical(false, -1);
        updateRangeControls();
    }
}

void MainUI::updateconversion()
{
    workthread->setconvertphysical(true, pixeledt->text().toDouble());
    if (pixeledt->text().toDouble() != 0.0)
    {
        updateRangeControls();
        currconv = pixeledt->text().toDouble();
    }
}

void MainUI::timerEvent(QTimerEvent * ev)
{
    if (ev->timerId() != timerid)
        return;

    QTime t(0, 0);
    t = t.addMSecs(elapsed);
    if (timer.isValid())
        t = t.addMSecs(timer.elapsed());
    else
    {
        killTimer(timerid);
        timerid = -1;
    }

    if (!workthread->isfinished())
        timeel->setText(t.toString("hh:mm:ss"));

    if (estcheck && nfiles > 0)
    {
        qint64 estimated = qint64(double(double(esttime) / double(nfiles)) *
            double(filelist.size() - nfiles));
        QTime e(0, 0);
        e = e.addMSecs(estimated);
        timeest->setText(e.toString("hh:mm:ss"));

        estcheck = false;
    }
    else if (estcheck)
        estcheck = false;

}
