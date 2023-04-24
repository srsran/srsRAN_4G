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

# - Try to find mbedtls
#
# Once done this will define
#  MBEDTLS_FOUND        - System has mbedtls
#  MBEDTLS_INCLUDE_DIRS - The mbedtls include directories
#  MBEDTLS_LIBRARIES    - The mbedtls library

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(PC_MBEDTLS mbedtls)

#find Mbedtls
FIND_PATH(
    MBEDTLS_INCLUDE_DIRS
    NAMES mbedtls/md.h
    HINTS $ENV{MBEDTLS_DIR}/include
          ${PC_MBEDTLS_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    MBEDTLS_LIBRARIES
    NAMES mbedcrypto
    HINTS $ENV{MBEDTLS_DIR}/lib
          ${PC_MBEDTLS_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

FIND_LIBRARY(
    MBEDTLS_STATIC_LIBRARIES
    NAMES libmbedcrypto.a
    HINTS $ENV{MBEDTLS_DIR}/lib
          ${PC_MBEDTLS_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

message(STATUS "MBEDTLS LIBRARIES: " ${MBEDTLS_LIBRARIES})
message(STATUS "MBEDTLS STATIC LIBRARIES: " ${MBEDTLS_STATIC_LIBRARIES})
message(STATUS "MBEDTLS INCLUDE DIRS: " ${MBEDTLS_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MbedTLS DEFAULT_MSG MBEDTLS_LIBRARIES MBEDTLS_INCLUDE_DIRS)
MARK_AS_ADVANCED(MBEDTLS_LIBRARIES MBEDTLS_STATIC_LIBRARIES MBEDTLS_INCLUDE_DIRS)
