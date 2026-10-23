# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

if(NOT LIMESDR_FOUND)
  pkg_check_modules (LIMESDR_PKG LimeSuite)

  if(NOT "$ENV{LIMESUITE_DIR}" STREQUAL "")
    set(_LIMESUITE_INCLUDE_HINTS "$ENV{LIMESUITE_DIR}/include"
                                 "$ENV{LIMESUITE_DIR}/include/lime")
    set(_LIMESUITE_LIB_HINTS     "$ENV{LIMESUITE_DIR}/lib")
  endif()

  find_path(LIMESDR_INCLUDE_DIRS
    NAMES LimeSuite.h
    HINTS ${_LIMESUITE_INCLUDE_HINTS}
          ${LIMESDR_PKG_INCLUDE_DIRS}
    PATHS /usr/include/lime
          /usr/local/include/lime
  )

  find_library(LIMESDR_LIBRARIES
    NAMES LimeSuite
    HINTS ${_LIMESUITE_LIB_HINTS}
          ${LIMESDR_PKG_LIBRARY_DIRS}
    PATHS /usr/lib
          /usr/local/lib
  )

  unset(_LIMESUITE_INCLUDE_HINTS)
  unset(_LIMESUITE_LIB_HINTS)

if(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND TRUE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "Found libLimeSuite: ${LIMESDR_INCLUDE_DIRS}, ${LIMESDR_LIBRARIES}")
else(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND FALSE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "libLimeSuite not found.")
endif(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)

mark_as_advanced(LIMESDR_LIBRARIES LIMESDR_INCLUDE_DIRS)

endif(NOT LIMESDR_FOUND)
