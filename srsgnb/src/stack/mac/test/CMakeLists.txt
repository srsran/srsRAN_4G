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

set_directory_properties(PROPERTIES LABELS "sched;nr")

add_library(sched_nr_test_suite STATIC sched_nr_common_test.cc sched_nr_ue_ded_test_suite.cc sched_nr_sim_ue.cc)
target_link_libraries(sched_nr_test_suite srsgnb_mac srsran_common rrc_nr_asn1)

add_executable(sched_nr_parallel_test sched_nr_parallel_test.cc)
target_link_libraries(sched_nr_parallel_test
        srsgnb_mac
        sched_nr_test_suite
        srsran_common
        rrc_nr_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_parallel_test sched_nr_parallel_test)

add_executable(sched_nr_prb_test sched_nr_prb_test.cc)
target_link_libraries(sched_nr_prb_test
        srsgnb_mac
        srsran_common
        rrc_nr_asn1
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_prb_test sched_nr_prb_test)

add_executable(sched_nr_pdcch_test sched_nr_pdcch_test.cc)
target_link_libraries(sched_nr_pdcch_test srsgnb_mac sched_nr_test_suite srsran_common rrc_nr_asn1)
add_nr_test(sched_nr_pdcch_test sched_nr_pdcch_test)

add_executable(sched_nr_sch_test sched_nr_sch_test.cc)
target_link_libraries(sched_nr_sch_test srsgnb_mac sched_nr_test_suite srsran_common rrc_nr_asn1)
add_nr_test(sched_nr_sch_test sched_nr_sch_test)

add_executable(sched_nr_rar_test sched_nr_rar_test.cc)
target_link_libraries(sched_nr_rar_test srsgnb_mac sched_nr_test_suite srsran_common rrc_nr_asn1)
add_nr_test(sched_nr_rar_test sched_nr_rar_test)

add_executable(sched_nr_dci_utilities_tests sched_nr_dci_utilities_tests.cc)
target_link_libraries(sched_nr_dci_utilities_tests srsgnb_mac srsran_common rrc_nr_asn1)
add_nr_test(sched_nr_dci_utilities_tests sched_nr_dci_utilities_tests)

add_executable(sched_nr_test sched_nr_test.cc)
target_link_libraries(sched_nr_test
        srsgnb_mac
        sched_nr_test_suite
        rrc_nr_asn1
        srsran_common ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_test sched_nr_test)
