# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

file(GLOB SOURCES "*.cc")
add_library(srsepc_mme STATIC ${SOURCES})