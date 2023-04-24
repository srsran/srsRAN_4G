#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_subdirectory(test)

set(SOURCES demux.cc dl_harq.cc mac.cc mux.cc proc_bsr.cc proc_phr.cc proc_ra.cc proc_sr.cc ul_harq.cc)
add_library(srsue_mac STATIC ${SOURCES})
target_link_libraries(srsue_mac srsue_mac_common ${ATOMIC_LIBS})