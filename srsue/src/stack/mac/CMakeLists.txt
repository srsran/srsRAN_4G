#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES demux.cc dl_harq.cc mac.cc mux.cc proc_bsr.cc proc_phr.cc proc_ra.cc proc_sr.cc ul_harq.cc)
add_library(srsue_mac STATIC ${SOURCES})