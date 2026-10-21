# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(test)

set(SOURCES demux.cc dl_harq.cc mac.cc mux.cc proc_bsr.cc proc_phr.cc proc_ra.cc proc_sr.cc ul_harq.cc)
add_library(srsue_mac STATIC ${SOURCES})
target_link_libraries(srsue_mac srsue_mac_common ${ATOMIC_LIBS})