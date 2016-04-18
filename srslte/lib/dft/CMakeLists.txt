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



find_package(FFTW3F REQUIRED)
if(FFTW3F_FOUND)
  include_directories(${FFTW3F_INCLUDE_DIRS})
  link_directories(${FFTW3F_LIBRARY_DIRS})
endif(FFTW3F_FOUND)

set(SRCS dft_fftw.c dft_precoding.c ofdm.c)
add_library(srslte_dft ${SRCS})
target_link_libraries(srslte_dft    srslte_utils
                                    m
                                    ${FFTW3F_LIBRARIES})

add_subdirectory(test)
