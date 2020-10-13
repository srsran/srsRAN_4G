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

add_library(scheduler_test_common STATIC scheduler_test_common.cc)

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
add_executable(scheduler_test_rand scheduler_test_rand.cc)
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
add_executable(scheduler_ca_test scheduler_ca_test.cc scheduler_test_common.cc)
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

add_executable(sched_lc_ch_test sched_lc_ch_test.cc scheduler_test_common.cc)
target_link_libraries(sched_lc_ch_test srsenb_mac srslte_common srslte_mac scheduler_test_common)