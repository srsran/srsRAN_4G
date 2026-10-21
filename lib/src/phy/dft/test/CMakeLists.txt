# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# FFT TEST  
########################################################################

add_executable(ofdm_test ofdm_test.c)
target_link_libraries(ofdm_test srsran_phy)

add_test(ofdm_normal ofdm_test -r 1)
add_test(ofdm_extended ofdm_test -e -r 1)
add_test(ofdm_shifted ofdm_test -s 0.5 -r 1)
add_test(ofdm_offset ofdm_test -o 0.5 -r 1)
add_test(ofdm_force ofdm_test -N 4096 -r 1)
add_test(ofdm_extended_shifted_offset_force ofdm_test -e -o 0.5 -s 0.5 -N 4096 -r 1)
add_test(ofdm_normal_phase_compensation ofdm_test -r 1 -p 2.4e9)
add_test(ofdm_extended_phase_compensation ofdm_test -e -r 1 -p 2.4e9)
