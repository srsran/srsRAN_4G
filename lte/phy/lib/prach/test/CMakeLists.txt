#
# Copyright 2012-2013 The libLTE Developers. See the
# COPYRIGHT file at the top-level directory of this distribution.
#
# This file is part of the libLTE library.
#
# libLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

########################################################################
# PRACH TEST
########################################################################

ADD_EXECUTABLE(prach_test prach_test.c)
TARGET_LINK_LIBRARIES(prach_test lte_phy)

ADD_EXECUTABLE(prach_bench prach_bench.c)
TARGET_LINK_LIBRARIES(prach_bench lte_phy)

ADD_TEST(prach prach_test)

ADD_TEST(prach_256 prach_test -N 256)
ADD_TEST(prach_512 prach_test -N 512)
ADD_TEST(prach_1024 prach_test -N 1024)
ADD_TEST(prach_1536 prach_test -N 1536)
ADD_TEST(prach_2048 prach_test -N 2048)

ADD_TEST(prach_f0 prach_test -f 0)
ADD_TEST(prach_f1 prach_test -f 1)
ADD_TEST(prach_f2 prach_test -f 2)
ADD_TEST(prach_f3 prach_test -f 3)

ADD_TEST(prach_rs1 prach_test -r 1)
ADD_TEST(prach_rs2 prach_test -r 2)
ADD_TEST(prach_rs3 prach_test -r 3)

ADD_TEST(prach_zc0 prach_test -z 0)
ADD_TEST(prach_zc2 prach_test -z 2)
ADD_TEST(prach_zc3 prach_test -z 3)
 
ADD_EXECUTABLE(prach_test_multi prach_test_multi.c)
TARGET_LINK_LIBRARIES(prach_test_multi lte_phy)

ADD_TEST(prach_test_multi prach_test_multi)

ADD_TEST(prach_test_multi_n32 prach_test_multi -n 32)
ADD_TEST(prach_test_multi_n16 prach_test_multi -n 16)
ADD_TEST(prach_test_multi_n8 prach_test_multi -n 8)
ADD_TEST(prach_test_multi_n4 prach_test_multi -n 4)
