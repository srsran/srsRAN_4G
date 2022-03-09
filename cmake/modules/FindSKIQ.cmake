#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

FIND_PACKAGE(PkgConfig REQUIRED)
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

FIND_LIBRARY(
        SKIQ_LIBRARY_TIRPC
        NAMES libtirpc.so.3
        PATHS /usr/local/lib
        /usr/lib64
        /usr/lib/epiq
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib64
        /usr/local/lib32
)

set(SKIQ_LIBRARIES ${SKIQ_LIBRARY} ${SKIQ_LIBRARY_GLIB} ${SKIQ_LIBRARY_USB} ${SKIQ_LIBRARY_TIRPC})

message(STATUS "SKIQ LIBRARIES " ${SKIQ_LIBRARIES})
message(STATUS "SKIQ INCLUDE DIRS " ${SKIQ_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SKIQ DEFAULT_MSG SKIQ_LIBRARIES SKIQ_INCLUDE_DIRS)
MARK_AS_ADVANCED(SKIQ_LIBRARIES SKIQ_INCLUDE_DIRS)

ENDIF(NOT SKIQ_FOUND)
