#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

#######################################################################
# ADT TESTS
#######################################################################

add_executable(scope_exit_test scope_exit_test.cc)
target_link_libraries(scope_exit_test srslte_common)
add_test(scope_exit_test scope_exit_test)

add_executable(expected_test expected_test.cc)
target_link_libraries(expected_test srslte_common)
add_test(expected_test expected_test)

add_executable(bounded_bitset_test bounded_bitset_test.cc)
target_link_libraries(bounded_bitset_test srslte_common)
add_test(bounded_bitset_test bounded_bitset_test)

add_executable(span_test span_test.cc)
target_link_libraries(span_test srslte_common)
add_test(span_test span_test)

add_executable(interval_test interval_test.cc)
target_link_libraries(interval_test srslte_common)
add_test(interval_test interval_test)

add_executable(observer_test observer_test.cc)
target_link_libraries(observer_test srslte_common)
add_test(observer_test observer_test)
