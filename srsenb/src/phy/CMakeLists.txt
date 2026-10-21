# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES
        lte/cc_worker.cc
        lte/sf_worker.cc
        lte/worker_pool.cc
        nr/slot_worker.cc
        nr/worker_pool.cc
        phy.cc
        phy_common.cc
        phy_ue_db.cc
        prach_worker.cc
        txrx.cc)
add_library(srsenb_phy STATIC ${SOURCES})

if (ENABLE_GUI AND SRSGUI_FOUND)
    target_link_libraries(srsenb_phy ${SRSGUI_LIBRARIES})
endif ()
