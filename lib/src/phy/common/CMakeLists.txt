# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES phy_common.c phy_common_sl.c  phy_common_nr.c sequence.c timestamp.c zc_sequence.c sliv.c)
add_library(srsran_phy_common OBJECT ${SOURCES})

add_subdirectory(test)