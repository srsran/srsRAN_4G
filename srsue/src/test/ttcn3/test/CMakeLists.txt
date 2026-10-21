# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

include_directories(${PROJECT_SOURCE_DIR}/srsue/src/test/ttcn3/hdr)

add_executable(rapidjson_test rapidjson_test.cc)
target_link_libraries(rapidjson_test srsran_common)
add_test(rapidjson_test rapidjson_test)

add_executable(ttcn3_if_handler_test ttcn3_if_handler_test.cc)
target_link_libraries(ttcn3_if_handler_test sctp srsran_phy srsran_common rrc_asn1)
add_test(ttcn3_if_handler_test ttcn3_if_handler_test)