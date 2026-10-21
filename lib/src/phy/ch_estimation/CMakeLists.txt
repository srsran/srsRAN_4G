# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

file(GLOB SOURCES "*.c")
add_library(srsran_ch_estimation OBJECT ${SOURCES})
add_subdirectory(test)
