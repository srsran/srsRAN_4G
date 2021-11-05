#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(mac)
add_subdirectory(rrc)
add_subdirectory(s1ap)
add_subdirectory(upper)

set(SOURCES enb_stack_lte.cc)

add_library(srsenb_stack STATIC ${SOURCES})