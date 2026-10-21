# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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

add_subdirectory(phy)