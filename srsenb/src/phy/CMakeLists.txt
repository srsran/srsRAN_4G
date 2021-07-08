#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

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
