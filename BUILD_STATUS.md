# srsRAN 4G Build Status and Dependencies

## Current System Environment

**Operating System:** Ubuntu 25.04 (Codename: plucky)
**Compiler:** GCC/G++ 14.2.0
**CMake:** 3.31.6

## Dependency Analysis

### Currently Installed
- `libfftw3-single3:amd64` - FFTW3 single precision library (3.3.10-2fakesync1build1)

### Missing Required Dependencies (Verified Available)

Based on the [official build requirements](https://docs.srsran.com/projects/4g/en/latest/general/source/1_installation.html), the following packages are available and need to be installed:

**Package Versions Available:**
- `libboost-program-options-dev`: 1.83.0.2ubuntu1 ⚠️ *Very recent Boost version*
- `libmbedtls-dev`: 3.6.2-3ubuntu1
- `libconfig++-dev`: 1.7.3-2  
- `libsctp-dev`: 1.0.21+dfsg-1
- `libfftw3-dev`: 3.3.10-2fakesync1build1
- `libzmq3-dev`: 4.3.5-1build2
- `libpcsclite-dev`: 2.3.1-1
- `libdw-dev`: 0.192-4ubuntu1

#### Mandatory Dependencies
```bash
sudo apt-get install build-essential cmake libfftw3-dev libmbedtls-dev \
libboost-program-options-dev libconfig++-dev libsctp-dev
```

**Package Breakdown:**
- `build-essential` - Essential build tools (gcc, g++, make, etc.)
- `cmake` - Build system (already installed but ensuring latest)
- `libfftw3-dev` - Fast Fourier Transform library development files
- `libmbedtls-dev` - Crypto library for security functions
- `libboost-program-options-dev` - Boost program options library
- `libconfig++-dev` - Configuration file parsing library
- `libsctp-dev` - Stream Control Transmission Protocol library

#### Optional Dependencies
```bash
sudo apt-get install libpcsclite-dev libdw-dev
```

- `libpcsclite-dev` - Smart card reader access (for USIM support)
- `libdw-dev` - Debug information library for better backtraces

#### RF Frontend Dependencies (Choose based on hardware)
```bash
# UHD (USRP support) - Recommended
sudo apt-get install libuhd-dev uhd-host

# SoapySDR (Multi-vendor SDR support)
sudo apt-get install libsoapysdr-dev soapysdr-tools

# BladeRF support
sudo apt-get install libbladerf-dev

# ZeroMQ (for simulation/testing)
sudo apt-get install libzmq3-dev
```

## Potential Compatibility Issues

### Ubuntu 25.04 Considerations
- **Very Recent Release**: Ubuntu 25.04 is a development/cutting-edge release
- **Package Availability**: Some packages may have newer versions that could introduce compatibility issues
- **Boost Version**: Ubuntu 25.04 likely has Boost 1.83+ which may have API changes

### Compiler Compatibility
- **GCC 14.2.0**: Very recent compiler version
- **Potential Issues**: Newer compilers are stricter about warnings and may treat some warnings as errors
- **Solution**: May need to use `-Wno-error` or specific warning suppressions

### CMake Version
- **CMake 3.31.6**: Very recent version
- **Compatibility**: Should be fully compatible (srsRAN requires CMake 3.5+)

## Build Strategy

1. **Install Core Dependencies First**
   - Start with mandatory packages to identify version conflicts early
   
2. **Handle Boost Compatibility**
   - Check Boost version after installation
   - May need to use older Boost version if compatibility issues arise
   
3. **RF Frontend Selection**
   - Start with ZeroMQ for simulation/testing (no hardware required)
   - Add UHD support if USRP hardware is available
   
4. **Build Configuration**
   - Use conservative CMake options initially
   - Enable specific features incrementally

## Next Steps

1. Install mandatory dependencies
2. Check for version compatibility issues
3. Configure build with minimal options
4. Address any compiler or linking errors
5. Gradually enable additional features

## Command Sequence

```bash
# Install mandatory dependencies
sudo apt update
sudo apt install build-essential cmake libfftw3-dev libmbedtls-dev \
    libboost-program-options-dev libconfig++-dev libsctp-dev

# Install optional debugging support
sudo apt install libpcsclite-dev libdw-dev

# Install ZeroMQ for testing/simulation
sudo apt install libzmq3-dev

# Create build directory
mkdir -p build && cd build

# Configure with conservative options
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_UHD=OFF \
      -DENABLE_BLADERF=OFF \
      -DENABLE_SOAPYSDR=OFF \
      -DENABLE_ZEROMQ=ON \
      ../

# Build
make -j$(nproc)

# Test
make test
```