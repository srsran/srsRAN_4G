#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

#######################################################################
# PHY TESTS
#######################################################################
set(CTEST_LABELS "lib;phy")

add_executable(phy_dl_test phy_dl_test.c)
target_link_libraries(phy_dl_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# All valid number of PRBs for PUSCH
set(ue_dl_min_mcs 0)
set(ue_dl_max_mcs 28)
set(ue_dl_step_mcs 7)

foreach (cell_n_prb 6 15 25 50 75 100)
    foreach (allow_256 0 1)
        foreach (ue_dl_tm 1 2 3 4)
            foreach (ue_dl_mcs RANGE ${ue_dl_min_mcs} ${ue_dl_max_mcs} ${ue_dl_step_mcs})
                set(phy_dl_test_args "")

                set(phy_dl_test_args ${phy_dl_test_args} -p ${cell_n_prb})
                set(phy_dl_test_args ${phy_dl_test_args} -t ${ue_dl_tm})
                if (${allow_256})
                    if (${ue_dl_mcs} EQUAL 28)
                        if (${cell_n_prb} EQUAL 15)
                            set(ue_dl_mcs 26)
                        else (${cell_n_prb} EQUAL 15)
                            set(ue_dl_mcs 27)
                        endif (${cell_n_prb} EQUAL 15)
                    endif (${ue_dl_mcs} EQUAL 28)

                    set(phy_dl_test_args ${phy_dl_test_args} -q)
                endif (${allow_256})
                set(phy_dl_test_args ${phy_dl_test_args} -m ${ue_dl_mcs})
                string(REGEX REPLACE "\ " "" test_name_args ${phy_dl_test_args})
                add_lte_test(phy_dl_test${test_name_args} phy_dl_test ${phy_dl_test_args})
            endforeach (ue_dl_mcs)
        endforeach (ue_dl_tm)
    endforeach (allow_256 0 1)
endforeach (cell_n_prb)

add_executable(pucch_ca_test pucch_ca_test.c)
target_link_libraries(pucch_ca_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_lte_test(pucch_ca_test pucch_ca_test)

add_executable(phy_dl_nr_test phy_dl_nr_test.c)
target_link_libraries(phy_dl_nr_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_nr_test(phy_dl_nr_test phy_dl_nr_test -p 100 -m 28)