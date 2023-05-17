#
# Copyright 2013-2023 Software Radio Systems Limited
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

add_library(sched_test_common STATIC sched_test_common.cc sched_common_test_suite.cc sched_ue_ded_test_suite.cc
        sched_sim_ue.cc sched_sim_ue.cc)
target_link_libraries(sched_test_common srsran_common srsran_mac srsenb_mac)

# Scheduler subcomponent testing
add_executable(sched_grid_test sched_grid_test.cc)
target_link_libraries(sched_grid_test srsenb_mac
        srsenb_phy
        srsran_common
        srsran_mac
        sched_test_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_grid_test sched_grid_test)

# Scheduler test random
add_executable(sched_test_rand sched_test_rand.cc)
target_link_libraries(sched_test_rand srsenb_mac
        srsenb_phy
        srsran_common
        srsran_mac
        sched_test_common
        srsran_phy
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_test_rand sched_test_rand)

# Scheduler test random for CA
add_executable(sched_ca_test sched_ca_test.cc)
target_link_libraries(sched_ca_test srsenb_mac
        srsenb_phy
        srsran_common
        srsran_mac
        srsran_phy
        sched_test_common
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_ca_test sched_ca_test)

add_executable(sched_lc_ch_test sched_lc_ch_test.cc)
target_link_libraries(sched_lc_ch_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_lc_ch_test sched_lc_ch_test)

add_executable(sched_tpc_test sched_tpc_test.cc)
target_link_libraries(sched_tpc_test srsran_common srsran_mac sched_test_common)
add_test(sched_tpc_test sched_tpc_test)

add_executable(sched_dci_test sched_dci_test.cc)
target_link_libraries(sched_dci_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_dci_test sched_dci_test)

add_executable(sched_ue_cell_test sched_ue_cell_test.cc)
target_link_libraries(sched_ue_cell_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_ue_cell_test sched_ue_cell_test)

add_executable(sched_benchmark_test sched_benchmark.cc)
target_link_libraries(sched_benchmark_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_benchmark_test sched_benchmark_test)

add_executable(sched_cqi_test sched_cqi_test.cc)
target_link_libraries(sched_cqi_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_cqi_test sched_cqi_test)

add_executable(sched_phy_resource_test sched_phy_resource_test.cc)
target_link_libraries(sched_phy_resource_test srsran_common srsenb_mac srsran_mac sched_test_common)
add_test(sched_phy_resource_test sched_phy_resource_test)