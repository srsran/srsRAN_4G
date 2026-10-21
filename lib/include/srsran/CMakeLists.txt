# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION "${INCLUDE_DIR}/srsran"
)
