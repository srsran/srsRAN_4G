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
# RATEMATCHING TEST  
########################################################################

ADD_EXECUTABLE(rm_conv_test rm_conv_test.c)
TARGET_LINK_LIBRARIES(rm_conv_test lte)

ADD_TEST(rm_conv_test_1 rm_conv_test -t 480 -r 1920) 
ADD_TEST(rm_conv_test_2 rm_conv_test -t 1920 -r 480) 
 


