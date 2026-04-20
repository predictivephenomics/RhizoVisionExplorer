# Quick Installation for Intel Mac

This guide is specifically for **Intel Macs only** (x86_64 architecture).

⚠️ **Note:** This software does not currently support Apple Silicon (M1/M2/M3) due to architecture-specific CPU instructions in the cvutil dependency.

## One-Command Installation

Run this single command to install everything:

```bash
./install_macos_intel.sh
```

This script will:
- ✅ Verify you're on an Intel Mac
- ✅ Install Homebrew (if needed)
- ✅ Install all dependencies (CMake, Qt6, OpenCV)
- ✅ Download and build cvutil library with macOS patches
- ✅ Build RhizoVision Explorer
- ✅ Create convenient launch scripts

**Estimated time:** 15-30 minutes depending on your internet speed and whether dependencies are already installed.

## What Gets Installed

### Dependencies (via Homebrew)
- **CMake** - Build system
- **Qt6** - GUI framework
- **OpenCV** - Computer vision library

### Libraries (built from source)
- **cvutil** - Custom library for RhizoVision Explorer
  - Installed to: `/usr/local`
  - Patched for macOS compatibility

### RhizoVision Explorer
- **GUI App**: `build/macos-intel/RhizoVisionExplorer.app`
- **CLI Tool**: `build/macos-intel/rv`
- **Symlink**: `/usr/local/bin/rhizovision-cli`

## Running the Application

### GUI Application

```bash
./launch_gui.sh
```

Or double-click `RhizoVisionExplorer.app` in Finder:
```bash
open build/macos-intel/RhizoVisionExplorer.app
```

### CLI Application

```bash
./launch_cli.sh --help
```

Or use the system-wide command:
```bash
rhizovision-cli --help
```

### Example CLI Usage

```bash
# Process a single image
rhizovision-cli -i input_image.jpg -o output_directory

# Process a directory of images
rhizovision-cli -i input_folder/ -o output_folder/

# Show help
rhizovision-cli --help
```

## Troubleshooting

### Wrong Architecture Error

```
[ERROR] This script is for Intel Macs only (x86_64 architecture).
```

**Solution:** This script only works on Intel Macs. If you have an Apple Silicon Mac, the software currently cannot be built due to CPU architecture incompatibilities in the cvutil library.

### Homebrew Installation Prompts

The script may pause to ask for your password when installing Homebrew or system libraries. This is normal.

### Build Failures

If the build fails:

1. Check you have enough disk space (need ~5GB free)
2. Make sure you're connected to the internet
3. Try running the script again (it's idempotent)
4. Check the error message for specific issues

### Permission Denied

If you get permission errors:

```bash
chmod +x install_macos_intel.sh
./install_macos_intel.sh
```

### Missing Dependencies After Installation

If dependencies seem missing after installation, run:

```bash
brew install cmake qt@6 opencv
```

## Uninstalling

To remove RhizoVision Explorer:

```bash
# Remove the build
rm -rf build/

# Remove the launch scripts
rm launch_gui.sh launch_cli.sh

# Remove system-wide CLI symlink
sudo rm /usr/local/bin/rhizovision-cli

# Remove cvutil library (optional)
sudo rm -rf /usr/local/lib/libcvutil*
sudo rm -rf /usr/local/lib/libPluginManager*
sudo rm -rf /usr/local/lib/libRoiManager*
sudo rm -rf /usr/local/include/cvutil
sudo rm -rf /usr/local/lib/cmake/cvutil
```

To remove Homebrew dependencies (only if you don't need them for other projects):

```bash
brew uninstall cmake qt@6 opencv
```

## Manual Installation

If you prefer to install step-by-step instead of using the one-shot script, see [README_macOS.md](README_macOS.md) for detailed manual instructions.

## System Requirements

- **Architecture:** Intel (x86_64) - **Apple Silicon NOT supported**
- **macOS Version:** 10.15 (Catalina) or later recommended
- **Disk Space:** ~5GB for dependencies and build
- **RAM:** 4GB minimum, 8GB recommended
- **Xcode Command Line Tools:** Installed automatically by script

## Getting Help

For issues specific to macOS builds:
- Check the full documentation: [README_macOS.md](README_macOS.md)
- Report issues: https://github.com/rootphenomicslab/RhizoVisionExplorer/issues

For general RhizoVision Explorer questions:
- See main documentation: [README.md](README.md)
- Example usage: [example_usage.md](example_usage.md)

## Credits

RhizoVision Explorer is developed by:
- Anand Seethepalli (seethepallia@ornl.gov)
- Larry York (yorklm@ornl.gov)

Copyright (C) 2025, Oak Ridge National Laboratory

macOS support added by Claude Code.
