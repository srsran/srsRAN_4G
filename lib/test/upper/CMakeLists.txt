#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CTEST_LABELS "lib;upper")

add_executable(rlc_am_data_test rlc_am_data_test.cc)
target_link_libraries(rlc_am_data_test srsran_upper srsran_phy srsran_common)
add_lte_test(rlc_am_data_test rlc_am_data_test)

add_executable(rlc_am_control_test rlc_am_control_test.cc)
target_link_libraries(rlc_am_control_test srsran_upper srsran_phy)
add_lte_test(rlc_am_control_test rlc_am_control_test)

add_executable(rlc_am_test rlc_am_test.cc)
target_link_libraries(rlc_am_test srsran_upper srsran_phy srsran_common)
add_lte_test(rlc_am_test rlc_am_test)

add_executable(rlc_am_nr_pdu_test rlc_am_nr_pdu_test.cc)
target_link_libraries(rlc_am_nr_pdu_test srsran_upper srsran_phy)
add_nr_test(rlc_am_nr_pdu_test rlc_am_nr_pdu_test)

add_executable(rlc_stress_test rlc_stress_test.cc)
target_link_libraries(rlc_stress_test srsran_upper srsran_mac srsran_phy srsran_common ${Boost_LIBRARIES})
add_lte_test(rlc_am_stress_test rlc_stress_test --mode=AM --loglevel 1 --sdu_gen_delay 250)
add_lte_test(rlc_um_stress_test rlc_stress_test --mode=UM --loglevel 1)
add_lte_test(rlc_tm_stress_test rlc_stress_test --mode=TM --loglevel 1 --random_opp=false)

add_nr_test(rlc_um_nr_stress_test rlc_stress_test --rat NR --mode=UM --loglevel 1)

add_executable(rlc_um_data_test rlc_um_data_test.cc)
target_link_libraries(rlc_um_data_test srsran_upper srsran_phy srsran_common)
add_test(rlc_um_data_test rlc_um_data_test)

add_executable(rlc_um_test rlc_um_test.cc)
target_link_libraries(rlc_um_test srsran_upper srsran_phy)
add_test(rlc_um_test rlc_um_test)

add_executable(rlc_common_test rlc_common_test.cc)
target_link_libraries(rlc_common_test srsran_upper srsran_phy)
add_test(rlc_common_test rlc_common_test)

add_executable(rlc_um_nr_pdu_test rlc_um_nr_pdu_test.cc)
target_link_libraries(rlc_um_nr_pdu_test srsran_upper srsran_mac srsran_phy)
add_nr_test(rlc_um_nr_pdu_test rlc_um_nr_pdu_test)

add_executable(rlc_um_nr_test rlc_um_nr_test.cc)
target_link_libraries(rlc_um_nr_test srsran_upper srsran_phy)
add_nr_test(rlc_um_nr_test rlc_um_nr_test)

add_executable(pdcp_nr_test_tx pdcp_nr_test_tx.cc)
target_link_libraries(pdcp_nr_test_tx srsran_upper srsran_common)
add_nr_test(pdcp_nr_test_tx pdcp_nr_test_tx)

add_executable(pdcp_nr_test_rx pdcp_nr_test_rx.cc)
target_link_libraries(pdcp_nr_test_rx srsran_upper srsran_common)
add_nr_test(pdcp_nr_test_rx pdcp_nr_test_rx)

add_executable(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu.cc)
target_link_libraries(pdcp_nr_test_discard_sdu srsran_upper srsran_common)
add_nr_test(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu)

add_executable(pdcp_lte_test_rx pdcp_lte_test_rx.cc)
target_link_libraries(pdcp_lte_test_rx srsran_upper srsran_common)
add_test(pdcp_lte_test_rx pdcp_lte_test_rx)

add_executable(pdcp_lte_test_discard_sdu pdcp_lte_test_discard_sdu.cc)
target_link_libraries(pdcp_lte_test_discard_sdu srsran_upper srsran_common)
add_test(pdcp_lte_test_discard_sdu pdcp_lte_test_discard_sdu)

add_executable(pdcp_lte_test_status_report pdcp_lte_test_status_report.cc)
target_link_libraries(pdcp_lte_test_status_report srsran_upper srsran_common)
add_test(pdcp_lte_test_status_report pdcp_lte_test_status_report)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")
