# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# SEQUENCE TEST
########################################################################

add_executable(sequence_test sequence_test.c)
target_link_libraries(sequence_test srsran_phy)

add_test(sequence_test sequence_test)

########################################################################
# SLIV TEST
########################################################################

add_executable(sliv_test sliv_test.c)
target_link_libraries(sliv_test srsran_phy)

add_test(sliv_test_14 sliv_test 14)
add_test(sliv_test_52 sliv_test 48)
add_test(sliv_test_52 sliv_test 52)

########################################################################
# PHY COMMON TEST
########################################################################

add_executable(phy_common_test phy_common_test.c)
target_link_libraries(phy_common_test srsran_phy)

add_test(phy_common_test phy_common_test)