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
# Add headers to cmake project (useful for IDEs)
########################################################################
SET(HEADERS_ALL "")
FILE(GLOB headers *)
FOREACH (_header ${headers})
  IF(IS_DIRECTORY ${_header})
    FILE(GLOB_RECURSE tmp "${_header}/*.h")
    LIST(APPEND HEADERS_ALL ${tmp})
  ENDIF(IS_DIRECTORY ${_header})
ENDFOREACH()
ADD_CUSTOM_TARGET (add_srslte_headers SOURCES ${HEADERS_ALL})

########################################################################
# Find Dependencies
########################################################################
FIND_PACKAGE(UHD)
FIND_PACKAGE(bladeRF)

IF(UHD_FOUND)
  INCLUDE_DIRECTORIES(${UHD_INCLUDE_DIRS})
  LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})
ENDIF(UHD_FOUND)

IF(BLADERF_FOUND)
  INCLUDE_DIRECTORIES(${BLADERF_INCLUDE_DIRS})
  LINK_DIRECTORIES(${BLADERF_LIBRARY_DIRS})
ENDIF(BLADERF_FOUND)

IF(BLADERF_FOUND OR UHD_FOUND)
  set(RF_FOUND TRUE CACHE INTERNAL "RF frontend found")
ELSE(BLADERF_FOUND OR UHD_FOUND)
  set(RF_FOUND FALSE CACHE INTERNAL "RF frontend found")
  add_definitions(-DDISABLE_RF)
ENDIF(BLADERF_FOUND OR UHD_FOUND)

include(CheckFunctionExistsMath)
IF(${DISABLE_VOLK})
  IF(${DISABLE_VOLK} EQUAL 0)
    FIND_PACKAGE(Volk)
  ELSE(${DISABLE_VOLK} EQUAL 0)
    MESSAGE(STATUS "VOLK library disabled (DISABLE_VOLK=1)")
  ENDIF(${DISABLE_VOLK} EQUAL 0)
ELSE(${DISABLE_VOLK})
  FIND_PACKAGE(Volk)
ENDIF(${DISABLE_VOLK})

IF(VOLK_FOUND)
  INCLUDE_DIRECTORIES(${VOLK_INCLUDE_DIRS})
  LINK_DIRECTORIES(${VOLK_LIBRARY_DIRS})
  MESSAGE(STATUS "   Compiling with VOLK SIMD library.")
ELSE(VOLK_FOUND)
  MESSAGE(STATUS "   VOLK SIMD library NOT found. Using generic implementation.")
ENDIF(VOLK_FOUND)

########################################################################
# Add subdirectories
########################################################################
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(examples)
