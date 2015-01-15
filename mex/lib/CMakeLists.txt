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

if(NOT MATLAB_FOUND)
  find_package(MATLAB)
endif()

if(NOT OCTAVE_FOUND)
  find_package(OCTAVE)
endif()

# CMake 2.8.12 & earlier apparently don't define the
# Mex script path, so find it.
if(NOT MATLAB_MEX_PATH)
  find_program( MATLAB_MEX_PATH mex
    HINTS ${MATLAB_ROOT}/bin
    PATHS ${MATLAB_ROOT}/bin
    DOC "The mex program path"
    )
endif()

IF (MATLAB_FOUND)
  message(STATUS "Found MATLAB in ${MATLAB_ROOT}")
ENDIF(MATLAB_FOUND)
IF (OCTAVE_FOUND)
  message(STATUS "Found OCTAVE in ${OCTAVE_INCLUDE_PATHS}")
ENDIF(OCTAVE_FOUND)


IF (MATLAB_FOUND OR OCTAVE_FOUND)
  ADD_LIBRARY(liblte_mex SHARED mexutils.c)
  INSTALL(TARGETS liblte_mex DESTINATION ${LIBRARY_DIR})
  LIBLTE_SET_PIC(liblte_mex)
  if (MATLAB_FOUND)
    include_directories(${MATLAB_INCLUDE_DIR})
  endif(MATLAB_FOUND)
  if (OCTAVE_FOUND) 
    include_directories(${OCTAVE_INCLUDE_DIR})
  endif (OCTAVE_FOUND)

ELSEIF (MATLAB_FOUND OR OCTAVE_FOUND)
  message(STATUS "Could NOT find OCTAVE or MATLAB. MEX files won't be compiled")
ENDIF(MATLAB_FOUND OR OCTAVE_FOUND)


