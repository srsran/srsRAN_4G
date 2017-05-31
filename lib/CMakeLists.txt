#
# Copyright 2013-2015 Software Radio Systems Limited
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# Install headers
########################################################################
INSTALL(DIRECTORY include/ 
        DESTINATION "${INCLUDE_DIR}" 
        FILES_MATCHING PATTERN "*.h"
)

########################################################################
# Find Dependencies
########################################################################

find_package(MKL)
if(MKL_FOUND)
  include_directories(${MKL_INCLUDE_DIRS})
  link_directories(${MKL_LIBRARY_DIRS})
else(MKL_FOUND)
  find_package(FFTW3F REQUIRED)
  if(FFTW3F_FOUND)
    include_directories(${FFTW3F_INCLUDE_DIRS})
    link_directories(${FFTW3F_LIBRARY_DIRS})
  endif(FFTW3F_FOUND)
endif(MKL_FOUND)

find_package(UHD)
if(UHD_FOUND)
  include_directories(${UHD_INCLUDE_DIRS})
  link_directories(${UHD_LIBRARY_DIRS})
endif(UHD_FOUND)

if(NOT DisableBladeRF) 
  find_package(bladeRF)
  if(BLADERF_FOUND)
    include_directories(${BLADERF_INCLUDE_DIRS})
    link_directories(${BLADERF_LIBRARY_DIRS})
  endif(BLADERF_FOUND)
endif(NOT DisableBladeRF)

find_package(SoapySDR)
if(SOAPYSDR_FOUND)
  include_directories(${SOAPYSDR_INCLUDE_DIRS})
  link_directories(${SOAPYSDR_LIBRARY_DIRS})
endif(SOAPYSDR_FOUND)


find_package(LimeSDR)
if(LIMESDR_FOUND)
  include_directories(${LIMESDR_INCLUDE_DIRS})
  link_directories(${LIMESDR_LIBRARY_DIRS})
endif(LIMESDR_FOUND)



if(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR LIMESDR_FOUND)
  set(RF_FOUND TRUE CACHE INTERNAL "RF frontend found")
else(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR LIMESDR_FOUND)
  set(RF_FOUND FALSE CACHE INTERNAL "RF frontend found")
  add_definitions(-DDISABLE_RF)
endif(BLADERF_FOUND OR UHD_FOUND OR SOAPYSDR_FOUND OR LIMESDR_FOUND)

include(CheckFunctionExistsMath)
if(${DISABLE_VOLK})
  if(${DISABLE_VOLK} EQUAL 0)
    find_package(Volk)
  else(${DISABLE_VOLK} EQUAL 0)
    message(STATUS "VOLK library disabled (DISABLE_VOLK=1)")
  endif(${DISABLE_VOLK} EQUAL 0)
else(${DISABLE_VOLK})
  find_package(Volk)
endif(${DISABLE_VOLK})

if(VOLK_FOUND)
  include_directories(${VOLK_INCLUDE_DIRS})
  link_directories(${VOLK_LIBRARY_DIRS})
  message(STATUS "   Compiling with VOLK SIMD library.")
else(VOLK_FOUND)
  message(STATUS "   VOLK SIMD library NOT found. Using generic implementation.")
endif(VOLK_FOUND)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(examples)
add_subdirectory(test)
