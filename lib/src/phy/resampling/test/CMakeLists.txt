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

