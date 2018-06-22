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

add_executable(usim_test usim_test.cc)
target_link_libraries(usim_test srsue_upper srslte_upper srslte_phy)
add_test(usim_test usim_test)

if(HAVE_PCSC)
  add_executable(pcsc_usim_test pcsc_usim_test.cc)
  target_link_libraries(pcsc_usim_test srsue_upper srslte_upper srslte_phy)
endif(HAVE_PCSC)

add_executable(rrc_reconfig_test rrc_reconfig_test.cc)
target_link_libraries(rrc_reconfig_test srsue_upper srslte_upper srslte_phy)
add_test(rrc_reconfig_test rrc_reconfig_test)

add_executable(nas_test nas_test.cc)
target_link_libraries(nas_test srsue_upper srslte_upper srslte_phy)
add_test(nas_test nas_test)

########################################################################
# Option to run command after build (useful for remote builds)
########################################################################
if (NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "Added custom post-build command: ${BUILD_CMD}")
  add_custom_command(TARGET ip_test POST_BUILD COMMAND ${BUILD_CMD})
else(NOT ${BUILD_CMD} STREQUAL "")
  message(STATUS "No post-build command defined")
endif (NOT ${BUILD_CMD} STREQUAL "")

