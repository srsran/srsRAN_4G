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

if(NOT BLADERF_FOUND)
  pkg_check_modules (BLADERF_PKG libbladeRF)

  find_path(BLADERF_INCLUDE_DIRS 
    NAMES libbladeRF.h
    HINTS $ENV{BLADERF_DIR}/include
    PATHS ${BLADERF_PKG_INCLUDE_DIRS}
          /usr/include
          /usr/local/include
  )

  find_library(BLADERF_LIBRARIES 
    NAMES bladeRF
    HINTS $ENV{BLADERF_DIR}/lib
    PATHS ${BLADERF_PKG_LIBRARY_DIRS}
          /usr/lib
          /usr/local/lib
  )

if(BLADERF_INCLUDE_DIRS AND BLADERF_LIBRARIES)
  get_filename_component(bladerf_lib_dir ${BLADERF_LIBRARIES} DIRECTORY)
  CHECK_LIBRARY_EXISTS(bladeRF bladerf_get_board_name ${bladerf_lib_dir} BLADERF_VERSION_OK)
  if (BLADERF_VERSION_OK)
    set(BLADERF_FOUND TRUE CACHE INTERNAL "libbladeRF found")
    message(STATUS "Found libbladeRF: ${BLADERF_INCLUDE_DIRS}, ${BLADERF_LIBRARIES}")
  else (BLADERF_VERSION_OK)
    set(BLADERF_FOUND FALSE CACHE INTERNAL "libbladeRF found")
    message(STATUS "libbladeRF found but not compatible. Upgrade your driver or use SoapySDR.")
  endif (BLADERF_VERSION_OK)
else(BLADERF_INCLUDE_DIRS AND BLADERF_LIBRARIES)
  set(BLADERF_FOUND FALSE CACHE INTERNAL "libbladeRF found")
  message(STATUS "libbladeRF not found.")
endif(BLADERF_INCLUDE_DIRS AND BLADERF_LIBRARIES)

mark_as_advanced(BLADERF_LIBRARIES BLADERF_INCLUDE_DIRS)

endif(NOT BLADERF_FOUND)
