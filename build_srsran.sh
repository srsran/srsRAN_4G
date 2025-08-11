#!/bin/bash

# srsRAN 4G Build Script with Compatibility Handling
# For Ubuntu 25.04 with GCC 14.2 and Boost 1.83

set -e

echo "=== srsRAN 4G Build Script ==="
echo "System: $(lsb_release -d)"
echo "Compiler: $(gcc --version | head -1)"
echo "CMake: $(cmake --version | head -1)"
echo

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run from srsRAN 4G root directory."
    exit 1
fi

# Create build directory
echo "Creating build directory..."
rm -rf build
mkdir build
cd build

# Configure with compatibility flags for GCC 14.2 and Boost 1.83
echo "Configuring build with compatibility options..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS="-Wno-maybe-uninitialized -Wno-stringop-overflow" \
    -DCMAKE_CXX_FLAGS="-Wno-maybe-uninitialized -Wno-stringop-overflow -Wno-deprecated-declarations" \
    -DENABLE_UHD=OFF \
    -DENABLE_BLADERF=OFF \
    -DENABLE_SOAPYSDR=OFF \
    -DENABLE_ZEROMQ=ON \
    -DENABLE_WERROR=OFF \
    -DENABLE_GUI=OFF \
    ../

if [ $? -ne 0 ]; then
    echo "Configuration failed. Trying with more conservative settings..."
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_C_FLAGS="-O1 -Wno-error" \
        -DCMAKE_CXX_FLAGS="-O1 -Wno-error" \
        -DENABLE_UHD=OFF \
        -DENABLE_BLADERF=OFF \
        -DENABLE_SOAPYSDR=OFF \
        -DENABLE_ZEROMQ=ON \
        -DENABLE_WERROR=OFF \
        -DENABLE_GUI=OFF \
        -DENABLE_ALL_TEST=OFF \
        ../
fi

# Build with limited parallelism to avoid memory issues
echo "Building srsRAN 4G..."
NPROC=$(nproc)
if [ $NPROC -gt 4 ]; then
    JOBS=4
else
    JOBS=$NPROC
fi

echo "Using $JOBS parallel jobs..."
make -j$JOBS

if [ $? -eq 0 ]; then
    echo
    echo "=== Build successful! ==="
    echo "Built applications:"
    ls -la srsue/src/srsue 2>/dev/null && echo "  ✓ srsUE"
    ls -la srsenb/src/srsenb 2>/dev/null && echo "  ✓ srsENB"  
    ls -la srsepc/src/srsepc 2>/dev/null && echo "  ✓ srsEPC"
    
    echo
    echo "Running basic tests..."
    make test || echo "Some tests failed, but build completed successfully"
    
    echo
    echo "To install system-wide, run: sudo make install"
    echo "To install config files for user, run: srsran_install_configs.sh user"
else
    echo
    echo "=== Build failed ==="
    echo "Check build logs above for errors."
    exit 1
fi