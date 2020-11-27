#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES sched_base.cc sched_time_rr.cc sched_time_pf.cc)
add_library(mac_schedulers OBJECT ${SOURCES})
