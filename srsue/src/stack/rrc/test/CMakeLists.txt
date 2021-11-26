#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(rrc_reconfig_test rrc_reconfig_test.cc)
target_link_libraries(rrc_reconfig_test srsue_upper srsran_phy srsran_asn1 rrc_asn1 srslog)
add_test(rrc_reconfig_test rrc_reconfig_test)

add_executable(rrc_meas_test rrc_meas_test.cc)
target_link_libraries(rrc_meas_test srsue_rrc srsue_upper srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)
add_test(rrc_meas_test rrc_meas_test)

add_executable(rrc_phy_ctrl_test rrc_phy_ctrl_test.cc)
target_link_libraries(rrc_phy_ctrl_test srsran_common srsue_rrc ${ATOMIC_LIBS})
add_test(rrc_phy_ctrl_test rrc_phy_ctrl_test)

add_executable(rrc_cell_test rrc_cell_test.cc)
target_link_libraries(rrc_cell_test srsue_rrc srsue_upper srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)
add_test(rrc_cell_test rrc_cell_test)

add_executable(rrc_rlf_report_test rrc_rlf_report_test.cc)
target_link_libraries(rrc_rlf_report_test srsue_rrc srsue_upper srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)
add_test(rrc_rlf_report_test rrc_rlf_report_test)