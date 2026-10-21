# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(mac)
add_subdirectory(ngap)
add_subdirectory(ric)
add_subdirectory(rrc)
add_subdirectory(sdap)

set(SOURCES gnb_stack_nr.cc)

add_library(srsgnb_stack STATIC ${SOURCES})