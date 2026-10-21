# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

SET(SOURCES pdu.cc pdu_queue.cc mac_sch_pdu_nr.cc mac_rar_pdu_nr.cc)

add_library(srsran_mac STATIC ${SOURCES})
target_link_libraries(srsran_mac srsran_common)
install(TARGETS srsran_mac DESTINATION ${LIBRARY_DIR} OPTIONAL)

add_subdirectory(test)