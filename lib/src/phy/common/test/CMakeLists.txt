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
# SEQUENCE TEST
########################################################################

add_executable(sequence_test sequence_test.c)
target_link_libraries(sequence_test srsran_phy)

add_test(sequence_test sequence_test)

########################################################################
# SLIV TEST
########################################################################

add_executable(sliv_test sliv_test.c)
target_link_libraries(sliv_test srsran_phy)

add_test(sliv_test_14 sliv_test 14)
add_test(sliv_test_52 sliv_test 48)
add_test(sliv_test_52 sliv_test 52)

########################################################################
# PHY COMMON TEST
########################################################################

add_executable(phy_common_test phy_common_test.c)
target_link_libraries(phy_common_test srsran_phy)

add_test(phy_common_test phy_common_test)