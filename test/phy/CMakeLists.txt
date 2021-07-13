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

    add_nr_test(nr_phy_test_10MHz_dl_only nr_phy_test
            --duration=100
            --gnb.stack.pdsch.slots=\"0,1,2,3,4,5\"
            --gnb.stack.pusch.slots=\"\")

    add_nr_test(nr_phy_test_10MHz_ul_only nr_phy_test
            --duration=100 # 100 slots
            --gnb.stack.pdsch.slots=6 # No PDSCH
            --gnb.stack.pusch.slots=6,7,8,9 # All possible UL slots
            --gnb.stack.pusch.start=0 # Start at RB 0
            --gnb.stack.pusch.length=52 # Full 10 MHz BW
            --gnb.stack.pusch.mcs=28 # Maximum MCS
            )

    add_nr_test(nr_phy_test_10MHz_bidir nr_phy_test
            --duration=100 # 100 slots
            --gnb.stack.pdsch.slots=0,1,2,3,4,5 # All possible DL slots
            --gnb.stack.pdsch.start=0 # Start at RB 0
            --gnb.stack.pdsch.length=52 # Full 10 MHz BW
            --gnb.stack.pdsch.mcs=28 # Maximum MCS
            --gnb.stack.pusch.slots=6,7,8,9 # All possible UL slots
            --gnb.stack.pusch.start=0 # Start at RB 0
            --gnb.stack.pusch.length=52 # Full 10 MHz BW
            --gnb.stack.pusch.mcs=28 # Maximum MCS
            )

    add_nr_test(nr_phy_test_10MHz_prach nr_phy_test
            --duration=1000 # 100 slots
            --gnb.stack.pdsch.slots=6 # No PDSCH
            --gnb.stack.pusch.slots=0 # No PUSCH
            --ue.stack.prach.period=30 # Transmit PRACH every 30 radio frames
            --ue.stack.prach.preamble=10 # Use preamble 10
            )
endif ()
