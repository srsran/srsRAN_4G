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
# NPDSCH_ENODEB/UE TESTS 
########################################################################

SET(CTEST_OUTPUT_ON_FAILURE TRUE)

# Run eNB and generate DL signal
# - we generate 512 DL frames
# - schedulingInfoSIB1-NB-r13 is 2, so 16 SIB repetitions are used
# - each frame contains a NPDCCH and NPDSCH with TBS=24 for RNTI 0x1234
set(ARG "-o;/tmp/enbtest1.bin;-n;512;-t;2")
add_test(NAME enb1
         COMMAND ${CMAKE_COMMAND} -DCMD=$<TARGET_FILE:npdsch_enodeb> "-DARG=${ARG}" -V -P ${CMAKE_CURRENT_SOURCE_DIR}/iqtests.cmake)

# Run npdsch_npdcch file tests

# Try to decode SIB (-k)
set(ARG "-i;/tmp/enbtest1.bin;-m;512;-t;2;-w;0;-k")
add_test(NAME npdsch_npdcch_file1
         COMMAND ${CMAKE_COMMAND} -DCMD=$<TARGET_FILE:npdsch_npdcch_file_test> "-DARG=${ARG}" -V -P ${CMAKE_CURRENT_SOURCE_DIR}/iqtests.cmake)
# Specify test, SIB1 should be decoded once
set_property(TEST npdsch_npdcch_file1 PROPERTY PASS_REGULAR_EXPRESSION "pkt_ok=1")
set_property(TEST npdsch_npdcch_file1 APPEND PROPERTY DEPENDS enb1)


# Try to decode NPDCCH+NPDSCH for user
set(ARG "-i;/tmp/enbtest1.bin;-m;512;-t;2;-w;0;-r;0x1234")
add_test(NAME npdsch_npdcch_file2
         COMMAND ${CMAKE_COMMAND} -DCMD=$<TARGET_FILE:npdsch_npdcch_file_test> "-DARG=${ARG}" -V -P ${CMAKE_CURRENT_SOURCE_DIR}/iqtests.cmake)
# Specify test
set_property(TEST npdsch_npdcch_file2 PROPERTY PASS_REGULAR_EXPRESSION "pkt_ok=512")
set_property(TEST npdsch_npdcch_file2 APPEND PROPERTY DEPENDS enb1)

# Run eNB and generate DL signal
# - we generate 10 DL frames
# - schedulingInfoSIB1-NB-r13 is 0, so 4 SIB repetitions are used
# - i_sf is 1, so two subframes
# - i_rep is 2, so four repetitions
# - i_mcs is 4, so TBS of 120
# - each NPDSCH lasts over 8 subframes in total, starting in sf_idx,
#   so in total, two frames are needed for NPDCCH+NPDSCH, hence, 10/2=5 frames
#   should be received
set(ARG "-o;/tmp/enbtest2.bin;-n;10;-t;0;-i;1;-m;4;-r;2")
add_test(NAME enb2
         COMMAND ${CMAKE_COMMAND} -DCMD=$<TARGET_FILE:npdsch_enodeb> "-DARG=${ARG}" -V -P ${CMAKE_CURRENT_SOURCE_DIR}/iqtests.cmake)

# Try to decode NPDCCH+NPDSCH for user
set(ARG "-i;/tmp/enbtest2.bin;-m;512;-w;0;-r;0x1234")
add_test(NAME npdsch_npdcch_file3
         COMMAND ${CMAKE_COMMAND} -DCMD=$<TARGET_FILE:npdsch_npdcch_file_test> "-DARG=${ARG}" -V -P ${CMAKE_CURRENT_SOURCE_DIR}/iqtests.cmake)
# Specify test
set_property(TEST npdsch_npdcch_file3 PROPERTY PASS_REGULAR_EXPRESSION "pkt_ok=5")
set_property(TEST npdsch_npdcch_file3 APPEND PROPERTY DEPENDS enb2)