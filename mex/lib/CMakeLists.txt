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

if(MATLAB_FOUND)
  message(STATUS "Found MATLAB in ${MATLAB_ROOT}")
  add_library(srslte_mex SHARED mexutils.c)
  install(TARGETS srslte_mex DESTINATION ${LIBRARY_DIR})
  SRSLTE_SET_PIC(srslte_mex)
  include_directories(${MATLAB_INCLUDE_DIR})
endif(MATLAB_FOUND)

if(OCTAVE_FOUND)
  message(STATUS "Found OCTAVE in ${OCTAVE_INCLUDE_PATHS}")
  if (NOT MATLAB_FOUND)
    add_library(srslte_mex SHARED mexutils.c)
    install(TARGETS srslte_mex DESTINATION ${LIBRARY_DIR})
    SRSLTE_SET_PIC(srslte_mex)
  endif (NOT MATLAB_FOUND)
  include_directories(${OCTAVE_INCLUDE_DIR})
endif(OCTAVE_FOUND)

if(NOT MATLAB_FOUND)
  if(NOT OCTAVE_FOUND)
    message(STATUS "Could NOT find OCTAVE or MATLAB. MEX files won't be compiled")
  endif(NOT OCTAVE_FOUND)
endif(NOT MATLAB_FOUND)


