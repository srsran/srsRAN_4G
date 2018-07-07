#
# Copyright 2013-2017 Software Radio Systems Limited
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# PBCH TEST  
########################################################################

add_executable(pbch_test pbch_test.c)
target_link_libraries(pbch_test srslte_phy)

add_test(pbch_test_6 pbch_test -p 1 -n 6 -c 100) 
add_test(pbch_test_62 pbch_test -p 2 -n 6 -c 100) 
add_test(pbch_test_64 pbch_test -p 4 -n 6 -c 100) 
add_test(pbch_test_50 pbch_test -p 1 -n 50 -c 50) 
add_test(pbch_test_502 pbch_test -p 2 -n 50 -c 50) 
add_test(pbch_test_504 pbch_test -p 4 -n 50 -c 50) 


########################################################################
# PCFICH TEST  
########################################################################

add_executable(pcfich_test pcfich_test.c)
target_link_libraries(pcfich_test srslte_phy)

add_test(pcfich_test_6 pcfich_test -p 1 -n 6) 
add_test(pcfich_test_62 pcfich_test -p 2 -n 6) 
add_test(pcfich_test_64 pcfich_test -p 4 -n 6) 
add_test(pcfich_test_10 pcfich_test -p 1 -n 10) 
add_test(pcfich_test_102 pcfich_test -p 2 -n 10) 
add_test(pcfich_test_104 pcfich_test -p 4 -n 10) 

########################################################################
# PHICH TEST  
########################################################################

add_executable(phich_test phich_test.c)
target_link_libraries(phich_test srslte_phy)

add_test(phich_test_6 phich_test -p 1 -n 6) 
add_test(phich_test_62 phich_test -p 2 -n 6) 
add_test(phich_test_64 phich_test -p 4 -n 6 -g 1/6)
 
add_test(phich_test_6e phich_test -p 1 -n 6 -e) 
add_test(phich_test_62e phich_test -p 2 -n 6 -e -l) 
add_test(phich_test_64e phich_test -p 4 -n 6 -e -l -g 2)
 
add_test(phich_test_10 phich_test -p 1 -n 10 -e) 
add_test(phich_test_102 phich_test -p 2 -n 10 -g 2) 
add_test(phich_test_104 phich_test -p 4 -n 10 -e -l -g 1/2) 

########################################################################
# PDCCH TEST  
########################################################################

add_executable(pdcch_test pdcch_test.c)
target_link_libraries(pdcch_test srslte_phy)

add_test(pdcch_test pdcch_test) 

########################################################################
# PDSCH TEST  
########################################################################

add_executable(pdsch_test pdsch_test.c)
target_link_libraries(pdsch_test srslte_phy)

add_test(pdsch_test_qpsk pdsch_test -m 10 -n 50 -r 1)
add_test(pdsch_test_qam16 pdsch_test -m 20 -n 100)
add_test(pdsch_test_qam16 pdsch_test -m 20 -n 100 -r 2)
add_test(pdsch_test_qam64 pdsch_test -n 100)

# PDSCH test for single transmision mode and 2 Rx antennas
add_test(pdsch_test_sin_6   pdsch_test -x single -a 2 -n 6)
add_test(pdsch_test_sin_12  pdsch_test -x single -a 2 -n 12)
add_test(pdsch_test_sin_25  pdsch_test -x single -a 2 -n 25)
add_test(pdsch_test_sin_50  pdsch_test -x single -a 2 -n 50)
add_test(pdsch_test_sin_75  pdsch_test -x single -a 2 -n 75)
add_test(pdsch_test_sin_100 pdsch_test -x single -a 2 -n 100)

# PDSCH test for transmit diversity transmision mode (1 codeword)
add_test(pdsch_test_div_6   pdsch_test -x diversity -a 2 -n 6)
add_test(pdsch_test_div_12  pdsch_test -x diversity -a 2 -n 12)
add_test(pdsch_test_div_25  pdsch_test -x diversity -a 2 -n 25)
add_test(pdsch_test_div_50  pdsch_test -x diversity -a 2 -n 50)
add_test(pdsch_test_div_75  pdsch_test -x diversity -a 2 -n 75)
add_test(pdsch_test_div_100 pdsch_test -x diversity -a 2 -n 100)

# PDSCH test for CDD transmision mode (2 codeword)
add_test(pdsch_test_cdd_6   pdsch_test -x cdd -a 2 -t 0 -n 6)
add_test(pdsch_test_cdd_12  pdsch_test -x cdd -a 2 -t 0 -n 12)
add_test(pdsch_test_cdd_25  pdsch_test -x cdd -a 2 -t 0 -n 25)
add_test(pdsch_test_cdd_50  pdsch_test -x cdd -a 2 -t 0 -n 50)
add_test(pdsch_test_cdd_75  pdsch_test -x cdd -a 2 -t 0 -n 75)
add_test(pdsch_test_cdd_100 pdsch_test -x cdd -a 2 -t 0 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 0 (1 codeword)
add_test(pdsch_test_multiplex1cw_p0_6   pdsch_test -x multiplex -a 2 -p 0 -n 6)
add_test(pdsch_test_multiplex1cw_p0_12  pdsch_test -x multiplex -a 2 -p 0 -n 12)
add_test(pdsch_test_multiplex1cw_p0_25  pdsch_test -x multiplex -a 2 -p 0 -n 25)
add_test(pdsch_test_multiplex1cw_p0_50  pdsch_test -x multiplex -a 2 -p 0 -n 50)
add_test(pdsch_test_multiplex1cw_p0_75  pdsch_test -x multiplex -a 2 -p 0 -n 75)
add_test(pdsch_test_multiplex1cw_p0_100 pdsch_test -x multiplex -a 2 -p 0 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 0 (1 codeword, swapped)
add_test(pdsch_test_multiplex1cw_p0_6_swap   pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 6 -F 1)
add_test(pdsch_test_multiplex1cw_p0_15_swap  pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 15)
add_test(pdsch_test_multiplex1cw_p0_25_swap  pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 25)
add_test(pdsch_test_multiplex1cw_p0_50_swap  pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 50)
add_test(pdsch_test_multiplex1cw_p0_75_swap  pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 75)
add_test(pdsch_test_multiplex1cw_p0_100_swap pdsch_test -x multiplex -a 2 -p 0 -m 0 -r 1 -M 28 -t 0 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 1 (1 codeword)
add_test(pdsch_test_multiplex1cw_p1_6   pdsch_test -x multiplex -a 2 -p 1 -n 6)
add_test(pdsch_test_multiplex1cw_p1_12  pdsch_test -x multiplex -a 2 -p 1 -n 12)
add_test(pdsch_test_multiplex1cw_p1_25  pdsch_test -x multiplex -a 2 -p 1 -n 25)
add_test(pdsch_test_multiplex1cw_p1_50  pdsch_test -x multiplex -a 2 -p 1 -n 50)
add_test(pdsch_test_multiplex1cw_p1_75  pdsch_test -x multiplex -a 2 -p 1 -n 75)
add_test(pdsch_test_multiplex1cw_p1_100 pdsch_test -x multiplex -a 2 -p 1 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 2 (1 codeword)
add_test(pdsch_test_multiplex1cw_p2_6   pdsch_test -x multiplex -a 2 -p 2 -n 6)
add_test(pdsch_test_multiplex1cw_p2_12  pdsch_test -x multiplex -a 2 -p 2 -n 12)
add_test(pdsch_test_multiplex1cw_p2_25  pdsch_test -x multiplex -a 2 -p 2 -n 25)
add_test(pdsch_test_multiplex1cw_p2_50  pdsch_test -x multiplex -a 2 -p 2 -n 50)
add_test(pdsch_test_multiplex1cw_p2_75  pdsch_test -x multiplex -a 2 -p 2 -n 75)
add_test(pdsch_test_multiplex1cw_p2_100 pdsch_test -x multiplex -a 2 -p 2 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 3 (1 codeword)
add_test(pdsch_test_multiplex1cw_p3_6   pdsch_test -x multiplex -a 2 -p 3 -n 6)
add_test(pdsch_test_multiplex1cw_p3_12  pdsch_test -x multiplex -a 2 -p 3 -n 12)
add_test(pdsch_test_multiplex1cw_p3_25  pdsch_test -x multiplex -a 2 -p 3 -n 25)
add_test(pdsch_test_multiplex1cw_p3_50  pdsch_test -x multiplex -a 2 -p 3 -n 50)
add_test(pdsch_test_multiplex1cw_p3_75  pdsch_test -x multiplex -a 2 -p 3 -n 75)
add_test(pdsch_test_multiplex1cw_p3_100 pdsch_test -x multiplex -a 2 -p 3 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 0 (2 codeword)
add_test(pdsch_test_multiplex2cw_p0_6   pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 6)
add_test(pdsch_test_multiplex2cw_p0_12  pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 12)
add_test(pdsch_test_multiplex2cw_p0_25  pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 25)
add_test(pdsch_test_multiplex2cw_p0_50  pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 50)
add_test(pdsch_test_multiplex2cw_p0_75  pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 75)
add_test(pdsch_test_multiplex2cw_p0_100 pdsch_test -x multiplex -a 2 -t 0 -p 0 -n 100)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 0 (2 codeword, swapped)
add_test(pdsch_test_multiplex2cw_p0_6_swap   pdsch_test -x multiplex -a 2 -t 0 -p 0 -M 28 -n 6 -w -F 1)
add_test(pdsch_test_multiplex2cw_p0_12_swap  pdsch_test -x multiplex -a 2 -t 0 -p 0 -m 28 -n 12 -w)
add_test(pdsch_test_multiplex2cw_p0_25_swap  pdsch_test -x multiplex -a 2 -t 0 -p 0 -M 28 -n 25 -w)
add_test(pdsch_test_multiplex2cw_p0_50_swap  pdsch_test -x multiplex -a 2 -t 0 -p 0 -m 28 -n 50 -w)
add_test(pdsch_test_multiplex2cw_p0_75_swap  pdsch_test -x multiplex -a 2 -t 0 -p 0 -M 28 -n 75 -w)
add_test(pdsch_test_multiplex2cw_p0_100_swap pdsch_test -x multiplex -a 2 -t 0 -p 0 -m 28 -n 100 -w)

# PDSCH test for Spatial Multiplex transmision mode with PMI = 1 (2 codeword)
add_test(pdsch_test_multiplex2cw_p1_6   pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 6)
add_test(pdsch_test_multiplex2cw_p1_12  pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 12)
add_test(pdsch_test_multiplex2cw_p1_25  pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 25)
add_test(pdsch_test_multiplex2cw_p1_50  pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 50)
add_test(pdsch_test_multiplex2cw_p1_75  pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 75)
add_test(pdsch_test_multiplex2cw_p1_100 pdsch_test -x multiplex -a 2 -t 0 -p 1 -n 100)

########################################################################
# PMCH TEST  
########################################################################


add_executable(pmch_test pmch_test.c)
target_link_libraries(pmch_test srslte_phy)

add_test(pmch_test_qpsk pmch_test -m 6 -n 50)
add_test(pmch_test_qam16 pmch_test -m 15 -n 100)
add_test(pmch_test_qam64 pmch_test -m 25 -n 100)


########################################################################
# FILE TEST  
########################################################################

add_executable(pbch_file_test pbch_file_test.c)
target_link_libraries(pbch_file_test srslte_phy)

add_executable(pcfich_file_test pcfich_file_test.c)
target_link_libraries(pcfich_file_test srslte_phy)

add_executable(phich_file_test phich_file_test.c)
target_link_libraries(phich_file_test srslte_phy)

add_executable(pdcch_file_test pdcch_file_test.c)
target_link_libraries(pdcch_file_test srslte_phy)

add_executable(pdsch_pdcch_file_test pdsch_pdcch_file_test.c)
target_link_libraries(pdsch_pdcch_file_test srslte_phy)

add_executable(pmch_file_test pmch_file_test.c)
target_link_libraries(pmch_file_test srslte_phy)

add_test(pbch_file_test pbch_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.dat) 
add_test(pcfich_file_test pcfich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
add_test(phich_file_test phich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
add_test(pdcch_file_test pdcch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat) 
add_test(pdsch_pdcch_file_test pdsch_pdcch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat)
add_test(pmch_file_test pmch_file_test  -i ${CMAKE_CURRENT_SOURCE_DIR}/pmch_100prbs_MCS2_SR0.bin) 

########################################################################
# PUSCH TEST  
########################################################################

add_executable(pusch_test pusch_test.c)
target_link_libraries(pusch_test srslte_phy)

if (NOT DEFINED TEST_EXTENSION)
  set(TEST_EXTENSION Normal)
endif(NOT DEFINED TEST_EXTENSION)

if (TEST_EXTENSION STREQUAL Paranoid)
  # All valid number of PRBs for PUSCH
  set(cell_n_prb_valid 1 2 3 4 5 6 8 9 10 12 15 16 18 20 24 25 27 30 32 36 40 45 48 50 54 60 64 72 75 80 81 90 96 100)

  set(pusch_min_mcs 0)
  set(pusch_max_mcs 28)
  set(pusch_step_mcs 1)

  set(pusch_acks -1 0 1)

  set(pusch_cqi none wideband)

else (TEST_EXTENSION STREQUAL Paranoid)
  set(cell_n_prb_valid 6 15 25 50 100)

  set(pusch_min_mcs 0)
  set(pusch_max_mcs 28)
  set(pusch_step_mcs 10)

  set(pusch_acks -1 0)

  set(pusch_cqi none wideband)

endif (TEST_EXTENSION STREQUAL Paranoid)

foreach (cell_n_prb 6 15 25 50 75 100)
  set(pusch_cell_n_prb)
  foreach (n_prb ${cell_n_prb_valid})
    if (NOT (${n_prb} GREATER ${cell_n_prb}))
      set(pusch_cell_n_prb ${pusch_cell_n_prb} ${n_prb})
    endif (NOT (${n_prb} GREATER ${cell_n_prb}))
  endforeach (n_prb)

  foreach (n_prb ${pusch_cell_n_prb})
    foreach (mcs RANGE ${pusch_min_mcs} ${pusch_max_mcs} ${pusch_step_mcs})
      foreach (ack ${pusch_acks})
        foreach (cqi ${pusch_cqi})
          set(pusch_test_args "")

          set(pusch_test_args ${pusch_test_args} -n ${cell_n_prb})
          set(pusch_test_args ${pusch_test_args} -L ${n_prb})

          if (NOT (${ack} EQUAL -1))
            set(pusch_test_args ${pusch_test_args} -p uci_ack ${ack})
            if (mcs EQUAL 28)
              set(mcs 27)
            endif (mcs EQUAL 28)
          endif (NOT (${ack} EQUAL -1))

          if (NOT (${cqi} STREQUAL none))
            set(pusch_test_args ${pusch_test_args} -p uci_cqi ${cqi})
            #if (mcs EQUAL 28)
            #  set(mcs 27)
            #endif (mcs EQUAL 28)
          endif (NOT (${cqi} STREQUAL none))

          set(pusch_test_args ${pusch_test_args} -m ${mcs})

          string(REGEX REPLACE "\ " "" test_name_args ${pusch_test_args})

          add_test(pusch_test${test_name_args} pusch_test ${pusch_test_args})
        endforeach (cqi)
      endforeach (ack)
    endforeach (mcs)
  endforeach (n_prb)
endforeach (cell_n_prb)

########################################################################
# PUCCH TEST  
########################################################################

add_executable(pucch_test pucch_test.c)
target_link_libraries(pucch_test srslte_phy)

add_test(pucch_test pucch_test)
add_test(pucch_test_uci_cqi_decoder pucch_test -q)

########################################################################
# PRACH TEST  
########################################################################

add_executable(prach_test prach_test.c)
target_link_libraries(prach_test srslte_phy)

add_test(prach prach_test)

add_test(prach_256 prach_test -N 256)
add_test(prach_512 prach_test -N 512)
add_test(prach_1024 prach_test -N 1024)
add_test(prach_1536 prach_test -N 1536)
add_test(prach_2048 prach_test -N 2048)

add_test(prach_f0 prach_test -f 0)
add_test(prach_f1 prach_test -f 1)
add_test(prach_f2 prach_test -f 2)
add_test(prach_f3 prach_test -f 3)

add_test(prach_rs1 prach_test -r 1)
add_test(prach_rs2 prach_test -r 2)
add_test(prach_rs3 prach_test -r 3)

add_test(prach_zc0 prach_test -z 0)
add_test(prach_zc2 prach_test -z 2)
add_test(prach_zc3 prach_test -z 3)
 
add_executable(prach_test_multi prach_test_multi.c)
target_link_libraries(prach_test_multi srslte_phy)

add_test(prach_test_multi prach_test_multi)

add_test(prach_test_multi_n32 prach_test_multi -n 32)
add_test(prach_test_multi_n16 prach_test_multi -n 16)
add_test(prach_test_multi_n8 prach_test_multi -n 8)
add_test(prach_test_multi_n4 prach_test_multi -n 4)


if(UHD_FOUND)
  add_executable(prach_test_usrp prach_test_usrp.c)
  target_link_libraries(prach_test_usrp srslte_rf srslte_phy pthread)
endif(UHD_FOUND)
