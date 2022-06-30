#
# Copyright 2013-2022 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# CRC TEST  
########################################################################

add_executable(crc_test crc_test.c)
target_link_libraries(crc_test srsran_phy)

add_test(crc_24A crc_test -n 5001 -l 24 -p 0x1864CFB -s 1)
add_test(crc_24B crc_test -n 5001 -l 24 -p 0x1800063 -s 1)
add_test(crc_16 crc_test -n 5001 -l 16 -p 0x11021 -s 1)
add_test(crc_8 crc_test -n 5001 -l 8 -p 0x19B -s 1)
add_test(crc_11 crc_test -n 30 -l 11 -p 0xE21 -s 1)
add_test(crc_6 crc_test -n 20 -l 6 -p 0x61 -s 1)

 
