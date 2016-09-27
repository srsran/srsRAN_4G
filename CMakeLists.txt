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
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(SRSLTEVersion) #sets version information
include(SRSLTEPackage) #setup cpack
include(BuildMex)

include(CTest)
set( CTEST_MEMORYCHECK_COMMAND valgrind ) 
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    IMMEDIATE @ONLY)

########################################################################
# Options
########################################################################
option(DisableMEX "DisableMEX" ON)

########################################################################
# Install Dirs
########################################################################
if (NOT CMAKE_INSTALL_LIBDIR)
    include(GNUInstallDirs)
endif (NOT CMAKE_INSTALL_LIBDIR)

# Fall back to just "lib" if the item provided by GNUInstallDirs doesn't exist
if (NOT EXISTS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    message(STATUS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} does not exist. Defaulting install location to ${CMAKE_INSTALL_PREFIX}/lib.")
    set(CMAKE_INSTALL_LIBDIR lib)
endif()

set(RUNTIME_DIR bin)
set(LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR})
set(INCLUDE_DIR include)
set(MEX_DIR mex)
set(DOC_DIR "share/doc/${CPACK_PACKAGE_NAME}")
set(DATA_DIR share/${CPACK_PACKAGE_NAME})


if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
   message(STATUS "Build type not specified: defaulting to Release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# Compiler specific setup
########################################################################
macro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if(${have})
        add_definitions(${flag})        
    endif(${have})
endmacro(ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

if(CMAKE_COMPILER_IS_GNUCXX)
    #Any additional flags for CXX
endif(CMAKE_COMPILER_IS_GNUCXX)


if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-write-strings -Wno-format-extra-args -Winline -Wno-unused-result -Wno-format -std=c99 -D_GNU_SOURCE -g")
    
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
  else(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    find_package(SSE)
    if(HAVE_AVX)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mfpmath=sse -mavx -DLV_HAVE_AVX -DLV_HAVE_SSE")
    elseif(HAVE_SSE)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -mfpmath=sse -msse4.1 -DLV_HAVE_SSE")
    endif(HAVE_AVX)
  endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  
  
  if(NOT WIN32)
      ADD_CXX_COMPILER_FLAG_IF_AVAILABLE(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
  endif(NOT WIN32)
endif(CMAKE_COMPILER_IS_GNUCC)

if(MSVC)
    include_directories(${PROJECT_SOURCE_DIR}/msvc) #missing headers
    add_definitions(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    add_definitions(-DNOMINMAX) #disables stupidity and enables std::min and std::max
    add_definitions( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    add_definitions(/MP) #build with multiple processors
endif(MSVC)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # The following is needed for weak linking to work under OS X
   set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")



########################################################################
# Create uninstall targets
########################################################################
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    

########################################################################
# Macro to add -fPIC property to static libs 
########################################################################
macro(SRSLTE_SET_PIC)
    set_target_properties(${ARGV} PROPERTIES COMPILE_FLAGS -fPIC)
endmacro(SRSLTE_SET_PIC)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION}")

########################################################################
# Add general includes and dependencies
########################################################################
include_directories(${PROJECT_BINARY_DIR}/srslte/include/)
include_directories(${PROJECT_SOURCE_DIR}/srslte/include/)
include_directories(${PROJECT_SOURCE_DIR}/mex/include)

########################################################################
# Add the subdirectories
########################################################################
add_subdirectory(srslte)
add_subdirectory(mex)
