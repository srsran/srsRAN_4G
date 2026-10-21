# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(mac)
add_subdirectory(rrc)
add_subdirectory(s1ap)
add_subdirectory(upper)

set(SOURCES enb_stack_lte.cc)

add_library(srsenb_stack STATIC ${SOURCES})