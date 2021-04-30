#
# Copyright 2013-2021 Software Radio Systems Limited
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

# - Try to find polarssl
#
# Once done this will define
#  POLARSSL_FOUND        - System has polarssl
#  POLARSSL_INCLUDE_DIRS - The polarssl include directories
#  POLARSSL_LIBRARIES    - The polarssl library

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(PC_POLARSSL polarssl)

FIND_PATH(
    POLARSSL_INCLUDE_DIRS
    NAMES polarssl/version.h
    HINTS $ENV{POLARSSL_DIR}/include
          ${PC_POLARSSL_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    POLARSSL_LIBRARIES
    NAMES polarssl
    HINTS $ENV{POLARSSL_DIR}/lib
          ${PC_POLARSSL_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

FIND_LIBRARY(
    POLARSSL_STATIC_LIBRARIES
    NAMES libpolarssl.a
    HINTS $ENV{POLARSSL_DIR}/lib
          ${PC_POLARSSL_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

message(STATUS "POLARSSL LIBRARIES: " ${POLARSSL_LIBRARIES})
message(STATUS "POLARSSL STATIC LIBRARIES: " ${POLARSSL_STATIC_LIBRARIES})
message(STATUS "POLARSSL INCLUDE DIRS: " ${POLARSSL_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Polarssl DEFAULT_MSG POLARSSL_LIBRARIES POLARSSL_INCLUDE_DIRS)
MARK_AS_ADVANCED(POLARSSL_STATIC_LIBRARIES POLARSSL_LIBRARIES POLARSSL_INCLUDE_DIRS)
