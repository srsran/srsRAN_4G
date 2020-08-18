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
