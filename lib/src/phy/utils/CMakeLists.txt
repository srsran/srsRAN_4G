# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

file(GLOB SOURCES "*.c" "*.cpp")
add_library(srsran_utils OBJECT ${SOURCES})

if(VOLK_FOUND)
  set_target_properties(srsran_utils PROPERTIES COMPILE_DEFINITIONS "${VOLK_DEFINITIONS}")
endif(VOLK_FOUND)

add_subdirectory(test)
