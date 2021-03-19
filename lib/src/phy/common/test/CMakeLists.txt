#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# SEQUENCE TEST
########################################################################

add_executable(sequence_test sequence_test.c)
target_link_libraries(sequence_test srsran_phy)

add_test(sequence_test sequence_test)
