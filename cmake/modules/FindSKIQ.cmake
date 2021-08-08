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

INCLUDE(FindPkgConfig)
#PKG_CHECK_MODULES(SKIQ SKIQ)
IF(NOT SKIQ_FOUND)

FIND_PATH(
    SKIQ_INCLUDE_DIRS
    NAMES sidekiq_api.h
    HINTS $ENV{SKIQ_DIR}/inc
        $ENV{SKIQ_DIR}/sidekiq_core/inc
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SKIQ_LIBRARY
    NAMES sidekiq__x86_64.gcc
    HINTS $ENV{SKIQ_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib/x86_64-linux-gnu
          /usr/local/lib64
          /usr/local/lib32
)

FIND_LIBRARY(
        SKIQ_LIBRARY_GLIB
        NAMES libglib-2.0.a
        HINTS $ENV{SKIQ_DIR}/lib/support/x86_64.gcc/usr/lib/epiq
        PATHS /usr/local/lib
        /usr/lib
        /usr/lib/epiq
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        /usr/local/lib32
)

FIND_LIBRARY(
        SKIQ_LIBRARY_USB
        NAMES libusb-1.0.a
        HINTS $ENV{SKIQ_DIR}/lib/support/x86_64.gcc/usr/lib/epiq
        PATHS /usr/local/lib
        /usr/lib
        /usr/lib/epiq
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        /usr/local/lib32
)

set(SKIQ_LIBRARIES ${SKIQ_LIBRARY} ${SKIQ_LIBRARY_GLIB} ${SKIQ_LIBRARY_USB})

message(STATUS "SKIQ LIBRARIES " ${SKIQ_LIBRARIES})
message(STATUS "SKIQ INCLUDE DIRS " ${SKIQ_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SKIQ DEFAULT_MSG SKIQ_LIBRARIES SKIQ_INCLUDE_DIRS)
MARK_AS_ADVANCED(SKIQ_LIBRARIES SKIQ_INCLUDE_DIRS)

ENDIF(NOT SKIQ_FOUND)
