#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(sched_test_common STATIC sched_test_common.cc sched_common_test_suite.cc sched_ue_ded_test_suite.cc
        sched_sim_ue.cc sched_sim_ue.cc)
target_link_libraries(sched_test_common srslte_common srslte_mac srsenb_mac)

# Scheduler subcomponent testing
add_executable(sched_grid_test sched_grid_test.cc)
target_link_libraries(sched_grid_test srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        sched_test_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_grid_test sched_grid_test)

# Scheduler test random
add_executable(sched_test_rand sched_test_rand.cc)
target_link_libraries(sched_test_rand srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        sched_test_common
        srslte_phy
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_test_rand sched_test_rand)

# Scheduler test random for CA
add_executable(sched_ca_test sched_ca_test.cc)
target_link_libraries(sched_ca_test srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        srslte_phy
        sched_test_common
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_ca_test sched_ca_test)

add_executable(sched_lc_ch_test sched_lc_ch_test.cc)
target_link_libraries(sched_lc_ch_test srslte_common srsenb_mac srslte_mac sched_test_common)
add_test(sched_lc_ch_test sched_lc_ch_test)

add_executable(sched_tpc_test sched_tpc_test.cc)
target_link_libraries(sched_tpc_test srslte_common srslte_mac sched_test_common)
add_test(sched_tpc_test sched_tpc_test)

add_executable(sched_dci_test sched_dci_test.cc)
target_link_libraries(sched_dci_test srslte_common srsenb_mac srslte_mac sched_test_common)
add_test(sched_dci_test sched_dci_test)

add_executable(sched_benchmark_test sched_benchmark.cc)
target_link_libraries(sched_benchmark_test srslte_common srsenb_mac srslte_mac sched_test_common)
add_test(sched_benchmark_test sched_benchmark_test)
