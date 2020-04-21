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

find_package(SCTP REQUIRED)

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

add_executable(stack_procedure_test stack_procedure_test.cc)
add_test(stack_procedure_test stack_procedure_test)

add_executable(queue_test queue_test.cc)
target_link_libraries(queue_test srslte_common ${CMAKE_THREAD_LIBS_INIT})
add_test(queue_test queue_test)

add_executable(timer_test timer_test.cc)
target_link_libraries(timer_test srslte_common)
add_test(timer_test timer_test)

add_executable(network_utils_test network_utils_test.cc)
target_link_libraries(network_utils_test srslte_common ${SCTP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(network_utils_test network_utils_test)

add_executable(test_common_test test_common_test.cc)
target_link_libraries(test_common_test srslte_common)
add_test(test_common_test test_common_test)

add_executable(tti_point_test tti_point_test.cc)
target_link_libraries(tti_point_test srslte_common)
add_test(tti_point_test tti_point_test)

add_executable(fsm_test fsm_test.cc)
target_link_libraries(fsm_test srslte_common)
add_test(fsm_test fsm_test)

add_executable(choice_type_test choice_type_test.cc)
target_link_libraries(choice_type_test srslte_common)
add_test(choice_type_test choice_type_test)
