#
# Copyright 2013-2019 Software Radio Systems Limited
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

#######################################################################
# COMMON TESTS
#######################################################################
add_executable(logger_test logger_test.cc)
target_link_libraries(logger_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(logger_test logger_test)

add_executable(msg_queue_test msg_queue_test.cc)
target_link_libraries(msg_queue_test srslte_phy srslte_common ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(msg_queue_test msg_queue_test)

add_executable(test_eia1 test_eia1.cc)
target_link_libraries(test_eia1 srslte_common srslte_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eia1 test_eia1)

add_executable(test_eia3 test_eia3.cc)
target_link_libraries(test_eia3 srslte_common)
add_test(test_eia3 test_eia3)

add_executable(test_eea1 test_eea1.cc)
target_link_libraries(test_eea1 srslte_common srslte_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea1 test_eea1)

add_executable(test_eea2 test_eea2.cc)
target_link_libraries(test_eea2 srslte_common srslte_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea2 test_eea2)

add_executable(test_eea3 test_eea3.cc)
target_link_libraries(test_eea3 srslte_common srslte_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea3 test_eea3)

add_executable(test_f12345 test_f12345.cc)
target_link_libraries(test_f12345 srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(test_f12345 test_f12345)

add_executable(log_filter_test log_filter_test.cc)
target_link_libraries(log_filter_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

add_executable(timeout_test timeout_test.cc)
target_link_libraries(timeout_test srslte_phy ${CMAKE_THREAD_LIBS_INIT})

add_executable(bcd_helpers_test bcd_helpers_test.cc)

add_executable(pdu_test pdu_test.cc)
target_link_libraries(pdu_test srslte_phy srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(pdu_test pdu_test)

add_executable(mac_nr_pdu_test mac_nr_pdu_test.cc)
target_link_libraries(mac_nr_pdu_test srslte_phy srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(mac_nr_pdu_test mac_nr_pdu_test)

add_executable(stack_procedure_test stack_procedure_test.cc)

add_executable(queue_test queue_test.cc)
target_link_libraries(queue_test srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(queue_test queue_test)
