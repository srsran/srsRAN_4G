#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(sched_nr_test sched_nr_test.cc sched_nr_sim_ue.cc)
target_link_libraries(sched_nr_test
        srsgnb_mac
        srsran_common
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_test(sched_nr_test sched_nr_test)
