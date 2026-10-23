# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

# - Try to find mbedtls
#
# Once done this will define
#  MBEDTLS_FOUND        - System has mbedtls
#  MBEDTLS_INCLUDE_DIRS - The mbedtls include directories
#  MBEDTLS_LIBRARIES    - The mbedtls library

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(PC_MBEDTLS mbedtls)

if(NOT "$ENV{MBEDTLS_DIR}" STREQUAL "")
  set(_MBEDTLS_INCLUDE_HINTS "$ENV{MBEDTLS_DIR}/include")
  set(_MBEDTLS_LIB_HINTS     "$ENV{MBEDTLS_DIR}/lib")
endif()

#find Mbedtls
FIND_PATH(
    MBEDTLS_INCLUDE_DIRS
    NAMES mbedtls/md.h
    HINTS ${_MBEDTLS_INCLUDE_HINTS}
          ${PC_MBEDTLS_INCLUDEDIR}
          ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    MBEDTLS_LIBRARIES
    NAMES mbedcrypto
    HINTS ${_MBEDTLS_LIB_HINTS}
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
    HINTS ${_MBEDTLS_LIB_HINTS}
          ${PC_MBEDTLS_LIBDIR}
          ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

unset(_MBEDTLS_INCLUDE_HINTS)
unset(_MBEDTLS_LIB_HINTS)

message(STATUS "MBEDTLS LIBRARIES: " ${MBEDTLS_LIBRARIES})
message(STATUS "MBEDTLS STATIC LIBRARIES: " ${MBEDTLS_STATIC_LIBRARIES})
message(STATUS "MBEDTLS INCLUDE DIRS: " ${MBEDTLS_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MbedTLS DEFAULT_MSG MBEDTLS_LIBRARIES MBEDTLS_INCLUDE_DIRS)
MARK_AS_ADVANCED(MBEDTLS_LIBRARIES MBEDTLS_STATIC_LIBRARIES MBEDTLS_INCLUDE_DIRS)
