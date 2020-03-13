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
# RATEMATCHING TEST  
########################################################################

add_executable(rm_conv_test rm_conv_test.c)
target_link_libraries(rm_conv_test srslte_phy)

add_executable(rm_turbo_test rm_turbo_test.c)
target_link_libraries(rm_turbo_test srslte_phy)

add_test(rm_conv_test_1 rm_conv_test -t 480 -r 1920) 
add_test(rm_conv_test_2 rm_conv_test -t 1920 -r 480) 

add_test(rm_turbo_test_1 rm_turbo_test -e 1920) 
add_test(rm_turbo_test_2 rm_turbo_test -e 8192)

########################################################################
# Turbo Coder TEST  
########################################################################
add_executable(turbodecoder_test turbodecoder_test.c)
target_link_libraries(turbodecoder_test srslte_phy)

add_test(turbodecoder_test_504_1 turbodecoder_test -n 100 -s 1 -l 504 -e 1.0 -t) 
add_test(turbodecoder_test_504_2 turbodecoder_test -n 100 -s 1 -l 504 -e 2.0 -t) 
add_test(turbodecoder_test_6114_1_5 turbodecoder_test -n 100 -s 1 -l 6144 -e 1.5 -t)
add_test(turbodecoder_test_known turbodecoder_test -n 1 -s 1 -k -e 0.5)  

add_executable(turbocoder_test turbocoder_test.c)
target_link_libraries(turbocoder_test srslte_phy)
add_test(turbocoder_test_all turbocoder_test)

########################################################################
# Viterbi TEST  
########################################################################

add_executable(viterbi_test viterbi_test.c)
target_link_libraries(viterbi_test srslte_phy)

add_test(viterbi_40_0 viterbi_test -n 1000 -s 1 -l 40 -t -e 0.0) 
add_test(viterbi_40_2 viterbi_test -n 1000 -s 1 -l 40 -t -e 2.0) 
add_test(viterbi_40_3 viterbi_test -n 1000 -s 1 -l 40 -t -e 3.0)
add_test(viterbi_40_4 viterbi_test -n 1000 -s 1 -l 40 -t -e 4.5)

add_test(viterbi_1000_0 viterbi_test -n 100 -s 1 -l 1000 -t -e 0.0) 
add_test(viterbi_1000_2 viterbi_test -n 100 -s 1 -l 1000 -t -e 2.0) 
add_test(viterbi_1000_3 viterbi_test -n 100 -s 1 -l 1000 -t -e 3.0)
add_test(viterbi_1000_4 viterbi_test -n 100 -s 1 -l 1000 -t -e 4.5)

add_test(viterbi_56_4 viterbi_test -n 1000 -s 1 -l 56 -t -e 4.5)

########################################################################
# CRC TEST  
########################################################################

add_executable(crc_test crc_test.c)
target_link_libraries(crc_test srslte_phy)

add_test(crc_24A crc_test -n 5001 -l 24 -p 0x1864CFB -s 1)
add_test(crc_24B crc_test -n 5001 -l 24 -p 0x1800063 -s 1)
add_test(crc_16 crc_test -n 5001 -l 16 -p 0x11021 -s 1)
add_test(crc_8 crc_test -n 5001 -l 8 -p 0x19B -s 1)

 
