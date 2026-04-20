#!/bin/bash
set -e

#==============================================================================
# RhizoVision Explorer - One-Shot macOS Intel Installation Script
#==============================================================================
# This script will:
# 1. Verify you're on an Intel Mac
# 2. Install Homebrew (if needed)
# 3. Install all dependencies (CMake, Qt6, OpenCV)
# 4. Clone and build cvutil library with macOS patches
# 5. Build RhizoVision Explorer
#==============================================================================

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo ""
    echo "=============================================================================="
    echo "$1"
    echo "=============================================================================="
}

#==============================================================================
# Step 1: Verify Architecture
#==============================================================================
print_header "Step 1: Verifying System Architecture"

ARCH=$(uname -m)
log_info "Detected architecture: $ARCH"

if [ "$ARCH" != "x86_64" ]; then
    log_error "This script is for Intel Macs only (x86_64 architecture)."
    log_error "Detected: $ARCH"
    log_error ""
    log_error "RhizoVision Explorer's cvutil dependency uses x86-specific instructions"
    log_error "and is not compatible with Apple Silicon (ARM64)."
    log_error ""
    log_error "Options:"
    log_error "  1. Run this on an Intel Mac"
    log_error "  2. Use Rosetta 2 on Apple Silicon (not tested)"
    exit 1
fi

log_success "Running on Intel Mac (x86_64)"

#==============================================================================
# Step 2: Install Homebrew
#==============================================================================
print_header "Step 2: Checking Homebrew Installation"

if ! command -v brew &> /dev/null; then
    log_warning "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    log_success "Homebrew installed"
else
    log_success "Homebrew already installed"
    BREW_VERSION=$(brew --version | head -1)
    log_info "$BREW_VERSION"
fi

#==============================================================================
# Step 3: Install Dependencies
#==============================================================================
print_header "Step 3: Installing Dependencies"

DEPS=(cmake qt@6 opencv)
for dep in "${DEPS[@]}"; do
    log_info "Checking $dep..."
    if brew list --formula | grep -q "^${dep}\$"; then
        log_success "$dep already installed"
    else
        log_info "Installing $dep..."
        brew install "$dep"
        log_success "$dep installed"
    fi
done

# Set Qt path
QT_PATH=$(brew --prefix qt@6)
export CMAKE_PREFIX_PATH="$QT_PATH:$CMAKE_PREFIX_PATH"
log_info "Qt6 path: $QT_PATH"

#==============================================================================
# Step 4: Build and Install cvutil Library
#==============================================================================
print_header "Step 4: Building cvutil Library"

CVUTIL_TEMP_DIR="/tmp/cvutil_build_$$"
log_info "Using temporary directory: $CVUTIL_TEMP_DIR"

# Clone cvutil
log_info "Cloning cvutil repository..."
rm -rf "$CVUTIL_TEMP_DIR"
git clone https://github.com/rootphenomicslab/cvutil.git "$CVUTIL_TEMP_DIR"
cd "$CVUTIL_TEMP_DIR"
log_success "cvutil cloned"

# Apply macOS patches
log_info "Applying macOS compatibility patches..."
for file in "$CVUTIL_TEMP_DIR/cvutil/CMakeLists.txt" \
            "$CVUTIL_TEMP_DIR/PluginManager/CMakeLists.txt" \
            "$CVUTIL_TEMP_DIR/RoiManager/CMakeLists.txt"; do
    if [ -f "$file" ]; then
        sed -i '' 's/elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")/elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")/g' "$file"
        log_info "Patched $(basename $file)"
    fi
done
log_success "Patches applied"

# Build cvutil
log_info "Configuring cvutil build..."
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$QT_PATH" \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DUSE_MIMALLOC=OFF \
      ..

log_info "Building cvutil (this may take a few minutes)..."
NUM_CORES=$(sysctl -n hw.ncpu)
make -j"$NUM_CORES"
log_success "cvutil built successfully"

# Install cvutil
log_info "Installing cvutil (requires sudo)..."
sudo make install
log_success "cvutil installed to /usr/local"

# Clean up
cd /
rm -rf "$CVUTIL_TEMP_DIR"
log_info "Cleaned up temporary files"

#==============================================================================
# Step 5: Build RhizoVision Explorer
#==============================================================================
print_header "Step 5: Building RhizoVision Explorer"

# Return to RhizoVision Explorer directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"
log_info "Working directory: $SCRIPT_DIR"

# Create build directory
BUILD_DIR="$SCRIPT_DIR/build/macos-intel"
log_info "Build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
log_info "Configuring RhizoVision Explorer..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="$QT_PATH" \
      -DENABLE_AVX512=OFF \
      -DENABLE_AVX2_FMA=ON \
      -G "Unix Makefiles" \
      ../..

# Build
log_info "Building RhizoVision Explorer (this may take a few minutes)..."
cmake --build . -j"$NUM_CORES"
log_success "RhizoVision Explorer built successfully!"

#==============================================================================
# Step 6: Create Launch Scripts
#==============================================================================
print_header "Step 6: Creating Launch Scripts"

# Create GUI launcher
cat > "$SCRIPT_DIR/launch_gui.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
open "$SCRIPT_DIR/build/macos-intel/RhizoVisionExplorer.app"
EOF
chmod +x "$SCRIPT_DIR/launch_gui.sh"
log_success "Created launch_gui.sh"

# Create CLI launcher
cat > "$SCRIPT_DIR/launch_cli.sh" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
"$SCRIPT_DIR/build/macos-intel/rv" "$@"
EOF
chmod +x "$SCRIPT_DIR/launch_cli.sh"
log_success "Created launch_cli.sh"

# Create symlinks in /usr/local/bin (optional)
log_info "Creating symlinks in /usr/local/bin (requires sudo)..."
sudo ln -sf "$BUILD_DIR/rv" /usr/local/bin/rhizovision-cli || log_warning "Could not create CLI symlink"
log_info "CLI available as: rhizovision-cli"

#==============================================================================
# Installation Complete!
#==============================================================================
print_header "Installation Complete! 🎉"

echo ""
log_success "RhizoVision Explorer has been successfully installed!"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "  📂 Build Location:"
echo "     $BUILD_DIR"
echo ""
echo "  🚀 How to Run:"
echo ""
echo "     GUI Application:"
echo "       ./launch_gui.sh"
echo "       or"
echo "       open $BUILD_DIR/RhizoVisionExplorer.app"
echo ""
echo "     CLI Application:"
echo "       ./launch_cli.sh --help"
echo "       or"
echo "       rhizovision-cli --help"
echo ""
echo "  📚 Documentation:"
echo "     See README_macOS.md for detailed usage instructions"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Test the installation
log_info "Testing installation..."
if [ -f "$BUILD_DIR/RhizoVisionExplorer.app/Contents/MacOS/RhizoVisionExplorer" ]; then
    log_success "GUI application found and ready"
else
    log_warning "GUI application not found at expected location"
fi

if [ -f "$BUILD_DIR/rv" ]; then
    log_success "CLI application found and ready"
    "$BUILD_DIR/rv" --version 2>/dev/null || log_info "CLI binary exists (version check skipped)"
else
    log_warning "CLI application not found at expected location"
fi

echo ""
log_success "All done! You can now use RhizoVision Explorer."
echo ""
