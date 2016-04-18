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
# FFT TEST  
########################################################################

add_executable(ofdm_test ofdm_test.c)
target_link_libraries(ofdm_test srslte)

add_test(ofdm_normal ofdm_test) 
add_test(ofdm_extended ofdm_test -e) 

add_test(ofdm_normal_single ofdm_test -n 6) 
add_test(ofdm_extended_single ofdm_test -e -n 6) 

