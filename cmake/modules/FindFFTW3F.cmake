#
# Copyright 2013-2022 Software Radio Systems Limited
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

# - Try to find fftw3f - the single-precision version of FFTW3
# Once done this will define
#  FFTW3F_FOUND - System has fftw3f
#  FFTW3F_INCLUDE_DIRS - The fftw3f include directories
#  FFTW3F_LIBRARIES - The libraries needed to use fftw3f
#  FFTW3F_DEFINITIONS - Compiler switches required for using fftw3f

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_FFTW3F "fftw3f >= 3.0")
set(FFTW3F_DEFINITIONS ${PC_FFTW3F_CFLAGS_OTHER})

find_path(FFTW3F_INCLUDE_DIR 
            NAMES fftw3.h
            HINTS ${PC_FFTW3F_INCLUDEDIR} ${PC_FFTW3F_INCLUDE_DIRS} $ENV{FFTW3_DIR}/include
            PATHS /usr/local/include 
                  /usr/include )

find_library(FFTW3F_STATIC_LIBRARY
            NAMES fftw3f.a libfftw3f.a libfftw3f-3.a
            HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} $ENV{FFTW3_DIR}/lib
            PATHS /usr/local/lib
                  /usr/lib)

find_library(FFTW3F_LIBRARY 
            NAMES fftw3f libfftw3f libfftw3f-3
            HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} $ENV{FFTW3_DIR}/lib
            PATHS /usr/local/lib
                  /usr/lib)

set(FFTW3F_LIBRARIES ${FFTW3F_LIBRARY} )
set(FFTW3F_STATIC_LIBRARIES ${FFTW3F_STATIC_LIBRARY} )
set(FFTW3F_INCLUDE_DIRS ${FFTW3F_INCLUDE_DIR} )

message(STATUS "FFTW3F LIBRARIES: " ${FFTW3F_LIBRARIES})
message(STATUS "FFTW3F STATIC LIBRARIES: " ${FFTW3F_STATIC_LIBRARIES})
message(STATUS "FFTW3F INCLUDE DIRS: " ${FFTW3F_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFTW3F_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FFTW3F DEFAULT_MSG FFTW3F_LIBRARY FFTW3F_INCLUDE_DIR)

mark_as_advanced(FFTW3F_INCLUDE_DIR FFTW3F_STATIC_LIBRARY FFTW3F_LIBRARY )
