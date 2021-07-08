#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(test)

file(GLOB_RECURSE SOURCES "*.cc")
add_library(srsue_phy STATIC ${SOURCES})

if(ENABLE_GUI AND SRSGUI_FOUND)
  target_link_libraries(srsue_phy ${SRSGUI_LIBRARIES})
endif()