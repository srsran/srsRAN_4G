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
# FFT TEST  
########################################################################

add_executable(ofdm_test ofdm_test.c)
target_link_libraries(ofdm_test srslte_phy)

add_test(ofdm_normal ofdm_test -r 1)
add_test(ofdm_extended ofdm_test -e -r 1)
add_test(ofdm_shifted ofdm_test -s 0.5 -r 1)
add_test(ofdm_offset ofdm_test -o 0.5 -r 1)
add_test(ofdm_force ofdm_test -N 4096 -r 1)
add_test(ofdm_extended_shifted_offset_force ofdm_test -e -o 0.5 -s 0.5 -N 4096 -r 1)
