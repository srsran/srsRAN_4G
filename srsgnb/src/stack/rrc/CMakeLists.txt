#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(SOURCES rrc_nr_config_utils.cc)
add_library(srsgnb_rrc_config_utils STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc_config_utils srsran_phy)

set(SOURCES rrc_nr.cc rrc_nr_ue.cc rrc_nr_security_context.cc cell_asn1_config.cc rrc_nr_du_manager.cc)
add_library(srsgnb_rrc STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc srsgnb_rrc_config_utils)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(test)
