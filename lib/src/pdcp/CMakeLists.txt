# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES pdcp.cc
            pdcp_entity_base.cc
            pdcp_entity_lte.cc
            pdcp_entity_nr.cc)

add_library(srsran_pdcp STATIC ${SOURCES})
target_link_libraries(srsran_pdcp srsran_common srsran_asn1 ${ATOMIC_LIBS})
install(TARGETS srsran_pdcp DESTINATION ${LIBRARY_DIR} OPTIONAL)
