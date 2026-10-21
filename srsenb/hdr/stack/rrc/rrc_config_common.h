// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_RRC_CONFIG_COMMON_H
#define SRSRAN_RRC_CONFIG_COMMON_H

#include <cstdint>

namespace srsenb {

enum rrc_cfg_cqi_mode_t { RRC_CFG_CQI_MODE_PERIODIC = 0, RRC_CFG_CQI_MODE_APERIODIC, RRC_CFG_CQI_MODE_N_ITEMS };
static const char* rrc_cfg_cqi_mode_text[] = {"periodic", "aperiodic"};

inline const char* to_string(rrc_cfg_cqi_mode_t mode)
{
  return mode < RRC_CFG_CQI_MODE_N_ITEMS ? rrc_cfg_cqi_mode_text[mode] : "invalid CQI mode";
}

struct rrc_cfg_cqi_t {
  uint32_t           sf_mapping[80];
  uint32_t           nof_subframes;
  uint32_t           period;
  uint32_t           m_ri;
  bool               is_subband_enabled;
  uint32_t           subband_k;
  bool               simultaneousAckCQI;
  rrc_cfg_cqi_mode_t mode;
};

} // namespace srsenb

#endif // SRSRAN_RRC_CONFIG_COMMON_H
