#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
            rlc_am_lte.cc
            pdcp_entity_nr.cc
            rlc_um_nr.cc
            rlc_am_nr.cc)

add_library(srslte_upper STATIC ${SOURCES})
target_link_libraries(srslte_upper srslte_common srslte_asn1)
INSTALL(TARGETS srslte_upper DESTINATION ${LIBRARY_DIR})