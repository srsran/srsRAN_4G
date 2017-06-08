#
# Copyright 2013-2017 Software Radio Systems Limited
#
# This file is part of srsLTE
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
add_subdirectory(enb)

set(srslte_srcs     $<TARGET_OBJECTS:srslte_agc>
                    $<TARGET_OBJECTS:srslte_ch_estimation>
                    $<TARGET_OBJECTS:srslte_phy_common>
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
                    $<TARGET_OBJECTS:srslte_enb>
)

add_library(srslte_phy SHARED ${srslte_srcs})
target_link_libraries(srslte_phy pthread m)
set_target_properties(srslte_phy PROPERTIES
                                 VERSION ${SRSLTE_VERSION_MAJOR}.${SRSLTE_VERSION_MINOR})

if(NOT DisableMEX)
  add_library(srslte_phy_static STATIC ${srslte_srcs})
endif(NOT DisableMEX)

find_package(MKL)
if(MKL_FOUND)
  include_directories(${MKL_INCLUDE_DIRS})
  link_directories(${MKL_LIBRARY_DIRS})
else(MKL_FOUND)
  find_package(FFTW3F REQUIRED)
  if(FFTW3F_FOUND)
    include_directories(${FFTW3F_INCLUDE_DIRS})
    link_directories(${FFTW3F_LIBRARY_DIRS})
  endif(FFTW3F_FOUND)
endif(MKL_FOUND)


if(MKL_FOUND)
  if(STATIC_MKL)
    target_link_libraries(srslte_phy ${MKL_STATIC_LIBRARIES})
    if(NOT DisableMEX)
      target_link_libraries(srslte_phy_static ${MKL_STATIC_LIBRARIES})
    endif(NOT DisableMEX)
  else(STATIC_MKL)
    target_link_libraries(srslte_phy ${MKL_LIBRARIES})
    if(NOT DisableMEX)
      target_link_libraries(srslte_phy_static ${MKL_LIBRARIES})
    endif(NOT DisableMEX)
  endif(STATIC_MKL)
else(MKL_FOUND)
  target_link_libraries(srslte_phy ${FFTW3F_LIBRARIES})
  if(NOT DisableMEX)
    target_link_libraries(srslte_phy_static ${FFTW3F_LIBRARIES})
  endif(NOT DisableMEX)
endif(MKL_FOUND)

## This linkage is required for the examples and tests only
if(RF_FOUND)

  target_link_libraries(srslte_phy srslte_rf)

  if(UHD_FOUND)
    target_link_libraries(srslte_phy ${UHD_LIBRARIES})
  endif(UHD_FOUND)

  if(BLADERF_FOUND)
    target_link_libraries(srslte_phy ${BLADERF_LIBRARIES})
  endif(BLADERF_FOUND)

  if(LIMESDR_FOUND)
    target_link_libraries(srslte_phy ${LIMESDR_LIBRARIES})
  endif(LIMESDR_FOUND)

  if(SOAPYSDR_FOUND)
    target_link_libraries(srslte_phy ${SOAPYSDR_LIBRARIES})
  endif(SOAPYSDR_FOUND)

endif(RF_FOUND)

if(VOLK_FOUND)
  target_link_libraries(srslte_phy ${VOLK_LIBRARIES})
  if(NOT DisableMEX)
    target_link_libraries(srslte_phy_static ${VOLK_LIBRARIES})
  endif(NOT DisableMEX)
endif(VOLK_FOUND)

install(TARGETS srslte_phy DESTINATION ${LIBRARY_DIR})
SRSLTE_SET_PIC(srslte_phy)

