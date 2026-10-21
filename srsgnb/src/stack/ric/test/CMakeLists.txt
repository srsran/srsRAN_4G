# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_executable(e2ap_test e2ap_test.cc)
target_link_libraries(e2ap_test srsran_common  ric_e2 srsgnb_ric srsenb_upper  srsgnb_stack  ${SCTP_LIBRARIES})

add_test(e2ap_test e2ap_test)