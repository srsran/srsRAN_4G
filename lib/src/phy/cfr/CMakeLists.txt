# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SRCS cfr.c)
add_library(srsran_cfr OBJECT ${SRCS})

add_subdirectory(test)

