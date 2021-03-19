#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(usim_test usim_test.cc)
target_link_libraries(usim_test srsue_upper srsran_upper srsran_phy rrc_asn1)
add_test(usim_test usim_test)

if(HAVE_PCSC)
  add_executable(pcsc_usim_test pcsc_usim_test.cc)
  target_link_libraries(pcsc_usim_test srsue_upper srsran_upper srsran_phy)
endif(HAVE_PCSC)

add_executable(rrc_reconfig_test rrc_reconfig_test.cc)
target_link_libraries(rrc_reconfig_test srsue_upper srsran_upper srsran_phy rrc_asn1)
add_test(rrc_reconfig_test rrc_reconfig_test)

add_executable(rrc_meas_test rrc_meas_test.cc)
target_link_libraries(rrc_meas_test srsue_rrc srsue_upper srsran_upper srsran_phy rrc_asn1 rrc_nr_asn1)
add_test(rrc_meas_test rrc_meas_test)

add_executable(nas_test nas_test.cc)
target_link_libraries(nas_test srsue_upper srsran_upper srsran_phy rrc_asn1)
add_test(nas_test nas_test)

add_executable(gw_test gw_test.cc)
target_link_libraries(gw_test srsue_upper srsran_upper srsran_phy)
add_test(gw_test gw_test)

add_executable(tft_test tft_test.cc)
target_link_libraries(tft_test srsue_upper srsran_upper srsran_phy)
add_test(tft_test tft_test)

add_executable(rrc_phy_ctrl_test rrc_phy_ctrl_test.cc)
target_link_libraries(rrc_phy_ctrl_test srsran_common srsue_rrc)
add_test(rrc_phy_ctrl_test rrc_phy_ctrl_test)

add_executable(rrc_cell_test rrc_cell_test.cc)
target_link_libraries(rrc_cell_test srsue_rrc srsue_upper srsran_upper srsran_phy rrc_asn1 rrc_nr_asn1)
add_test(rrc_cell_test rrc_cell_test)

add_executable(ue_rrc_nr_test ue_rrc_nr_test.cc)
target_link_libraries(ue_rrc_nr_test srsue_rrc srsue_upper srsran_upper srsran_phy rrc_asn1 rrc_nr_asn1)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")

