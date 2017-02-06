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

find_package(SRSGUI)

########################################################################
# PROGRAM TO DEBUG PSS FROM USRP  
########################################################################

add_executable(pss_file pss_file.c)
target_link_libraries(pss_file srslte)

if(UHD_FOUND)
  add_executable(pss_usrp pss_usrp.c)
  target_link_libraries(pss_usrp srslte)  
endif(UHD_FOUND)


if(SRSGUI_FOUND)
  include_directories(${SRSGUI_INCLUDE_DIRS})
  target_link_libraries(pss_file ${SRSGUI_LIBRARIES})
  if(UHD_FOUND)
    target_link_libraries(pss_usrp ${SRSGUI_LIBRARIES})
  endif(UHD_FOUND)
else(SRSGUI_FOUND)
  add_definitions(-DDISABLE_GRAPHICS)
endif(SRSGUI_FOUND)


BuildMex(MEXNAME pss SOURCES pss_mex.c LIBRARIES srslte_static srslte_mex)
BuildMex(MEXNAME sss SOURCES sss_mex.c LIBRARIES srslte_static srslte_mex)

# Build MEX for cp-based synchronization
BuildMex(MEXNAME cp_synch SOURCES cp_mex.c LIBRARIES srslte_static srslte_mex)

########################################################################
# SYNC TEST  
########################################################################

add_executable(sync_test sync_test.c)
target_link_libraries(sync_test srslte)

add_test(sync_test_100 sync_test -o 100 -c 501)
add_test(sync_test_400 sync_test -o 400 -c 2)
add_test(sync_test_100_e sync_test -o 100 -e -c 150)
add_test(sync_test_400_e sync_test -o 400 -e -c 151)

add_test(sync_test_100 sync_test -o 100 -p 50 -c 501)
add_test(sync_test_400 sync_test -o 400 -p 50 -c 500)
add_test(sync_test_100_e sync_test -o 100 -e -p 50 -c 133)
add_test(sync_test_400_e sync_test -o 400 -e -p 50 -c 123)

########################################################################
# CFO TEST  
########################################################################

add_executable(cfo_test cfo_test.c)
target_link_libraries(cfo_test srslte)

add_test(cfo_test_1 cfo_test -f 0.12345 -n 1000)
add_test(cfo_test_2 cfo_test -f 0.99849 -n 1000)
 



