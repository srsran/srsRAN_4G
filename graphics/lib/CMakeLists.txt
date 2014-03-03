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
 
 
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../)
 
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

IF(QT4_FOUND AND QWT_FOUND)
  QT4_WRAP_CPP(lineplotwraps common/Lineplot.h)
  QT4_WRAP_CPP(pointplotwraps common/Pointplot.h)
  QT4_WRAP_CPP(spectrogramplotwraps common/Spectrogramplot.h)
  QT4_WRAP_CPP(complex complexplot/ComplexWidget.h complexplot/ComplexplotWrapper.h)
  QT4_WRAP_CPP(real realplot/RealWidget.h realplot/RealplotWrapper.h)
  QT4_WRAP_CPP(scatter scatterplot/ScatterWidget.h scatterplot/ScatterplotWrapper.h)
  QT4_WRAP_CPP(waterfall waterfallplot/WaterfallWidget.h waterfallplot/WaterfallplotWrapper.h)

  INCLUDE_DIRECTORIES(common complexplot realplot scatterplot waterfallplot) 
  
  ADD_LIBRARY(graphics ${eventwraps} ${lineplotwraps} ${pointplotwraps} ${spectrogramplotwraps} ${complex} ${real} ${scatter} ${waterfall} ${SOURCES_ALL} )
  TARGET_LINK_LIBRARIES(graphics pthread ${QT_LIBRARIES} ${QWT_LIBRARIES})
  INSTALL(TARGETS graphics DESTINATION ${LIBRARY_DIR})
  LIBLTE_SET_PIC(graphics)
  
  APPEND_INTERNAL_LIST(OPTIONAL_LIBS graphics)

  MESSAGE(STATUS "   graphics library will be installed.")

ELSE(QT4_FOUND AND QWT_FOUND) 

  MESSAGE(STATUS "   QT4/Qwt not found. GRAPHICS library is not generated")
  
ENDIF(QT4_FOUND AND QWT_FOUND)


ADD_SUBDIRECTORY(complexplot/test)
ADD_SUBDIRECTORY(realplot/test)
ADD_SUBDIRECTORY(scatterplot/test)
ADD_SUBDIRECTORY(waterfallplot/test)


