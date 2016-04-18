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

add_subdirectory(agc)
add_subdirectory(ch_estimation)
add_subdirectory(common)
add_subdirectory(fec)
add_subdirectory(mimo)
add_subdirectory(phch)
add_subdirectory(rf)
add_subdirectory(sync)
add_subdirectory(utils)
add_subdirectory(channel)
add_subdirectory(dft)
add_subdirectory(io)
add_subdirectory(modem)
add_subdirectory(resampling)
add_subdirectory(scrambling)
add_subdirectory(ue)

add_library(srslte  SHARED  version.c)
target_link_libraries(srslte  srslte_agc
                              srslte_ch_estimation
                              srslte_common
                              srslte_fec
                              srslte_mimo
                              srslte_phch
                              srslte_sync
                              srslte_utils
                              srslte_channel
                              srslte_dft
                              srslte_io
                              srslte_modem
                              srslte_resampling
                              srslte_scrambling
                              srslte_ue
                              srslte_rf
                              pthread)
set_target_properties(srslte  PROPERTIES
                              VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR})

if(VOLK_FOUND)
  set_target_properties(srslte PROPERTIES COMPILE_DEFINITIONS "${VOLK_DEFINITIONS}")
  target_link_libraries(srslte ${VOLK_LIBRARIES})
endif(VOLK_FOUND)

INSTALL(TARGETS srslte DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srslte)

