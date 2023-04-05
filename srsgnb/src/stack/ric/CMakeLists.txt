#
# Copyright 2013-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES e2_agent.cc e2ap_ric_subscription.cc e2ap.cc e2sm_kpm_common.cc e2sm_kpm.cc e2sm_kpm_report_service.cc)
add_library(srsgnb_ric STATIC ${SOURCES})
target_link_libraries(srsgnb_ric srsran_asn1 ric_e2)

add_subdirectory(test)
