#
# Copyright 2013-2019 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

#######################################################################
# PHY TESTS
#######################################################################
add_executable(phy_dl_test phy_dl_test.c)
target_link_libraries(phy_dl_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(phy_dl_test phy_dl_test)

# Blacklist of tests for ARM
set(arm_black_list -p6-t2-q-m27 -p6-t3-q-m27 -p6-t4-q-m27 -p25-t3-m28 -p25-t4-m28 -p25-t2-q-m27 -p25-t3-q-m27 -p25-t4-q-m27 )

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

                list (FIND arm_black_list "${test_name_args}" _index)
                if (${_index} GREATER -1 AND ${HAVE_NEON} MATCHES "True" )
                    message(STATUS "Skipping phy_dl_test${test_name_args} on ARM")
                else()
                    add_test(phy_dl_test${test_name_args} phy_dl_test ${phy_dl_test_args})
                endif()
            endforeach (ue_dl_mcs)
        endforeach (ue_dl_tm)
    endforeach (allow_256 0 1)
endforeach (cell_n_prb)
