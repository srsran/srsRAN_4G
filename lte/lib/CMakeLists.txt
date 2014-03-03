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
# Find Dependencies  
########################################################################

FIND_PACKAGE(FFTW3F REQUIRED) # TODO: distribute kissfft instead
include_directories(${FFTW3F_INCLUDE_DIRS})


########################################################################
# Recurse subdirectories and compile all source files into the same lib  
########################################################################

FILE(GLOB modules *) 
SET(SOURCES_ALL "")
FOREACH (_module ${modules})
	IF(IS_DIRECTORY ${_module})
		FILE(GLOB_RECURSE tmp "${_module}/src/*.c")
		LIST(APPEND SOURCES_ALL ${tmp})		
	ENDIF(IS_DIRECTORY ${_module})
ENDFOREACH()

ADD_LIBRARY(lte ${SOURCES_ALL})
TARGET_LINK_LIBRARIES(lte m ${FFTW3F_LIBRARIES})
INSTALL(TARGETS lte DESTINATION ${LIBRARY_DIR})
LIBLTE_SET_PIC(lte)









