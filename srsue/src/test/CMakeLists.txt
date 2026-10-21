# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

if (ENABLE_TTCN3)
  add_subdirectory(ttcn3)
endif (ENABLE_TTCN3)

add_executable(metrics_test metrics_test.cc ../metrics_stdout.cc ../metrics_csv.cc)
target_link_libraries(metrics_test srsran_phy srsran_common)
add_test(metrics_test metrics_test -o ${CMAKE_CURRENT_BINARY_DIR}/ue_metrics.csv)