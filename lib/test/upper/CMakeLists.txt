# Copyright 2015 Software Radio Systems Limited
#
# This file is part of srsUE
#
# srsUE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsUE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_executable(rlc_am_data_test rlc_am_data_test.cc)
target_link_libraries(rlc_am_data_test srslte_upper srslte_phy lte)
add_test(rlc_am_data_test rlc_am_data_test)

add_executable(rlc_am_control_test rlc_am_control_test.cc)
target_link_libraries(rlc_am_control_test srslte_upper srslte_phy lte)
add_test(rlc_am_control_test rlc_am_control_test)

add_executable(rlc_am_test rlc_am_test.cc)
target_link_libraries(rlc_am_test srslte_upper srslte_phy lte)
add_test(rlc_am_test rlc_am_test)

add_executable(rlc_um_data_test rlc_um_data_test.cc)
target_link_libraries(rlc_um_data_test srslte_upper srslte_phy lte)
add_test(rlc_um_data_test rlc_um_data_test)

add_executable(rlc_um_test rlc_um_test.cc)
target_link_libraries(rlc_um_test srslte_upper srslte_phy lte)
add_test(rlc_um_test rlc_um_test)
  
add_executable(usim_test usim_test.cc)
target_link_libraries(usim_test srslte_upper srslte_phy lte)
add_test(usim_test usim_test)

add_executable(rrc_reconfig_test rrc_reconfig_test.cc)
target_link_libraries(rrc_reconfig_test srslte_upper srslte_phy lte)
add_test(rrc_reconfig_test rrc_reconfig_test)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")
