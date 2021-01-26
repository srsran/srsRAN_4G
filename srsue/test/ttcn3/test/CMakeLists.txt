#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/srsue/test/ttcn3/hdr)

add_executable(rapidjson_test rapidjson_test.cc)
target_link_libraries(rapidjson_test srslte_common)
add_test(rapidjson_test rapidjson_test)

add_executable(ttcn3_if_handler_test ttcn3_if_handler_test.cc)
target_link_libraries(ttcn3_if_handler_test sctp srslte_phy srslte_common rrc_asn1)
add_test(ttcn3_if_handler_test ttcn3_if_handler_test)