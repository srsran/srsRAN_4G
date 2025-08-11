# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

srsRAN 4G is an open-source 4G software radio suite that provides a complete LTE/5G software stack. The project includes three main applications: srsUE (User Equipment), srsENB (evolved NodeB), and srsEPC (Evolved Packet Core), along with a comprehensive PHY/MAC/RLC/PDCP/RRC protocol stack library.

## Build System and Common Commands

This project uses **CMake** as the build system. The repository requires out-of-tree builds.

### Essential Build Commands
```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build all components
make -j$(nproc)

# Install binaries and libraries
make install

# Run tests
make test
# or specific test categories
ctest -L lte  # LTE tests only
ctest -L nr   # NR/5G tests only

# Build with specific options
cmake -DENABLE_SRSUE=ON -DENABLE_SRSENB=ON -DENABLE_SRSEPC=ON ..
```

### Development and Testing
```bash
# End-to-end LTE testing (requires root privileges)
cd test
sudo ./run_lte.sh ~/build 50 1

# ZMQ-based E2E tests
cmake -DENABLE_ZMQ_TEST=True ..
cd test && sudo make test

# Build trial package
make trial
```

### Configuration and Examples
- Configuration examples: `srsue/ue.conf.example`, `srsenb/enb.conf.example`, `srsepc/epc.conf.example`
- Network setup helper: `srsepc/srsepc_if_masq.sh`

## Architecture Overview

### Core Applications
- **srsUE**: Complete UE implementation with LTE and prototype 5G support
- **srsENB**: eNodeB implementation for LTE networks
- **srsEPC**: Lightweight 4G core (MME, HSS, S/P-GW)
- **srsGNB**: 5G gNodeB implementation (NR support)

### Library Structure (`lib/`)
- **PHY Layer** (`lib/src/phy/`): Physical layer implementations including:
  - Channel estimation, sync, OFDM, FEC coding
  - RF frontend abstractions (UHD, BladeRF, SoapySDR, ZeroMQ)
  - LTE and NR physical channels (PDSCH, PUSCH, PUCCH, etc.)
- **MAC Layer** (`lib/src/mac/`): Medium access control
- **RLC Layer** (`lib/src/rlc/`): Radio link control (AM, UM, TM modes)
- **PDCP Layer** (`lib/src/pdcp/`): Packet data convergence protocol
- **Common** (`lib/src/common/`): Shared utilities, security, logging
- **ASN.1** (`lib/src/asn1/`): Protocol message encoding/decoding
- **srslog** (`lib/src/srslog/`): High-performance logging framework

### Protocol Stack Layers
```
Application Layer
    |
PDCP (Packet Data Convergence Protocol)
    |
RLC (Radio Link Control) - AM/UM/TM
    |
MAC (Medium Access Control)
    |
PHY (Physical Layer) - LTE/NR
    |
RF Hardware (UHD/BladeRF/SoapySDR/ZMQ)
```

## Key Build Options

Critical CMake options that affect functionality:
- `ENABLE_SRSUE/SRSENB/SRSEPC`: Enable specific applications
- `ENABLE_UHD/BLADERF/SOAPYSDR/ZEROMQ`: RF hardware support
- `ENABLE_GUI`: srsGUI visualization support  
- `ENABLE_TTCN3`: Test framework integration
- `USE_LTE_RATES`: Standard LTE sampling rates
- `ENABLE_ASAN/TSAN/MSAN`: Sanitizers for debugging

## Testing Framework

- **Unit Tests**: Component-level tests in each module's `test/` directory
- **Integration Tests**: End-to-end LTE testing with network namespaces
- **TTCN-3 Integration**: Standards-compliant protocol testing
- **Benchmark Tests**: Performance evaluation tools

## Important File Locations

- **Main Applications**: `srsue/src/main.cc`, `srsenb/src/main.cc`, `srsepc/src/main.cc`
- **Protocol Headers**: `lib/include/srsran/`
- **Configuration Examples**: `*/*.conf.example` files
- **Test Scripts**: `test/run_lte.sh` for E2E testing

## Development Notes

- The codebase uses C++14 standard with C99 for PHY layer components
- SIMD optimizations available for x86 (SSE/AVX) and ARM (NEON)
- Thread-safe design with worker pool patterns for real-time processing
- Extensive use of interfaces for modularity between layers
- Support for both file-based and real-time RF operation via ZeroMQ