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
# Prevent in-tree builds
########################################################################
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (SRSLTE)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
INCLUDE(SRSLTEVersion) #sets version information
INCLUDE(SRSLTEPackage) #setup cpack
INCLUDE(BuildMex)

include(CTest)
set( CTEST_MEMORYCHECK_COMMAND valgrind ) 
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    IMMEDIATE @ONLY)

########################################################################
# Options
########################################################################
option(DisableMEX "DisableMEX" OFF)

########################################################################
# Install Dirs
########################################################################
SET(RUNTIME_DIR bin)
SET(LIBRARY_DIR lib)
SET(INCLUDE_DIR include)
SET(MEX_DIR mex)
SET(DOC_DIR "share/doc/${CPACK_PACKAGE_NAME}")
SET(DATA_DIR share/${CPACK_PACKAGE_NAME})


IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
   MESSAGE(STATUS "Build type not specified: defaulting to Release.")
ENDIF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(${flag} ${have})
    if(${have})
        add_definitions(${flag})        
    endif(${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

IF(CMAKE_COMPILER_IS_GNUCXX)
    #Any additional flags for CXX
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


IF(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-write-strings -Wno-format-extra-args -Winline -Wno-unused-result -Wno-format -std=c99 -D_GNU_SOURCE -g")
    
  IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
  ELSE(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    FIND_PACKAGE(SSE)
    IF(HAVE_AVX) 
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    ELSEIF(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    ENDIF(HAVE_AVX)    
  ENDIF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  
  
  IF(NOT WIN32)
      ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
  ENDIF(NOT WIN32)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(MSVC)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/msvc) #missing headers
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    ADD_DEFINITIONS(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    ADD_DEFINITIONS( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    ADD_DEFINITIONS(/MP) #build with multiple processors
ENDIF(MSVC)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # The following is needed for weak linking to work under OS X
   SET(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")



########################################################################
# Create uninstall targets
########################################################################
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    

########################################################################
# Macro to add -fPIC property to static libs 
########################################################################
MACRO(SRSLTE_SET_PIC)
    IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
        SET_TARGET_PROPERTIES(${ARGV} PROPERTIES COMPILE_FLAGS -fPIC)
    ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
ENDMACRO(SRSLTE_SET_PIC)

########################################################################
# Print summary
########################################################################
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Building for version: ${VERSION}") 

########################################################################
# Add general includes and dependencies
########################################################################
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/srslte/include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/srslte/include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/mex/include)

########################################################################
# Add the subdirectories
########################################################################
ADD_SUBDIRECTORY(srslte)
ADD_SUBDIRECTORY(mex)
