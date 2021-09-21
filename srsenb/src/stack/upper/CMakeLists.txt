#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES gtpu.cc pdcp.cc rlc.cc)
add_library(srsenb_upper STATIC ${SOURCES})
target_link_libraries(srsenb_upper srsran_asn1 srsran_gtpu)

set(SOURCES sdap.cc)
add_library(srsgnb_upper STATIC ${SOURCES})