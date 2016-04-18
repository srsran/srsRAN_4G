#
# Copyright 2013-2015 Software Radio Systems Limited
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
target_link_libraries(dft_test srslte)

add_test(dft_test dft_test)
add_test(dft_reverse dft_test -b) # Backwards first
add_test(dft_mirror dft_test -m)  # Mirror the freq bins
add_test(dft_norm dft_test -n)    # Normalize each transform
add_test(dft_dc dft_test -b -d)   # Backwards first & handle dc internally
add_test(dft_odd dft_test -N 255) # Odd-length
add_test(dft_odd_dc dft_test -N 255 -b -d) # Odd-length, backwards first, handle dc

