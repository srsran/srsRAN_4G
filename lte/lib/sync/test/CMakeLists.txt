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
# SYNC TEST  
########################################################################

ADD_EXECUTABLE(sync_test sync_test.c)
TARGET_LINK_LIBRARIES(sync_test lte)

ADD_TEST(sync_test_100 sync_test -o 100) 
ADD_TEST(sync_test_400 sync_test -o 400) 
ADD_TEST(sync_test_100_e sync_test -o 100 -e) 
ADD_TEST(sync_test_400_e sync_test -o 400 -e) 
 
########################################################################
# CFO TEST  
########################################################################

ADD_EXECUTABLE(cfo_test cfo_test.c)
TARGET_LINK_LIBRARIES(cfo_test lte)

ADD_TEST(cfo_test_1 cfo_test -f 0.12345 -n 1000) 
ADD_TEST(cfo_test_2 cfo_test -f 0.99849 -n 1000) 
 



