#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (ENABLE_5GNR)
    add_executable(rrc_nr_test rrc_nr_test.cc)
    target_link_libraries(rrc_nr_test srsgnb_rrc srslte_common rrc_nr_asn1)
    add_test(rrc_nr_test rrc_nr_test)
endif ()
