#
# Copyright 2013-2017 Software Radio Systems Limited
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

add_executable(rlc_stress_test rlc_stress_test.cc)
target_link_libraries(rlc_stress_test srslte_upper srslte_phy srslte_common ${Boost_LIBRARIES})
add_test(rlc_am_stress_test rlc_stress_test --mode=AM --loglevel 1 --sdu_gen_delay 250)
add_test(rlc_um_stress_test rlc_stress_test --mode=UM --loglevel 1)
add_test(rlc_tm_stress_test rlc_stress_test --mode=TM --loglevel 1 --opp_sdu_ratio=1.0)

add_executable(rlc_um_data_test rlc_um_data_test.cc)
target_link_libraries(rlc_um_data_test srslte_upper srslte_phy srslte_common)
add_test(rlc_um_data_test rlc_um_data_test)

add_executable(rlc_um_test rlc_um_test.cc)
target_link_libraries(rlc_um_test srslte_upper srslte_phy)
add_test(rlc_um_test rlc_um_test)
  

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")
