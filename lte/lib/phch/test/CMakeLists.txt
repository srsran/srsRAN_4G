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
# PBCH TEST  
########################################################################

ADD_EXECUTABLE(pbch_test pbch_test.c)
TARGET_LINK_LIBRARIES(pbch_test lte)

ADD_TEST(pbch_test_6 pbch_test -p 6 -c 100) 
ADD_TEST(pbch_test_50 pbch_test -p 50 -c 50) 
 
########################################################################
# PBCH FILE TEST  
########################################################################

ADD_EXECUTABLE(pbch_file_test pbch_file_test.c)
TARGET_LINK_LIBRARIES(pbch_file_test lte)

ADD_TEST(pbch_file_test pbch_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/h3g.mib.dat) 
 


