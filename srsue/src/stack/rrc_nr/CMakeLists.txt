# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_subdirectory(test)

set(SOURCES rrc_nr.cc rrc_nr_procedures.cc ../rrc/rrc_cell.cc)
add_library(srsue_rrc_nr STATIC ${SOURCES})