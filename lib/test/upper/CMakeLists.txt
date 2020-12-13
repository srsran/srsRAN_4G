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

add_executable(rlc_am_data_test rlc_am_data_test.cc)
target_link_libraries(rlc_am_data_test srslte_upper srslte_phy srslte_common)
add_test(rlc_am_data_test rlc_am_data_test)

add_executable(rlc_am_control_test rlc_am_control_test.cc)
target_link_libraries(rlc_am_control_test srslte_upper srslte_phy)
add_test(rlc_am_control_test rlc_am_control_test)

add_executable(rlc_am_test rlc_am_test.cc)
target_link_libraries(rlc_am_test srslte_upper srslte_phy srslte_common)
add_test(rlc_am_test rlc_am_test)

if (ENABLE_5GNR)
  add_executable(rlc_am_nr_pdu_test rlc_am_nr_pdu_test.cc)
  target_link_libraries(rlc_am_nr_pdu_test srslte_upper srslte_phy)
  add_test(rlc_am_nr_pdu_test rlc_am_nr_pdu_test)
endif(ENABLE_5GNR)

add_executable(rlc_stress_test rlc_stress_test.cc)
target_link_libraries(rlc_stress_test srslte_upper srslte_mac srslte_phy srslte_common ${Boost_LIBRARIES})
add_test(rlc_am_stress_test rlc_stress_test --mode=AM --loglevel 1 --sdu_gen_delay 250)
add_test(rlc_um_stress_test rlc_stress_test --mode=UM --loglevel 1)
add_test(rlc_tm_stress_test rlc_stress_test --mode=TM --loglevel 1 --random_opp=false)
if (ENABLE_5GNR)
  add_test(rlc_um_nr_stress_test rlc_stress_test --rat NR --mode=UM --loglevel 1)
  set_tests_properties(rlc_um_nr_stress_test PROPERTIES LABELS "long;upper;lib;nr")
endif(ENABLE_5GNR)
set_tests_properties(rlc_am_stress_test PROPERTIES TIMEOUT 3000)
set_tests_properties(rlc_um_stress_test PROPERTIES TIMEOUT 3000)
set_tests_properties(rlc_tm_stress_test PROPERTIES TIMEOUT 3000)

add_executable(rlc_um_data_test rlc_um_data_test.cc)
target_link_libraries(rlc_um_data_test srslte_upper srslte_phy srslte_common)
add_test(rlc_um_data_test rlc_um_data_test)

add_executable(rlc_um_test rlc_um_test.cc)
target_link_libraries(rlc_um_test srslte_upper srslte_phy)
add_test(rlc_um_test rlc_um_test)

add_executable(rlc_common_test rlc_common_test.cc)
target_link_libraries(rlc_common_test srslte_upper srslte_phy)
add_test(rlc_common_test rlc_common_test)

if (ENABLE_5GNR)
  add_executable(rlc_um_nr_pdu_test rlc_um_nr_pdu_test.cc)
  target_link_libraries(rlc_um_nr_pdu_test srslte_upper srslte_phy)
  add_test(rlc_um_nr_pdu_test rlc_um_nr_pdu_test)

  add_executable(rlc_um_nr_test rlc_um_nr_test.cc)
  target_link_libraries(rlc_um_nr_test srslte_upper srslte_phy)
  add_test(rlc_um_nr_test rlc_um_nr_test)

  add_executable(pdcp_nr_test_tx pdcp_nr_test_tx.cc)
  target_link_libraries(pdcp_nr_test_tx srslte_upper srslte_common)
  add_test(pdcp_nr_test_tx pdcp_nr_test_tx)
  set_tests_properties(pdcp_nr_test_tx PROPERTIES LABELS "long;upper")

  add_executable(pdcp_nr_test_rx pdcp_nr_test_rx.cc)
  target_link_libraries(pdcp_nr_test_rx srslte_upper srslte_common)
  add_test(pdcp_nr_test_rx pdcp_nr_test_rx)

  add_executable(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu.cc)
  target_link_libraries(pdcp_nr_test_discard_sdu srslte_upper srslte_common)
  add_test(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu)
endif(ENABLE_5GNR)

add_executable(pdcp_lte_test_rx pdcp_lte_test_rx.cc)
target_link_libraries(pdcp_lte_test_rx srslte_upper srslte_common)
add_test(pdcp_lte_test_rx pdcp_lte_test_rx)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")
