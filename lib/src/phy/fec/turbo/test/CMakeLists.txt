#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(CTEST_LABELS "lib;phy;fec;turbo")

########################################################################
# RATEMATCHING TEST  
########################################################################

add_executable(rm_conv_test rm_conv_test.c)
target_link_libraries(rm_conv_test srsran_phy)

add_executable(rm_turbo_test rm_turbo_test.c)
target_link_libraries(rm_turbo_test srsran_phy)

add_lte_test(rm_conv_test_1 rm_conv_test -t 480 -r 1920)
add_lte_test(rm_conv_test_2 rm_conv_test -t 1920 -r 480)

add_lte_test(rm_turbo_test_1 rm_turbo_test -e 1920)
add_lte_test(rm_turbo_test_2 rm_turbo_test -e 8192)

########################################################################
# Turbo Coder TEST  
########################################################################
add_executable(turbodecoder_test turbodecoder_test.c)
target_link_libraries(turbodecoder_test srsran_phy)

add_lte_test(turbodecoder_test_504_1 turbodecoder_test -n 100 -s 1 -l 504 -e 1.0 -t)
add_lte_test(turbodecoder_test_504_2 turbodecoder_test -n 100 -s 1 -l 504 -e 2.0 -t)
add_lte_test(turbodecoder_test_6114_1_5 turbodecoder_test -n 100 -s 1 -l 6144 -e 1.5 -t)
add_lte_test(turbodecoder_test_known turbodecoder_test -n 1 -s 1 -k -e 0.5)

add_executable(turbocoder_test turbocoder_test.c)
target_link_libraries(turbocoder_test srsran_phy)
add_lte_test(turbocoder_test_all turbocoder_test)
