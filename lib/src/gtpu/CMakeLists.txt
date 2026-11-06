# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SOURCES gtpu.cc)

add_library(srsran_gtpu STATIC ${SOURCES})
target_link_libraries(srsran_gtpu srsran_common srsran_asn1 ${ATOMIC_LIBS})
install(TARGETS srsran_gtpu DESTINATION ${LIBRARY_DIR} OPTIONAL)
