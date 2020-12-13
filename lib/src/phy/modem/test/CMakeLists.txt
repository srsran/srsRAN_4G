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
# MODEM TEST  
########################################################################

add_executable(modem_test modem_test.c)
target_link_libraries(modem_test srslte_phy)

add_test(modem_bpsk modem_test -n 1024 -m 1) 
add_test(modem_qpsk modem_test -n 1024 -m 2)
add_test(modem_qam16 modem_test -n 1024 -m 4)
add_test(modem_qam64 modem_test -n 1008 -m 6)
add_test(modem_qam256 modem_test -n 1024 -m 8)

add_test(modem_bpsk_soft modem_test -n 1024 -m 1) 
add_test(modem_qpsk_soft modem_test -n 1024 -m 2)
add_test(modem_qam16_soft modem_test -n 1024 -m 4)
add_test(modem_qam64_soft modem_test -n 1008 -m 6)
add_test(modem_qam256_soft modem_test -n 1024 -m 8)
 
add_executable(soft_demod_test soft_demod_test.c)
target_link_libraries(soft_demod_test srslte_phy)

 


