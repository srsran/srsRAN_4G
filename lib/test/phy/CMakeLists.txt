#
# Copyright 2013-2021 Software Radio Systems Limited
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

#######################################################################
# PHY TESTS
#######################################################################
set(CTEST_LABELS "lib;phy")

add_executable(phy_dl_test phy_dl_test.c)
target_link_libraries(phy_dl_test srsran_phy srsran_common srsran_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# All valid number of PRBs for PUSCH
set(ue_dl_min_mcs 0)
set(ue_dl_max_mcs 28)
set(ue_dl_step_mcs 7)
foreach (cp  0  1)
  foreach (cell_n_prb 6 15 25 50 75 100)
      foreach (allow_256 0 1)
          foreach (ue_dl_tm 1 2 3 4)
              foreach (ue_dl_mcs RANGE ${ue_dl_min_mcs} ${ue_dl_max_mcs} ${ue_dl_step_mcs})
                  set(phy_dl_test_args "")
                  if(NOT ((${cp} EQUAL 1) AND (ue_dl_mcs GREATER 26)))
                    set(phy_dl_test_args ${phy_dl_test_args} -p ${cell_n_prb})
                    set(phy_dl_test_args ${phy_dl_test_args} -t ${ue_dl_tm})
                    set(phy_dl_test_args ${phy_dl_test_args} -E ${cp})
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
                  endif(NOT ((${cp} EQUAL 1) AND (ue_dl_mcs GREATER 26)))
              endforeach (ue_dl_mcs)
          endforeach (ue_dl_tm)
      endforeach (allow_256 0 1)
  endforeach (cell_n_prb)
endforeach (cp)

add_executable(pucch_ca_test pucch_ca_test.c)
target_link_libraries(pucch_ca_test srsran_phy srsran_common srsran_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_lte_test(pucch_ca_test pucch_ca_test)

add_executable(phy_dl_nr_test phy_dl_nr_test.c)
target_link_libraries(phy_dl_nr_test srsran_phy srsran_common srsran_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# Create a test case for each possible bandwidth described in TS 38.104 Table 5.3.2-1: Transmission bandwidth
# configuration N RB for FR1
foreach(rb 25 52 79 106 133 160 216 270)
  # Basic test with 25 RB grant, maximum MCS, 64QAM
  add_nr_test(phy_dl_nr_test_${rb}prb phy_dl_nr_test -P ${rb} -p 25 -m 28)

  # Full BW grant, minimum MCS, with reserved RE
  add_nr_test(phy_dl_nr_test_${rb}prb_rvd phy_dl_nr_test -P ${rb} -p ${rb} -m 0
          -R 0 ${rb} 1 010010010010 00000000010000
          -R 0 ${rb} 1 100100100100 00000010000000)

  # 25 RB grant with interleaved PDCCH
  add_nr_test(phy_dl_nr_test_${rb}prb_interleaved phy_dl_nr_test -P ${rb} -p 25 -m 10 -I)

  # Maximum throughput with 256QAM
  add_nr_test(phy_dl_nr_test_${rb}prb_256qam phy_dl_nr_test -P ${rb} -p ${rb} -m 27 -T 256qam -v -d 1 1 1 -n 10)

  # Maximum throughput with 64QAM and CFO+Delay impairments
  add_nr_test(phy_dl_nr_test_${rb}prb_cfo_delay phy_dl_nr_test -P ${rb} -p ${rb} -m 27 -C 100.0 -D 4 -n 10)

endforeach()
