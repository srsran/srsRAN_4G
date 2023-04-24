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

########################################################################
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

add_executable(pss_file pss_file.c)
target_link_libraries(pss_file srsran_phy)

add_executable(npss_file npss_file.c)
target_link_libraries(npss_file srsran_phy)

if(RF_FOUND)
  add_executable(pss_usrp pss_usrp.c)
  target_link_libraries(pss_usrp srsran_rf srsran_phy)

  add_executable(npss_usrp npss_usrp.c)
  target_link_libraries(npss_usrp srsran_rf srsran_phy)

  add_executable(nsss_usrp nsss_usrp.c)
  target_link_libraries(nsss_usrp srsran_rf srsran_phy)
endif(RF_FOUND)

if(SRSGUI_FOUND)
  include_directories(${SRSGUI_INCLUDE_DIRS})
  target_link_libraries(pss_file ${SRSGUI_LIBRARIES})
  if(RF_FOUND)
    target_link_libraries(pss_usrp ${SRSGUI_LIBRARIES})
    target_link_libraries(npss_usrp ${SRSGUI_LIBRARIES})
  endif(RF_FOUND)
else(SRSGUI_FOUND)
  add_definitions(-DDISABLE_GRAPHICS)
endif(SRSGUI_FOUND)

########################################################################
# SYNC TEST  
########################################################################

add_executable(sync_test sync_test.c)
target_link_libraries(sync_test srsran_phy)

add_executable(sync_nbiot_test sync_nbiot_test.c)
target_link_libraries(sync_nbiot_test srsran_phy)

add_executable(npss_test npss_test.c)
target_link_libraries(npss_test srsran_phy)

add_executable(nsss_test nsss_test.c)
target_link_libraries(nsss_test srsran_phy)

add_test(sync_test_100 sync_test -o 100 -c 501)
add_test(sync_test_400 sync_test -o 400 -c 2)
add_test(sync_test_100_e sync_test -o 100 -e -c 150)
add_test(sync_test_400_e sync_test -o 400 -e -c 151)

add_test(sync_test_100 sync_test -o 100 -p 50 -c 501)
add_test(sync_test_400 sync_test -o 400 -p 50 -c 500)
add_test(sync_test_100_e sync_test -o 100 -e -p 50 -c 133)
add_test(sync_test_400_e sync_test -o 400 -e -p 50 -c 123)

########################################################################
# SYNC NB-IoT TEST
########################################################################

add_test(npss_test_nonoise npss_test)
add_test(nsss_test_nonoise_2 nsss_test -c 2)
add_test(nsss_test_nonoise_501 nsss_test -c 501)

########################################################################
# SYNC SL TEST
########################################################################

add_executable(sync_sl_test sync_sl_test.c)
target_link_libraries(sync_sl_test srsran_phy)

add_executable(psss_file_test psss_file_test.c)
target_link_libraries(psss_file_test srsran_phy)

# SL TM 2
add_test(sync_sl_test_tm2_p6_c_0 sync_sl_test -p 6 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p6_c0_s1.92e6.dat)
add_test(sync_sl_test_tm2_p15_c_84 sync_sl_test -p 15 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p15_c84_s3.84e6.dat)
add_test(sync_sl_test_tm2_p25_c_168 sync_sl_test -p 25 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p25_c168_s7.68e6.dat)
add_test(sync_sl_test_tm2_p50_c_252 sync_sl_test -p 50 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p50_c252_s15.36e6.dat)
add_test(sync_sl_test_tm2_p100_c_335 sync_sl_test -p 100 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p100_c335_s30.72e6.dat)
# Sample offset
add_test(sync_sl_test_tm2_p25_c_168_so sync_sl_test -p 25 -d -o 300 -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm2_p25_c168_s7.68e6.dat)
# Self-test
add_test(sync_sl_test_tm2_self_test_p25_c_168 sync_sl_test -p 25 -c 168 -d)
# Self-test with frequency offset
add_test(sync_sl_test_tm2_self_test_p25_c_168_fo sync_sl_test -p 25 -c 168 -d -f 100)
# Self-test with frequency offset and sample offset
add_test(sync_sl_test_tm2_self_test_p25_c_168_fo_so sync_sl_test -p 25 -c 168 -d -f 100 -o 3600)

# SL TM 4
add_test(sync_sl_test_tm4_p6_c_0 sync_sl_test -p 6 -t 4 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p6_c0_size6_num1_cshift0_s1.92e6.dat)
add_test(sync_sl_test_tm4_p15_c_84 sync_sl_test -p 15 -t 4 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p15_c84_size5_num3_cshift0_s3.84e6.dat)
add_test(sync_sl_test_tm4_p25_c_168 sync_sl_test -p 25 -t 4 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p25_c168_size5_num5_cshift0_s7.68e6.dat)
add_test(sync_sl_test_tm4_p50_c_252 sync_sl_test -p 50 -t 4 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p50_c252_size10_num5_cshift0_s15.36e6.dat)
#add_test(sync_sl_test_tm4_p100_c_335 sync_sl_test -p 100 -t 4 -d -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p100_c335_size10_num10_cshift0_s30.72e6.dat)
# Sample offset
add_test(sync_sl_test_tm4_p25_c_168_so sync_sl_test -p 25 -t 4 -d -o 300 -i ${CMAKE_CURRENT_SOURCE_DIR}/../../phch/test/signal_sidelink_ideal_tm4_p25_c168_size5_num5_cshift0_s7.68e6.dat )
# Self-test
add_test(sync_sl_test_self_test_tm4_p25_c_168 sync_sl_test -p 25 -t 4 -c 168 -d)
# Self-test with frequency offset
add_test(sync_sl_test_self_test_tm4_p25_c_168_fo sync_sl_test -p 25 -t 4 -c 168 -d -f 100)
# Self-test with frequency offset and sample offset
add_test(sync_sl_test_self_test_tm4_p25_c_168_fo_so sync_sl_test -p 25 -t 4 -c 168 -d -f 100 -o 3600)

########################################################################
# CFO TEST  
########################################################################

add_executable(cfo_test cfo_test.c)
target_link_libraries(cfo_test srsran_phy)

add_test(cfo_test_1 cfo_test -f 0.12345 -n 1000)
add_test(cfo_test_2 cfo_test -f 0.99849 -n 1000)


########################################################################
# NR TEST
########################################################################

add_executable(ssb_measure_test ssb_measure_test.c)
target_link_libraries(ssb_measure_test srsran_phy)

add_executable(ssb_decode_test ssb_decode_test.c)
target_link_libraries(ssb_decode_test srsran_phy)

add_executable(ssb_grid_test ssb_grid_test.c)
target_link_libraries(ssb_grid_test srsran_phy)

# For 1.0 GHz and 3.5 GHz Center frequencies
foreach (CELL_FREQ 1000000 3500000)
  # For each supported Cell/Carrier subcarrier spacing
  foreach (CELL_SCS 15 30)
    # For SSB centered at -960, 0 and 960 kHz from the center frequency
    foreach (SSB_OFFSET_FREQ -960 +0 +960)
      # For each supported SSB subcarrier spacing
      foreach (SSB_SCS 15 30)
        # For patterns A, B, C
        foreach (SSB_PATTERN A B C)
          # Calculate Actual SSB center frequency
          math(EXPR SSB_FREQ "${CELL_FREQ}${SSB_OFFSET_FREQ}")

          # Test SSB measurements
          add_nr_test(ssb_measure_test_${CELL_FREQ}000_${CELL_SCS}_${SSB_FREQ}000_${SSB_SCS}_${SSB_PATTERN} ssb_measure_test
                  -F ${CELL_FREQ}000 -S ${CELL_SCS} -f ${SSB_FREQ}000 -s ${SSB_SCS} -P ${SSB_PATTERN})

          # Test SSB PBCH decoding
          add_nr_test(ssb_decode_test_${CELL_FREQ}000_${CELL_SCS}_${SSB_FREQ}000_${SSB_SCS}_${SSB_PATTERN} ssb_decode_test
                  -F ${CELL_FREQ}000 -S ${CELL_SCS} -f ${SSB_FREQ}000 -s ${SSB_SCS} -P ${SSB_PATTERN})

          # Test SSB grid put/get decoding
          add_nr_test(ssb_grid_test_${CELL_FREQ}000_${CELL_SCS}_${SSB_FREQ}000_${SSB_SCS}_${SSB_PATTERN} ssb_grid_test
                  -F ${CELL_FREQ}000 -S ${CELL_SCS} -f ${SSB_FREQ}000 -s ${SSB_SCS} -P ${SSB_PATTERN})
        endforeach ()
      endforeach ()
    endforeach ()
  endforeach ()
endforeach ()

add_executable(ssb_file_test ssb_file_test.c)
target_link_libraries(ssb_file_test srsran_phy)

# File test 1
# Captured with command: lib/examples/usrp_capture -a type=x300,clock=external,sampling_rate=46.08e6,rx_subdev_spec=B:0 -g 20 -r 46.08e6 -n 460800  -f 3502.8e6 -o /tmp/n78.fo35028.fs2304M.data
add_nr_test(ssb_file_test_tdd ssb_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/n78.fo35028.fs4608M.data -v -r 46.08e6 -f 3502.8e6 -F 3512.64e6 -n 460800 -A 500 357802 2 0 1 0)
# Capture with third-party gNB on band n3 (FDD) 15kHz SSB SCS, f_s=15.36e6, f_c=1842.5e6, f_c_ssb=1842.05e6, PCI=500
add_nr_test(ssb_file_test_fdd ssb_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../../ue/test/ue_dl_nr_pci500_rb52_si_coreset0_idx6_s15.36e6.dat -v -r 15.36e6 -f 1842.5e6 -F 1842.05e6 -n 15360 -d fdd -s 15 -A 500 2200 0 0 0 0)
