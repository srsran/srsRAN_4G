#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(NOT LIMESDR_FOUND)
  pkg_check_modules (LIMESDR_PKG LimeSuite)

  find_path(LIMESDR_INCLUDE_DIRS 
    NAMES LimeSuite.h
    HINTS $ENV{LIMESUITE_DIR}/include
    PATHS ${LIMESDR_PKG_INCLUDE_DIRS}
          /usr/include/lime
          /usr/local/include/lime
          $ENV{LIMESUITE_DIR}/include/lime
  )

  find_library(LIMESDR_LIBRARIES 
    NAMES LimeSuite
    HINTS $ENV{LIMESUITE_DIR}/lib
    PATHS ${LIMESDR_PKG_LIBRARY_DIRS}
          /usr/lib
          /usr/local/lib
  )

if(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND TRUE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "Found libLimeSuite: ${LIMESDR_INCLUDE_DIRS}, ${LIMESDR_LIBRARIES}")
else(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)
  set(LIMESDR_FOUND FALSE CACHE INTERNAL "libLimeSuite found")
  message(STATUS "libLimeSuite not found.")
endif(LIMESDR_INCLUDE_DIRS AND LIMESDR_LIBRARIES)

mark_as_advanced(LIMESDR_LIBRARIES LIMESDR_INCLUDE_DIRS)

endif(NOT LIMESDR_FOUND)
