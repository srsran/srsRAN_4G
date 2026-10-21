# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_library(test_helpers STATIC test_helpers.cc)
target_link_libraries(test_helpers srsenb_rrc srsenb_common rrc_asn1 rrc_nr_asn1 s1ap_asn1 srsran_common enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

add_executable(rrc_meascfg_test rrc_meascfg_test.cc)
target_link_libraries(rrc_meascfg_test test_helpers ${ATOMIC_LIBS})

add_executable(erab_setup_test erab_setup_test.cc)
target_link_libraries(erab_setup_test test_helpers ${LIBCONFIGPP_LIBRARIES} ${ATOMIC_LIBS})

add_executable(rrc_mobility_test rrc_mobility_test.cc)
target_link_libraries(rrc_mobility_test srsran_asn1 test_helpers ${ATOMIC_LIBS})

add_executable(rrc_paging_test rrc_paging_test.cc)
target_link_libraries(rrc_paging_test srsran_asn1 test_helpers)

add_test(rrc_mobility_test rrc_mobility_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(erab_setup_test erab_setup_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(rrc_meascfg_test rrc_meascfg_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(rrc_paging_test rrc_paging_test)
