#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

########################################################################
# CRC TEST  
########################################################################

add_executable(crc_test crc_test.c)
target_link_libraries(crc_test srslte_phy)

add_test(crc_24A crc_test -n 5001 -l 24 -p 0x1864CFB -s 1)
add_test(crc_24B crc_test -n 5001 -l 24 -p 0x1800063 -s 1)
add_test(crc_16 crc_test -n 5001 -l 16 -p 0x11021 -s 1)
add_test(crc_8 crc_test -n 5001 -l 8 -p 0x19B -s 1)
add_test(crc_11 crc_test -n 30 -l 11 -p 0xE21 -s 1)
add_test(crc_6 crc_test -n 20 -l 6 -p 0x61 -s 1)

 
