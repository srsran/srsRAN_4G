#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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

