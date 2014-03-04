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
# Setup Qt and Qwt
########################################################################

FIND_PACKAGE(Qt4)
IF(QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF(QT4_FOUND) 

FIND_PACKAGE(Qwt)
IF(QT4_FOUND AND QWT_FOUND)
    INCLUDE_DIRECTORIES(${QWT_INCLUDE_DIRS})
ENDIF(QT4_FOUND AND QWT_FOUND)


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
# Build the graphics library
########################################################################

file(GLOB modules *) 

SET(SOURCES_ALL "")
FOREACH (_module ${modules})
	IF(IS_DIRECTORY ${_module})
		FILE(GLOB tmp "${_module}/*.cpp")
		LIST(APPEND SOURCES_ALL ${tmp})		
	ENDIF(IS_DIRECTORY ${_module})
ENDFOREACH(_module ${modules})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../)

IF(QWT_MAJOR_VERSION LESS 6)
	MESSAGE(STATUS "QWT6 is required.")
ENDIF(QWT_MAJOR_VERSION LESS 6)

IF(QT4_FOUND AND QWT_FOUND AND Boost_FOUND AND QWT_MAJOR_VERSION EQUAL 6)
  QT4_WRAP_CPP(lineplotwraps common/Lineplot.h)
  QT4_WRAP_CPP(pointplotwraps common/Pointplot.h)
  QT4_WRAP_CPP(spectrogramplotwraps common/Spectrogramplot.h)
  QT4_WRAP_CPP(complex complexplot/ComplexWidget.h complexplot/ComplexplotWrapper.h)
  QT4_WRAP_CPP(real realplot/RealWidget.h realplot/RealplotWrapper.h)
  QT4_WRAP_CPP(scatter scatterplot/ScatterWidget.h scatterplot/ScatterplotWrapper.h)
  QT4_WRAP_CPP(waterfall waterfallplot/WaterfallWidget.h waterfallplot/WaterfallplotWrapper.h)

  INCLUDE_DIRECTORIES(common complexplot realplot scatterplot waterfallplot ${Boost_INCLUDE_DIRS}) 
  
  ADD_LIBRARY(graphics ${eventwraps} ${lineplotwraps} ${pointplotwraps} ${spectrogramplotwraps} ${complex} ${real} ${scatter} ${waterfall} ${SOURCES_ALL} )
  TARGET_LINK_LIBRARIES(graphics pthread ${QT_LIBRARIES} ${QWT_LIBRARIES})
  INSTALL(TARGETS graphics DESTINATION ${LIBRARY_DIR})
  LIBLTE_SET_PIC(graphics)
  
  APPEND_INTERNAL_LIST(OPTIONAL_LIBS graphics)


  ADD_SUBDIRECTORY(complexplot/test)
  ADD_SUBDIRECTORY(realplot/test)
  ADD_SUBDIRECTORY(scatterplot/test)
  ADD_SUBDIRECTORY(waterfallplot/test)

  MESSAGE(STATUS "   GRAPHICS library will be installed.")
  

ELSE(QT4_FOUND AND QWT_FOUND AND Boost_FOUND AND QWT_MAJOR_VERSION EQUAL 6) 

  MESSAGE(STATUS "   QT4/Qwt6 or Boost not found. GRAPHICS library is not generated")
  
ENDIF(QT4_FOUND AND QWT_FOUND AND Boost_FOUND AND QWT_MAJOR_VERSION EQUAL 6)




