# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES rrc_nr_config_utils.cc)
add_library(srsgnb_rrc_config_utils STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc_config_utils srsran_phy)

set(SOURCES rrc_nr.cc rrc_nr_ue.cc rrc_nr_security_context.cc cell_asn1_config.cc rrc_nr_du_manager.cc)
add_library(srsgnb_rrc STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc srsgnb_rrc_config_utils)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
