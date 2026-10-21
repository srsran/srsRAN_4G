# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

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