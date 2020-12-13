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

set(SOURCES ttcn3_ue.cc ttcn3_syssim.cc)
add_executable(ttcn3_dut ttcn3_dut.cc lte_ttcn3_phy.cc ${SOURCES})
target_link_libraries(ttcn3_dut srsue_stack
                                srsue_upper
                                srsue_rrc
                                srslte_upper
                                srslte_common
                                srslte_mac
                                srslte_phy
                                srslte_radio
                                srslte_upper
                                srsue_phy
                                srsue_mac
                                rrc_asn1
                                sctp
                                ${Boost_LIBRARIES})
include_directories(${PROJECT_SOURCE_DIR}/srsue/test/ttcn3/hdr)
