# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_executable(s1ap_test s1ap_test.cc)
target_link_libraries(s1ap_test srsran_common s1ap_asn1 srsenb_s1ap srsenb_upper s1ap_asn1 ${SCTP_LIBRARIES} ${ATOMIC_LIBS})
add_test(s1ap_test s1ap_test)