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
# Downlink Channel Estimation TEST  
########################################################################

ADD_EXECUTABLE(chest_test_dl chest_test_dl.c)
TARGET_LINK_LIBRARIES(chest_test_dl lte_phy)

ADD_TEST(chest_test_dl_cellid0 chest_test_dl -c 0) 
ADD_TEST(chest_test_dl_cellid1 chest_test_dl -c 1) 
ADD_TEST(chest_test_dl_cellid2 chest_test_dl -c 2) 

########################################################################
# Downlink MEX libs  
########################################################################

BuildMex(MEXNAME chest SOURCES chest_test_dl_mex.c LIBRARIES lte_phy)


########################################################################
# Uplink Channel Estimation TEST  
########################################################################

#ADD_EXECUTABLE(chest_test_ul chest_test_ul.c)
#TARGET_LINK_LIBRARIES(chest_test_ul lte_phy)

#ADD_TEST(chest_test_ul_cellid0 chest_ul_test -c 0) 
#ADD_TEST(chest_test_ul_cellid1 chest_ul_test -c 1) 
#ADD_TEST(chest_test_ul_cellid2 chest_ul_test -c 2) 









