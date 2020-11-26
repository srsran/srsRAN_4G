#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(test_helpers test_helpers.cc)
target_link_libraries(test_helpers srsenb_rrc rrc_asn1 s1ap_asn1 srslte_common enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test rrc_asn1)

add_executable(rrc_mobility_test rrc_mobility_test.cc)
target_link_libraries(rrc_mobility_test srslte_asn1 test_helpers)

add_executable(erab_setup_test erab_setup_test.cc)
target_link_libraries(erab_setup_test srsenb_rrc rrc_asn1 s1ap_asn1 srslte_common srslte_asn1 enb_cfg_parser test_helpers ${LIBCONFIGPP_LIBRARIES})

add_executable(rrc_meascfg_test rrc_meascfg_test.cc)
target_link_libraries(rrc_meascfg_test test_helpers)

add_test(rrc_mobility_test rrc_mobility_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(erab_setup_test erab_setup_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(rrc_meascfg_test rrc_meascfg_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)

