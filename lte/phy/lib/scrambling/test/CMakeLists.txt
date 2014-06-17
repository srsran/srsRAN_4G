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
# SCRAMBLING TEST  
########################################################################

ADD_EXECUTABLE(scrambling_test scrambling_test.c)
TARGET_LINK_LIBRARIES(scrambling_test lte_phy)

ADD_TEST(scrambling_pbch_bit scrambling_test -s PBCH -c 50) 
ADD_TEST(scrambling_pbch_float scrambling_test -s PBCH -c 50 -f) 
ADD_TEST(scrambling_pbch_e_bit scrambling_test -s PBCH -c 50 -e) 
ADD_TEST(scrambling_pbch_e_float scrambling_test -s PBCH -c 50 -f -e) 
 


