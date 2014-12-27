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
# PBCH TEST  
########################################################################

ADD_EXECUTABLE(pbch_test pbch_test.c)
TARGET_LINK_LIBRARIES(pbch_test lte_phy)

ADD_TEST(pbch_test_6 pbch_test -p 1 -n 6 -c 100) 
ADD_TEST(pbch_test_62 pbch_test -p 2 -n 6 -c 100) 
ADD_TEST(pbch_test_64 pbch_test -p 4 -n 6 -c 100) 
ADD_TEST(pbch_test_50 pbch_test -p 1 -n 50 -c 50) 
ADD_TEST(pbch_test_502 pbch_test -p 2 -n 50 -c 50) 
ADD_TEST(pbch_test_504 pbch_test -p 4 -n 50 -c 50) 
 
BuildMex(MEXNAME pbch SOURCES pbch_test_mex.c LIBRARIES lte_phy liblte_mex)


########################################################################
# PCFICH TEST  
########################################################################

ADD_EXECUTABLE(pcfich_test pcfich_test.c)
TARGET_LINK_LIBRARIES(pcfich_test lte_phy)

ADD_TEST(pcfich_test_6 pcfich_test -p 1 -n 6) 
ADD_TEST(pcfich_test_62 pcfich_test -p 2 -n 6) 
ADD_TEST(pcfich_test_64 pcfich_test -p 4 -n 6) 
ADD_TEST(pcfich_test_10 pcfich_test -p 1 -n 10) 
ADD_TEST(pcfich_test_102 pcfich_test -p 2 -n 10) 
ADD_TEST(pcfich_test_104 pcfich_test -p 4 -n 10) 

BuildMex(MEXNAME pcfich SOURCES pcfich_test_mex.c LIBRARIES lte_phy liblte_mex)

########################################################################
# PHICH TEST  
########################################################################

ADD_EXECUTABLE(phich_test phich_test.c)
TARGET_LINK_LIBRARIES(phich_test lte_phy)

ADD_TEST(phich_test_6 phich_test -p 1 -n 6) 
ADD_TEST(phich_test_62 phich_test -p 2 -n 6) 
ADD_TEST(phich_test_64 phich_test -p 4 -n 6 -g 1/6)
 
ADD_TEST(phich_test_6e phich_test -p 1 -n 6 -e) 
ADD_TEST(phich_test_62e phich_test -p 2 -n 6 -e -l) 
ADD_TEST(phich_test_64e phich_test -p 4 -n 6 -e -l -g 2)
 
ADD_TEST(phich_test_10 phich_test -p 1 -n 10 -e) 
ADD_TEST(phich_test_102 phich_test -p 2 -n 10 -g 2) 
ADD_TEST(phich_test_104 phich_test -p 4 -n 10 -e -l -g 1/2) 
 

########################################################################
# PDCCH TEST  
########################################################################

ADD_EXECUTABLE(pdcch_test pdcch_test.c)
TARGET_LINK_LIBRARIES(pdcch_test lte_phy)

ADD_TEST(pdcch_test pdcch_test) 

ADD_EXECUTABLE(dci_unpacking dci_unpacking.c)
TARGET_LINK_LIBRARIES(dci_unpacking lte_phy)

BuildMex(MEXNAME pdcch SOURCES pdcch_test_mex.c LIBRARIES lte_phy liblte_mex)

########################################################################
# PDSCH TEST  
########################################################################

ADD_EXECUTABLE(pdsch_test pdsch_test.c)
TARGET_LINK_LIBRARIES(pdsch_test lte_phy)

ADD_TEST(pdsch_test_bpsk pdsch_test -l 500 -m 1 -n 50 -r 2)
ADD_TEST(pdsch_test_qpsk pdsch_test -l 1000 -m 2 -n 50 -r 1)
ADD_TEST(pdsch_test_qam16 pdsch_test -l 50000 -m 4 -n 110)
ADD_TEST(pdsch_test_qam64 pdsch_test -l 5000 -m 6 -n 50 -r 0)

########################################################################
# FILE TEST  
########################################################################

ADD_EXECUTABLE(pbch_file_test pbch_file_test.c)
TARGET_LINK_LIBRARIES(pbch_file_test lte_phy)

ADD_EXECUTABLE(pcfich_file_test pcfich_file_test.c)
TARGET_LINK_LIBRARIES(pcfich_file_test lte_phy)

ADD_EXECUTABLE(phich_file_test phich_file_test.c)
TARGET_LINK_LIBRARIES(phich_file_test lte_phy)

ADD_EXECUTABLE(pdcch_file_test pdcch_file_test.c)
TARGET_LINK_LIBRARIES(pdcch_file_test lte_phy)

ADD_EXECUTABLE(pdsch_file_test pdsch_file_test.c)
TARGET_LINK_LIBRARIES(pdsch_file_test lte_phy)

ADD_TEST(pbch_file_test pbch_file_test -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.dat) 
ADD_TEST(pcfich_file_test pcfich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
ADD_TEST(phich_file_test phich_file_test -c 150 -n 50 -p 2 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.10M.dat) 
ADD_TEST(pdcch_file_test pdcch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat) 
ADD_TEST(pdsch_file_test pdsch_file_test -c 1 -f 3 -n 6 -p 1 -i ${CMAKE_CURRENT_SOURCE_DIR}/signal.1.92M.amar.dat) 

########################################################################
# PRACH TEST  
########################################################################

ADD_EXECUTABLE(prach_test prach_test.c)
TARGET_LINK_LIBRARIES(prach_test lte_phy)

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
 
