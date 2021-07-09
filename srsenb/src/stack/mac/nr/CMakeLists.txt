#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES mac_nr.cc sched_nr.cc sched_nr_ue.cc sched_nr_worker.cc sched_nr_rb_grid.cc sched_nr_harq.cc
            sched_nr_pdcch.cc sched_nr_cfg.cc sched_nr_phy.cc sched_nr_bwp.cc sched_nr_rb.cc)

add_library(srsgnb_mac STATIC ${SOURCES})
