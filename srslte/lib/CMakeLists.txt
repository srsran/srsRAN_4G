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

add_library(srslte  SHARED  version.c
                            $<TARGET_OBJECTS:srslte_agc>
                            $<TARGET_OBJECTS:srslte_ch_estimation>
                            $<TARGET_OBJECTS:srslte_common>
                            $<TARGET_OBJECTS:srslte_fec>
                            $<TARGET_OBJECTS:srslte_mimo>
                            $<TARGET_OBJECTS:srslte_phch>
                            $<TARGET_OBJECTS:srslte_sync>
                            $<TARGET_OBJECTS:srslte_utils>
                            $<TARGET_OBJECTS:srslte_channel>
                            $<TARGET_OBJECTS:srslte_dft>
                            $<TARGET_OBJECTS:srslte_io>
                            $<TARGET_OBJECTS:srslte_modem>
                            $<TARGET_OBJECTS:srslte_resampling>
                            $<TARGET_OBJECTS:srslte_scrambling>
                            $<TARGET_OBJECTS:srslte_ue>
                            $<TARGET_OBJECTS:srslte_rf>
)


if(NOT DisableMEX)
  add_library(srslte_static  STATIC  version.c
                            $<TARGET_OBJECTS:srslte_agc>
                            $<TARGET_OBJECTS:srslte_ch_estimation>
                            $<TARGET_OBJECTS:srslte_common>
                            $<TARGET_OBJECTS:srslte_fec>
                            $<TARGET_OBJECTS:srslte_mimo>
                            $<TARGET_OBJECTS:srslte_phch>
                            $<TARGET_OBJECTS:srslte_sync>
                            $<TARGET_OBJECTS:srslte_utils>
                            $<TARGET_OBJECTS:srslte_channel>
                            $<TARGET_OBJECTS:srslte_dft>
                            $<TARGET_OBJECTS:srslte_io>
                            $<TARGET_OBJECTS:srslte_modem>
                            $<TARGET_OBJECTS:srslte_resampling>
                            $<TARGET_OBJECTS:srslte_scrambling>
                            $<TARGET_OBJECTS:srslte_ue>
                            $<TARGET_OBJECTS:srslte_rf>
  )
endif(NOT DisableMEX)

target_link_libraries(srslte pthread m)
set_target_properties(srslte  PROPERTIES
                              VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR})

if(MKL_FOUND)
  target_link_libraries(srslte ${MKL_LIBRARIES})
else(MKL_FOUND)
  target_link_libraries(srslte ${FFTW3F_LIBRARIES})
endif(MKL_FOUND)

if(RF_FOUND)
  if(UHD_FOUND)
    target_link_libraries(srslte ${UHD_LIBRARIES})
  endif(UHD_FOUND)

  if(BLADERF_FOUND)
    target_link_libraries(srslte ${BLADERF_LIBRARIES})
  endif(BLADERF_FOUND)
endif(RF_FOUND)

if(VOLK_FOUND)
  target_link_libraries(srslte ${VOLK_LIBRARIES})
endif(VOLK_FOUND)

INSTALL(TARGETS srslte DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srslte)

