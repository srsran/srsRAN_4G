#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
