// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_NPDSCH_CFG_H
#define SRSRAN_NPDSCH_CFG_H

#include "srsran/phy/phch/ra_nbiot.h"

/*
 * @brief Narrowband Physical downlink shared channel configuration
 *
 * Reference: 3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.3
 */
typedef struct SRSRAN_API {
  srsran_ra_nbiot_dl_grant_t grant;
  srsran_ra_nbits_t          nbits;
  bool                       is_encoded;
  bool                       has_bcch; // Whether this NPDSCH is carrying the BCCH
  uint32_t                   sf_idx;   // The current idx within the entire NPDSCH
  uint32_t                   rep_idx;  // The current repetion within this NPDSCH
  uint32_t                   num_sf;   // Total number of subframes tx'ed in this NPDSCH
} srsran_npdsch_cfg_t;

#endif // SRSRAN_NPDSCH_CFG_H
