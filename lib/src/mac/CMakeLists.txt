#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

SET(SOURCES pdu.cc pdu_queue.cc mac_sch_pdu_nr.cc mac_rar_pdu_nr.cc)

add_library(srsran_mac STATIC ${SOURCES})
target_link_libraries(srsran_mac srsran_common)
install(TARGETS srsran_mac DESTINATION ${LIBRARY_DIR} OPTIONAL)

add_subdirectory(test)