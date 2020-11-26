/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_NPDSCH_CFG_H
#define SRSLTE_NPDSCH_CFG_H

#include "srslte/phy/phch/ra_nbiot.h"

/*
 * @brief Narrowband Physical downlink shared channel configuration
 *
 * Reference: 3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.3
 */
typedef struct SRSLTE_API {
  srslte_ra_nbiot_dl_grant_t grant;
  srslte_ra_nbits_t          nbits;
  bool                       is_encoded;
  bool                       has_bcch; // Whether this NPDSCH is carrying the BCCH
  uint32_t                   sf_idx;   // The current idx within the entire NPDSCH
  uint32_t                   rep_idx;  // The current repetion within this NPDSCH
  uint32_t                   num_sf;   // Total number of subframes tx'ed in this NPDSCH
} srslte_npdsch_cfg_t;

#endif // SRSLTE_NPDSCH_CFG_H
