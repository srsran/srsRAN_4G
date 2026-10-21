# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_executable(mac_test mac_test.cc)
target_link_libraries(mac_test srsue_mac srsue_phy srsran_common srsran_mac srsran_phy srsran_radio srsran_asn1 rrc_asn1 ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_test(mac_test mac_test)