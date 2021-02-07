#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Downlink Channel Estimation TEST  
########################################################################

add_executable(chest_test_dl chest_test_dl.c)
target_link_libraries(chest_test_dl srslte_phy)

add_test(chest_test_dl_cellid0 chest_test_dl -c 0) 
add_test(chest_test_dl_cellid1 chest_test_dl -c 1) 
add_test(chest_test_dl_cellid2 chest_test_dl -c 2) 
set_tests_properties(chest_test_dl_cellid0 PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_dl_cellid1 PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_dl_cellid2 PROPERTIES LABELS "lib;phy")

add_test(chest_test_dl_cellid0_50prb chest_test_dl -c 0 -r 50)
add_test(chest_test_dl_cellid1_50prb chest_test_dl -c 1 -r 50)
add_test(chest_test_dl_cellid2_50prb chest_test_dl -c 2 -r 50)
set_tests_properties(chest_test_dl_cellid0_50prb PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_dl_cellid1_50prb PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_dl_cellid2_50prb PROPERTIES LABELS "lib;phy")


########################################################################
# Uplink Channel Estimation TEST  
########################################################################

add_executable(chest_test_ul chest_test_ul.c)
target_link_libraries(chest_test_ul srslte_phy srslte_common)

add_executable(refsignal_ul_test_all refsignal_ul_test.c)
target_link_libraries(refsignal_ul_test_all srslte_phy srslte_common)

add_test(chest_test_ul_cellid0 chest_test_ul -c 0 -r 50) 
add_test(chest_test_ul_cellid1 chest_test_ul -c 1 -r 50) 
add_test(chest_test_ul_cellid2 chest_test_ul -c 2 -r 50)
set_tests_properties(chest_test_ul_cellid0 PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_ul_cellid1 PROPERTIES LABELS "lib;phy")
set_tests_properties(chest_test_ul_cellid2 PROPERTIES LABELS "lib;phy")

########################################################################
# Uplink Sounding Reference Signals Channel Estimation TEST
########################################################################

add_executable(chest_test_srs chest_test_srs.c)
target_link_libraries(chest_test_srs srslte_phy srslte_common)

foreach (cell_n_prb 6 15 25 50 75 100)
  add_test(chest_test_srs_${cell_n_prb} chest_test_srs -c 2 -r ${cell_n_prb})
  set_tests_properties(chest_test_srs_${cell_n_prb} PROPERTIES LABELS "lib;long;phy")
endforeach(cell_n_prb 6 15 25 50 75 100)


########################################################################
# Downlink Channel Estimation for NB-IoT TEST
########################################################################

add_executable(chest_nbiot_test_dl chest_nbiot_test_dl.c)
target_link_libraries(chest_nbiot_test_dl srslte_phy)

add_test(chest_nbiot_test_dl chest_nbiot_test_dl)
set_tests_properties(chest_nbiot_test_dl PROPERTIES LABELS "lib;phy")


########################################################################
# Sidelink Channel Estimation TEST
########################################################################

add_executable(chest_test_sl chest_test_sl.c)
target_link_libraries(chest_test_sl srslte_phy)

add_test(chest_test_sl_psbch chest_test_sl)
set_tests_properties(chest_test_sl_psbch PROPERTIES LABELS "lib;phy")


########################################################################
# NR PDSCH DMRS Channel Estimation TEST
########################################################################

add_executable(dmrs_pdsch_test dmrs_pdsch_test.c)
target_link_libraries(dmrs_pdsch_test srslte_phy)

add_test(dmrs_pdsch_test dmrs_pdsch_test)
set_tests_properties(dmrs_pdsch_test PROPERTIES LABELS "lib;phy")


########################################################################
# NR PDSCH DMRS Channel Estimation TEST
########################################################################

add_executable(dmrs_pdcch_test dmrs_pdcch_test.c)
target_link_libraries(dmrs_pdcch_test srslte_phy)

add_test(dmrs_pdcch_test dmrs_pdcch_test)
set_tests_properties(dmrs_pdcch_test PROPERTIES LABELS "lib;phy")


########################################################################
# NR CSI RS Measurement test
########################################################################

add_executable(csi_rs_test csi_rs_test.c)
target_link_libraries(csi_rs_test srslte_phy)

add_test(csi_rs_test csi_rs_test -o 3 -S 0 -L 150 -f 3 -p 15)
set_tests_properties(csi_rs_test PROPERTIES LABELS "lib;phy")

