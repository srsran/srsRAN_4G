#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CTEST_LABELS "srsue;phy")

include_directories(
        ${Boost_INCLUDE_DIRS}
        ${SEC_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}
)

link_directories(
        ${Boost_LIBRARY_DIRS}
        ${SEC_LIBRARY_DIRS}
)

add_executable(ue_phy_test ue_phy_test.cc)
target_link_libraries(ue_phy_test
        srsue_phy
        srslte_common
        srslte_phy
        srslte_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
# Test disabled, it is not 100 deterministic.
#add_test(ue_phy_test ue_phy_test)

add_executable(scell_search_test scell_search_test.cc)
target_link_libraries(scell_search_test
        srsue_phy
        srslte_common
        srslte_phy
        srslte_radio
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})
add_lte_test(scell_search_test scell_search_test --duration=5 --cell.nof_prb=6 --active_cell_list=2,3,4,5,6 --simulation_cell_list=1,2,3,4,5,6 --channel_period_s=30 --channel.hst.fd=750 --channel.delay_max=10000)
