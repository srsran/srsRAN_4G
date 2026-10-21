# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES
    backend_worker.cpp
    srslog.cpp
    srslog_c.cpp
    event_trace.cpp)

include_directories(${PROJECT_SOURCE_DIR}/lib/include/srsran/srslog/bundled/)
include_directories(${PROJECT_SOURCE_DIR}/lib/include/srsran/srslog/formatters)

set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/bundled/fmt/format.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/bundled/fmt/os.cc)

set(SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/formatters/json_formatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formatters/text_formatter.cpp)


find_package(Threads REQUIRED)

add_library(srslog STATIC ${SOURCES})
target_link_libraries(srslog ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS srslog DESTINATION ${LIBRARY_DIR} OPTIONAL)
