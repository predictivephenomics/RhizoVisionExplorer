# Building RhizoVision Explorer on macOS

This guide explains how to build and run RhizoVision Explorer on macOS (both Intel and Apple Silicon).

## Prerequisites

### 1. Install Xcode Command Line Tools

```bash
xcode-select --install
```

### 2. Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 3. Install Dependencies

```bash
# Install CMake, Qt6, and OpenCV
brew install cmake qt@6 opencv

# Add Qt to your PATH (add this to ~/.zshrc or ~/.bash_profile for persistence)
export PATH="$(brew --prefix qt@6)/bin:$PATH"
```

### 4. Install cvutil

cvutil is a custom library required by RhizoVision Explorer. You need to build it from source:

```bash
# Clone the cvutil repository
cd ~
git clone https://github.com/rootphenomicslab/cvutil.git
cd cvutil

# Build and install
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)" \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ..
make -j$(sysctl -n hw.ncpu)
sudo make install

# Return to your working directory
cd -
```

## Building RhizoVision Explorer

### Quick Build (Recommended)

Use the provided build script:

```bash
./build_macos.sh
```

### Manual Build

If you prefer to build manually:

```bash
# Create build directory
mkdir -p build/macos
cd build/macos

# Configure (the script auto-detects Intel vs Apple Silicon)
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)" \
      -G "Unix Makefiles" \
      ../..

# Build
cmake --build . -j$(sysctl -n hw.ncpu)
```

## Running the Application

### GUI Application

```bash
# From the build directory
open build/macos/RhizoVisionExplorer.app

# Or double-click RhizoVisionExplorer.app in Finder
```

### CLI Application

```bash
# From the build directory
./build/macos/rv --help

# Example usage
./build/macos/rv -i input_image.jpg -o output_dir
```

## Architecture-Specific Notes

### Apple Silicon (M1/M2/M3)
- AVX/AVX2 instructions are automatically disabled (ARM doesn't support them)
- Performance optimizations use ARM NEON instructions instead

### Intel Macs
- AVX2 instructions are enabled by default for better performance
- Requires processors with AVX2 support (most Intel Macs from 2013+)

## Creating a DMG Package

To create a distributable DMG file:

```bash
cd build/macos
cpack -G DragNDrop
```

This will create a `.dmg` file that can be distributed to other macOS users.

## Troubleshooting

### Qt not found
```bash
# Ensure Qt6 is in CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH="$(brew --prefix qt@6):$CMAKE_PREFIX_PATH"
```

### cvutil not found
```bash
# Make sure cvutil was installed correctly
ls /usr/local/lib/cmake/cvutil
```

### OpenCV not found
```bash
# Reinstall OpenCV
brew reinstall opencv
```

### Permission denied when running build script
```bash
chmod +x build_macos.sh
```

## Platform Differences from Windows

- **App Bundle**: The GUI application is packaged as a `.app` bundle (standard macOS format)
- **No .rc file**: Windows resource files are not used on macOS
- **Different packaging**: DMG instead of Windows installer

## Support

For issues specific to macOS builds, please open an issue on the GitHub repository.
