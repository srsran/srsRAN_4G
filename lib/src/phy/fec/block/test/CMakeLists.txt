# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# Viterbi TEST  
########################################################################

add_executable(block_test block_test.c)
target_link_libraries(block_test srsran_phy)

add_test(block_test_20 block_test -E 20)
add_test(block_test_32 block_test -E 32)
add_test(block_test_48 block_test -E 48)
