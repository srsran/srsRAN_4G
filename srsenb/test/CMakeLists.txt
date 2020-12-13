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

add_subdirectory(mac)
add_subdirectory(phy)
add_subdirectory(upper)
add_subdirectory(rrc)

add_executable(enb_metrics_test enb_metrics_test.cc ../src/metrics_stdout.cc ../src/metrics_csv.cc)
target_link_libraries(enb_metrics_test srslte_phy srslte_common)
add_test(enb_metrics_test enb_metrics_test -o ${CMAKE_CURRENT_BINARY_DIR}/enb_metrics.csv)
