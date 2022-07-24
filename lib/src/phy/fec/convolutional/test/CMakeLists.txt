#
# Copyright 2013-2022 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
