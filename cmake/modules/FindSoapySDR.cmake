# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

message(STATUS "FINDING SOAPY.")
if(NOT SOAPYSDR_FOUND)
  pkg_check_modules (SOAPYSDR_PKG SoapySDR)

  find_path(SOAPYSDR_INCLUDE_DIRS 
    NAMES SoapySDR/Device.h
    HINTS $ENV{SOAPY_DIR}/include
    PATHS ${SOAPYSDR_PKG_INCLUDE_DIRS}
          /usr/include
          /usr/local/include
  )

  find_library(SOAPYSDR_LIBRARIES 
    NAMES SoapySDR
    HINTS $ENV{SOAPY_DIR}/lib
    PATHS ${SOAPYSDR_PKG_LIBRARY_DIRS}
          /usr/lib
          /usr/local/lib
          /usr/lib/arm-linux-gnueabihf     
  )


if(SOAPYSDR_INCLUDE_DIRS AND SOAPYSDR_LIBRARIES)
  set(SOAPYSDR_FOUND TRUE CACHE INTERNAL "libSOAPYSDR found")
  message(STATUS "Found libSOAPYSDR: ${SOAPYSDR_INCLUDE_DIRS}, ${SOAPYSDR_LIBRARIES}")
else(SOAPYSDR_INCLUDE_DIRS AND SOAPYSDR_LIBRARIES)
  set(SOAPYSDR_FOUND FALSE CACHE INTERNAL "libSOAPYSDR found")
  message(STATUS "libSOAPYSDR not found.")
endif(SOAPYSDR_INCLUDE_DIRS AND SOAPYSDR_LIBRARIES)

mark_as_advanced(SOAPYSDR_LIBRARIES SOAPYSDR_INCLUDE_DIRS)

endif(NOT SOAPYSDR_FOUND)
