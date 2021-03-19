#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES mac_nr.cc proc_ra_nr.cc mux_nr.cc)
add_library(srsue_mac_nr STATIC ${SOURCES})
target_link_libraries(srsue_mac_nr srsran_mac)