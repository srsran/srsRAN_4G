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

if(RF_FOUND)
  add_executable(benchmark_radio benchmark_radio.cc)
  target_link_libraries(benchmark_radio srslte_common srslte_phy srslte_radio)
  if(SRSGUI_FOUND)
    include_directories(${SRSGUI_INCLUDE_DIRS})
    target_link_libraries(benchmark_radio ${SRSGUI_LIBRARIES})
  endif(SRSGUI_FOUND)
endif(RF_FOUND)


