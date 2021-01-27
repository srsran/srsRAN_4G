#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

SET(SOURCES pdu.cc pdu_queue.cc)

if (ENABLE_5GNR)
    set(SOURCES ${SOURCES} mac_sch_pdu_nr.cc mac_rar_pdu_nr.cc)
endif(ENABLE_5GNR)

add_library(srslte_mac STATIC ${SOURCES})
target_link_libraries(srslte_mac srslte_common)
INSTALL(TARGETS srslte_mac DESTINATION ${LIBRARY_DIR})