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
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

add_executable(pss_file pss_file.c)
target_link_libraries(pss_file srslte_phy)

add_executable(npss_file npss_file.c)
target_link_libraries(npss_file srslte_phy)

if(RF_FOUND)
  add_executable(pss_usrp pss_usrp.c)
  target_link_libraries(pss_usrp srslte_rf srslte_phy)

  add_executable(npss_usrp npss_usrp.c)
  target_link_libraries(npss_usrp srslte_rf srslte_phy)

  add_executable(nsss_usrp nsss_usrp.c)
  target_link_libraries(nsss_usrp srslte_rf srslte_phy)
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
target_link_libraries(sync_test srslte_phy)

add_executable(sync_nbiot_test sync_nbiot_test.c)
target_link_libraries(sync_nbiot_test srslte_phy)

add_executable(npss_test npss_test.c)
target_link_libraries(npss_test srslte_phy)

add_executable(nsss_test nsss_test.c)
target_link_libraries(nsss_test srslte_phy)

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
target_link_libraries(sync_sl_test srslte_phy)

add_executable(psss_file_test psss_file_test.c)
target_link_libraries(psss_file_test srslte_phy)

# SL TM 2
add_test(sync_sl_test_tm2_p6_c_0 sync_sl_test -p 6 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p6_c0_s1.92e6.dat)
add_test(sync_sl_test_tm2_p15_c_84 sync_sl_test -p 15 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p15_c84_s3.84e6.dat)
add_test(sync_sl_test_tm2_p25_c_168 sync_sl_test -p 25 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p25_c168_s7.68e6.dat)
add_test(sync_sl_test_tm2_p50_c_252 sync_sl_test -p 50 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p50_c252_s15.36e6.dat)
add_test(sync_sl_test_tm2_p100_c_335 sync_sl_test -p 100 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p100_c335_s30.72e6.dat)
# Sample offset
add_test(sync_sl_test_tm2_p25_c_168_so sync_sl_test -p 25 -d -o 300 -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm2_p25_c168_s7.68e6.dat)
# Self-test
add_test(sync_sl_test_tm2_self_test_p25_c_168 sync_sl_test -p 25 -c 168 -d)
# Self-test with frequency offset
add_test(sync_sl_test_tm2_self_test_p25_c_168_fo sync_sl_test -p 25 -c 168 -d -f 100)
# Self-test with frequency offset and sample offset
add_test(sync_sl_test_tm2_self_test_p25_c_168_fo_so sync_sl_test -p 25 -c 168 -d -f 100 -o 3600)

# SL TM 4
add_test(sync_sl_test_tm4_p6_c_0 sync_sl_test -p 6 -t 4 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p6_c0_size6_num1_cshift0_s1.92e6.dat)
add_test(sync_sl_test_tm4_p15_c_84 sync_sl_test -p 15 -t 4 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p15_c84_size5_num3_cshift0_s3.84e6.dat)
add_test(sync_sl_test_tm4_p25_c_168 sync_sl_test -p 25 -t 4 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p25_c168_size5_num5_cshift0_s7.68e6.dat)
add_test(sync_sl_test_tm4_p50_c_252 sync_sl_test -p 50 -t 4 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p50_c252_size10_num5_cshift0_s15.36e6.dat)
#add_test(sync_sl_test_tm4_p100_c_335 sync_sl_test -p 100 -t 4 -d -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p100_c335_size10_num10_cshift0_s30.72e6.dat)
# Sample offset
add_test(sync_sl_test_tm4_p25_c_168_so sync_sl_test -p 25 -t 4 -d -o 300 -i ${CMAKE_HOME_DIRECTORY}/lib/src/phy/phch/test/signal_sidelink_ideal_tm4_p25_c168_size5_num5_cshift0_s7.68e6.dat )
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
target_link_libraries(cfo_test srslte_phy)

add_test(cfo_test_1 cfo_test -f 0.12345 -n 1000)
add_test(cfo_test_2 cfo_test -f 0.99849 -n 1000)
