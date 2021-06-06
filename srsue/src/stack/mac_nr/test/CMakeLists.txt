#
# Copyright 2013-2021 Software Radio Systems Limited
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

add_executable(proc_ra_nr_test proc_ra_nr_test.cc)
target_link_libraries(proc_ra_nr_test srsue_mac_nr srsran_common ${ATOMIC_LIBS})
add_test(proc_ra_nr_test proc_ra_nr_test)

add_executable(proc_bsr_nr_test proc_bsr_nr_test.cc)
target_link_libraries(proc_bsr_nr_test srsue_mac_nr srsran_common ${ATOMIC_LIBS})
add_test(proc_bsr_nr_test proc_bsr_nr_test)

add_executable(proc_sr_nr_test proc_sr_nr_test.cc)
target_link_libraries(proc_sr_nr_test srsue_mac_nr srsran_common)
add_test(proc_sr_nr_test proc_sr_nr_test)

add_executable(mac_nr_test mac_nr_test.cc)
target_link_libraries(mac_nr_test srsue_mac_nr srsran_common ${ATOMIC_LIBS})
add_test(mac_nr_test mac_nr_test)