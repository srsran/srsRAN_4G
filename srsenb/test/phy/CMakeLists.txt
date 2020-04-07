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

set(ENB_PHY_TEST_DURATION 128)

# eNb PHY test:
#  - Single carrier
#  - Transmission Mode 1
#  - 1 eNb cell/carrier (no carrier aggregation)
#  - 100 PRB
add_test(enb_phy_test_tm1 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --cell.nof_prb=100 --tm=1)

# Single carrier TM2 eNb PHY test:
#  - Single carrier
#  - Transmission Mode 2
#  - 1 eNb cell/carrier (no carrier aggregation)
#  - 6 PRB
add_test(enb_phy_test_tm2 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --cell.nof_prb=100 --tm=2)

# Single carrier TM3 eNb PHY test:
#  - Single carrier
#  - Transmission Mode 3
#  - 1 eNb cell/carrier (no carrier aggregation)
#  - 6 PRB
add_test(enb_phy_test_tm3 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --cell.nof_prb=100 --tm=3)

# Single carrier eNb PHY test:
#  - Single carrier
#  - Transmission Mode 4
#  - 1 eNb cell/carrier (no carrier aggregation)
#  - 6 PRB
add_test(enb_phy_test_tm4 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --cell.nof_prb=100 --tm=4)

# Five carrier aggregation using PUCCH3:
#  - 6 eNb cell/carrier
#  - Transmission Mode 1
#  - 5 Aggregated carriers
#  - 6 PRB
#  - PUCCH format 3 ACK/NACK feedback mode and more than 2 ACK/NACK bits in PUSCH
add_test(enb_phy_test_tm1_ca_pucch3 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --nof_enb_cells=6 --ue_cell_list=3,4,0,1,2 --ack_mode=pucch3 --cell.nof_prb=6 --tm=1)

# Five carrier aggregation using PUCCH3:
#  - 6 eNb cell/carrier
#  - Transmission Mode 4
#  - 5 Aggregated carriers
#  - 6 PRB
#  - PUCCH format 3 ACK/NACK feedback mode and more than 2 ACK/NACK bits in PUSCH
add_test(enb_phy_test_tm4_ca_pucch3 enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --nof_enb_cells=6 --ue_cell_list=0,4,3,1,2 --ack_mode=pucch3 --cell.nof_prb=6 --tm=4)

# Two carrier aggregation using Channel Selection:
#  - 6 eNb cell/carrier
#  - Transmission Mode 1
#  - 2 Aggregated carriers
#  - 6 PRB
#  - PUCCH format 1b with Channel selection ACK/NACK feedback mode
add_test(enb_phy_test_tm1_ca_cs enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --nof_enb_cells=6 --ue_cell_list=5,4 --ack_mode=cs --cell.nof_prb=6 --tm=1)

# Two carrier aggregation using Channel Selection:
#  - 6 eNb cell/carrier
#  - Transmission Mode 4
#  - 2 Aggregated carriers
#  - 6 PRB
#  - PUCCH format 1b with Channel selection ACK/NACK feedback mode
add_test(enb_phy_test_tm4_ca_cs enb_phy_test --duration=${ENB_PHY_TEST_DURATION} --nof_enb_cells=6 --ue_cell_list=1,5 --ack_mode=cs --cell.nof_prb=6 --tm=4)
