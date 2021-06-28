#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test rrc_asn1)

add_executable(gtpu_test gtpu_test.cc)
target_link_libraries(gtpu_test srsran_common s1ap_asn1 srsenb_upper srsran_gtpu ${SCTP_LIBRARIES})

add_test(plmn_test plmn_test)
add_test(gtpu_test gtpu_test)

