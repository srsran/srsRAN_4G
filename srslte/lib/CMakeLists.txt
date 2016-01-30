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
# Find Dependencies  
########################################################################
include(CheckFunctionExistsMath)

FIND_PACKAGE(SRSGUI)
FIND_PACKAGE(FFTW3F REQUIRED) # TODO: distribute kissfft instead

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
ENDIF(VOLK_FOUND)

IF(FFTW3F_FOUND)
  INCLUDE_DIRECTORIES(${FFTW3F_INCLUDE_DIRS})
  LINK_DIRECTORIES(${FFTW3F_LIBRARY_DIRS})
ENDIF(FFTW3F_FOUND)

IF(SRSGUI_FOUND)
  INCLUDE_DIRECTORIES(${SRSGUI_INCLUDE_DIRS})
  LINK_DIRECTORIES(${SRSGUI_LIBRARY_DIRS})
ENDIF(SRSGUI_FOUND)

########################################################################
# Recurse subdirectories and compile all source files into the same lib  
########################################################################
FILE(GLOB modules *) 
SET(SOURCES_ALL "")
FOREACH (_module ${modules})
  IF(IS_DIRECTORY ${_module})
    IF (NOT ${_module} MATCHES "rf")
      FILE(GLOB_RECURSE tmp "${_module}/src/*.c")
      LIST(APPEND SOURCES_ALL ${tmp})
    ENDIF (NOT ${_module} MATCHES "rf")
  ENDIF(IS_DIRECTORY ${_module})
ENDFOREACH()

ADD_LIBRARY(srslte SHARED ${SOURCES_ALL})
TARGET_LINK_LIBRARIES(srslte m ${FFTW3F_LIBRARIES})
SET_TARGET_PROPERTIES(srslte
  PROPERTIES VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR}
)

IF(RF_FOUND)
  # Include common RF files
  SET(SOURCES_RF "")
  LIST(APPEND SOURCES_RF "rf/src/rf_imp.c" "rf/src/rf_utils.c")

  IF (UHD_FOUND)
    INCLUDE_DIRECTORIES(${UHD_INCLUDE_DIRS})
    LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})
    add_definitions(-DENABLE_UHD)
    LIST(APPEND SOURCES_RF "rf/src/rf_uhd_imp.c" "rf/src/uhd_c_api.cpp")
  ENDIF (UHD_FOUND)

  IF (BLADERF_FOUND)
    INCLUDE_DIRECTORIES(${BLADERF_INCLUDE_DIRS})
    add_definitions(-DENABLE_BLADERF)
    LIST(APPEND SOURCES_RF "rf/src/rf_blade_imp.c")
  ENDIF (BLADERF_FOUND)

  ADD_LIBRARY(srslte_rf SHARED ${SOURCES_RF})
  SET_TARGET_PROPERTIES(srslte_rf
    PROPERTIES VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR}
  )
  
  IF (UHD_FOUND)
    TARGET_LINK_LIBRARIES(srslte_rf ${UHD_LIBRARIES})
  ENDIF (UHD_FOUND)

  IF (BLADERF_FOUND)
    TARGET_LINK_LIBRARIES(srslte_rf ${BLADERF_LIBRARIES})
  ENDIF (BLADERF_FOUND)

  INSTALL(TARGETS srslte_rf DESTINATION ${LIBRARY_DIR})
  SRSLTE_SET_PIC(srslte_rf)
ENDIF(RF_FOUND)

INSTALL(TARGETS srslte DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srslte)

IF(VOLK_FOUND)
  INCLUDE_DIRECTORIES(${VOLK_INCLUDE_DIRS})
  SET_TARGET_PROPERTIES(srslte PROPERTIES COMPILE_DEFINITIONS "${VOLK_DEFINITIONS}")    
  TARGET_LINK_LIBRARIES(srslte ${VOLK_LIBRARIES})
  MESSAGE(STATUS "   Compiling with VOLK SIMD library.")
ELSE(VOLK_FOUND)
  MESSAGE(STATUS "   VOLK SIMD library NOT found. Using generic implementation.")
ENDIF(VOLK_FOUND)

########################################################################
# Recurse subdirectories and find all directories with a CMakeLists.txt file in it  
########################################################################
FILE(GLOB_RECURSE cmakefiles CMakeLists.txt) 
FOREACH (_file ${cmakefiles})
  GET_FILENAME_COMPONENT(dir ${_file} PATH)
  IF (NOT ${dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    ADD_SUBDIRECTORY(${dir})
  ENDIF ()
ENDFOREACH()

