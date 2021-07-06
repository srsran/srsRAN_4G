#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (RF_FOUND AND ENABLE_SRSUE AND ENABLE_SRSENB)
    add_executable(nr_phy_test nr_phy_test.cc)
    target_link_libraries(nr_phy_test
            srsue_phy_nr
            srsue_phy
            srsran_common
            srsran_phy
            srsran_radio
            srsenb_phy
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${ATOMIC_LIBS})

    add_nr_test(nr_phy_test_10MHz_dl_only nr_phy_test --duration=100 --gnb.stack.pusch.slots="")
    add_nr_test(nr_phy_test_10MHz_ul_only nr_phy_test --duration=100 --gnb.stack.pdsch.slots="")
    add_nr_test(nr_phy_test_10MHz_bidir nr_phy_test --duration=100)
endif ()
