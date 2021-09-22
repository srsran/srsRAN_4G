#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(test_helpers test_helpers.cc)
target_link_libraries(test_helpers srsenb_rrc srsenb_common rrc_asn1 rrc_nr_asn1 s1ap_asn1 srsran_common enb_cfg_parser ${LIBCONFIGPP_LIBRARIES})

add_executable(rrc_nr_test rrc_nr_test.cc)
target_link_libraries(rrc_nr_test srsgnb_rrc srsran_common rrc_nr_asn1 ${ATOMIC_LIBS})
add_test(rrc_nr_test rrc_nr_test)

add_executable(rrc_meascfg_test rrc_meascfg_test.cc)
target_link_libraries(rrc_meascfg_test test_helpers ${ATOMIC_LIBS})

add_executable(erab_setup_test erab_setup_test.cc)
target_link_libraries(erab_setup_test test_helpers ${LIBCONFIGPP_LIBRARIES} ${ATOMIC_LIBS})

add_executable(rrc_mobility_test rrc_mobility_test.cc)
target_link_libraries(rrc_mobility_test srsran_asn1 test_helpers ${ATOMIC_LIBS})

add_test(rrc_mobility_test rrc_mobility_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(erab_setup_test erab_setup_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
add_test(rrc_meascfg_test rrc_meascfg_test -i ${CMAKE_CURRENT_SOURCE_DIR}/../..)
