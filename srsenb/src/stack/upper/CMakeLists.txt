# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SOURCES gtpu.cc pdcp.cc rlc.cc)
add_library(srsenb_upper STATIC ${SOURCES})
target_link_libraries(srsenb_upper srsran_asn1 srsran_gtpu)