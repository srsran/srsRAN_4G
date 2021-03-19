#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu.cc pdcp.cc rlc.cc s1ap.cc)
add_library(srsenb_upper STATIC ${SOURCES})

set(SOURCES pdcp_nr.cc rlc_nr.cc sdap.cc)
add_library(srsgnb_upper STATIC ${SOURCES})
