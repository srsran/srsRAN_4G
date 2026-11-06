# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(FEC_SOURCES ${FEC_SOURCES}
        block/block.c
        PARENT_SCOPE)

add_subdirectory(test)
