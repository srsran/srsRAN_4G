# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# MODEM TEST  
########################################################################

add_executable(modem_test modem_test.c)
target_link_libraries(modem_test srsran_phy)

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
target_link_libraries(soft_demod_test srsran_phy)

 


