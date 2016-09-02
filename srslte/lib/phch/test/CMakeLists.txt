#
# Copyright 2013-2015 Software Radio Systems Limited
#
# This file is part of the srsLTE library.
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
target_link_libraries(pbch_test srslte)

add_test(pbch_test_6 pbch_test -p 1 -n 6 -c 100) 
add_test(pbch_test_62 pbch_test -p 2 -n 6 -c 100) 
add_test(pbch_test_64 pbch_test -p 4 -n 6 -c 100) 
add_test(pbch_test_50 pbch_test -p 1 -n 50 -c 50) 
add_test(pbch_test_502 pbch_test -p 2 -n 50 -c 50) 
add_test(pbch_test_504 pbch_test -p 4 -n 50 -c 50) 
 
BuildMex(MEXNAME pbch SOURCES pbch_test_mex.c LIBRARIES srslte_static srslte_mex)


########################################################################
# PCFICH TEST  
########################################################################

add_executable(pcfich_test pcfich_test.c)
target_link_libraries(pcfich_test srslte)

add_test(pcfich_test_6 pcfich_test -p 1 -n 6) 
add_test(pcfich_test_62 pcfich_test -p 2 -n 6) 
add_test(pcfich_test_64 pcfich_test -p 4 -n 6) 
add_test(pcfich_test_10 pcfich_test -p 1 -n 10) 
add_test(pcfich_test_102 pcfich_test -p 2 -n 10) 
add_test(pcfich_test_104 pcfich_test -p 4 -n 10) 

BuildMex(MEXNAME pcfich SOURCES pcfich_test_mex.c LIBRARIES srslte_static srslte_mex)

########################################################################
# PHICH TEST  
########################################################################

add_executable(phich_test phich_test.c)
target_link_libraries(phich_test srslte)

add_test(phich_test_6 phich_test -p 1 -n 6) 
add_test(phich_test_62 phich_test -p 2 -n 6) 
add_test(phich_test_64 phich_test -p 4 -n 6 -g 1/6)
 
add_test(phich_test_6e phich_test -p 1 -n 6 -e) 
add_test(phich_test_62e phich_test -p 2 -n 6 -e -l) 
add_test(phich_test_64e phich_test -p 4 -n 6 -e -l -g 2)
 
add_test(phich_test_10 phich_test -p 1 -n 10 -e) 
add_test(phich_test_102 phich_test -p 2 -n 10 -g 2) 
add_test(phich_test_104 phich_test -p 4 -n 10 -e -l -g 1/2) 
 
BuildMex(MEXNAME phich SOURCES phich_test_mex.c LIBRARIES srslte_static srslte_mex)

########################################################################
# PDCCH TEST  
########################################################################

add_executable(pdcch_test pdcch_test.c)
target_link_libraries(pdcch_test srslte)

add_test(pdcch_test pdcch_test) 

BuildMex(MEXNAME pdcch SOURCES pdcch_test_mex.c LIBRARIES srslte_static srslte_mex)

########################################################################
# PDSCH TEST  
########################################################################

add_executable(pdsch_test pdsch_test.c)
target_link_libraries(pdsch_test srslte)

add_test(pdsch_test_qpsk pdsch_test -m 10 -n 50 -r 1)
add_test(pdsch_test_qam16 pdsch_test -m 20 -n 100)
add_test(pdsch_test_qam16 pdsch_test -m 20 -n 100 -r 2)
add_test(pdsch_test_qam64 pdsch_test -m 28 -n 100)

BuildMex(MEXNAME pdsch SOURCES pdsch_test_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME dlsch_encode SOURCES dlsch_encode_test_mex.c LIBRARIES srslte_static srslte_mex)

########################################################################
# FILE TEST  
########################################################################

add_executable(pbch_file_test pbch_file_test.c)
target_link_libraries(pbch_file_test srslte)

add_executable(pcfich_file_test pcfich_file_test.c)
target_link_libraries(pcfich_file_test srslte)

add_executable(phich_file_test phich_file_test.c)
target_link_libraries(phich_file_test srslte)

add_executable(pdcch_file_test pdcch_file_test.c)
target_link_libraries(pdcch_file_test srslte)

add_executable(pdsch_pdcch_file_test pdsch_pdcch_file_test.c)
target_link_libraries(pdsch_pdcch_file_test srslte)

add_test(pbch_file_test pbch_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.dat) 
add_test(pcfich_file_test pcfich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
add_test(phich_file_test phich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
add_test(pdcch_file_test pdcch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat) 
add_test(pdsch_pdcch_file_test pdsch_pdcch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat) 

########################################################################
# PUSCH TEST  
########################################################################

add_executable(pusch_test pusch_test.c)
target_link_libraries(pusch_test srslte)

BuildMex(MEXNAME ulsch_encode SOURCES ulsch_encode_test_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME pusch_encode SOURCES pusch_encode_test_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME pusch SOURCES pusch_test_mex.c LIBRARIES srslte_static srslte_mex)

add_test(pusch_test pusch_test)

########################################################################
# PUCCH TEST  
########################################################################

add_executable(pucch_test pucch_test.c)
target_link_libraries(pucch_test srslte)

add_test(pucch_test pucch_test)

BuildMex(MEXNAME pucch_encode SOURCES pucch_encode_test_mex.c LIBRARIES srslte_static srslte_mex)





########################################################################
# PRACH TEST  
########################################################################

add_executable(prach_test prach_test.c)
target_link_libraries(prach_test srslte)

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
target_link_libraries(prach_test_multi srslte)

add_test(prach_test_multi prach_test_multi)

add_test(prach_test_multi_n32 prach_test_multi -n 32)
add_test(prach_test_multi_n16 prach_test_multi -n 16)
add_test(prach_test_multi_n8 prach_test_multi -n 8)
add_test(prach_test_multi_n4 prach_test_multi -n 4)
 
 
BuildMex(MEXNAME prach SOURCES prach_test_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME prach_detect SOURCES prach_detect_test_mex.c LIBRARIES srslte_static srslte_mex)


if(UHD_FOUND)
  add_executable(prach_test_usrp prach_test_usrp.c)
  target_link_libraries(prach_test_usrp srslte pthread)
endif(UHD_FOUND)
