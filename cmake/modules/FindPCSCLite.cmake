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

# - Find PCSC-Lite
# Find the native PCSC-Lite includes and library
#
#  PCSCLITE_INCLUDE_DIR - where to find winscard.h, wintypes.h, etc.
#  PCSCLITE_LIBRARIES   - List of libraries when using PCSC-Lite.
#  PCSCLITE_FOUND       - True if PCSC-Lite found.

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(PC_PCSCLITE libpcsclite)

IF(NOT PCSCLITE_FOUND)

FIND_PATH(PCSCLITE_INCLUDE_DIR
  NAMES winscard.h
  HINTS /usr/include/PCSC
        /usr/local/include/PCSC
        ${PC_PCSCLITE_INCLUDEDIR}
        ${PC_PCSCLITE_INCLUDE_DIRS}
        ${PC_PCSCLITE_INCLUDE_DIRS}/PCSC
        ${CMAKE_INSTALL_PREFIX}/include
)
FIND_LIBRARY(PCSCLITE_LIBRARY NAMES pcsclite libpcsclite PCSC
  HINTS ${PC_PCSCLITE_LIBDIR}
        ${PC_PCSCLITE_LIBRARY_DIRS}
        ${CMAKE_INSTALL_PREFIX}/lib
        ${CMAKE_INSTALL_PREFIX}/lib64
  PATHS /usr/local/lib
        /usr/local/lib64
        /usr/lib
        /usr/lib64
)

# handle the QUIETLY and REQUIRED arguments and set PCSCLITE_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCSCLite DEFAULT_MSG PCSCLITE_LIBRARY PCSCLITE_INCLUDE_DIR)

IF(PCSCLITE_FOUND)
  SET(PCSCLITE_LIBRARIES ${PCSCLITE_LIBRARY})
ELSE(PCSCLITE_FOUND)
  SET(PCSCLITE_LIBRARIES )
ENDIF(PCSCLITE_FOUND)

message(STATUS "PCSC LIBRARIES: " ${PCSCLITE_LIBRARY})
message(STATUS "PCSC INCLUDE DIRS: " ${PCSCLITE_INCLUDE_DIR})

MARK_AS_ADVANCED( PCSCLITE_LIBRARY PCSCLITE_INCLUDE_DIR )
ENDIF(NOT PCSCLITE_FOUND)
