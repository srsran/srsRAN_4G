#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(proc_ra_nr_test proc_ra_nr_test.cc)
target_link_libraries(proc_ra_nr_test srsue_mac_nr srslte_common)
add_test(proc_ra_nr_test proc_ra_nr_test)