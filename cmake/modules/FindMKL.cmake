# - Try to find mkl - the Intel Math Kernel Library
# Once done this will define
#  MKL_FOUND - System has mkl
#  MKL_INCLUDE_DIRS - The mkl include directories
#  MKL_LIBRARIES - The libraries needed to use mkl
#  MKL_DEFINITIONS - Compiler switches required for using mkl

find_path(MKL_INCLUDE_DIR
            NAMES mkl.h
            HINTS $ENV{MKL_DIR}/include
            PATHS)

find_path(MKL_FFTW_INCLUDE_DIR
            NAMES fftw3.h
            HINTS $ENV{MKL_DIR}/include/fftw
            PATHS)

find_library(MKL_LIBRARIES
            NAMES mkl_rt
            HINTS $ENV{MKL_DIR}/lib/intel64
            PATHS)

find_library(MKL_CORE
            NAMES libmkl_core.a
            HINTS $ENV{MKL_DIR}/lib/intel64
            PATHS)

find_library(MKL_ILP
            NAMES libmkl_intel_ilp64.a
            HINTS $ENV{MKL_DIR}/lib/intel64
            PATHS)

find_library(MKL_SEQ
            NAMES libmkl_sequential.a
            HINTS $ENV{MKL_DIR}/lib/intel64
            PATHS)

set(MKL_STATIC_LIBRARIES -Wl,--start-group ${MKL_CORE} ${MKL_ILP} ${MKL_SEQ} -Wl,--end-group -lpthread -lm -ldl)
set(MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR} ${MKL_FFTW_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MKL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(mkl  DEFAULT_MSG
                                  MKL_LIBRARIES MKL_CORE MKL_ILP MKL_SEQ MKL_INCLUDE_DIRS)

if(MKL_FOUND)
  MESSAGE(STATUS "Found MKL_INCLUDE_DIRS: ${MKL_INCLUDE_DIRS}" )
  MESSAGE(STATUS "Found MKL_LIBRARIES: ${MKL_LIBRARIES}" )
  MESSAGE(STATUS "Found MKL_STATIC_LIBRARIES: ${MKL_STATIC_LIBRARIES}" )
endif(MKL_FOUND)

mark_as_advanced(MKL_INCLUDE_DIR MKL_FFTW_INCLUDE_DIR MKL_LIBRARIES MKL_CORE MKL_ILP MKL_SEQ)
