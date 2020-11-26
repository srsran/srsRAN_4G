#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(RF_FOUND)
  add_library(srslte_radio STATIC radio.cc radio_null.cc channel_mapping.cc)
  target_link_libraries(srslte_radio srslte_rf srslte_common)
  INSTALL(TARGETS srslte_radio DESTINATION ${LIBRARY_DIR})
endif(RF_FOUND)

add_subdirectory(test)
