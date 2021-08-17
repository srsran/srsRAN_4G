#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

#######################################################################
# ADT TESTS
#######################################################################

add_executable(scope_exit_test scope_exit_test.cc)
target_link_libraries(scope_exit_test srsran_common)
add_test(scope_exit_test scope_exit_test)

add_executable(expected_test expected_test.cc)
target_link_libraries(expected_test srsran_common)
add_test(expected_test expected_test)

add_executable(bounded_bitset_test bounded_bitset_test.cc)
target_link_libraries(bounded_bitset_test srsran_common)
add_test(bounded_bitset_test bounded_bitset_test)

add_executable(span_test span_test.cc)
target_link_libraries(span_test srsran_common)
add_test(span_test span_test)

add_executable(interval_test interval_test.cc)
target_link_libraries(interval_test srsran_common)
add_test(interval_test interval_test)

add_executable(observer_test observer_test.cc)
target_link_libraries(observer_test srsran_common)
add_test(observer_test observer_test)

add_executable(bounded_vector_test bounded_vector_test.cc)
target_link_libraries(bounded_vector_test srsran_common)
add_test(bounded_vector_test bounded_vector_test)

add_executable(mem_pool_test mem_pool_test.cc)
target_link_libraries(mem_pool_test srsran_common)
add_test(mem_pool_test mem_pool_test)

add_executable(circular_buffer_test circular_buffer_test.cc)
target_link_libraries(circular_buffer_test srsran_common)
add_test(circular_buffer_test circular_buffer_test)

add_executable(circular_map_test circular_map_test.cc)
target_link_libraries(circular_map_test srsran_common)
add_test(circular_map_test circular_map_test)

add_executable(fsm_test fsm_test.cc)
target_link_libraries(fsm_test srsran_common)
add_test(fsm_test fsm_test)

add_executable(optional_test optional_test.cc)
target_link_libraries(optional_test srsran_common)
add_test(optional_test optional_test)

add_executable(cached_alloc_test cached_alloc_test.cc)
target_link_libraries(cached_alloc_test srsran_common)
add_test(cached_alloc_test cached_alloc_test)

add_executable(optional_table_test optional_table_test.cc)
target_link_libraries(optional_table_test srsran_common)
add_test(optional_table_test optional_table_test)
