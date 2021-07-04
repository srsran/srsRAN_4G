#
# Copyright 2013-2021 Software Radio Systems Limited
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

set(SOURCES ttcn3_ue.cc ttcn3_syssim.cc)
add_executable(ttcn3_dut ttcn3_dut.cc lte_ttcn3_phy.cc ${SOURCES})

set(LINK_LIBRARIES srsue_stack
                    srsue_upper
                    srsue_rrc
                    srsue_rrc_nr
                    srsran_gtpu
                    srsran_pdcp
                    srsran_rlc
                    srsran_common
                    srsran_mac
                    srsran_phy
                    srsran_radio
                    srsran_gtpu
                    srsran_pdcp
                    srsue_phy
                    srsue_mac
                    srsue_mac_nr
                    rrc_asn1
                    rrc_nr_asn1
                    sctp
                    ${Boost_LIBRARIES})

target_link_libraries(ttcn3_dut ${LINK_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR}/srsue/src/test/ttcn3/hdr)
