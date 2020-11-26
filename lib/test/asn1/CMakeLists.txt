#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(s1ap_test s1ap_test.cc)
target_link_libraries(s1ap_test srslte_common srslte_asn1 s1ap_asn1)
add_test(s1ap_test s1ap_test)

add_executable(srslte_asn1_rrc_mcch_test srslte_asn1_rrc_mcch_test.cc)
target_link_libraries(srslte_asn1_rrc_mcch_test rrc_asn1 srslte_common)
add_test(srslte_asn1_rrc_mcch_test srslte_asn1_rrc_mcch_test)

add_executable(srslte_asn1_rrc_meas_test srslte_asn1_rrc_meas_test.cc)
target_link_libraries(srslte_asn1_rrc_meas_test rrc_asn1 srslte_common)
add_test(srslte_asn1_rrc_meas_test srslte_asn1_rrc_meas_test)

add_executable(srslte_asn1_rrc_ul_dcch_test srslte_asn1_rrc_ul_dcch_test.cc)
target_link_libraries(srslte_asn1_rrc_ul_dcch_test rrc_asn1 srslte_common)
add_test(srslte_asn1_rrc_ul_dcch_test srslte_asn1_rrc_ul_dcch_test)

add_executable(srslte_asn1_rrc_dl_ccch_test srslte_asn1_rrc_dl_ccch_test.cc)
target_link_libraries(srslte_asn1_rrc_dl_ccch_test rrc_asn1 srslte_common)
add_test(srslte_asn1_rrc_dl_ccch_test srslte_asn1_rrc_dl_ccch_test)

add_executable(srslte_asn1_rrc_dl_dcch_test srslte_asn1_rrc_dl_dcch_test.cc)
target_link_libraries(srslte_asn1_rrc_dl_dcch_test rrc_asn1 srslte_common)
add_test(srslte_asn1_rrc_dl_dcch_test srslte_asn1_rrc_dl_dcch_test)

add_executable(srslte_asn1_nas_test srslte_asn1_nas_test.cc)
target_link_libraries(srslte_asn1_nas_test srslte_common srslte_phy srslte_asn1)
add_test(srslte_asn1_nas_test srslte_asn1_nas_test)

add_executable(asn1_utils_test asn1_utils_test.cc)
target_link_libraries(asn1_utils_test asn1_utils srslte_common)
add_test(asn1_utils_test asn1_utils_test)

add_executable(rrc_asn1_test rrc_test.cc)
target_link_libraries(rrc_asn1_test rrc_asn1 asn1_utils srslte_common)
add_test(rrc_asn1_test rrc_asn1_test)

if (ENABLE_5GNR)
    add_executable(ngap_asn1_test ngap_test.cc)
    target_link_libraries(ngap_asn1_test ngap_nr_asn1 srslte_common)
    add_test(ngap_asn1_test ngap_asn1_test)
endif(ENABLE_5GNR)

add_executable(rrc_asn1_decoder rrc_asn1_decoder.cc)
target_link_libraries(rrc_asn1_decoder rrc_asn1)

add_executable(nas_decoder nas_decoder.cc)
target_link_libraries(nas_decoder srslte_asn1)
