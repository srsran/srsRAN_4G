#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(SOURCES rrc.cc rrc_procedures.cc rrc_meas.cc rrc_cell.cc phy_controller.cc)
add_library(srsue_rrc STATIC ${SOURCES})

if(ENABLE_5GNR)
    set(SOURCES rrc_nr.cc)
    add_library(srsue_rrc_nr STATIC ${SOURCES})
endif()
