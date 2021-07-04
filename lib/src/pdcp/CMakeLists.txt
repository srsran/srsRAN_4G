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

set(SOURCES pdcp.cc
            pdcp_entity_base.cc
            pdcp_entity_lte.cc
            pdcp_entity_nr.cc)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp srsran_common srsran_asn1 ${ATOMIC_LIBS})
INSTALL(TARGETS srsran_pdcp DESTINATION ${LIBRARY_DIR})
