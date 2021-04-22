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

add_library(test_helpers test_helpers.cc)
target_link_libraries(test_helpers srsenb_rrc srsenb_common rrc_asn1 s1ap_asn1 srsran_common enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test rrc_asn1)

add_executable(rrc_mobility_test rrc_mobility_test.cc)
target_link_libraries(rrc_mobility_test srsran_asn1 test_helpers)

add_executable(erab_setup_test erab_setup_test.cc)
target_link_libraries(erab_setup_test test_helpers ${LIBCONFIGPP_LIBRARIES})

add_executable(rrc_meascfg_test rrc_meascfg_test.cc)
target_link_libraries(rrc_meascfg_test test_helpers)

add_executable(gtpu_test gtpu_test.cc)
target_link_libraries(gtpu_test srsran_common s1ap_asn1 srsenb_upper srsran_upper ${SCTP_LIBRARIES})

add_executable(s1ap_test s1ap_test.cc)
target_link_libraries(s1ap_test srsran_common s1ap_asn1 srsenb_upper srsran_upper s1ap_asn1 ${SCTP_LIBRARIES})

add_test(rrc_mobility_test rrc_mobility_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(erab_setup_test erab_setup_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(rrc_meascfg_test rrc_meascfg_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(plmn_test plmn_test)
add_test(gtpu_test gtpu_test)

