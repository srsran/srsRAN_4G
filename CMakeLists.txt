#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#


########################################################################
# Prevent in-tree builds
########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.5)
project( SRSRAN )
message( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
message( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
message( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(SRSRANVersion) #sets version information
include(SRSRANPackage) #setup cpack

include(CTest)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    IMMEDIATE @ONLY)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Generate CMake to include build information
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/modules/SRSRANbuildinfo.cmake.in
  ${CMAKE_BINARY_DIR}/SRSRANbuildinfo.cmake
)

########################################################################
# Options
########################################################################
option(ENABLE_SRSUE          "Build srsUE application"                  ON)
option(ENABLE_SRSENB         "Build srsENB application"                 ON)
option(ENABLE_SRSEPC         "Build srsEPC application"                 ON)
option(DISABLE_SIMD          "Disable SIMD instructions"                OFF)
option(AUTO_DETECT_ISA       "Autodetect supported ISA extensions"      ON)

option(ENABLE_GUI            "Enable GUI (using srsGUI)"                ON)
option(ENABLE_RF_PLUGINS     "Enable RF plugins"                        ON)
option(ENABLE_UHD            "Enable UHD"                               ON)
option(ENABLE_BLADERF        "Enable BladeRF"                           ON)
option(ENABLE_SOAPYSDR       "Enable SoapySDR"                          ON)
option(ENABLE_SKIQ           "Enable Sidekiq SDK"                       ON)
option(ENABLE_ZEROMQ         "Enable ZeroMQ"                            ON)
option(ENABLE_HARDSIM        "Enable support for SIM cards"             ON)

option(ENABLE_TTCN3          "Enable TTCN3 test binaries"               OFF)
option(ENABLE_ZMQ_TEST       "Enable ZMQ based E2E tests"               OFF)

option(BUILD_STATIC          "Attempt to statically link external deps" OFF)
option(RPATH                 "Enable RPATH"                             OFF)
option(ENABLE_ASAN           "Enable gcc/clang address sanitizer"       OFF)
option(ENABLE_GCOV           "Enable gcov"                              OFF)
option(ENABLE_MSAN           "Enable clang memory sanitizer"            OFF)
option(ENABLE_TSAN           "Enable clang thread sanitizer"            OFF)
option(ENABLE_TIDY           "Enable clang tidy"                        OFF)

option(USE_LTE_RATES         "Use standard LTE sampling rates"          OFF)
option(USE_MKL               "Use MKL instead of fftw"                  OFF)

option(ENABLE_TIMEPROF       "Enable time profiling"                    ON)

option(FORCE_32BIT           "Add flags to force 32 bit compilation"    OFF)

option(ENABLE_SRSLOG_TRACING "Enable event tracing using srslog"        OFF)
option(ASSERTS_ENABLED       "Enable srsRAN asserts"                    ON)
option(STOP_ON_WARNING       "Interrupt application on warning"         OFF)
option(ENABLE_WERROR         "Stop compilation on errors"               ON)

option(ENABLE_ALL_TEST       "Enable all unit/component test"           OFF)

# Users that want to try this feature need to make sure the lto plugin is
# loaded by bintools (ar, nm, ...). Older versions of bintools will not do
# it automatically so it is necessary to use the gcc wrappers of the compiler
# (gcc-ar, gcc-nm, ...).
option(BUILD_WITH_LTO        "Enable LTO (experimental)"                OFF)

if(NOT GCC_ARCH)
  if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(GCC_ARCH armv8-a CACHE STRING "GCC compile for specific architecture.")
    message(STATUS "Detected aarch64 processor")
  else(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(GCC_ARCH native CACHE STRING "GCC compile for specific architecture.")
  endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
endif()

# On RAM constrained (embedded) systems it may be useful to limit parallel compilation with, e.g. -DPARALLEL_COMPILE_JOBS=1
if (PARALLEL_COMPILE_JOBS)
  set(CMAKE_JOB_POOL_COMPILE compile_job_pool${CMAKE_CURRENT_SOURCE_DIR})
  string (REGEX REPLACE "[^a-zA-Z0-9]+" "_" CMAKE_JOB_POOL_COMPILE ${CMAKE_JOB_POOL_COMPILE})
  set_property(GLOBAL APPEND PROPERTY JOB_POOLS ${CMAKE_JOB_POOL_COMPILE}=${PARALLEL_COMPILE_JOBS})
  message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: Limiting compiler jobs to ${PARALLEL_COMPILE_JOBS}")
endif ()

if (ENABLE_SRSLOG_TRACING)
  add_definitions(-DENABLE_SRSLOG_EVENT_TRACE)
endif (ENABLE_SRSLOG_TRACING)

if (ASSERTS_ENABLED)
  add_definitions(-DASSERTS_ENABLED)
endif()

if (STOP_ON_WARNING)
  add_definitions(-DSTOP_ON_WARNING)
endif()

# Test for Atomics
include(CheckAtomic)
if(NOT HAVE_CXX_ATOMICS_WITHOUT_LIB OR NOT HAVE_CXX_ATOMICS64_WITHOUT_LIB)
    set(ATOMIC_LIBS "atomic")
endif()

########################################################################
# Find dependencies
########################################################################

# Enable ccache if not already enabled
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if(CCACHE_EXECUTABLE)
  foreach(LANG C CXX)
    if(NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
      message(STATUS "Enabling ccache for ${LANG}")
      set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
    endif()
  endforeach()
endif()

# Threads
find_package(Threads REQUIRED)

# FFT
if(USE_MKL)
  find_package(MKL REQUIRED)
  include_directories(${MKL_INCLUDE_DIRS})
  link_directories(${MKL_LIBRARY_DIRS})
  set(FFT_LIBRARIES "${MKL_STATIC_LIBRARIES}") # Static by default
else(USE_MKL)
  find_package(FFTW3F REQUIRED)
  if(FFTW3F_FOUND)
    include_directories(${FFTW3F_INCLUDE_DIRS})
    link_directories(${FFTW3F_LIBRARY_DIRS})
    if(BUILD_STATIC)
      set(FFT_LIBRARIES "${FFTW3F_STATIC_LIBRARIES}")
    else(BUILD_STATIC)
      set(FFT_LIBRARIES "${FFTW3F_LIBRARIES}")
    endif(BUILD_STATIC)
    message(STATUS "FFT_LIBRARIES: " ${FFT_LIBRARIES})
  endif(FFTW3F_FOUND)
endif(USE_MKL)

# Crypto
find_package(MbedTLS REQUIRED)
if (MBEDTLS_FOUND)
  set(SEC_INCLUDE_DIRS "${MBEDTLS_INCLUDE_DIRS}")
  if(BUILD_STATIC)
    set(SEC_LIBRARIES    "${MBEDTLS_STATIC_LIBRARIES}")
  else(BUILD_STATIC)
    set(SEC_LIBRARIES    "${MBEDTLS_LIBRARIES}")
  endif(BUILD_STATIC)
else(MBEDTLS_FOUND)
  message(FATAL_ERROR "mbedTLS is required to build srsRAN")
endif (MBEDTLS_FOUND)

# Hard-SIM support
if(ENABLE_HARDSIM)
  find_package(PCSCLite)
  if (PCSCLITE_FOUND)
    message(STATUS "Building with PCSC support.")
    add_definitions(-DHAVE_PCSC)
    set(HAVE_PCSC TRUE)
    include_directories(${PCSCLITE_INCLUDE_DIR})
    #link_directories(${PCSCLITE_LIBRARIES})
  endif (PCSCLITE_FOUND)
endif(ENABLE_HARDSIM)

# UHD
if(ENABLE_UHD)
  find_package(UHD)
  if(UHD_FOUND)
    include_directories(${UHD_INCLUDE_DIRS})
    link_directories(${UHD_LIBRARY_DIRS})
  endif(UHD_FOUND)
endif(ENABLE_UHD)

# SKIQ
if (ENABLE_SKIQ)
  find_package(SKIQ)
  if(SKIQ_FOUND)
    include_directories(${SKIQ_INCLUDE_DIRS})
    link_directories(${SKIQ_LIBRARY_DIRS})
  endif(SKIQ_FOUND)
endif (ENABLE_SKIQ)

# BladeRF
if(ENABLE_BLADERF)
  find_package(bladeRF)
  if(BLADERF_FOUND)
    include_directories(${BLADERF_INCLUDE_DIRS})
    link_directories(${BLADERF_LIBRARY_DIRS})
  endif(BLADERF_FOUND)
endif(ENABLE_BLADERF)

# Soapy
if(ENABLE_SOAPYSDR)
  find_package(SoapySDR)
  if(SOAPYSDR_FOUND)
    include_directories(${SOAPYSDR_INCLUDE_DIRS})
    link_directories(${SOAPYSDR_LIBRARY_DIRS})
  endif(SOAPYSDR_FOUND)
endif(ENABLE_SOAPYSDR)

# ZeroMQ
if(ENABLE_ZEROMQ)
  find_package(ZeroMQ)
  if(ZEROMQ_FOUND)
    include_directories(${ZEROMQ_INCLUDE_DIRS})
    link_directories(${ZEROMQ_LIBRARY_DIRS})
  endif(ZEROMQ_FOUND)
endif(ENABLE_ZEROMQ)

# TimeProf
if(ENABLE_TIMEPROF)
    add_definitions(-DENABLE_TIMEPROF)
endif(ENABLE_TIMEPROF)

if(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR ZEROMQ_FOUND OR SKIQ_FOUND)
  set(RF_FOUND TRUE CACHE INTERNAL "RF frontend found")
else(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR ZEROMQ_FOUND OR SKIQ_FOUND)
  set(RF_FOUND FALSE CACHE INTERNAL "RF frontend found")
  add_definitions(-DDISABLE_RF)
endif(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR ZEROMQ_FOUND OR SKIQ_FOUND)

# Boost
if(BUILD_STATIC)
  set(Boost_USE_STATIC_LIBS ON)
endif(BUILD_STATIC)

set(BOOST_REQUIRED_COMPONENTS
  program_options
)
if(UHD_FOUND) # Ubuntu 18.04 requires component 'system' to link UHD
  list(APPEND BOOST_REQUIRED_COMPONENTS "system")
endif(UHD_FOUND)
if(UNIX AND EXISTS "/usr/lib64")
  list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
endif(UNIX AND EXISTS "/usr/lib64")
set(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
find_package(Boost "1.35" COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
else(Boost_FOUND)
  message(FATAL_ERROR "Boost required to build srsRAN")
endif (Boost_FOUND)

# srsGUI
if(ENABLE_GUI)
  find_package(SRSGUI)
  if(SRSGUI_FOUND)
    add_definitions(-DENABLE_GUI)
    include_directories(${SRSGUI_INCLUDE_DIRS})
    link_directories(${SRSGUI_LIBRARY_DIRS})
  endif(SRSGUI_FOUND)
endif(ENABLE_GUI)

if (ENABLE_TTCN3)
  find_package(RapidJSON REQUIRED)
  add_definitions(-DENABLE_TTCN3)
  include_directories(${RAPIDJSON_INCLUDE_DIRS})
  link_directories(${RAPIDJSON_LIBRARY_DIRS})
  message(STATUS "Building with TTCN3 binaries")
endif (ENABLE_TTCN3)

# Backward-cpp
find_package(Backward)
if(Backward_FOUND)
  if(BACKWARD_HAS_EXTERNAL_LIBRARIES)
    add_definitions(-DHAVE_BACKWARD)
    message(STATUS "Building with backward-cpp support")
  else (BACKWARD_HAS_EXTERNAL_LIBRARIES)
    message(STATUS "Backward-cpp found, but external libraries are missing.")
  endif()
endif()

########################################################################
# Install Dirs
########################################################################
if (NOT CMAKE_INSTALL_LIBDIR)
    include(GNUInstallDirs)
endif (NOT CMAKE_INSTALL_LIBDIR)

# Fall back to just "lib" if the item provided by GNUInstallDirs doesn't exist
if (NOT EXISTS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    message(STATUS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} does not exist. Defaulting install location to ${CMAKE_INSTALL_PREFIX}/lib.")
    set(CMAKE_INSTALL_LIBDIR lib)
endif()

set(RUNTIME_DIR bin)
set(LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR})
set(INCLUDE_DIR include)
set(DOC_DIR "share/doc/${CPACK_PACKAGE_NAME}")
set(DATA_DIR share/${CPACK_PACKAGE_NAME})

# Auto-generate config install helper and mark for installation
configure_file(
        ${PROJECT_SOURCE_DIR}/cmake/modules/SRSRAN_install_configs.sh.in
        ${CMAKE_BINARY_DIR}/srsran_install_configs.sh
)
install(PROGRAMS ${CMAKE_BINARY_DIR}/srsran_install_configs.sh DESTINATION ${RUNTIME_DIR})

# Disables the project to build when calling "make install"
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if(${have})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endif(${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

macro(ADD_C_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCCompilerFlag)
    check_c_compiler_flag(${flag} ${have})
    if(${have})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endif(${have})
endmacro(ADD_C_COMPILER_FLAG_IF_AVAILABLE)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-comment -Wno-reorder -Wno-unused-variable -Wtype-limits -std=c++14 -fno-strict-aliasing")

  ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-Wno-unused-but-set-variable" HAVE_WNO_UNUSED_BUT_SET_VARIABLE)

  if (AUTO_DETECT_ISA)
    find_package(SSE)
  endif (AUTO_DETECT_ISA)

  ADD_C_COMPILER_FLAG_IF_AVAILABLE("-march=${GCC_ARCH}" HAVE_MARCH_${GCC_ARCH})
  ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-march=${GCC_ARCH}" HAVE_MARCH_${GCC_ARCH})

  if (HAVE_AVX2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -mavx2 -DLV_HAVE_AVX2 -DLV_HAVE_AVX -DLV_HAVE_SSE")
  else (HAVE_AVX2)
    if(HAVE_AVX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    elseif(HAVE_SSE)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    endif(HAVE_AVX)
  endif (HAVE_AVX2)

  # Do not hide symbols in debug mode so backtraces can display function info.
  if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(NOT WIN32)
      ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN_CXX)
    endif(NOT WIN32)
  endif(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  if(FORCE_32BIT)
    ADD_C_COMPILER_FLAG_IF_AVAILABLE("-m32" HAVE_M32)
    ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-m32" HAVE_M32)
    set(CMAKE_SHARED_LINKER_FLAGS "-m32")
  endif(FORCE_32BIT)

endif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")

ADD_C_COMPILER_FLAG_IF_AVAILABLE("-Werror=incompatible-pointer-types" HAVE_ERROR_INCOMPATIBLE)

if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-comment -Wno-write-strings -Wno-unused-result -Wformat -Wmissing-field-initializers -Wtype-limits -std=c99 -fno-strict-aliasing -D_GNU_SOURCE")

  ADD_C_COMPILER_FLAG_IF_AVAILABLE("-Wno-unused-but-set-variable" HAVE_WNO_UNUSED_BUT_SET_VARIABLE)
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -O0 -DDEBUG_MODE -DBUILD_TYPE_DEBUG")
  else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -DBUILD_TYPE_RELWITHDEBINFO")
    else(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fno-trapping-math -fno-math-errno -DBUILD_TYPE_RELEASE")
      if(BUILD_WITH_LTO)
        ADD_C_COMPILER_FLAG_IF_AVAILABLE(-flto HAVE_FLTO)
        ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-flto HAVE_FLTO)
      endif(BUILD_WITH_LTO)
    endif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  if (USE_LTE_RATES)
    message(STATUS "Using standard LTE sampling rates")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFORCE_STANDARD_RATE")
  endif (USE_LTE_RATES)

  if (AUTO_DETECT_ISA)
    find_package(SSE)
  endif (AUTO_DETECT_ISA)
  if (HAVE_AVX2)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -mavx2 -DLV_HAVE_AVX2 -DLV_HAVE_AVX -DLV_HAVE_SSE")
  else (HAVE_AVX2)
    if(HAVE_AVX)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    elseif(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    endif(HAVE_AVX)
  endif (HAVE_AVX2)

  if (HAVE_FMA)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfma -DLV_HAVE_FMA")
  endif (HAVE_FMA)

  if (HAVE_AVX512)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f -mavx512cd -mavx512bw -mavx512dq -DLV_HAVE_AVX512")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512cd -mavx512bw -mavx512dq -DLV_HAVE_AVX512")
  endif(HAVE_AVX512)

  if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast -funroll-loops")
    endif(HAVE_SSE)
  endif(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DIS_ARM -DHAVE_NEON")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIS_ARM")
    message(STATUS "Detected ARM processor")
    set(HAVE_NEON "True")
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
      set(HAVE_NEONv8 "True")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_NEONv8")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_NEONv8")
    endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mfpu=neon")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=hard -mfpu=neon")
    endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
  else(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(HAVE_NEON "False")
  endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_C_FLAGS})

  if(NOT HAVE_SSE AND NOT HAVE_NEON AND NOT DISABLE_SIMD)
    message(FATAL_ERROR "no SIMD instructions found")
  endif(NOT HAVE_SSE AND NOT HAVE_NEON AND NOT DISABLE_SIMD)

  # Do not hide symbols in debug mode so backtraces can display function info.
  if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if(NOT WIN32)
      ADD_C_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN_C)
    endif(NOT WIN32)
  endif(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

  if ((ENABLE_ASAN AND ENABLE_MSAN) OR (ENABLE_ASAN AND ENABLE_TSAN) OR (ENABLE_MSAN AND ENABLE_TSAN))
    message(FATAL_ERROR "ASAN, MSAN and TSAN cannot be enabled at the same time.")
  endif ()

  if (ENABLE_ASAN)
    # Note: When using ASAN, we need to ensure the use of RPATH instead of RUNPATH via "-Wl,--disable-new-dtags"
    # While RPATH is default, some systems (e.g. Ubuntu 18.04 and 20.04) use RUNPATH by default, which is non-transitive.
    # Since ASAN intercepts dlopen(), by which it replaces the dynamic string token "$ORIGIN" to its own location,
    # the RF plugins won't be found by "libsrsran_rf.so" when using ASAN + RUNPATH in the top-level executable.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -Wl,--disable-new-dtags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -Wl,--disable-new-dtags")
  endif (ENABLE_ASAN)

  if (ENABLE_TSAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -DHAVE_TSAN")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
  endif (ENABLE_TSAN)

  if (ENABLE_MSAN AND CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins -fPIE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer -fsanitize-memory-track-origins -fPIE")
  endif (ENABLE_MSAN AND CMAKE_C_COMPILER_ID MATCHES "Clang")

  if (ENABLE_GCOV)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  endif (ENABLE_GCOV)

endif(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # The following is needed for weak linking to work under OS X
   set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Add colored output when using the Ninja generator
if("Ninja" STREQUAL ${CMAKE_GENERATOR})
  ADD_C_COMPILER_FLAG_IF_AVAILABLE("-fdiagnostics-color=always" HAVE_DIAGNOSTIC_COLOR_C)
  ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-fdiagnostics-color=always" HAVE_DIAGNOSTIC_COLOR_CXX)
endif()

# Add -Werror to C/C++ flags for newer compilers
if(ENABLE_WERROR AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# GCC >= 11.2.0: Disable analysis on "maybe-uninitialized" variables because of the high false-positive rate
if(CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-maybe-uninitialized")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
  # Increase inlining limit to allow gcc compilation on e.g. RPi2
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --param large-function-growth=1600")
endif(CMAKE_C_COMPILER_ID MATCHES "GNU")

if (EXTRA_TERM_TIMEOUT_S)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSRSRAN_TERM_TIMEOUT_S=${EXTRA_TERM_TIMEOUT_S}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSRSRAN_TERM_TIMEOUT_S=${EXTRA_TERM_TIMEOUT_S}")
endif (EXTRA_TERM_TIMEOUT_S)

message(STATUS "CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")


########################################################################
# clang-tidy check
########################################################################
if(ENABLE_TIDY)
  find_program(
          CLANG_TIDY_BIN
          NAMES "clang-tidy"
          DOC "Path to clang-tidy executable"
  )
  if(NOT CLANG_TIDY_BIN)
    message(STATUS "clang-tidy not found.")
  else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_BIN}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_BIN}" "-checks=*,-clang-analyzer-alpha.*,-modernize-*,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-bounds-constant-array-index")
  endif()
endif(ENABLE_TIDY)

########################################################################
# Create uninstall targets
########################################################################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

########################################################################
# Create optional target to build osmo-gsm-tester trial
########################################################################
add_custom_target(trial
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build_trial.sh)

########################################################################
# Add -fPIC property to all targets
########################################################################
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION}")

########################################################################
# Ctest function helpers
########################################################################

function(add_lte_test)
  add_test(${ARGN})
  set(TNAME ${ARGV0})
  if(${TNAME} STREQUAL NAME)
    set(TNAME ${ARGV1})
  endif()
  set_tests_properties(${TNAME} PROPERTIES LABELS "lte;${CTEST_LABELS}")
endfunction()

function(add_nr_test)
  add_test(${ARGN})
  set(TNAME ${ARGV0})
  if(${TNAME} STREQUAL NAME)
    set(TNAME ${ARGV1})
  endif()
  set_tests_properties(${TNAME} PROPERTIES LABELS "nr;${CTEST_LABELS}")
endfunction()

function(add_nr_advanced_test)
  if (NOT ${ENABLE_ALL_TEST})
    return()
  endif()
  add_test(${ARGN})
  set(TNAME ${ARGV0})
  if(${TNAME} STREQUAL NAME)
    set(TNAME ${ARGV1})
  endif()
  set_tests_properties(${TNAME} PROPERTIES LABELS "nr;${CTEST_LABELS}")
endfunction()

########################################################################
# Add general includes and dependencies
########################################################################
include_directories(${PROJECT_BINARY_DIR}/lib/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/include)

########################################################################
# Add headers to cmake project (useful for IDEs)
########################################################################
set(HEADERS_ALL "")
file(GLOB headers *)
foreach(_header ${headers})
    if(IS_DIRECTORY ${_header})
        file(GLOB_RECURSE tmp "${_header}/*.h")
        list(APPEND HEADERS_ALL ${tmp})
    endif(IS_DIRECTORY ${_header})
endforeach()
add_custom_target(add_srsran_headers SOURCES ${HEADERS_ALL})

########################################################################
# Add the subdirectories
########################################################################
add_subdirectory(lib)

if(RF_FOUND)
  if(ENABLE_SRSUE)
    message(STATUS "Building with srsUE")
    add_subdirectory(srsue)
  else(ENABLE_SRSUE)
    message(STATUS "srsUE build disabled")
  endif(ENABLE_SRSUE)

  if(ENABLE_SRSENB)
    message(STATUS "Building with srsENB/srsGNB")
    add_subdirectory(srsenb)
    add_subdirectory(srsgnb)
  else(ENABLE_SRSENB)
    message(STATUS "srsENB/srsGNB build disabled")
  endif(ENABLE_SRSENB)
else(RF_FOUND)
  message(STATUS "srsUE and srsENB/srsGNB builds disabled due to missing RF driver")
endif(RF_FOUND)

if(ENABLE_SRSEPC)
  message(STATUS "Building with srsEPC")
  add_subdirectory(srsepc)
else(ENABLE_SRSEPC)
  message(STATUS "srsEPC build disabled")
endif(ENABLE_SRSEPC)

add_subdirectory(test)
