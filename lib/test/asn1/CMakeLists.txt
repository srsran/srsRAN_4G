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

add_executable(srslte_asn1_m2ap_test srslte_asn1_m2ap_test.cc)
target_link_libraries(srslte_asn1_m2ap_test srslte_common srslte_phy srslte_asn1)
add_test(srslte_asn1_m2ap_test srslte_asn1_m2ap_test)

add_executable(srslte_asn1_nas_test srslte_asn1_nas_test.cc)
target_link_libraries(srslte_asn1_nas_test srslte_common srslte_phy srslte_asn1)
add_test(srslte_asn1_nas_test srslte_asn1_nas_test)

add_executable(rrc_asn1_test rrc_asn1_test.cc)
target_link_libraries(rrc_asn1_test rrc_asn1 asn1_utils srslte_common)
add_test(rrc_asn1_test rrc_asn1_test)

if (ENABLE_5GNR)
    add_executable(ngap_asn1_test ngap_asn1_test.cc)
    target_link_libraries(ngap_asn1_test ngap_nr_asn1 srslte_common)
    add_test(ngap_asn1_test ngap_asn1_test)
endif(ENABLE_5GNR)

add_executable(rrc_asn1_decoder rrc_asn1_decoder.cc)
target_link_libraries(rrc_asn1_decoder rrc_asn1)

add_executable(nas_decoder nas_decoder.cc)
target_link_libraries(nas_decoder srslte_asn1)