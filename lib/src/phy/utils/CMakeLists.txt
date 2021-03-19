#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

file(GLOB SOURCES "*.c" "*.cpp")
add_library(srsran_utils OBJECT ${SOURCES})

if(VOLK_FOUND)
  set_target_properties(srsran_utils PROPERTIES COMPILE_DEFINITIONS "${VOLK_DEFINITIONS}")
endif(VOLK_FOUND)

add_subdirectory(test)
