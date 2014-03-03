#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
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
PROJECT (LIBLTE)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

INCLUDE(libLTEPackage)	#setup cpack

########################################################################
# Install Dirs
########################################################################
SET(RUNTIME_DIR bin)
SET(LIBRARY_DIR lib)
SET(INCLUDE_DIR include)
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
IF(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-format-extra-args -Wno-reorder -Winline -Wno-unused-result -Wno-format -D_GNU_SOURCE")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-format-extra-args -Winline -Wno-unused-result -Wno-format -std=c99 -D_GNU_SOURCE")
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
    LIST(APPEND IRIS_CORE_COMMON_FLAGS_AND_DEFINES -DHAVE_CONFIG_H)
    ADD_DEFINITIONS(/MP) #build with multiple processors
ENDIF(MSVC)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # The following is needed for weak linking to work under OS X
   SET(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


########################################################################
# Setup Boost
########################################################################
MESSAGE(STATUS "")
MESSAGE(STATUS "Configuring Boost C++ Libraries...")
SET(BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
    unit_test_framework
)

IF(UNIX AND EXISTS "/usr/lib64")
    LIST(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
ENDIF(UNIX AND EXISTS "/usr/lib64")

IF(MSVC)
    SET(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    IF(BOOST_ALL_DYN_LINK)
        ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    ELSE(BOOST_ALL_DYN_LINK)
        UNSET(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    ENDIF(BOOST_ALL_DYN_LINK)
ENDIF(MSVC)

SET(Boost_ADDITIONAL_VERSIONS 
    "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39" "1.40.0" "1.40"
    "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44" "1.45.0" "1.45" 
    "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49" 
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53")
FIND_PACKAGE(Boost 1.37 REQUIRED ${BOOST_REQUIRED_COMPONENTS})
MESSAGE(STATUS "Boost version: ${Boost_VERSION}")

IF(Boost_VERSION LESS 104600)
    ADD_DEFINITIONS( -DBOOST_FILESYSTEM_VERSION=2 ) #use filesystem version 2 in boost < 1.46
    MESSAGE(STATUS "Using Boost Filesystem V2")
ENDIF(Boost_VERSION LESS 104600)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

MESSAGE(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "Boost library directories: ${Boost_LIBRARY_DIRS}")
MESSAGE(STATUS "Boost libraries: ${Boost_LIBRARIES}")


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
MACRO(LIBLTE_SET_PIC)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
        SET_TARGET_PROPERTIES(${ARGV} PROPERTIES COMPILE_FLAGS -fPIC)
    ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
ENDMACRO(LIBLTE_SET_PIC)

########################################################################
# A macro for passing lists between different directories
# through an internal cache variable.
########################################################################
MACRO (APPEND_INTERNAL_LIST  LIST_NAME  VALUE)

   # If the list in not in the cache, create it.
   IF (${LIST_NAME})
      SET (${LIST_NAME} "${${LIST_NAME}};${VALUE}" CACHE INTERNAL "Internal
variable")
   ELSE (${LIST_NAME})
      SET (${LIST_NAME} "${VALUE}" CACHE INTERNAL "Internal variable")
   ENDIF (${LIST_NAME})

ENDMACRO (APPEND_INTERNAL_LIST)


########################################################################
# Print summary
########################################################################
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Building for version: ${VERSION}") 

########################################################################
# Add general includes and dependencies
########################################################################
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lte/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/cuhd/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/graphics/include)

########################################################################
# Add the subdirectories
########################################################################

ADD_SUBDIRECTORY(lte)
ADD_SUBDIRECTORY(cuhd)
ADD_SUBDIRECTORY(graphics)
ADD_SUBDIRECTORY(examples)

