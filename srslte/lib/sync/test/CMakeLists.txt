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
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

ADD_EXECUTABLE(pss_file pss_file.c)
TARGET_LINK_LIBRARIES(pss_file srslte)

IF(SRSGUI_FOUND)
  target_link_libraries(pss_file ${SRSGUI_LIBRARIES})
ELSE(SRSGUI_FOUND)
  SET_TARGET_PROPERTIES(pss_file PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
ENDIF(SRSGUI_FOUND)

IF(UHD_FOUND)
  ADD_EXECUTABLE(pss_usrp pss_usrp.c)
  TARGET_LINK_LIBRARIES(pss_usrp srslte srslte_uhd)
  IF(SRSGUI_FOUND)
    target_link_libraries(pss_usrp ${SRSGUI_LIBRARIES})
  ELSE(SRSGUI_FOUND)
    SET_TARGET_PROPERTIES(pss_usrp PROPERTIES COMPILE_DEFINITIONS "DISABLE_GRAPHICS")
  ENDIF(SRSGUI_FOUND)
ENDIF(UHD_FOUND)

BuildMex(MEXNAME pss SOURCES pss_mex.c LIBRARIES srslte srslte_mex)
BuildMex(MEXNAME sss SOURCES sss_mex.c LIBRARIES srslte srslte_mex)

# Build MEX for cp-based synchronization
BuildMex(MEXNAME cp_synch SOURCES cp_mex.c LIBRARIES srslte srslte_mex)

########################################################################
# SYNC TEST  
########################################################################

ADD_EXECUTABLE(sync_test sync_test.c)
TARGET_LINK_LIBRARIES(sync_test srslte)

ADD_TEST(sync_test_100 sync_test -o 100 -c 501) 
ADD_TEST(sync_test_400 sync_test -o 400 -c 2) 
ADD_TEST(sync_test_100_e sync_test -o 100 -e -c 150) 
ADD_TEST(sync_test_400_e sync_test -o 400 -e -c 151) 

ADD_TEST(sync_test_100 sync_test -o 100 -p 50 -c 501) 
ADD_TEST(sync_test_400 sync_test -o 400 -p 50 -c 500) 
ADD_TEST(sync_test_100_e sync_test -o 100 -e -p 50 -c 133) 
ADD_TEST(sync_test_400_e sync_test -o 400 -e -p 50 -c 123) 

########################################################################
# CFO TEST  
########################################################################

ADD_EXECUTABLE(cfo_test cfo_test.c)
TARGET_LINK_LIBRARIES(cfo_test srslte)

ADD_TEST(cfo_test_1 cfo_test -f 0.12345 -n 1000) 
ADD_TEST(cfo_test_2 cfo_test -f 0.99849 -n 1000) 
 



