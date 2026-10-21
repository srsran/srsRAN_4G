# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(mac)
add_subdirectory(phy)
add_subdirectory(upper)
add_subdirectory(rrc)
add_subdirectory(s1ap)

add_executable(enb_metrics_test enb_metrics_test.cc ../src/metrics_stdout.cc ../src/metrics_csv.cc)
target_link_libraries(enb_metrics_test srsran_phy srsran_common)
add_test(enb_metrics_test enb_metrics_test -o ${CMAKE_CURRENT_BINARY_DIR}/enb_metrics.csv)
