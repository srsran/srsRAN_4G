# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES rrc.cc rrc_ue.cc rrc_mobility.cc rrc_cell_cfg.cc rrc_bearer_cfg.cc mac_controller.cc ue_rr_cfg.cc ue_meas_cfg.cc rrc_endc.cc)
add_library(srsenb_rrc STATIC ${SOURCES})
  