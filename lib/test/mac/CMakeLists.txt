#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(pdu_test pdu_test.cc)
target_link_libraries(pdu_test srslte_phy srslte_common srslte_mac ${CMAKE_THREAD_LIBS_INIT})
add_test(pdu_test pdu_test)

if (ENABLE_5GNR)
    add_executable(mac_nr_pdu_test mac_nr_pdu_test.cc)
    target_link_libraries(mac_nr_pdu_test srslte_phy srslte_mac srslte_common ${CMAKE_THREAD_LIBS_INIT})
    add_test(mac_nr_pdu_test mac_nr_pdu_test)
endif (ENABLE_5GNR)
