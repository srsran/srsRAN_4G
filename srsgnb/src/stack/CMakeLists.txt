#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(mac)
add_subdirectory(ngap)
add_subdirectory(rrc)
add_subdirectory(sdap)

set(SOURCES gnb_stack_nr.cc)

add_library(srsgnb_stack STATIC ${SOURCES})