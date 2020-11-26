#
# Copyright 2013-2020 Software Radio Systems Limited
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

find_package(Threads REQUIRED)

add_library(srslog STATIC ${SOURCES})
target_link_libraries(srslog fmt "${CMAKE_THREAD_LIBS_INIT}")
INSTALL(TARGETS srslog DESTINATION ${LIBRARY_DIR})
