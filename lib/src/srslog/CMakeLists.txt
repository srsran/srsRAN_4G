#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
