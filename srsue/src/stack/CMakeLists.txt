# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(mac_common)
add_subdirectory(mac)
add_subdirectory(mac_nr)
add_subdirectory(rrc)
add_subdirectory(rrc_nr)
add_subdirectory(upper)

set(SOURCES ue_stack_lte.cc)
add_library(srsue_stack STATIC ${SOURCES})

set(SOURCES ue_stack_nr.cc)
add_library(srsue_nr_stack STATIC ${SOURCES})
