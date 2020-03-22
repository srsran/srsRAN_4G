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

########################################################################
# END-TO-END TESTS
########################################################################

SET(CTEST_OUTPUT_ON_FAILURE TRUE)

if (ZEROMQ_FOUND AND ENABLE_ZMQ_TEST)
  foreach (num_cc 1 2)
    foreach (cell_n_prb 6 15 25 50 75 100)
      add_test(e2e_${cell_n_prb}prb_${num_cc}cc ${CMAKE_CURRENT_SOURCE_DIR}/run_lte.sh ${CMAKE_CURRENT_BINARY_DIR}/../ ${cell_n_prb} ${num_cc})
    endforeach (cell_n_prb)
  endforeach (num_cc)
endif (ZEROMQ_FOUND AND ENABLE_ZMQ_TEST)
