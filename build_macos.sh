#!/bin/bash
set -e

echo "Building RhizoVision Explorer for macOS..."

# Detect if Qt is installed via Homebrew
if command -v brew &> /dev/null; then
    QT_PATH=$(brew --prefix qt@6 2>/dev/null || echo "")
    if [ -n "$QT_PATH" ]; then
        echo "Found Qt6 at: $QT_PATH"
        export CMAKE_PREFIX_PATH="$QT_PATH:$CMAKE_PREFIX_PATH"
    else
        echo "Warning: Qt6 not found via Homebrew. Please install with: brew install qt@6"
    fi
fi

# Detect architecture
ARCH=$(uname -m)
echo "Building for architecture: $ARCH"

# Create build directory
BUILD_DIR="build/macos"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo "Configuring CMake..."
if [ "$ARCH" = "arm64" ]; then
    # Apple Silicon - disable AVX
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DENABLE_AVX512=OFF \
          -DENABLE_AVX2_FMA=OFF \
          -G "Unix Makefiles" \
          ../..
else
    # Intel Mac - enable AVX2
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DENABLE_AVX512=OFF \
          -DENABLE_AVX2_FMA=ON \
          -G "Unix Makefiles" \
          ../..
fi

# Build
echo "Building..."
NUM_CORES=$(sysctl -n hw.ncpu)
cmake --build . -j"$NUM_CORES"

echo ""
echo "Build complete!"
echo "GUI Application: $BUILD_DIR/RhizoVisionExplorer.app"
echo "CLI Application: $BUILD_DIR/rv"
echo ""
echo "To run the GUI: open $BUILD_DIR/RhizoVisionExplorer.app"
echo "To run the CLI: $BUILD_DIR/rv --help"
