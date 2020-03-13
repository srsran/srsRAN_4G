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

set(SOURCES gtpu.cc
            pdcp.cc
            pdcp_entity_base.cc
            pdcp_entity_lte.cc
            rlc.cc
            rlc_tm.cc
            rlc_um_base.cc
            rlc_um_lte.cc
            rlc_am_base.cc
            rlc_am_lte.cc)

if (ENABLE_5GNR)
  set(SOURCES ${SOURCES} pdcp_entity_nr.cc rlc_um_nr.cc rlc_am_nr.cc)
endif(ENABLE_5GNR)

add_library(srslte_upper STATIC ${SOURCES})
target_link_libraries(srslte_upper srslte_common srslte_asn1)