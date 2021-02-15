#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(schedulers)

set(SOURCES mac.cc ue.cc sched.cc sched_carrier.cc sched_grid.cc sched_ue_ctrl/sched_harq.cc sched_ue.cc
        sched_ue_ctrl/sched_lch.cc sched_ue_ctrl/sched_ue_cell.cc sched_phy_ch/pdcch_sched.cc sched_helpers.cc)
add_library(srsenb_mac STATIC ${SOURCES} $<TARGET_OBJECTS:mac_schedulers>)

if(ENABLE_5GNR)
    set(SOURCES mac_nr.cc)
    add_library(srsgnb_mac STATIC ${SOURCES})
endif()
