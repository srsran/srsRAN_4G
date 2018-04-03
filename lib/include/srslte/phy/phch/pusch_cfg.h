/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         pdsch_cfg.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.4
 *****************************************************************************/

#ifndef SRSLTE_PUSCH_CFG_H
#define SRSLTE_PUSCH_CFG_H

#include "srslte/phy/phch/ra.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/fec/cbsegm.h"

typedef struct SRSLTE_API {
  uint32_t I_offset_cqi;
  uint32_t I_offset_ri;
  uint32_t I_offset_ack;
} srslte_uci_cfg_t;

typedef struct SRSLTE_API {
  srslte_cbsegm_t cb_segm; 
  srslte_ra_ul_grant_t grant; 
  srslte_ra_nbits_t nbits; 
  srslte_uci_cfg_t uci_cfg; 
  uint32_t rv; 
  uint32_t sf_idx;
  uint32_t tti; 
  srslte_cp_t cp; 
  uint32_t last_O_cqi;
} srslte_pusch_cfg_t;

#endif // SRSLTE_PUSCH_CFG_H

