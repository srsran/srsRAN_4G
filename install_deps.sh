#!/bin/bash

# srsRAN 4G Dependency Installation Script
# For Ubuntu 25.04

set -e

echo "=== srsRAN 4G Dependencies Installation ==="
echo "System: $(lsb_release -d)"
echo "Date: $(date)"
echo

# Update package lists
echo "Updating package lists..."
sudo apt update

# Install mandatory dependencies
echo "Installing mandatory dependencies..."
sudo apt install -y \
    build-essential \
    cmake \
    libfftw3-dev \
    libmbedtls-dev \
    libboost-program-options-dev \
    libconfig++-dev \
    libsctp-dev

# Install optional dependencies
echo "Installing optional dependencies..."
sudo apt install -y \
    libpcsclite-dev \
    libdw-dev \
    libzmq3-dev \
    libboost-system-dev \
    libboost-test-dev \
    libboost-thread-dev \
    qtbase5-dev \
    libqwt-qt5-dev

# Verify installations
echo "Verifying installations..."
echo "Boost version:"
dpkg -l | grep libboost-program-options-dev

echo "MbedTLS version:"  
dpkg -l | grep libmbedtls-dev

echo "FFTW3 version:"
dpkg -l | grep libfftw3-dev

echo "CMake version:"
cmake --version

echo "Compiler versions:"
gcc --version | head -1
g++ --version | head -1

echo
echo "=== Dependencies installation complete ==="
echo "You can now proceed with building srsRAN 4G"