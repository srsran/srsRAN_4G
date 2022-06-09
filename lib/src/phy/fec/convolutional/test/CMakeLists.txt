#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# Viterbi TEST  
########################################################################

add_executable(viterbi_test viterbi_test.c)
target_link_libraries(viterbi_test srsran_phy)

add_test(viterbi_40_0 viterbi_test -n 1000 -s 1 -l 40 -t -e 0.0) 
add_test(viterbi_40_2 viterbi_test -n 1000 -s 1 -l 40 -t -e 2.0) 
add_test(viterbi_40_3 viterbi_test -n 1000 -s 1 -l 40 -t -e 3.0)
add_test(viterbi_40_4 viterbi_test -n 1000 -s 1 -l 40 -t -e 4.5)

if (HAVE_AVX2)
  # The accuracy of the 8-bit implementation of the Viterbi decoder used on
  # non-AVX2 machines falls below the theoretical accuracy at 0dB.
  add_test(viterbi_1000_0 viterbi_test -n 100 -s 1 -l 1000 -t -e 0.0)
endif()
add_test(viterbi_1000_2 viterbi_test -n 100 -s 1 -l 1000 -t -e 2.0) 
add_test(viterbi_1000_3 viterbi_test -n 100 -s 1 -l 1000 -t -e 3.0)
add_test(viterbi_1000_4 viterbi_test -n 100 -s 1 -l 1000 -t -e 4.5)

add_test(viterbi_56_4 viterbi_test -n 1000 -s 1 -l 56 -t -e 4.5)
