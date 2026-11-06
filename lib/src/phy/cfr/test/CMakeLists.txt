# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

########################################################################
# CFR Test
########################################################################

add_executable(cfr_test cfr_test.c)
target_link_libraries(cfr_test srsran_phy)

add_test(cfr_test_default cfr_test)

