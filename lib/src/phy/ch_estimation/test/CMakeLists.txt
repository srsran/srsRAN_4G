#
# Copyright 2013-2020 Software Radio Systems Limited
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

########################################################################
# Downlink Channel Estimation TEST  
########################################################################

add_executable(chest_test_dl chest_test_dl.c)
target_link_libraries(chest_test_dl srslte_phy)

add_test(chest_test_dl_cellid0 chest_test_dl -c 0) 
add_test(chest_test_dl_cellid1 chest_test_dl -c 1) 
add_test(chest_test_dl_cellid2 chest_test_dl -c 2) 

add_test(chest_test_dl_cellid0_50prb chest_test_dl -c 0 -r 50)
add_test(chest_test_dl_cellid1_50prb chest_test_dl -c 1 -r 50)
add_test(chest_test_dl_cellid2_50prb chest_test_dl -c 2 -r 50)


########################################################################
# Uplink Channel Estimation TEST  
########################################################################

add_executable(chest_test_ul chest_test_ul.c)
target_link_libraries(chest_test_ul srslte_phy srslte_common)

add_executable(refsignal_ul_test_all refsignal_ul_test.c)
target_link_libraries(refsignal_ul_test_all srslte_phy srslte_common)

add_test(chest_test_ul_cellid0 chest_test_ul -c 0 -r 50) 
add_test(chest_test_ul_cellid1 chest_test_ul -c 1 -r 50) 
add_test(chest_test_ul_cellid1 chest_test_ul -c 2 -r 50)

########################################################################
# Uplink Sounding Reference Signals Channel Estimation TEST
########################################################################

add_executable(chest_test_srs chest_test_srs.c)
target_link_libraries(chest_test_srs srslte_phy srslte_common)

foreach (cell_n_prb 6 15 25 50 75 100)
  add_test(chest_test_srs_${cell_n_prb} chest_test_srs -c 2 -r ${cell_n_prb})
  set_tests_properties(chest_test_srs_${cell_n_prb} PROPERTIES LABELS "long;phy")
endforeach(cell_n_prb 6 15 25 50 75 100)


########################################################################
# Downlink Channel Estimation for NB-IoT TEST
########################################################################

add_executable(chest_nbiot_test_dl chest_nbiot_test_dl.c)
target_link_libraries(chest_nbiot_test_dl srslte_phy)

add_test(chest_nbiot_test_dl chest_nbiot_test_dl)


########################################################################
# Sidelink Channel Estimation TEST
########################################################################

add_executable(chest_test_sl chest_test_sl.c)
target_link_libraries(chest_test_sl srslte_phy)

add_test(chest_test_sl_psbch chest_test_sl)


########################################################################
# NR PDSCH DMRS Channel Estimation TEST
########################################################################

add_executable(dmrs_pdsch_test dmrs_pdsch_test.c)
target_link_libraries(dmrs_pdsch_test srslte_phy)

add_test(dmrs_pdsch_test dmrs_pdsch_test)
