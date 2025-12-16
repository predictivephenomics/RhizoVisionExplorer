/*
Copyright (C) 2025, Oak Ridge National Laboratory

File: mv.cpp

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

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

#define RVE_CLI

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

#include "feature_extraction.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <regex>

#ifdef BUILD_GUI
// Not yet supported as RoiManager depends on QGraphicsRectItem
#include <RoiManager.h>
#define TO_STD_STRING(x) (x).toStdString()
#define FROM_STD_STRING(x) QString::fromStdString(x)
#else
#include <iomanip>
typedef std::ofstream QTextStream;
typedef std::vector<std::string> QStringList;
#define TO_STD_STRING(x) x
#define FROM_STD_STRING(x) x
#define qSetRealNumberPrecision(x) std::setprecision(x)
#endif

#include "indicators/indicators.hpp"

namespace fs = std::filesystem;

// Function to check if file has supported image extension
bool isSupportedImageFile(const std::string& filename)
{
    std::string ext = fs::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || 
           ext == ".bmp" || ext == ".tif" || ext == ".tiff";
}

// Function to collect all image files from input path
std::vector<std::string> collectImageFiles(const std::string& inputPath, bool recursive)
{
    std::vector<std::string> imageFiles;
    
    try
    {
        if (fs::is_regular_file(inputPath))
        {
            if (isSupportedImageFile(inputPath))
                imageFiles.push_back(inputPath);
            else
                std::cerr << "Error: " << inputPath << " is not a supported image file." << std::endl;
        }
        else if (fs::is_directory(inputPath))
        {
            if (recursive)
            {
                for (const auto& entry : fs::recursive_directory_iterator(inputPath))
                    if (entry.is_regular_file() && isSupportedImageFile(entry.path().string()))
                        imageFiles.push_back(entry.path().string());
            }
            else
            {
                for (const auto& entry : fs::directory_iterator(inputPath))
                    if (entry.is_regular_file() && isSupportedImageFile(entry.path().string()))
                        imageFiles.push_back(entry.path().string());
            }
        }
        else
            std::cerr << "Error: " << inputPath << " is not a valid file or directory." << std::endl;
    }
    catch (const fs::filesystem_error& ex)
    {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }
    
    return imageFiles;
}

static inline std::string trim_ws(std::string s)
{
    const char *ws = " \t\r\n";
    auto a = s.find_first_not_of(ws);
    auto b = s.find_last_not_of(ws);
    if (a == std::string::npos)
        return std::string();
    return s.substr(a, b - a + 1);
}

// Positive number (int/float) with optional leading '+', supports scientific notation (e.g., 1e-3)
static bool is_positive_number_like(const std::string &s)
{
    static const std::regex re(R"(^\+?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?$)");
    return std::regex_match(s, re);
}

static void append_checked_positive_ascending(double v, std::vector<double> &out)
{
    if (!(v > 0.0))
    {
        throw std::runtime_error("All --dranges values must be positive (> 0). Got: " + std::to_string(v));
    }
    if (!out.empty() && !(v >= out.back()))
    {
        throw std::runtime_error(
            "Values for --dranges must be ascending. Got " +
            std::to_string(v) + " after " + std::to_string(out.back()));
    }
    out.push_back(v);
}

// Returns true if token was a drange token (consumed); false means “not a drange token, stop.”
static bool consume_drange_token(const std::string &raw,
                                 std::vector<double> &out,
                                 std::string &err)
{
    std::string token = trim_ws(raw);
    if (token.empty())
        return false;

    // If token begins with '-', it's another option; negatives aren't allowed anyway.
    if (!token.empty() && token[0] == '-')
        return false;

    // CSV?
    if (token.find(',') != std::string::npos)
    {
        std::stringstream ss(token);
        std::string piece;
        bool saw_any = false;
        try
        {
            while (std::getline(ss, piece, ','))
            {
                std::string t = trim_ws(piece);
                if (t.empty())
                    continue; // tolerate ",  ,"
                if (!is_positive_number_like(t))
                {
                    err = "Invalid value in --dranges: '" + t + "' (must be positive number)";
                    return true; // it *is* a drange token, but invalid → caller should error
                }
                append_checked_positive_ascending(std::stod(t), out);
                saw_any = true;
            }
        }
        catch (const std::exception &ex)
        {
            err = ex.what();
            return true;
        }
        // If the token was only commas/spaces, treat it as “not a drange token”
        return saw_any;
    }

    // Single value?
    if (is_positive_number_like(token))
    {
        try
        {
            append_checked_positive_ascending(std::stod(token), out);
        }
        catch (const std::exception &ex)
        {
            err = ex.what();
        }
        return true; // consumed as a drange token (valid or invalid)
    }

    // Not a drange token → likely the input path (or something else). Stop.
    return false;
}

// Function to parse command line arguments
feature_config parseCommandLine(int argc, char* argv[])
{
    feature_config config;
    config.segsuffix = "_seg";
    config.prosuffix = "_features";
    bool contours = false, holes = false, convexhull = false;
    bool dpi = false;
    bool pixels = false;
    bool fgsize = false;
    bool bgsize = false;
    bool smooththreshold = false;
    bool prunethresh = false;
    bool ssuffix = false;
    bool fsuffix = false;
    double pixelFactor = 1.0;
    double dpiFactor = 1.0;
    
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // Help and info
        if (arg == "-h" || arg == "--help")
        {
            config.showHelp = true;
            config.showHelpMain = true;
        }
        else if (arg == "--version")
            config.showVersion = true;
        else if (arg == "--license")
            config.showLicense = true;
        else if (arg == "--credits")
            config.showCredits = true;
        // Output options
        else if (arg == "-na" || arg == "--noappend")
            config.noappend = true;
        else if (arg == "-op" || arg == "--output_path")
        {
            if (i + 1 < argc)
                config.outputPath = argv[++i];
            else
            {
                std::cerr << "Error: --output_path requires a path." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "-o" || arg == "--output")
        {
            if (i + 1 < argc)
                config.outputFile = argv[++i];
            else
            {
                std::cerr << "Error: --output requires a filename." << std::endl;
                config.showHelp = true;
            }
        }
#ifdef BUILD_GUI
        else if (arg == "--roipath")
        {
            if (i + 1 < argc)
            {
                // Currently not implemented
                std::string roipath = argv[++i];
                
                // Warn if the option is used and the file path does not exist
                if (!fs::exists(roipath))
                {
                    std::cerr << "Warning: ROI path " << roipath << " does not exist." << std::endl;
                    std::cerr << "Continuing without ROI annotations." << std::endl;
                }
                else if (!fs::is_regular_file(roipath) && !fs::is_directory(roipath))
                {
                    std::cerr << "Warning: ROI path " << roipath << " is not a file or directory." << std::endl;
                    std::cerr << "Continuing without ROI annotations." << std::endl;
                }

                RoiManager* mgr = RoiManager::GetInstance();
                mgr->loadAnnotation(FROM_STD_STRING(roipath));
            }
            else
            {
                std::cerr << "Error: --roipath requires a path." << std::endl;
                config.showHelp = true;
            }
        }
#endif
        else if (arg == "--metafile")
        {
            if (i + 1 < argc)
            {
                // Currently not implemented
                std::string metafile = argv[++i];
                std::cerr << "Warning: --metafile option is not implemented yet." << std::endl;
            }
            else
            {
                std::cerr << "Error: --metafile requires a filename." << std::endl;
                config.showHelp = true;
            }
        }

        // General options
        else if (arg == "-r" || arg == "--recursive")
            config.recursive = true;
        else if (arg == "-v" || arg == "--verbose")
            config.verbose = true;

        // Root analysis options
        else if (arg == "-rt" || arg == "--roottype")
        {
            if (i + 1 < argc)
            {
                int val = std::atoi(argv[++i]);
                if (val == 0 || val == 1)
                    config.roottype = val;
                else
                {
                    std::cerr << "Error: --roottype must be 0 (whole) or 1 (broken)." << std::endl;
                    config.showHelp = true;
                }
            }
            else
            {
                std::cerr << "Error: --roottype requires a value (0 or 1)." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "-t" || arg == "--threshold")
        {
            if (i + 1 < argc)
            {
                int val = std::atoi(argv[++i]);
                if (val >= 0 && val <= 255)
                    config.threshold = val;
                else
                {
                    std::cerr << "Error: --threshold must be between 0 and 255." << std::endl;
                    config.showHelp = true;
                }
            }
            else
            {
                std::cerr << "Error: --threshold requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "-i" || arg == "--invert")
            config.invertimage = true;

        // Filtering options
        else if (arg == "-kl" || arg == "--keeplargest")
            config.keepLargest = true;
        else if (arg == "--bgnoise")
            config.filterbknoise = true;
        else if (arg == "--fgnoise")
            config.filterfgnoise = true;
        else if (arg == "--bgsize")
        {
            bgsize = true;
            if (i + 1 < argc)
                config.maxcompsizebk = std::atof(argv[++i]);
            else
            {
                std::cerr << "Error: --bgsize requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "--fgsize")
        {
            fgsize = true;
            if (i + 1 < argc)
                config.maxcompsizefg = std::atof(argv[++i]);
            else
            {
                std::cerr << "Error: --fgsize requires a value." << std::endl;
                config.showHelp = true;
            }
        }

        // Smoothing options
        else if (arg == "-s" || arg == "--smooth")
            config.enablesmooththresh = true;
        else if (arg == "-st" || arg == "--smooththreshold")
        {
            smooththreshold = true;
            if (i + 1 < argc)
                config.smooththresh = std::atof(argv[++i]);
            else
            {
                std::cerr << "Error: --smooththreshold requires a value." << std::endl;
                config.showHelp = true;
            }
        }

        // Unit conversion options
        else if (arg == "--convert")
            config.pixelconv = true;
        else if (arg == "--factordpi")
        {
            if (i + 1 < argc)
            {
                dpiFactor = std::atof(argv[++i]);
                dpi = true;
            }
            else
            {
                std::cerr << "Error: --factordpi requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "--factorpixels")
        {
            if (i + 1 < argc)
            {
                pixelFactor = std::atof(argv[++i]);
                pixels = true;
            }
            else
            {
                std::cerr << "Error: --factorpixels requires a value." << std::endl;
                config.showHelp = true;
            }
        }

        // Analysis options
        else if (arg == "--prune")
            config.enableRootPruning = true;
        else if (arg == "-pt" || arg == "--prunethreshold")
        {
            prunethresh = true;
            if (i + 1 < argc)
                config.rootPruningThreshold = std::atoi(argv[++i]);
            else
            {
                std::cerr << "Error: --prunethreshold requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "--dranges")
        {
            // Optional list: may be empty; only consume clear numeric/CSV tokens.
            config.dranges.clear();

            int j = i + 1;
            for (; j < argc; ++j)
            {
                std::string next = argv[j];

                // Stop at next option; negatives are invalid for --dranges anyway.
                if (!next.empty() && next[0] == '-')
                    break;

                std::string parse_err;
                bool was_drange_token = consume_drange_token(next, config.dranges, parse_err);

                if (!was_drange_token)
                {
                    // next is not numeric/CSV → likely the input path or another positional → stop.
                    break;
                }
                if (!parse_err.empty())
                {
                    std::cerr << "Error: " << parse_err << std::endl;
                    config.showHelp = true;
                    // Still advance i to the last token we looked at:
                    i = j;
                    return config; // bail early; parsing further options is risky after an error
                }
            }

            // Move past what we consumed
            i = j - 1;

            // Nothing else to do; empty list is valid
        }

        // Output options
        else if (arg == "--segment")
            config.savesegmented = true;
        else if (arg == "--feature")
            config.saveprocessed = true;
        else if (arg == "--ssuffix")
        {
            ssuffix = true;
            if (i + 1 < argc)
                config.segsuffix = argv[++i];
            else
            {
                std::cerr << "Error: --ssuffix requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "--fsuffix")
        {
            fsuffix = true;
            if (i + 1 < argc)
                config.prosuffix = argv[++i];
            else
            {
                std::cerr << "Error: --fsuffix requires a value." << std::endl;
                config.showHelp = true;
            }
        }

        // Processed image options
        else if (arg == "-ch" || arg == "--convexhull")
        {
            config.showConvexHull = true;
            convexhull = true;
        }
        else if (arg == "-ho" || arg == "--holes")
        {
            holes = true;
            config.showHoles = true;
        }
        else if (arg == "-dm" || arg == "--distancemap")
            config.showDistMap = true;
        else if (arg == "-ma" || arg == "--medialaxis")
            config.showMedialAxis = true;
        else if (arg == "-mw" || arg == "--medialaxiswidth")
        {
            if (i + 1 < argc)
                config.medialaxiswidth = std::atoi(argv[++i]);
            else
            {
                std::cerr << "Error: --medialaxiswidth requires a value." << std::endl;
                config.showHelp = true;
            }
        }
        else if (arg == "-to" || arg == "--topology")
            config.showMedialAxisDiameter = false;
        else if (arg == "-co" || arg == "--contours")
        {
            config.showContours = true;
            contours = true;
        }
        else if (arg == "-cw" || arg == "--contourwidth")
        {
            if (i + 1 < argc)
                config.contourwidth = std::atoi(argv[++i]);
            else
            {
                std::cerr << "Error: --contourwidth requires a value." << std::endl;
                config.showHelp = true;
            }
        }

        // Input path
        else if (arg[0] != '-')
        {
            if (config.inputPath.empty())
                config.inputPath = arg;
            else
            {
                std::cerr << "Error: Multiple input paths specified." << std::endl;
                config.showHelp = true;
            }
        }
        // Unknown option
        else
        {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            config.showHelp = true;
        }
    }

    if (config.inputPath.empty())
    {
        if (!(config.showHelp || config.showVersion || config.showLicense || config.showCredits))
        {
            std::cerr << "Error: No input path specified." << std::endl;
            config.showHelp = true;
            return config;
        }
        else
            return config;
    }
    
    if (config.outputPath.empty())
    {
        if (!config.inputPath.empty()) // Extra safety guard
        {
            fs::path inPath(config.inputPath);
            if (fs::is_directory(inPath))
                config.outputPath = inPath.string();
            else
                config.outputPath = inPath.has_parent_path() && !inPath.parent_path().empty()
                    ? inPath.parent_path().string()
                    : fs::absolute(inPath).parent_path().string();
        }
        else // Should not reach here
            config.outputPath = fs::current_path().string();
    }
    
    if (!fs::exists(config.outputPath))
    {
        std::cerr << "Error: Output path " << config.outputPath << " does not exist." << std::endl;
        config.showHelp = true;
        return config;
    }
    
    // Check if outputFile is a file path or a directory path. If it is not a file path, then show error.
    fs::path outFilePath(config.outputFile);
    if (!config.outputFile.empty())
    {
        if (fs::is_directory(outFilePath))
        {
            std::cerr << "Error: Output file " << config.outputFile << " is a directory." << std::endl;
            config.showHelp = true;
            return config;
        }
        else if (outFilePath.is_absolute())
        {
            if (outFilePath.has_parent_path() && !outFilePath.parent_path().empty() && !fs::exists(outFilePath.parent_path()))
            {
                std::cerr << "Error: Directory for output file " << outFilePath.parent_path() << " does not exist." << std::endl;
                config.showHelp = true;
                return config;
            }
            config.outputFile = outFilePath.string();
        }
        else // Relative path, combine with outputPath
        {
            fs::path combinedPath = fs::path(config.outputPath) / outFilePath;
            if (combinedPath.has_parent_path() && !combinedPath.parent_path().empty() && !fs::exists(combinedPath.parent_path()))
            {
                std::cerr << "Error: Directory for output file " << combinedPath.parent_path() << " does not exist." << std::endl;
                config.showHelp = true;
                return config;
            }
            config.outputFile = combinedPath.string();
        }
    }
    else
    {
        config.outputFile = "features.csv";
        fs::path combinedPath = fs::path(config.outputPath) / config.outputFile;
        config.outputFile = combinedPath.string();
    }

    if (config.roottype == 1 && (convexhull || holes || contours))
    {
        std::cerr << "Warning: Convex hull, holes, and contours options are ignored for broken roots." << std::endl;
        config.showConvexHull = false;
        config.showHoles = false;
        config.showContours = false;
    }

    if (config.pixelconv)
    {
        if (pixels)
        {
            config.conversion = pixelFactor;
            config.pixelspermm = 1;
            if (dpi)
                std::cerr << "Warning: Both --factorpixels and --factordpi are set. Using --factorpixels." << std::endl;
        }
        else if (dpi)
        {
            config.conversion = dpiFactor;
            config.pixelspermm = 0;
        }
        else
        {
            config.conversion = 1.0; // Default to 1.0
            config.pixelspermm = 0; // Default to DPI conversion
            //std::cerr << "Warning: --convert is set but no conversion factor provided. Defaulting to 1.0 DPI." << std::endl;
        }
    }
    else
    {
        if (dpi || pixels)
            std::cerr << "Warning: Conversion factor provided but --convert is not set. Ignoring conversion factor." << std::endl;
        config.conversion = 1.0;
        config.pixelspermm = 0;
    }
    
    // Check if dranges are in ascending order
    if (config.dranges.size() > 0 && !std::is_sorted(config.dranges.begin(), config.dranges.end()))
    {
        std::cerr << "Error: Diameter ranges (--dranges) must be in ascending order." << std::endl;
        config.showHelp = true;
        return config;
    }

    // Validate dranges: positive & ascending
    for (size_t k = 0; k < config.dranges.size(); ++k)
    {
        if (!(config.dranges[k] > 0.0))
        {
            std::cerr << "Error: --dranges must contain positive values. Got "
                      << config.dranges[k] << std::endl;
            config.showHelp = true;
            return config;
        }
        if (k > 0 && !(config.dranges[k] >= config.dranges[k - 1]))
        {
            std::cerr << "Error: --dranges must be ascending." << std::endl;
            config.showHelp = true;
            return config;
        }
    }

    if (fgsize && !config.filterfgnoise)
        std::cerr << "Warning: --fgsize is set but --fgnoise is not enabled. Ignoring --fgsize." << std::endl;
    if (bgsize && !config.filterbknoise)
        std::cerr << "Warning: --bgsize is set but --bgnoise is not enabled. Ignoring --bgsize." << std::endl;
    if (smooththreshold && !config.enablesmooththresh)
        std::cerr << "Warning: --smooththreshold is set but --smooth is not enabled. Ignoring --smooththreshold." << std::endl;
    if (prunethresh && !config.enableRootPruning)
        std::cerr << "Warning: --prunethreshold is set but --prune is not enabled. Ignoring --prunethreshold." << std::endl;
    if (ssuffix && !config.savesegmented)
        std::cerr << "Warning: --ssuffix is set but --segment is not enabled. Ignoring --ssuffix." << std::endl;
    if (fsuffix && !config.saveprocessed)
        std::cerr << "Warning: --fsuffix is set but --feature is not enabled. Ignoring --fsuffix." << std::endl;
    
    return config;
}

// Function to print usage information
void printUsage(const char* programName)
{
    std::cout << "RhizoVision Command Line Interface\n\n";
    std::cout << "Usage: " << programName << " [OPTIONS] <input_path>\n\n";
    std::cout << "Arguments:\n";
#ifdef BUILD_GUI
    std::cout << "  --roipath PATH              Path to CSV file containing ROI annotations (optional)\n";
#endif
    std::cout << "  --metafile FILE             Path to metadata CSV file (optional)\n"; // FIXME: Not implemented yet
    std::cout << "  input_path                  Path to image file or directory containing images\n\n";
    
    std::cout << "Output Options:\n";
    std::cout << "  -na, --noappend             Do not append to output file if it exists. Overwrite\n";
    std::cout << "                              it (default: appends)\n";
    std::cout << "  -op, --output_path PATH     Output directory for processed images (default: same\n";
    std::cout << "                              directory as input)\n";
    std::cout << "  -o, --output FILE_NAME      Output CSV file (default: features.csv)\n";
    std::cout << "                              If --output_path is specified, this file will be\n";
    std::cout << "                              created in that directory. If the file path is absolute,\n";
    std::cout << "                              it will be used as is. If it is relative, it will be\n";
    std::cout << "                              created in the output directory (--output_path).\n\n";

    std::cout << "General Options:\n";
    std::cout << "  -h, --help                  Show this help message\n";
    std::cout << "  -r, --recursive             Process directories recursively\n";
    std::cout << "  -v, --verbose               Enable verbose output\n";
    std::cout << "  --version                   Show application version\n";
    std::cout << "  --license                   Show application license information\n";
    std::cout << "  --credits                   Show application credits\n\n";

    std::cout << "Root Analysis Options:\n";
    std::cout << "  -rt, --roottype TYPE        Root type: 0=whole root, 1=broken roots (default: 1)\n";
    std::cout << "  -t, --threshold VAL         Segmentation threshold 0-255 (default: 200)\n";
    std::cout << "  -i, --invert                Invert image colors before processing. The background\n";
    std::cout << "                              should be brighter than the roots by default.\n\n";

    std::cout << "Filtering Options:\n";
    std::cout << "  -kl, --keeplargest          Keep only the largest component\n";
    std::cout << "  --bgnoise                   Filter background noise components.\n";
    std::cout << "                              No filter is applied by default.\n";
    std::cout << "  --fgnoise                   Filter foreground noise components.\n";
    std::cout << "                              No filter is applied by default.\n";
    std::cout << "  --bgsize VAL                Max background component size (default: 1.0)\n";
    std::cout << "                              Components larger than this (in image area) are\n";
    std::cout << "                              removed if --bgnoise is set.\n";
    std::cout << "  --fgsize VAL                Max foreground component size (default: 1.0)\n";
    std::cout << "                              Components larger than this (in image area) are\n";
    std::cout << "                              removed if --fgnoise is set.\n\n";

    std::cout << "Smoothing Options:\n";
    std::cout << "  -s, --smooth                Enable contour smoothing. Off by default.\n";
    std::cout << "  -st, --smooththreshold VAL  Contour smoothing threshold in pixels (default: 2.0)\n";
    std::cout << "                              Applied to the contour when --smooth is enabled.\n\n";
    
    std::cout << "Unit Conversion Options:\n";
    std::cout << "  --convert                   Enable pixel to physical unit (mm) conversion.\n";
    std::cout << "                              Off by default. When specified, the factor defaults\n";
    std::cout << "                              to DPI conversion.\n";
    std::cout << "  --factordpi VAL             Conversion factor in DPI (default: 1.0).\n";
    std::cout << "  --factorpixels VAL          Use pixels per mm instead of DPI. (default: 1.0).\n";
    std::cout << "                              --factordpi is ignored if --factorpixels is set.\n\n";
    
    std::cout << "Analysis Options:\n";
    std::cout << "  --prune                     Enable root pruning. Off by default.\n";
    std::cout << "  -pt, --prunethreshold VAL   Root pruning threshold (default: 1). Roots shorter than this\n";
    std::cout << "                              (in pixels) ignoring parent lateral root radius are pruned when\n";
    std::cout << "                              --prune is enabled.\n";
    std::cout << "  --dranges VALS              Comma-separated diameter ranges for statistical features\n";
    std::cout << "                              The VALS should be in ascending order (default: 2.0,5.0)\n";
    std::cout << "                              If --convert is specified, the values are treated as physical\n";
    std::cout << "                              units (mm) instead of pixels.\n";

    std::cout << "Output Options:\n";
    std::cout << "  --segment                   Save segmented images. Off by default.\n";
    std::cout << "  --feature                   Save processed feature images. Off by default.\n";
    std::cout << "  --ssuffix SUFFIX            Suffix for segmented images (default: _seg), to be used when\n";
    std::cout << "                              saving segmented images.\n";
    std::cout << "  --fsuffix SUFFIX            Suffix for processed images (default: _features), to be used\n";
    std::cout << "                              when saving processed images.\n\n";

    std::cout << "Processed image options (Used when --feature is specified):\n"; // To be used when --save-processed is specified
    std::cout << "  -ch, --convexhull           Show convex hull in processed images.\n"; // For whole roots only
    std::cout << "                              On by default. For whole roots only. For broken roots, this\n";
    std::cout << "                              option is ignored.\n";
    std::cout << "  -ho, --holes                Show holes in processed images.\n"; // For whole roots only
    std::cout << "                              On by default. For whole roots only. For broken roots, this\n";
    std::cout << "                              option is ignored.\n";
    std::cout << "  -dm, --distancemap          Show distance map in processed images.\n";
    std::cout << "                              Off by default.\n";
    std::cout << "  -ma, --medialaxis           Show medial axis in processed images.\n";
    std::cout << "                              On by default.\n";
    std::cout << "  -mw, --medialaxiswidth VAL  Medial axis width (default: 3)\n";
    std::cout << "  -to, --topology             Show topology in processed images. By default (Off),\n";
    std::cout << "                              the medial axis is colored according to diameter\n";
    std::cout << "                              ranges specified using --dranges.\n";
    std::cout << "  -co, --contours             Show contours in processed images.\n"; // For whole roots only
    std::cout << "                              On by default. For whole roots only. For broken roots, this\n";
    std::cout << "                              option is ignored.\n";
    std::cout << "  -cw, --contourwidth VAL     Contour width (default: 1)\n\n";
    
    std::cout << "Input can be a single image file or a directory containing images.\n";
    std::cout << "Supported image formats: PNG, JPG, JPEG, BMP, TIF, TIFF\n\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " image.jpg\n";
    std::cout << "  " << programName << " -r -o results.csv /path/to/images/\n";
    std::cout << "  " << programName << " --verbose --threshold 150 --smooth images_folder\n";
    std::cout << "  " << programName << " --roottype 0 --convert --factordpi 0.1 image.png\n";
    std::cout << "  " << programName << " --feature --dranges 1.0,3.0,6.0 folder/\n";
}

// Function to analyze a single image
bool analyzeImage(feature_config* config)
{
    try
    {
        // Load the image
        cv::Mat input = cv::imread(config->inputPath, cv::IMREAD_ANYCOLOR);
        if (input.empty())
        {
            std::cerr << "Error: Could not load image " << config->inputPath << std::endl;
            return false;
        }
        
        if (input.channels() == 3)
            cv::cvtColor(input, input, cv::COLOR_BGR2RGB);
        
        // Set image name for reporting
        config->imagefilename = fs::path(config->inputPath).filename().string();
#ifdef BUILD_GUI
        // Get ROI info
        RoiManager* mgr = RoiManager::GetInstance();
        int roicount = mgr->getROICount();
        
        if (roicount > 0)
        {
            auto rois = mgr->getROIs();
            cv::Mat outputs, segs;
            vector<vector<double>> features;

            outputs = cv::Mat::ones(input.size(), CV_8UC3);
            outputs = cv::Scalar(255, 255, 255);
            segs = cv::Mat::ones(input.size(), CV_8UC1) * 255;

            features.resize(roicount);

            for (int i = 0; i < roicount; i++)
            {
                auto rect = rois[i]->rect();
                cv::Mat ip;

                if ((rect.x() + rect.width() + 1) <= input.cols && (rect.y() + rect.height() + 1) <= input.rows)
                    ip = input(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()));
                else
                {
                    std::cerr << "Warning: Ignoring the region-of-interest '" << mgr->getROIName(i) << "', as it is out-of-bounds for the image " << config->imagefilename << ".";
                    ip = input;
                }

                config->input = ip;

                config->features.clear();
                config->rootlengthhist.clear();

                if (rect.height() > 0 && rect.width() > 0)
                    feature_extractor(config);

                features[i] = config->features;
                
                cv::Mat roimat, roismat;

                if ((rect.x() + rect.width() + 1) <= input.cols && (rect.y() + rect.height() + 1) <= input.rows)
                {
                    roimat = outputs(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()));
                    roismat = segs(cv::Rect(rect.x(), rect.y(), rect.width(), rect.height()));
                }
                else
                {
                    roimat = outputs;
                    roismat = segs;
                }

                config->processed.copyTo(roimat);
                config->seg.copyTo(roismat);
            }

            // To consolidate the output
            config->roifeatures.clear();
            config->roifeatures = features;
            // config->roirootlengthhist.clear();
            config->input = input;
            config->seg = segs;
            config->processed = outputs;
        }
        else
#endif
        {
            // Call the feature extraction function
            config->input = input;
            config->features.clear();
            feature_extractor(config);
        }

        // Save segmented image if requested
        if (config->savesegmented && !config->seg.empty())
        {
            std::string savefile = (fs::path(config->inputPath).stem().string() + TO_STD_STRING(config->segsuffix) + ".png");
            std::string segFilename = (fs::path(config->outputPath) / savefile).string();
            cv::imwrite(segFilename, config->seg);
            if (config->verbose)
                std::cout << "  Segmented image saved as " << savefile << std::endl;
        }

        // Save processed image if requested
        if (config->saveprocessed && !config->processed.empty())
        {
            std::string savefile = (fs::path(config->inputPath).stem().string() + TO_STD_STRING(config->prosuffix) + ".png");
            std::string proFilename = (fs::path(config->outputPath) / savefile).string();
            cv::imwrite(proFilename, config->processed);
            if (config->verbose)
                std::cout << "  Processed image saved as " << savefile << std::endl;
        }
        
        return true;
    } 
    catch (const std::exception& ex)
    {
        std::cerr << "Error processing " << config->inputPath << ": \n\t" << ex.what() << std::endl;
        return false;
    }
}

// Function to write CSV header
void writeCsvHeader(feature_config &config, QTextStream &f)
{
    QString unitlength = (config.pixelconv) ? ".mm" : ".px";
    QString unitarea = (config.pixelconv) ? ".mm2" : ".px2";
    QString unitvolume = (config.pixelconv) ? ".mm3" : ".px3";
    QString perunitlength = (config.pixelconv) ? ".per.mm" : ".per.px";

    if (config.roottype == 0) // whole root
    {
        f << "File.Name,Region.of.Interest,Median.Number.of.Roots,Maximum.Number.of.Roots,Number.of.Root.Tips,"
          << "Total.Root.Length" << unitlength << ",Depth" << unitlength << ",Maximum.Width" << unitlength << ",Width-to-Depth.Ratio,Network.Area" << unitarea << ","
          << "Convex.Area" << unitarea << ",Solidity,Lower.Root.Area" << unitarea << ",Average.Diameter" << unitlength << ",Median.Diameter" << unitlength << ","
          << "Maximum.Diameter" << unitlength << ",Perimeter" << unitlength << ",Volume" << unitvolume << ",Surface.Area" << unitarea << ",Holes,Average.Hole.Size" << unitarea << ","
          << "Computation.Time.s,Average.Root.Orientation.deg,Shallow.Angle.Frequency,Medium.Angle.Frequency,"
          << "Steep.Angle.Frequency"; // , Fine Diameter Frequency, Medium Diameter Frequency, "
        //<< "Coarse Diameter Frequency\n";
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Root.Length.Diameter.Range." << (k + 1) << unitlength;
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Projected.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Surface.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config.dranges.size(); k++)
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
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Root.Length.Diameter.Range." << (k + 1) << unitlength;
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Projected.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Surface.Area.Diameter.Range." << (k + 1) << unitarea;
        for (int k = 0; k <= config.dranges.size(); k++)
            f << ",Volume.Diameter.Range." << (k + 1) << unitvolume;
        f << "\n";
    }
}

// Function to write analysis results to CSV
void writeResultsToCsv(feature_config &config,
                       const QStringList &imageFiles,
                       const std::vector<std::vector<double>> &allFeatures,
                       const QStringList &roiNames)
{
    fs::path csvFullPath;
    csvFullPath = config.outputFile;
    
    bool fileExists = fs::exists(csvFullPath);
    
    if (!config.noappend && fileExists && config.verbose)
        std::cerr << "Warning: Output file " << csvFullPath << " already exists. Appending results." << std::endl;
    
#ifdef BUILD_GUI
    QFile outf(csvFullPath);
    if (!outf.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        std::cerr << "Error: Could not open output file " << csvFullPath << std::endl;
        return;
    }
    QTextStream f(&outf);
#else
    std::ofstream f;
    f.open(csvFullPath, std::ios::binary | std::ios::app);
#endif
    if (!fileExists || config.noappend)
        writeCsvHeader(config, f);
    for (size_t i = 0; i < imageFiles.size(); ++i)
    {
        const auto &features = allFeatures[i];
        const QString filename = FROM_STD_STRING(fs::path(TO_STD_STRING(imageFiles[i])).filename().string());
        const QString roiname = roiNames[i];

        f << filename << "," << roiname;
        for (const double &feature : features)
        {
            if (!(isnan(feature) || isinf(feature) || feature == 0.0))
                f << "," << qSetRealNumberPrecision(ceil(log10(abs(feature))) + 6) << feature;
            else if (feature == 0.0)
                f << "," << qSetRealNumberPrecision(6) << feature;
            else
                f << "," << "NA";
        }

        f << "\n";
    }
#ifdef BUILD_GUI    
    outf.close();
#else
    f.close();
#endif
}

std::tuple<int, int, int> getEstimatedTimeRemaining(int currentImageIndex, int totalImages, double elapsedTime)
{
    double avgTimePerImage = elapsedTime / (currentImageIndex + 1);
    double estRemaining = (currentImageIndex > 0) ? avgTimePerImage * (totalImages - (currentImageIndex + 1)) : (totalImages * 2);
    int estMin = static_cast<int>(estRemaining) / 60;
    int estSec = static_cast<int>(estRemaining) % 60;
    int estHour = estMin / 60;
    estMin = estMin % 60;
    return std::make_tuple(estHour, estMin, estSec);
}

void printRemainingTime(int currentImageIndex, int totalImages, double elapsedTime)
{
    auto [estHour, estMin, estSec] = getEstimatedTimeRemaining(currentImageIndex, totalImages, elapsedTime);
    std::cout << "[remaining: " << estHour << "h " << estMin << "m " << estSec << "s] - ";
}

void printElapsedTime(double elapsedTime)
{
    int totalSec = static_cast<int>(elapsedTime);
    int hours = totalSec / 3600;
    int minutes = (totalSec % 3600) / 60;
    int seconds = totalSec % 60;
    std::cout << "[elapsed: " << hours << "h " << minutes << "m " << seconds << "s] - ";
}

int main(int argc, char *argv[])
{
    cv::setUseOptimized(true);
    
    if (!cv::checkHardwareSupport(CV_CPU_AVX2))
    {
        init(argc, argv, false, false);
        cv::setUseOptimized(false);
    }
    else
    {
        init(argc, argv, true, false);
    }
    
    feature_config config = parseCommandLine(argc, argv);
    
    if (config.showHelp)
    {
        printUsage(argv[0]);
        return (!config.showHelpMain) ? 1 : 0;
    }
    
    if (config.showVersion)
    {
        std::cout << "RhizoVision Explorer CLI Version " << RHIZOVISION_EXPLORER_VERSION << std::endl;
        return 0;
    }
    
    if (config.showLicense)
    {
        std::cout << "RhizoVision Explorer is licensed under the GPL-3.0 License.\n"
                  << "See COPYING file for details." << std::endl;
        return 0;
    }
    
    if (config.showCredits)
    {
        std::cout << "RhizoVision Explorer acknowledges the contributions of:\n"
                  << "- OpenCV library developers\n"
                  << "- Qt framework developers\n"
                  << "- All contributors to the RhizoVision project\n";
        return 0;
    }
    
    // Collect image files
    std::vector<std::string> imageFiles = collectImageFiles(config.inputPath, config.recursive);

    if (imageFiles.empty())
    {
        std::cerr << "No supported image files found in " << config.inputPath << std::endl;
        return 1;
    }

    if (config.verbose)
    {
        std::cout << "RhizoVision Command Line Interface" << std::endl;
        std::cout << "Found " << imageFiles.size() << " image file(s) to process." << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Root type: " << (config.roottype == 0 ? "whole root" : "broken roots") << std::endl;
        std::cout << "  Threshold: " << config.threshold << std::endl;
        std::cout << "  Invert image: " << (config.invertimage ? "yes" : "no") << std::endl;
        std::cout << "  Pixel conversion: " << (config.pixelconv ? "enabled" : "disabled") << std::endl;
        std::cout << "  Input path: " << config.inputPath << std::endl;
        std::cout << "  Output file: " << config.outputFile << std::endl;
        std::cout << "  Output path: " << config.outputPath << std::endl;
        if (config.pixelconv)
            std::cout << "  Conversion factor: " << config.conversion << std::endl;
    }
    
    // Process all images
    std::vector<std::vector<double>> allFeatures;
    QStringList processedFiles;
    QStringList roinames;
    int imgcount = imageFiles.size();
    
    config.consolemode = true;
    
#ifdef BUILD_GUI
    RoiManager *mgr = RoiManager::GetInstance();
    int roicount = mgr->getROICount();
#endif
    
    // Set time start for processing
    auto start = std::chrono::steady_clock::now();
    indicators::ProgressBar bar;
    
    // Setup progress bar if verbose is disabled and multiple images are to be processed
    if (!config.verbose && imgcount > 1)
    {
        bar.set_option(indicators::option::BarWidth{50});
        bar.set_option(indicators::option::Start{"["});
        bar.set_option(indicators::option::Fill{"="});
        bar.set_option(indicators::option::Lead{">"});
        bar.set_option(indicators::option::Remainder{" "});
        bar.set_option(indicators::option::End{"]"});
        bar.set_option(indicators::option::PrefixText{"Processing image: "});
        bar.set_option(indicators::option::ForegroundColor{indicators::Color::yellow});
        bar.set_option(indicators::option::ShowElapsedTime{true});
        bar.set_option(indicators::option::ShowRemainingTime{true});
        bar.set_option(indicators::option::MaxProgress{imgcount});
        std::cout << std::endl; // To ensure progress bar is on a new line
    }
    
    for (int i = 0; i < imgcount; ++i)
    {
        // Compute estimated time remaining in HH:MM:SS format
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        
        config.inputPath = imageFiles[i];
        
        if (config.verbose)
        {
            std::cout << "Processing (" << (i + 1) << " of " << imgcount << ") - ";
            printElapsedTime(elapsed);
            printRemainingTime(i, imgcount, elapsed);
            std::cout << fs::path(config.inputPath).filename().string() << std::endl;
        }
        if (!config.verbose && imgcount > 1)
        {
            std::cout << termcolor::reset << "\033[F"; // ANSI escape to move cursor up one line
            bar.set_option(indicators::option::PrefixText{"Processing image (" + std::to_string(i + 1) + " of " + std::to_string(imgcount) + ") : " + fs::path(config.inputPath).filename().string() + "\n"});
            bar.tick();
        }

        if (analyzeImage(&config))
        {
#ifdef BUILD_GUI
            if (roicount > 0)
            {
                for (int r = 0; r < roicount; r++)
                {
                    std::string roiName = mgr->getROIName(r);
                    allFeatures.push_back(config.roifeatures[r]);
                    processedFiles.push_back(FROM_STD_STRING(imageFiles[i]));
                    roinames.push_back(FROM_STD_STRING(roiName));
                }
            }
            else
#endif
            {
                allFeatures.push_back(config.features);
                processedFiles.push_back(FROM_STD_STRING(imageFiles[i]));
                roinames.push_back("Full");
            }
        }
    }

    if (processedFiles.empty())
    {
        std::cerr << "No images were successfully processed." << std::endl;
        return 1;
    }
    
    // Write results to CSV
    writeResultsToCsv(config, processedFiles, allFeatures, roinames);
    
    std::cout << "Successfully processed " << processedFiles.size() << " image(s)." << std::endl;
    
    return 0;
}




