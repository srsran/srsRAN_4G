#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES mac.cc ue.cc sched.cc sched_carrier.cc sched_grid.cc sched_harq.cc sched_metric.cc sched_ue.cc
            sched_lch.cc sched_interface_helpers.cc)
add_library(srsenb_mac STATIC ${SOURCES})

if(ENABLE_5GNR)
    set(SOURCES mac_nr.cc)
    add_library(srsgnb_mac STATIC ${SOURCES})
endif()
