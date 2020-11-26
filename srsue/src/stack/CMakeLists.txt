#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(mac)
add_subdirectory(rrc)
add_subdirectory(upper)

set(SOURCES ue_stack_lte.cc)
add_library(srsue_stack STATIC ${SOURCES})

if(ENABLE_5GNR)
    set(SOURCES ue_stack_nr.cc)
    add_library(srsue_nr_stack STATIC ${SOURCES})
endif()
