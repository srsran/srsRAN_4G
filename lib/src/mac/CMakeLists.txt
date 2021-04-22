#
# Copyright 2013-2021 Software Radio Systems Limited
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

SET(SOURCES pdu.cc pdu_queue.cc mac_sch_pdu_nr.cc mac_rar_pdu_nr.cc)

add_library(srsran_mac STATIC ${SOURCES})
target_link_libraries(srsran_mac srsran_common)
INSTALL(TARGETS srsran_mac DESTINATION ${LIBRARY_DIR})