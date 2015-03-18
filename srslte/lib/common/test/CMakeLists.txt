#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
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

ADD_EXECUTABLE(fft_test fft_test.c)
TARGET_LINK_LIBRARIES(fft_test lte_phy)

ADD_TEST(fft_normal fft_test) 
ADD_TEST(fft_extended fft_test -e) 

ADD_TEST(fft_normal_single fft_test -n 6) 
ADD_TEST(fft_extended_single fft_test -e -n 6) 

