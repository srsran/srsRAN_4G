#
# Copyright 2013-2021 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
add_subdirectory(gnb)
set(srsran_srcs     $<TARGET_OBJECTS:srsran_agc>
        $<TARGET_OBJECTS:srsran_ch_estimation>
        $<TARGET_OBJECTS:srsran_phy_common>
        $<TARGET_OBJECTS:srsran_fec>
        $<TARGET_OBJECTS:srsran_mimo>
        $<TARGET_OBJECTS:srsran_phch>
        $<TARGET_OBJECTS:srsran_sync>
        $<TARGET_OBJECTS:srsran_utils>
        $<TARGET_OBJECTS:srsran_channel>
        $<TARGET_OBJECTS:srsran_dft>
        $<TARGET_OBJECTS:srsran_io>
        $<TARGET_OBJECTS:srsran_modem>
        $<TARGET_OBJECTS:srsran_resampling>
        $<TARGET_OBJECTS:srsran_scrambling>
        $<TARGET_OBJECTS:srsran_ue>
        $<TARGET_OBJECTS:srsran_enb>
        $<TARGET_OBJECTS:srsran_gnb>
        )

add_library(srsran_phy STATIC ${srsran_srcs} )
target_link_libraries(srsran_phy pthread m ${FFT_LIBRARIES})
INSTALL(TARGETS srsran_phy DESTINATION ${LIBRARY_DIR})
