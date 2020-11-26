#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES nas.cc nas_emm_state.cc nas_idle_procedures.cc gw.cc usim_base.cc usim.cc tft_packet_filter.cc)

if(HAVE_PCSC)
  list(APPEND SOURCES "pcsc_usim.cc")
endif(HAVE_PCSC)

add_library(srsue_upper STATIC ${SOURCES})

if(HAVE_PCSC)
  target_link_libraries(srsue_upper ${PCSCLITE_LIBRARY})
endif(HAVE_PCSC)
