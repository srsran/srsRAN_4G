#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        sys_metrics_processor.cc)

find_package(Threads REQUIRED)

add_library(system STATIC ${SOURCES})
target_link_libraries(system "${CMAKE_THREAD_LIBS_INIT}")
