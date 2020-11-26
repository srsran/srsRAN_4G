#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
