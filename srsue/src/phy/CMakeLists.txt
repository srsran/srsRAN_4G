# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(test)

file(GLOB_RECURSE SOURCES "*.cc")
add_library(srsue_phy STATIC ${SOURCES})

if(ENABLE_GUI AND SRSGUI_FOUND)
  target_link_libraries(srsue_phy ${SRSGUI_LIBRARIES})
endif()