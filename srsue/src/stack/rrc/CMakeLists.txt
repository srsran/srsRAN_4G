# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

add_subdirectory(test)

set(SOURCES rrc.cc rrc_procedures.cc rrc_meas.cc rrc_cell.cc rrc_rlf_report.cc phy_controller.cc)
add_library(srsue_rrc STATIC ${SOURCES})