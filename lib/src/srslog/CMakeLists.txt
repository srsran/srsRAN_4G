#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    backend_worker.cpp
    srslog.cpp
    srslog_c.cpp
    event_trace.cpp)

add_subdirectory(bundled/fmt)
add_subdirectory(formatters)

find_package(Threads REQUIRED)

add_library(srslog STATIC ${SOURCES})
target_link_libraries(srslog formatters fmt "${CMAKE_THREAD_LIBS_INIT}")
