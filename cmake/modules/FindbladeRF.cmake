#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
