#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ngap_test ngap_test.cc)
target_link_libraries(ngap_test srsran_common ngap_nr_asn1 srsenb_upper srsran_gtpu ngap_nr_asn1 srsgnb_stack srsgnb_ngap ${SCTP_LIBRARIES})

add_test(ngap_test ngap_test)

