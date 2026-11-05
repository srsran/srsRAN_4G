# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

add_executable(ngap_test ngap_test.cc)
target_link_libraries(ngap_test srsran_common ngap_nr_asn1 srsenb_upper srsran_gtpu ngap_nr_asn1 srsgnb_stack srsgnb_ngap ${SCTP_LIBRARIES})

add_test(ngap_test ngap_test)

