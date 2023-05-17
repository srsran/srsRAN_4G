#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

if (RF_FOUND AND ENABLE_SRSUE AND ENABLE_SRSENB)
    # gNb options
    set(NR_PHY_TEST_GNB_NOF_THREADS 1)
    set(NR_PHY_TEST_GNB_PHY_LOG_LEVEL "error")
    set(NR_PHY_TEST_GNB_STACK_LOG_LEVEL "error")

    # UE options
    set(NR_PHY_TEST_UE_NOF_THREADS 1)
    set(NR_PHY_TEST_UE_PHY_LOG_LEVEL "error")

    # Build common arguments
    set(NR_PHY_TEST_COMMON_ARGS
        --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
        --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
        --ue.phy.log.level=${NR_PHY_TEST_UE_PHY_LOG_LEVEL}
        --gnb.phy.log.level=${NR_PHY_TEST_GNB_PHY_LOG_LEVEL}
        --gnb.stack.log.level=${NR_PHY_TEST_GNB_STACK_LOG_LEVEL})

    add_executable(nr_phy_test nr_phy_test.cc)
    target_link_libraries(nr_phy_test
            srsue_phy
            srsran_common
            rrc_nr_asn1
            srsran_phy
            srsran_radio
            srsenb_phy
            srsgnb_mac
            srsran_mac
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${ATOMIC_LIBS})
    # For each supported bandwidth
    foreach (NR_PHY_TEST_BW "10MHz" "20MHz")
        # For dummy and real scheduler
        foreach (NR_PHY_TEST_MAC_DUMMY "dummymac" "realmac")
            # For each supported frame structure
            foreach (NR_PHY_TEST_DUPLEX "FDD" "6D+4U" "FR1.15-1")
                set(NR_PHY_TEST_DURATION_MS 50)

                # DL flooding only
                foreach (NR_PHY_TEST_PDSCH "default" "ts38101/5.2-1")
                    add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_${NR_PHY_TEST_MAC_DUMMY}_${NR_PHY_TEST_DUPLEX}_dl_${NR_PHY_TEST_PDSCH} nr_phy_test
                            --reference=carrier=${NR_PHY_TEST_BW},duplex=${NR_PHY_TEST_DUPLEX},pdsch=${NR_PHY_TEST_PDSCH}
                            --duration=${NR_PHY_TEST_DURATION_MS}
                            --gnb.stack.pdsch.slots=all
                            --gnb.stack.pdsch.start=0 # Start at RB 0
                            --gnb.stack.pdsch.length=52 # Full 10 MHz BW
                            --gnb.stack.pdsch.mcs=27 # Maximum MCS
                            --gnb.stack.pusch.slots=none
                            --gnb.stack.use_dummy_mac=${NR_PHY_TEST_MAC_DUMMY} # Use real/dummy NR MAC
                            ${NR_PHY_TEST_COMMON_ARGS}
                            )
                endforeach ()

                # UL flooding
                add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_${NR_PHY_TEST_MAC_DUMMY}_${NR_PHY_TEST_DUPLEX}_ul_only nr_phy_test
                        --reference=carrier=${NR_PHY_TEST_BW},duplex=${NR_PHY_TEST_DUPLEX}
                        --duration=${NR_PHY_TEST_DURATION_MS}
                        --gnb.stack.pdsch.slots=none
                        --gnb.stack.pusch.slots=all
                        --gnb.stack.pusch.start=0 # Start at RB 0
                        --gnb.stack.pusch.length=52 # Full 10 MHz BW
                        --gnb.stack.pusch.mcs=28 # Maximum MCS
                        --gnb.stack.use_dummy_mac=${NR_PHY_TEST_MAC_DUMMY} # Use real/dummy NR MAC
                        ${NR_PHY_TEST_COMMON_ARGS}
                        )

                # DL and UL flooding
                add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_${NR_PHY_TEST_MAC_DUMMY}_${NR_PHY_TEST_DUPLEX}_bidir nr_phy_test
                        --reference=carrier=${NR_PHY_TEST_BW},duplex=${NR_PHY_TEST_DUPLEX}
                        --duration=${NR_PHY_TEST_DURATION_MS}
                        --gnb.stack.pdsch.slots=all
                        --gnb.stack.pdsch.start=0 # Start at RB 0
                        --gnb.stack.pdsch.length=52 # Full 10 MHz BW
                        --gnb.stack.pdsch.mcs=28 # Maximum MCS
                        --gnb.stack.pusch.slots=all
                        --gnb.stack.pusch.start=0 # Start at RB 0
                        --gnb.stack.pusch.length=52 # Full 10 MHz BW
                        --gnb.stack.pusch.mcs=28 # Maximum MCS
                        --gnb.stack.use_dummy_mac=${NR_PHY_TEST_MAC_DUMMY} # Use real/dummy NR MAC
                        ${NR_PHY_TEST_COMMON_ARGS}
                        )
            endforeach ()
        endforeach ()

        # Test PRACH transmission and detection
        add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_prach_fdd nr_phy_test
                --reference=carrier=${NR_PHY_TEST_BW},duplex=FDD
                --duration=1000 # 1000 slots
                --gnb.stack.pdsch.slots=none # No PDSCH
                --gnb.stack.pusch.slots=none # No PUSCH
                --gnb.phy.nof_threads=${NR_PHY_TEST_GNB_NOF_THREADS}
                --ue.stack.prach.period=30 # Transmit PRACH every 30 radio frames
                --ue.phy.nof_threads=${NR_PHY_TEST_UE_NOF_THREADS}
                )

        add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_prach_tdd nr_phy_test
                --reference=carrier=${NR_PHY_TEST_BW},duplex=6D+4U
                --duration=1000 # 1000 slots
                --gnb.stack.pdsch.slots=none # No PDSCH
                --gnb.stack.pusch.slots=none # No PUSCH
                --ue.stack.prach.period=30 # Transmit PRACH every 30 radio frames
                ${NR_PHY_TEST_COMMON_ARGS}
                )

        # Test scheduling request
        add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_sr nr_phy_test
                --reference=carrier=${NR_PHY_TEST_BW}
                --duration=1000 # 1000 slots
                --gnb.stack.pdsch.slots=none # No PDSCH
                --gnb.stack.pusch.slots=none # No PUSCH
                --ue.stack.sr.period=4 # Transmit SR every 4 opportunities
                ${NR_PHY_TEST_COMMON_ARGS}
                )

        # Test scheduling request multiplexed with HARQ ACK feedback
        add_nr_test(nr_phy_test_${NR_PHY_TEST_BW}_sr_harq nr_phy_test
                --reference=carrier=${NR_PHY_TEST_BW}
                --duration=1000 # 1000 slots
                --gnb.stack.pdsch.slots=all # All PDSCH
                --gnb.stack.pusch.slots=none # No PUSCH
                --ue.stack.sr.period=4 # Transmit SR every 4 opportunities
                ${NR_PHY_TEST_COMMON_ARGS}
                )
    endforeach ()
endif ()
