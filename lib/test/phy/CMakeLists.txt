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

#######################################################################
# PHY TESTS
#######################################################################
add_executable(phy_dl_test phy_dl_test.c)
target_link_libraries(phy_dl_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(phy_dl_test phy_dl_test)

# All valid number of PRBs for PUSCH
set(ue_dl_min_mcs 0)
set(ue_dl_max_mcs 28)
set(ue_dl_step_mcs 7)

foreach (cell_n_prb 6 15 25 50 75 100)
  foreach (ue_dl_tm 1 2 3 4)
    foreach (ue_dl_mcs RANGE ${ue_dl_min_mcs} ${ue_dl_max_mcs} ${ue_dl_step_mcs})
          set(phy_dl_test_args "")

          set(phy_dl_test_args ${phy_dl_test_args} -p ${cell_n_prb})
          set(phy_dl_test_args ${phy_dl_test_args} -t ${ue_dl_tm})
          set(phy_dl_test_args ${phy_dl_test_args} -m ${ue_dl_mcs})

          string(REGEX REPLACE "\ " "" test_name_args ${phy_dl_test_args})

          add_test(phy_dl_test${test_name_args} phy_dl_test ${phy_dl_test_args})
    endforeach (ue_dl_mcs)
  endforeach(ue_dl_tm)
endforeach (cell_n_prb)
