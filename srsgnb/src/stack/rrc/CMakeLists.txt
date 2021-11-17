#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES rrc_nr_config_utils.cc)
add_library(srsgnb_rrc_config_utils STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc_config_utils srsran_phy)

set(SOURCES rrc_nr.cc rrc_nr_ue.cc rrc_nr_security_context.cc cell_asn1_config.cc)
add_library(srsgnb_rrc STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc srsgnb_rrc_config_utils)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
