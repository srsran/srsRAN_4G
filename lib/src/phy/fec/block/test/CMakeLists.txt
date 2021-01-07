#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Viterbi TEST  
########################################################################

add_executable(block_test block_test.c)
target_link_libraries(block_test srslte_phy)

add_test(block_test block_test)
