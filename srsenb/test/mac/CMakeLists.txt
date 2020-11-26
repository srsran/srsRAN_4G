#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(scheduler_test_common STATIC sched_test_common.cc sched_common_test_suite.cc sched_ue_ded_test_suite.cc
            sched_sim_ue.cc sched_sim_ue.cc)

# Scheduler subcomponent testing
add_executable(sched_grid_test sched_grid_test.cc)
target_link_libraries(sched_grid_test srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        scheduler_test_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_grid_test sched_grid_test)

# Scheduler test random
add_executable(scheduler_test_rand sched_test_rand.cc)
target_link_libraries(scheduler_test_rand srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        scheduler_test_common
        srslte_phy
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(scheduler_test_rand scheduler_test_rand)

# Scheduler test random for CA
add_executable(scheduler_ca_test sched_ca_test.cc sched_test_common.cc)
target_link_libraries(scheduler_ca_test srsenb_mac
        srsenb_phy
        srslte_common
        srslte_mac
        srslte_phy
        scheduler_test_common
        rrc_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(scheduler_ca_test scheduler_ca_test)

add_executable(sched_lc_ch_test sched_lc_ch_test.cc sched_test_common.cc)
target_link_libraries(sched_lc_ch_test srsenb_mac srslte_common srslte_mac scheduler_test_common)