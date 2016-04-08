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

FIND_PACKAGE(FFTW3F REQUIRED)
IF(FFTW3F_FOUND)
  INCLUDE_DIRECTORIES(${FFTW3F_INCLUDE_DIRS})
  LINK_DIRECTORIES(${FFTW3F_LIBRARY_DIRS})
ENDIF(FFTW3F_FOUND)

FILE(GLOB SOURCES "*.c")
ADD_LIBRARY(srslte_dft ${SOURCES})
TARGET_LINK_LIBRARIES(srslte_dft    srslte_utils
                                    m
                                    ${FFTW3F_LIBRARIES})
ADD_SUBDIRECTORY(test)
