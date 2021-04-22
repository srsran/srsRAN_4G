/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_ENB_DL_NR_H
#define SRSRAN_ENB_DL_NR_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/pdcch_cfg_nr.h"
#include "srsran/phy/phch/pdcch_nr.h"
#include "srsran/phy/phch/pdsch_nr.h"

typedef struct SRSRAN_API {
  srsran_pdsch_nr_args_t pdsch;
  srsran_pdcch_nr_args_t pdcch;
  uint32_t               nof_tx_antennas;
  uint32_t               nof_max_prb;
} srsran_enb_dl_nr_args_t;

typedef struct SRSRAN_API {
  uint32_t              max_prb;
  uint32_t              nof_tx_antennas;
  srsran_carrier_nr_t   carrier;
  srsran_pdcch_cfg_nr_t pdcch_cfg;

  srsran_ofdm_t fft[SRSRAN_MAX_PORTS];

  cf_t*             sf_symbols[SRSRAN_MAX_PORTS];
  srsran_pdsch_nr_t pdsch;
  srsran_dmrs_sch_t dmrs;

  srsran_dci_nr_t   dci; ///< Stores DCI configuration
  srsran_pdcch_nr_t pdcch;
} srsran_enb_dl_nr_t;

SRSRAN_API int
srsran_enb_dl_nr_init(srsran_enb_dl_nr_t* q, cf_t* output[SRSRAN_MAX_PORTS], const srsran_enb_dl_nr_args_t* args);

SRSRAN_API int srsran_enb_dl_nr_set_carrier(srsran_enb_dl_nr_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_enb_dl_nr_set_pdcch_config(srsran_enb_dl_nr_t*          q,
                                                 const srsran_pdcch_cfg_nr_t* cfg,
                                                 const srsran_dci_cfg_nr_t*   dci_cfg);

SRSRAN_API void srsran_enb_dl_nr_free(srsran_enb_dl_nr_t* q);

SRSRAN_API int srsran_enb_dl_nr_base_zero(srsran_enb_dl_nr_t* q);

SRSRAN_API void srsran_enb_dl_nr_gen_signal(srsran_enb_dl_nr_t* q);

SRSRAN_API int
srsran_enb_dl_nr_pdcch_put(srsran_enb_dl_nr_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_dl_nr_t* dci_dl);

SRSRAN_API int srsran_enb_dl_nr_pdsch_put(srsran_enb_dl_nr_t*        q,
                                          const srsran_slot_cfg_t*   slot,
                                          const srsran_sch_cfg_nr_t* cfg,
                                          uint8_t*                   data[SRSRAN_MAX_TB]);

SRSRAN_API int
srsran_enb_dl_nr_pdsch_info(const srsran_enb_dl_nr_t* q, const srsran_sch_cfg_nr_t* cfg, char* str, uint32_t str_len);

#endif // SRSRAN_ENB_DL_NR_H
