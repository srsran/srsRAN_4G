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

#ifndef PDSCHCFG_
#define PDSCHCFG_

#include "srslte/phch/ra.h"
#include "srslte/fec/softbuffer.h"
#include "srslte/fec/cbsegm.h"

typedef struct SRSLTE_API {
  srslte_cbsegm_t cb_segm; 
  srslte_ra_dl_grant_t grant; 
  srslte_ra_nbits_t nbits; 
  uint32_t rv; 
  uint32_t sf_idx;  
} srslte_pdsch_cfg_t;

#endif

