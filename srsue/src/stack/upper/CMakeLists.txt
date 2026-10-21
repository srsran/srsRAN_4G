# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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