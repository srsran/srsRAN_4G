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

# - Try to find sctp
#
# Once done this will define
#  SCTP_FOUND        - System has mbedtls
#  SCTP_INCLUDE_DIRS - The mbedtls include directories
#  SCTP_LIBRARIES    - The mbedtls library

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(PC_SCTP sctp)

#find Mbedtls
FIND_PATH(
    SCTP_INCLUDE_DIRS
    NAMES netinet/sctp.h
    HINTS ${PC_SCTP_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SCTP_LIBRARIES
    NAMES sctp
    HINTS ${PC_SCTP_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          /usr/lib/x86_64-linux-gnu/
)

message(STATUS "SCTP LIBRARIES: " ${SCTP_LIBRARIES})
message(STATUS "SCTP INCLUDE DIRS: " ${SCTP_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SCTP DEFAULT_MSG SCTP_LIBRARIES SCTP_INCLUDE_DIRS)
MARK_AS_ADVANCED(SCTP_LIBRARIES SCTP_INCLUDE_DIRS)
