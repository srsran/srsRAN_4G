#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(e2ap_test e2ap_test.cc)
target_link_libraries(e2ap_test srsran_common  ric_e2 srsgnb_ric srsenb_upper  srsgnb_stack  ${SCTP_LIBRARIES})

add_test(e2ap_test e2ap_test)