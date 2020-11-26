#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# RESAMPLING TEST  
########################################################################

add_executable(resample_arb_test resample_arb_test.c)
target_link_libraries(resample_arb_test srslte_phy)

add_executable(resample_arb_bench resample_arb_bench.c)
target_link_libraries(resample_arb_bench srslte_phy)

add_test(resample resample_arb_test)

########################################################################
# FFT based interpolate/decimate
########################################################################
add_executable(resampler_test resampler_test.c)
target_link_libraries(resampler_test srslte_phy)

add_test(resampler_test_2 resampler_test -s 1920 -r 2 -f 2)
add_test(resampler_test_3 resampler_test -s 1920 -r 2 -f 3)
add_test(resampler_test_6 resampler_test -s 1920 -r 2 -f 6)
add_test(resampler_test_8 resampler_test -s 1920 -r 2 -f 8)
add_test(resampler_test_12 resampler_test -s 1920 -r 2 -f 12)
add_test(resampler_test_16 resampler_test -s 1920 -r 2 -f 16)

