# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES mac_nr.cc
            proc_ra_nr.cc
            proc_bsr_nr.cc
            proc_sr_nr.cc
            mux_nr.cc
            demux_nr.cc
            dl_harq_nr.cc
            ul_harq_nr.cc)
add_library(srsue_mac_nr STATIC ${SOURCES})
target_link_libraries(srsue_mac_nr srsue_mac_common srsran_mac)

add_subdirectory(test)
