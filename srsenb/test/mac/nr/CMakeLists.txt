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

add_library(sched_nr_test_suite sched_nr_common_test.cc sched_nr_ue_ded_test_suite.cc)

add_executable(sched_nr_test sched_nr_test.cc sched_nr_sim_ue.cc)
target_link_libraries(sched_nr_test
        srsgnb_mac
        sched_nr_test_suite
        srsran_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_test sched_nr_test)

add_executable(sched_nr_prb_test sched_nr_prb_test.cc)
target_link_libraries(sched_nr_prb_test
        srsgnb_mac
        srsran_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_prb_test sched_nr_prb_test)

add_executable(sched_nr_rar_test sched_nr_rar_test.cc)
target_link_libraries(sched_nr_rar_test srsgnb_mac sched_nr_test_suite srsran_common)
add_nr_test(sched_nr_rar_test sched_nr_rar_test)