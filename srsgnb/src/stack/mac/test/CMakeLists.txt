#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched;nr")

add_library(sched_nr_test_suite sched_nr_common_test.cc sched_nr_ue_ded_test_suite.cc sched_nr_sim_ue.cc)
target_link_libraries(sched_nr_test_suite srsgnb_mac srsran_common)

add_executable(sched_nr_parallel_test sched_nr_parallel_test.cc)
target_link_libraries(sched_nr_parallel_test
        srsgnb_mac
        sched_nr_test_suite
        srsran_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_parallel_test sched_nr_parallel_test)

add_executable(sched_nr_prb_test sched_nr_prb_test.cc)
target_link_libraries(sched_nr_prb_test
        srsgnb_mac
        srsran_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_prb_test sched_nr_prb_test)

add_executable(sched_nr_pdcch_test sched_nr_pdcch_test.cc)
target_link_libraries(sched_nr_pdcch_test srsgnb_mac sched_nr_test_suite srsran_common)
add_nr_test(sched_nr_pdcch_test sched_nr_pdcch_test)

add_executable(sched_nr_sch_test sched_nr_sch_test.cc)
target_link_libraries(sched_nr_sch_test srsgnb_mac sched_nr_test_suite srsran_common)
add_nr_test(sched_nr_sch_test sched_nr_sch_test)

add_executable(sched_nr_rar_test sched_nr_rar_test.cc)
target_link_libraries(sched_nr_rar_test srsgnb_mac sched_nr_test_suite srsran_common)
add_nr_test(sched_nr_rar_test sched_nr_rar_test)

add_executable(sched_nr_dci_utilities_tests sched_nr_dci_utilities_tests.cc)
target_link_libraries(sched_nr_dci_utilities_tests srsgnb_mac srsran_common srsran_phch)
add_nr_test(sched_nr_dci_utilities_tests sched_nr_dci_utilities_tests)

add_executable(sched_nr_test sched_nr_test.cc)
target_link_libraries(sched_nr_test
        srsgnb_mac
        sched_nr_test_suite
        srsran_common ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_nr_test(sched_nr_test sched_nr_test)