#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# DFT TEST
########################################################################

add_executable(dft_test dft_test.c)
target_link_libraries(dft_test srslte_phy)

add_test(dft_test dft_test)
add_test(dft_reverse dft_test -b) # Backwards first
add_test(dft_mirror dft_test -m)  # Mirror the freq bins
add_test(dft_norm dft_test -n)    # Normalize each transform
add_test(dft_dc dft_test -b -d)   # Backwards first & handle dc internally
add_test(dft_odd dft_test -N 255) # Odd-length
add_test(dft_odd_dc dft_test -N 255 -b -d) # Odd-length, backwards first, handle dc

########################################################################
# Algebra TEST
########################################################################

add_executable(algebra_test mat_test.c)
target_link_libraries(algebra_test srslte_phy)

add_test(algebra_2x2_zf_solver_test algebra_test -z)
add_test(algebra_2x2_mmse_solver_test algebra_test -m)

add_executable(vector_test vector_test.c)
target_link_libraries(vector_test srslte_phy)
add_test(vector_test vector_test)


########################################################################

add_executable(ringbuffer_test ring_buffer_test.c)
target_link_libraries(ringbuffer_test srslte_phy)

add_test(ringbuffer_tester ringbuffer_test)
########################################################################
