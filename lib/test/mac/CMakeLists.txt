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

add_executable(mac_pcap_test mac_pcap_test.cc)
target_link_libraries(mac_pcap_test srslte_common srslte_mac ${SCTP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(mac_pcap_test mac_pcap_test)

add_executable(mac_pdu_nr_test mac_pdu_nr_test.cc)
target_link_libraries(mac_pdu_nr_test srslte_mac srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(mac_pdu_nr_test mac_pdu_nr_test)

