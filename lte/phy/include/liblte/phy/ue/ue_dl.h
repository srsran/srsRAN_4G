/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef UEDL_H
#define UEDL_H

/*******************************************************
 * 
 * This module is a frontend to all the data and control channels processing 
 * modules. 
 ********************************************************/



#include "liblte/phy/ch_estimation/chest.h"
#include "liblte/phy/common/fft.h"
#include "liblte/phy/common/phy_common.h"

#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/phch/pcfich.h"
#include "liblte/phy/phch/pdcch.h"
#include "liblte/phy/phch/pdsch.h"
#include "liblte/phy/phch/phich.h"
#include "liblte/phy/phch/ra.h"
#include "liblte/phy/phch/regs.h"

#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#include "liblte/config.h"

#define NOF_HARQ_PROCESSES 8

typedef struct LIBLTE_API {
  pcfich_t pcfich;
  pdcch_t pdcch;
  pdsch_t pdsch;
  pdsch_harq_t harq_process[NOF_HARQ_PROCESSES];
  regs_t regs;
  lte_fft_t fft;
  chest_t chest;
  
  lte_cell_t cell;

  cf_t *sf_symbols; 
  cf_t *ce[MAX_PORTS];
  
  uint64_t pkt_errors; 
  uint64_t pkts_total;
  uint64_t nof_trials;

  uint16_t user_rnti; 
}ue_dl_t;

/* This function shall be called just after the initial synchronization */
LIBLTE_API int ue_dl_init(ue_dl_t *q, 
                          lte_cell_t cell,
                          phich_resources_t phich_resources, 
                          phich_length_t phich_length, 
                          uint16_t user_rnti);

LIBLTE_API void ue_dl_free(ue_dl_t *q);

LIBLTE_API int ue_dl_receive(ue_dl_t *q, 
                             cf_t *sf_buffer, 
                             char *data, 
                             uint32_t sf_idx, 
                             uint32_t sfn, 
                             uint16_t rnti);

#endif