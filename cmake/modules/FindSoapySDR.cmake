#
# Copyright 2013-2022 Software Radio Systems Limited
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
