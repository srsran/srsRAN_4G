#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES pdcp.cc
            pdcp_entity_base.cc
            pdcp_entity_lte.cc
            pdcp_entity_nr.cc)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp srsran_common srsran_asn1 ${ATOMIC_LIBS})
INSTALL(TARGETS srsran_pdcp DESTINATION ${LIBRARY_DIR})
