#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ue_rrc_nr_test ue_rrc_nr_test.cc)
target_link_libraries(ue_rrc_nr_test srsue_rrc srsue_rrc_nr srsue_upper srsran_common srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)