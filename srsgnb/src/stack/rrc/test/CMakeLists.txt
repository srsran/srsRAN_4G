#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(rrc_nr_test rrc_nr_test.cc)
target_link_libraries(rrc_nr_test srsgnb_rrc test_helpers ${ATOMIC_LIBS})
add_test(rrc_nr_test rrc_nr_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)