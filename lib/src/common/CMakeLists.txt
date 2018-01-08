#
# Copyright 2013-2017 Software Radio Systems Limited
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

file(GLOB CXX_SOURCES "*.cc")
file(GLOB C_SOURCES "*.c")

add_library(srslte_common STATIC ${C_SOURCES} ${CXX_SOURCES})
add_custom_target(gen_build_info COMMAND cmake -P ${CMAKE_BINARY_DIR}/SRSLTEbuildinfo.cmake)
add_dependencies(srslte_common gen_build_info)

target_include_directories(srslte_common PUBLIC ${SEC_INCLUDE_DIRS})
target_link_libraries(srslte_common ${SEC_LIBRARIES})
install(TARGETS srslte_common DESTINATION ${LIBRARY_DIR})
