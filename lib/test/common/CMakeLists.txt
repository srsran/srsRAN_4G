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

find_package(SCTP REQUIRED)

#######################################################################
# COMMON TESTS
#######################################################################
add_executable(byte_buffer_queue_test byte_buffer_queue_test.cc)
target_link_libraries(byte_buffer_queue_test srsran_phy srsran_common ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(byte_buffer_queue_test byte_buffer_queue_test)

add_executable(test_eia1 test_eia1.cc)
target_link_libraries(test_eia1 srsran_common srsran_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eia1 test_eia1)

add_executable(test_eia3 test_eia3.cc)
target_link_libraries(test_eia3 srsran_common)
add_test(test_eia3 test_eia3)

add_executable(test_eea1 test_eea1.cc)
target_link_libraries(test_eea1 srsran_common srsran_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea1 test_eea1)

add_executable(test_eea2 test_eea2.cc)
target_link_libraries(test_eea2 srsran_common srsran_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea2 test_eea2)

add_executable(test_eea3 test_eea3.cc)
target_link_libraries(test_eea3 srsran_common srsran_phy ${CMAKE_THREAD_LIBS_INIT})
add_test(test_eea3 test_eea3)

add_executable(test_f12345 test_f12345.cc)
target_link_libraries(test_f12345 srsran_common ${CMAKE_THREAD_LIBS_INIT})
add_test(test_f12345 test_f12345)

add_executable(test_security_kdf test_security_kdf.cc)
target_link_libraries(test_security_kdf srsran_common ${CMAKE_THREAD_LIBS_INIT})
add_test(test_security_kdf test_security_kdf)

add_executable(timeout_test timeout_test.cc)
target_link_libraries(timeout_test srsran_phy ${CMAKE_THREAD_LIBS_INIT})

add_executable(bcd_helpers_test bcd_helpers_test.cc)
target_link_libraries(bcd_helpers_test srsran_common)

add_executable(stack_procedure_test stack_procedure_test.cc)
target_link_libraries(stack_procedure_test srsran_common)
add_test(stack_procedure_test stack_procedure_test)

add_executable(queue_test multiqueue_test.cc)
target_link_libraries(queue_test srsran_common ${CMAKE_THREAD_LIBS_INIT})
add_test(queue_test queue_test)

add_executable(timer_test timer_test.cc)
target_link_libraries(timer_test srsran_common ${ATOMIC_LIBS})
add_test(timer_test timer_test)

add_executable(network_utils_test network_utils_test.cc)
target_link_libraries(network_utils_test srsran_common ${SCTP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(network_utils_test network_utils_test)

add_executable(tti_point_test tti_point_test.cc)
target_link_libraries(tti_point_test srsran_common)
add_test(tti_point_test tti_point_test)

add_executable(choice_type_test choice_type_test.cc)
target_link_libraries(choice_type_test srsran_common)
add_test(choice_type_test choice_type_test)

add_executable(task_scheduler_test task_scheduler_test.cc)
target_link_libraries(task_scheduler_test srsran_common ${ATOMIC_LIBS})
add_test(task_scheduler_test task_scheduler_test)

add_executable(mac_pcap_net_test mac_pcap_net_test.cc)
target_link_libraries(mac_pcap_net_test srsran_common ${SCTP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
