#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(common)

add_subdirectory(schedulers)

set(SOURCES mac.cc ue.cc sched.cc sched_carrier.cc sched_grid.cc sched_ue_ctrl/sched_harq.cc sched_ue.cc
            sched_ue_ctrl/sched_lch.cc sched_ue_ctrl/sched_ue_cell.cc sched_ue_ctrl/sched_dl_cqi.cc
            sched_phy_ch/sf_cch_allocator.cc sched_phy_ch/sched_dci.cc sched_phy_ch/sched_phy_resource.cc
            sched_helpers.cc)
add_library(srsenb_mac STATIC ${SOURCES} $<TARGET_OBJECTS:mac_schedulers>)
target_link_libraries(srsenb_mac srsenb_mac_common)