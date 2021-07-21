#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (RF_FOUND AND ENABLE_SRSUE AND ENABLE_SRSENB)
    set(NR_PHY_TEST_GNB_NOF_THREADS 1)
    set(NR_PHY_TEST_UE_NOF_THREADS 1)
    set(NR_PHY_TEST_BW 10MHz)

    add_executable(nr_phy_test nr_phy_test.cc)
    target_link_libraries(nr_phy_test
            srsue_phy
            srsran_common
            srsran_phy
            srsran_radio
            srsenb_phy
            srsgnb_mac
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${ATOMIC_LIBS})

    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_dl_default nr_phy_test
            --reference=carrier=${NR_PHY_TEST_BW}
            --duration=100
            --gnb.stack.pdsch.slots=0,1,2,3,4,5
            --gnb.stack.pusch.slots=none
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )

    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_ts38101/5.2-1 nr_phy_test
            --reference=carrier=${NR_PHY_TEST_BW},pdsch=ts38101/5.2-1
            --duration=100
            --gnb.stack.pdsch.mcs=27
            --gnb.stack.pdsch.start=0
            --gnb.stack.pdsch.length=52
            --gnb.stack.pdsch.slots=0,1,2,3,4,5
            --gnb.stack.pusch.slots=none
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )

    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_ul_only nr_phy_test
            --reference=carrier=${NR_PHY_TEST_BW}
            --duration=100 # 100 slots
            --gnb.stack.pdsch.slots=6 # No PDSCH
            --gnb.stack.pusch.slots=6,7,8,9 # All possible UL slots
            --gnb.stack.pusch.start=0 # Start at RB 0
            --gnb.stack.pusch.length=52 # Full 10 MHz BW
            --gnb.stack.pusch.mcs=28 # Maximum MCS
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )

    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_bidir nr_phy_test
            --reference=carrier=${NR_PHY_TEST_BW}
            --duration=100 # 100 slots
            --gnb.stack.pdsch.slots=0,1,2,3,4,5 # All possible DL slots
            --gnb.stack.pdsch.start=0 # Start at RB 0
            --gnb.stack.pdsch.length=52 # Full 10 MHz BW
            --gnb.stack.pdsch.mcs=28 # Maximum MCS
            --gnb.stack.pusch.slots=6,7,8,9 # All possible UL slots
            --gnb.stack.pusch.start=0 # Start at RB 0
            --gnb.stack.pusch.length=52 # Full 10 MHz BW
            --gnb.stack.pusch.mcs=28 # Maximum MCS
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )

    add_nr_test(nr_phy_test_10MHz_bidir_sched nr_phy_test
            --duration=100 # 100 slots
            --gnb.stack.pdsch.slots=0,1,2,3,4,5 # All possible DL slots
            --gnb.stack.pdsch.start=0 # Start at RB 0
            --gnb.stack.pdsch.length=52 # Full 10 MHz BW
            --gnb.stack.pdsch.mcs=28 # Maximum MCS
            --gnb.stack.pusch.slots=6,7,8,9 # All possible UL slots
            --gnb.stack.pusch.start=0 # Start at RB 0
            --gnb.stack.pusch.length=52 # Full 10 MHz BW
            --gnb.stack.pusch.mcs=28 # Maximum MCS
            --gnb.stack.use_dummy_sched=false # Use real NR scheduler
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )

    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_prach nr_phy_test
            --reference=carrier=${NR_PHY_TEST_BW}
            --duration=1000 # 100 slots
            --gnb.stack.pdsch.slots=none # No PDSCH
            --gnb.stack.pusch.slots=none # No PUSCH
            --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
            --ue.stack.prach.period=30 # Transmit PRACH every 30 radio frames
            --ue.stack.prach.preamble=10 # Use preamble 10
            --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
            )
endif ()
