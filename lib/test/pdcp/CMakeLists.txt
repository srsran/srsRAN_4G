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

set(CTEST_LABELS "lib;pdcp")

add_executable(pdcp_nr_test_tx pdcp_nr_test_tx.cc)
target_link_libraries(pdcp_nr_test_tx srsran_pdcp srsran_common)
add_nr_test(pdcp_nr_test_tx pdcp_nr_test_tx)

add_executable(pdcp_nr_test_rx pdcp_nr_test_rx.cc)
target_link_libraries(pdcp_nr_test_rx srsran_pdcp srsran_common)
add_nr_test(pdcp_nr_test_rx pdcp_nr_test_rx)

add_executable(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu.cc)
target_link_libraries(pdcp_nr_test_discard_sdu srsran_pdcp srsran_common ${ATOMIC_LIBS})
add_nr_test(pdcp_nr_test_discard_sdu pdcp_nr_test_discard_sdu)

add_executable(pdcp_lte_test_rx pdcp_lte_test_rx.cc)
target_link_libraries(pdcp_lte_test_rx srsran_pdcp srsran_common)
add_test(pdcp_lte_test_rx pdcp_lte_test_rx)

add_executable(pdcp_lte_test_discard_sdu pdcp_lte_test_discard_sdu.cc)
target_link_libraries(pdcp_lte_test_discard_sdu srsran_pdcp srsran_common)
add_test(pdcp_lte_test_discard_sdu pdcp_lte_test_discard_sdu)

add_executable(pdcp_lte_test_status_report pdcp_lte_test_status_report.cc)
target_link_libraries(pdcp_lte_test_status_report srsran_pdcp srsran_common)
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
