# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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
