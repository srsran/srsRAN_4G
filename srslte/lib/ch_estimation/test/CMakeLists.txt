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
# Downlink Channel Estimation TEST  
########################################################################

ADD_EXECUTABLE(chest_test_dl chest_test_dl.c)
TARGET_LINK_LIBRARIES(chest_test_dl srslte)

ADD_TEST(chest_test_dl_cellid0 chest_test_dl -c 0) 
ADD_TEST(chest_test_dl_cellid1 chest_test_dl -c 1) 
ADD_TEST(chest_test_dl_cellid2 chest_test_dl -c 2) 

ADD_TEST(chest_test_dl_cellid0 chest_test_dl -c 0 -r 50) 
ADD_TEST(chest_test_dl_cellid1 chest_test_dl -c 1 -r 50) 
ADD_TEST(chest_test_dl_cellid2 chest_test_dl -c 2 -r 50) 

########################################################################
# Downlink MEX libs  
########################################################################

BuildMex(MEXNAME chest SOURCES chest_test_dl_mex.c LIBRARIES srslte)


########################################################################
# Uplink Channel Estimation TEST  
########################################################################

ADD_EXECUTABLE(refsignal_ul_test_all refsignal_ul_test.c)
TARGET_LINK_LIBRARIES(refsignal_ul_test_all srslte)

BuildMex(MEXNAME refsignal_pusch SOURCES refsignal_pusch_mex.c LIBRARIES srslte srslte_mex)

BuildMex(MEXNAME refsignal_srs SOURCES refsignal_srs_mex.c LIBRARIES srslte srslte_mex)






