# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SRCS cfr.c)
add_library(srsran_cfr OBJECT ${SRCS})

add_subdirectory(test)

