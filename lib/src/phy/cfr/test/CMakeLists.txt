#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# CFR Test
########################################################################

add_executable(cfr_test cfr_test.c)
target_link_libraries(cfr_test srsran_phy)

add_test(cfr_test_default cfr_test)

