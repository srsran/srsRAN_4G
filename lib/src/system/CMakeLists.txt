# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES
        sys_metrics_processor.cc)

find_package(Threads REQUIRED)

add_library(system STATIC ${SOURCES})
target_link_libraries(system "${CMAKE_THREAD_LIBS_INIT}")
