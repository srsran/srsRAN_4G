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

add_executable(chest_test_dl chest_test_dl.c)
target_link_libraries(chest_test_dl srslte)

add_test(chest_test_dl_cellid0 chest_test_dl -c 0) 
add_test(chest_test_dl_cellid1 chest_test_dl -c 1) 
add_test(chest_test_dl_cellid2 chest_test_dl -c 2) 

add_test(chest_test_dl_cellid0 chest_test_dl -c 0 -r 50) 
add_test(chest_test_dl_cellid1 chest_test_dl -c 1 -r 50) 
add_test(chest_test_dl_cellid2 chest_test_dl -c 2 -r 50) 

BuildMex(MEXNAME chest_dl SOURCES chest_test_dl_mex.c LIBRARIES srslte_static srslte_mex)


########################################################################
# Uplink Channel Estimation TEST  
########################################################################

add_executable(chest_test_ul chest_test_ul.c)
target_link_libraries(chest_test_ul srslte)

add_executable(refsignal_ul_test_all refsignal_ul_test.c)
target_link_libraries(refsignal_ul_test_all srslte)

add_test(chest_test_ul_cellid0 chest_test_ul -c 0 -r 50) 
add_test(chest_test_ul_cellid1 chest_test_ul -c 1 -r 50) 
add_test(chest_test_ul_cellid1 chest_test_ul -c 2 -r 50) 

BuildMex(MEXNAME chest_ul SOURCES chest_test_ul_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME refsignal_pusch SOURCES refsignal_pusch_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME refsignal_srs SOURCES refsignal_srs_mex.c LIBRARIES srslte_static srslte_mex)






