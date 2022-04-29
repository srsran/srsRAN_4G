#
# Copyright 2013-2022 Software Radio Systems Limited
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

set(SOURCES nas.cc nas_emm_state.cc nas_idle_procedures.cc gw.cc usim_base.cc usim.cc tft_packet_filter.cc nas_base.cc nas_5g_procedures.cc nas_5g.cc nas_5gmm_state.cc sdap.cc)

if(HAVE_PCSC)
  list(APPEND SOURCES "pcsc_usim.cc")
endif(HAVE_PCSC)

add_library(srsue_upper STATIC ${SOURCES})
target_link_libraries(srsue_upper ${ATOMIC_LIBS} srsran_asn1)

target_link_libraries(srsue_upper nas_5g_msg)

if(HAVE_PCSC)
  target_link_libraries(srsue_upper ${PCSCLITE_LIBRARY})
endif(HAVE_PCSC)