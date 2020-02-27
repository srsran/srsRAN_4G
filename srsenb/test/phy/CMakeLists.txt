#
# Copyright 2013-2019 Software Radio Systems Limited
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

add_executable(enb_phy_test enb_phy_test.cc)
target_link_libraries(enb_phy_test
        srsenb_phy
        srsenb_stack
        srsenb_upper
        srsenb_mac
        srsenb_rrc
        srslte_common
        srslte_phy
        srslte_radio
        srslte_upper
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

# Basic eNb PHY test:
#  - 128 Frames
#  - 1 eNb cell/carrier (no carrier aggregation)
#  - maximum bandwidth 100
add_test(enb_phy_test enb_phy_test --duration=128 --cell.nof_prb=100 )

# Two carrier aggregation using PUCCH3
#  - 128 frames
#  - 3 eNb cell/carrier (only using 2 aggregated)
#  - minimum bandwidth to reduce memcheck time
#  - PUCCH format 3 ACK/NACK feedback mode
add_test(enb_phy_test enb_phy_test --duration=128 --nof_enb_cells=3 --ue_cell_list=2,0 --ack_mode=pucch3 --cell.nof_prb=6 )

# Two carrier aggregation using Channel Selection
#  - 128 frames
#  - 6 eNb cell/carrier (only using 2 aggregated)
#  - minimum bandwidth to reduce memcheck time
#  - PUCCH format 1b with Channel selection ACK/NACK feedback mode
add_test(enb_phy_test enb_phy_test --duration=128 --nof_enb_cells=6 --ue_cell_list=5,4 --ack_mode=cs --cell.nof_prb=6 )
