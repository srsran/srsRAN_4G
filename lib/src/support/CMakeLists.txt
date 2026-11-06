# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SOURCES emergency_handlers.cc
            signal_handler.cc)

add_library(support STATIC ${SOURCES})
