# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_executable(thread_pool_test thread_pool_test.cc)
target_link_libraries(thread_pool_test
        srsran_common)
add_test(thread_pool_test thread_pool_test)

add_executable(thread_test thread_test.cc)
target_link_libraries(thread_test
        srsran_common)
add_test(thread_test thread_test)

add_executable(band_helper_test band_helper_test.cc)
target_link_libraries(band_helper_test srsran_common)
add_test(band_helper_test band_helper_test)