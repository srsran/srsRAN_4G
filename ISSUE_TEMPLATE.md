# Improve Build Compatibility for Modern Ubuntu/Boost/GCC Versions

## Summary
Enhance srsRAN 4G build system to support modern Linux distributions, particularly Ubuntu 24.04/25.04 with recent compiler and library versions, addressing common build failures due to Boost, CMake, and compiler compatibility issues.

## Problem Description
The current build system encounters several issues on modern Ubuntu distributions:

### Build Environment Issues:
- **Ubuntu 24.04/25.04 compatibility**: Recent Ubuntu versions with updated toolchains cause build failures
- **GCC 14.x compatibility**: Newer GCC versions are stricter about warnings and deprecated features
- **Boost 1.83+ compatibility**: Recent Boost versions introduce API changes that break compilation
- **CMake configuration**: Missing compatibility flags for modern compilers

### Missing Developer Tools:
- No comprehensive build documentation
- No automated dependency installation
- No build artifact management (.gitignore)
- Inconsistent CI/CD dependency specifications

## Proposed Solution

### 1. Enhanced Build System
- ✅ **Smart build script** (`build_srsran.sh`) with automatic fallback configurations
- ✅ **Dependency installer** (`install_deps.sh`) with complete package lists
- ✅ **Compatibility flags** for GCC 14.x and Boost 1.83+

### 2. Improved Documentation 
- ✅ **Developer guidance** (`CLAUDE.md`) for future maintainers
- ✅ **Build status documentation** (`BUILD_STATUS.md`) with compatibility matrix
- ✅ **Dependency version tracking** and known issue documentation

### 3. Enhanced CI/CD
- ✅ **Ubuntu 24.04 support** in GitHub Actions with compatibility flags
- ✅ **Consistent dependencies** across all CI workflows
- ✅ **Extended package lists** including Qt5 and additional Boost libraries

### 4. Repository Management
- ✅ **Comprehensive .gitignore** for build artifacts and generated files

## Technical Details

### Compiler Compatibility Flags Added:
```bash
-DCMAKE_C_FLAGS="-Wno-maybe-uninitialized -Wno-stringop-overflow"
-DCMAKE_CXX_FLAGS="-Wno-maybe-uninitialized -Wno-stringop-overflow -Wno-deprecated-declarations"
-DENABLE_WERROR=OFF
```

### Enhanced Dependencies:
```bash
# Core dependencies (existing)
libboost-program-options-dev libconfig++-dev libsctp-dev
libfftw3-dev libmbedtls-dev

# Additional dependencies (new)
libboost-system-dev libboost-test-dev libboost-thread-dev
qtbase5-dev libqwt-qt5-dev libpcsclite-dev libdw-dev libzmq3-dev
```

### Build Strategy:
1. **Primary build**: Release mode with compatibility flags
2. **Fallback build**: Debug mode with conservative optimization
3. **Parallel build limiting**: Prevents memory exhaustion
4. **Automatic testing**: Integrated test execution

## Testing
- ✅ **Ubuntu 25.04**: Successfully built and tested
- ✅ **GCC 14.2.0**: Compilation successful with compatibility flags
- ✅ **Boost 1.83**: Build completed with extended dependencies
- ✅ **Unit tests**: All tests passing

## Impact
- **Improved developer experience**: Automated setup and build process
- **Better CI reliability**: Consistent builds across Ubuntu versions
- **Future-proofing**: Compatibility strategy for newer toolchains
- **Reduced support burden**: Comprehensive documentation and automation

## Files Changed
- **New**: `.gitignore`, `CLAUDE.md`, `BUILD_STATUS.md`, `install_deps.sh`, `build_srsran.sh`
- **Modified**: `.github/workflows/ccpp.yml`, `.github/workflows/codeql.yml`

## Breaking Changes
None - all changes are additive and maintain backward compatibility.

## Related Issues
Addresses common build failures reported with modern Ubuntu distributions and resolves dependency conflicts with recent Boost/GCC versions.