# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

# - Try to find mkl - the Intel Math Kernel Library
# Once done this will define
#  MKL_FOUND - System has mkl
#  MKL_INCLUDE_DIRS - The mkl include directories
#  MKL_LIBRARIES - The libraries needed to use mkl
#  MKL_DEFINITIONS - Compiler switches required for using mkl

if(NOT "$ENV{MKL_DIR}" STREQUAL "")
  set(_MKL_INCLUDE_HINTS      "$ENV{MKL_DIR}/include")
  set(_MKL_FFTW_INCLUDE_HINTS "$ENV{MKL_DIR}/include/fftw")
  set(_MKL_LIB_HINTS          "$ENV{MKL_DIR}/lib/intel64")
endif()

find_path(MKL_INCLUDE_DIR
            NAMES mkl.h
            HINTS ${_MKL_INCLUDE_HINTS}
                  /opt/intel/oneapi/mkl/latest/include
                  /opt/intel/mkl/include
                  /usr/include/mkl
            PATHS)

find_path(MKL_FFTW_INCLUDE_DIR
            NAMES fftw3.h
            HINTS ${_MKL_FFTW_INCLUDE_HINTS}
                  /opt/intel/oneapi/mkl/latest/include/fftw
                  /opt/intel/mkl/include/fftw
                  /usr/include/mkl/fftw
            PATHS)

find_library(MKL_LIBRARIES
            NAMES mkl_rt
            HINTS ${_MKL_LIB_HINTS}
                  /opt/intel/oneapi/mkl/latest/lib/intel64
                  /opt/intel/mkl/lib/intel64
            PATHS)

find_library(MKL_CORE
            NAMES libmkl_core.a
            HINTS ${_MKL_LIB_HINTS}
                  /opt/intel/oneapi/mkl/latest/lib/intel64/
                  /opt/intel/mkl/lib/intel64
            PATHS)

find_library(MKL_ILP
            NAMES libmkl_intel_ilp64.a
            HINTS ${_MKL_LIB_HINTS}
                  /opt/intel/oneapi/mkl/latest/lib/intel64/
                  /opt/intel/mkl/lib/intel64
            PATHS)

find_library(MKL_SEQ
            NAMES libmkl_sequential.a
            HINTS ${_MKL_LIB_HINTS}
                  /opt/intel/oneapi/mkl/latest/lib/intel64/
                  /opt/intel/mkl/lib/intel64
            PATHS)

unset(_MKL_INCLUDE_HINTS)
unset(_MKL_FFTW_INCLUDE_HINTS)
unset(_MKL_LIB_HINTS)

set(MKL_STATIC_LIBRARIES -Wl,--start-group ${MKL_CORE} ${MKL_ILP} ${MKL_SEQ} -Wl,--end-group -lpthread -lm -ldl)
set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR} ${MKL_FFTW_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MKL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(MKL  DEFAULT_MSG
                                  MKL_LIBRARIES MKL_CORE MKL_ILP MKL_SEQ MKL_INCLUDE_DIRS)

if(MKL_FOUND)
  MESSAGE(STATUS "Found MKL_INCLUDE_DIRS: ${MKL_INCLUDE_DIRS}" )
  MESSAGE(STATUS "Found MKL_LIBRARIES: ${MKL_LIBRARIES}" )
  MESSAGE(STATUS "Found MKL_STATIC_LIBRARIES: ${MKL_STATIC_LIBRARIES}" )
endif(MKL_FOUND)

mark_as_advanced(MKL_INCLUDE_DIR MKL_FFTW_INCLUDE_DIR MKL_LIBRARIES MKL_CORE MKL_ILP MKL_SEQ)
