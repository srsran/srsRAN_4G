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

add_subdirectory(phy)
add_subdirectory(upper)

if (ENABLE_TTCN3)
  add_subdirectory(ttcn3)
endif (ENABLE_TTCN3)

add_executable(metrics_test metrics_test.cc ../src/metrics_stdout.cc ../src/metrics_csv.cc)
target_link_libraries(metrics_test srslte_phy srslte_common)
add_test(metrics_test metrics_test -o ${CMAKE_CURRENT_BINARY_DIR}/ue_metrics.csv)

add_executable(mac_test mac_test.cc)
target_link_libraries(mac_test srsue_mac srsue_phy srslte_common srslte_mac srslte_phy srslte_radio srslte_asn1 rrc_asn1 ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(mac_test mac_test)