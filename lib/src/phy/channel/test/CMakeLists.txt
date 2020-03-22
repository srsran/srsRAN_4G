#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_executable(fading_channel_test fading_channel_test.c)

if(SRSGUI_FOUND)
  target_link_libraries(fading_channel_test ${SRSGUI_LIBRARIES})
endif(SRSGUI_FOUND)

target_link_libraries(fading_channel_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(fading_channel_test_epa5 fading_channel_test -m epa5 -s 26.04e6 -t 100)
add_test(fading_channel_test_eva70 fading_channel_test -m eva70 -s 23.04e6 -t 100)
add_test(fading_channel_test_etu300 fading_channel_test -m etu70 -s 23.04e6 -t 100)

add_executable(delay_channel_test delay_channel_test.c)
target_link_libraries(delay_channel_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(delay_channel_test delay_channel_test -m 10 -M 100 -t 1000 -T 1 -s 1.92e6)

add_executable(hst_channel_test hst_channel_test.c)
target_link_libraries(hst_channel_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(hst_channel_test hst_channel_test -f 750 -t 7.2 -i 0 -T 1 -s 1.92e6)

add_executable(awgn_channel_test awgn_channel_test.c)
if(SRSGUI_FOUND)
  target_link_libraries(awgn_channel_test ${SRSGUI_LIBRARIES})
endif(SRSGUI_FOUND)
target_link_libraries(awgn_channel_test srslte_phy srslte_common srslte_phy ${SEC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(awgn_channel_test awgn_channel_test)

