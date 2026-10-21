# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

########################################################################
# SCRAMBLING TEST  
########################################################################

add_executable(scrambling_test scrambling_test.c)
target_link_libraries(scrambling_test srsran_phy)

add_test(scrambling_pbch_bit scrambling_test -s PBCH -c 50) 
add_test(scrambling_pbch_float scrambling_test -s PBCH -c 50 -f) 
add_test(scrambling_pbch_e_bit scrambling_test -s PBCH -c 50 -e) 
add_test(scrambling_pbch_e_float scrambling_test -s PBCH -c 50 -f -e) 
 


