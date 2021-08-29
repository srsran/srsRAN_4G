#
# Copyright 2013-2021 Software Radio Systems Limited
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

add_executable(ue_rrc_nr_test ue_rrc_nr_test.cc)
target_link_libraries(ue_rrc_nr_test srsue_rrc_nr srsue_upper srsran_common srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")
