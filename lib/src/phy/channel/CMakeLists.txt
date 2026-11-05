# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

file(GLOB SOURCES "*.c" "*.cc")
add_library(srsran_channel OBJECT ${SOURCES})

add_subdirectory(test)