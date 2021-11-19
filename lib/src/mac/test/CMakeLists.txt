#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(pdu_test pdu_test.cc)
target_link_libraries(pdu_test srsran_phy srsran_common srsran_mac ${CMAKE_THREAD_LIBS_INIT})
add_test(pdu_test pdu_test)

add_executable(mac_pcap_test mac_pcap_test.cc)
target_link_libraries(mac_pcap_test srsran_common srsran_mac ${SCTP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(mac_pcap_test mac_pcap_test)

add_executable(mac_pdu_nr_test mac_pdu_nr_test.cc)
target_link_libraries(mac_pdu_nr_test srsran_mac srsran_common ${CMAKE_THREAD_LIBS_INIT})
add_test(mac_pdu_nr_test mac_pdu_nr_test)

