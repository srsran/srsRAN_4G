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

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test rrc_asn1)

add_executable(rrc_mobility_test rrc_mobility_test.cc)
target_link_libraries(rrc_mobility_test srsenb_rrc rrc_asn1 s1ap_asn1 srslte_common srslte_asn1 enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

add_executable(erab_setup_test erab_setup_test.cc)
target_link_libraries(erab_setup_test srsenb_rrc rrc_asn1 s1ap_asn1 srslte_common srslte_asn1 enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

add_test(rrc_mobility_test rrc_mobility_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(erab_setup_test erab_setup_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)

