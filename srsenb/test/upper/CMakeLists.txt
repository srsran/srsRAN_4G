# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

# Simple PLMN -> MCC/MNC test
add_executable(plmn_test plmn_test.cc)
target_link_libraries(plmn_test rrc_asn1)

add_executable(gtpu_test gtpu_test.cc)
target_link_libraries(gtpu_test srsran_common s1ap_asn1 srsenb_upper srsran_gtpu ${SCTP_LIBRARIES})

add_test(plmn_test plmn_test)
add_test(gtpu_test gtpu_test)

