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
# MODEM TEST  
########################################################################

ADD_EXECUTABLE(modem_test modem_test.c)
TARGET_LINK_LIBRARIES(modem_test lte_phy)

ADD_TEST(modem_bpsk modem_test -n 1020 -m 1) 
ADD_TEST(modem_qpsk modem_test -n 1020 -m 2)
ADD_TEST(modem_qam16 modem_test -n 1020 -m 4)
ADD_TEST(modem_qam64 modem_test -n 1020 -m 6)

ADD_TEST(modem_bpsk_soft modem_test -n 1020 -m 1 -s) 
ADD_TEST(modem_qpsk_soft modem_test -n 1020 -m 2 -s)
ADD_TEST(modem_qam16_soft modem_test -n 1020 -m 4 -s)
ADD_TEST(modem_qam64_soft modem_test -n 1020 -m 6 -s)
 
ADD_EXECUTABLE(soft_demod_test soft_demod_test.c)
TARGET_LINK_LIBRARIES(soft_demod_test lte_phy)

ADD_TEST(modem_bpsk_soft_approx soft_demod_test -n 1020 -m 1) 
ADD_TEST(modem_qpsk_soft_approx soft_demod_test -n 1020 -m 2)
ADD_TEST(modem_qam16_soft_approx soft_demod_test -n 1020 -m 4)
ADD_TEST(modem_qam64_soft_approx soft_demod_test -n 1020 -m 6)
 


