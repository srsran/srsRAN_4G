#
# Copyright 2012-2013 The srsLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the srsLTE library.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# FFT TEST  
########################################################################

ADD_EXECUTABLE(ofdm_test ofdm_test.c)
TARGET_LINK_LIBRARIES(ofdm_test srslte)

ADD_TEST(ofdm_normal ofdm_test) 
ADD_TEST(ofdm_extended ofdm_test -e) 

ADD_TEST(ofdm_normal_single ofdm_test -n 6) 
ADD_TEST(ofdm_extended_single ofdm_test -e -n 6) 

