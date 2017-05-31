# Copyright 2015 Software Radio Systems Limited
#
# This file is part of srsUE
#
# srsUE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsUE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
add_executable(mac_test mac_test.cc)
target_link_libraries(mac_test srsue_mac srsue_phy srslte_common srslte_phy srslte_radio srslte_asn1 ${SRSLTE_LIBRARIES} ${LIBLTE_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

