#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(RF_FOUND)
  add_library(srsran_radio STATIC radio.cc channel_mapping.cc)
  target_link_libraries(srsran_radio srsran_rf srsran_common)
  install(TARGETS srsran_radio DESTINATION ${LIBRARY_DIR} OPTIONAL)
endif(RF_FOUND)

add_subdirectory(test)
