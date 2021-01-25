#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(thread_pool_test thread_pool_test.cc)
target_link_libraries(thread_pool_test
        srslte_common)
add_test(thread_pool_test thread_pool_test)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test
        srslte_common)
add_test(thread_test thread_test)

add_executable(band_helper_test band_helper_test.cc)
target_link_libraries(band_helper_test srslte_common)
add_test(band_helper_test band_helper_test)