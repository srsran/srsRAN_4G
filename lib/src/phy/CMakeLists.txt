#
# Copyright 2013-2020 Software Radio Systems Limited
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

add_library(srslte_phy STATIC ${srslte_srcs})
target_link_libraries(srslte_phy pthread m ${FFT_LIBRARIES})