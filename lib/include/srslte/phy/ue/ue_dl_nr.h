/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_UE_DL_NR_H
#define SRSLTE_UE_DL_NR_H

#include "srslte/phy/ch_estimation/dmrs_pdcch.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/dci_nr.h"
#include "srslte/phy/phch/pdcch_nr.h"
#include "srslte/phy/phch/pdsch_nr.h"

typedef struct SRSLTE_API {
  srslte_pdsch_nr_args_t pdsch;
  srslte_pdcch_nr_args_t pdcch;
  uint32_t               nof_rx_antennas;
  uint32_t               nof_max_prb;
  float                  pdcch_dmrs_corr_thr;
} srslte_ue_dl_nr_args_t;

typedef struct SRSLTE_API {
  uint32_t max_prb;
  uint32_t nof_rx_antennas;
  float    pdcch_dmrs_corr_thr;
  float    pdcch_dmrs_epre_thr;

  srslte_carrier_nr_t carrier;
  srslte_coreset_t    coreset;

  srslte_ofdm_t fft[SRSLTE_MAX_PORTS];

  cf_t*                 sf_symbols[SRSLTE_MAX_PORTS];
  srslte_chest_dl_res_t chest;
  srslte_pdsch_nr_t     pdsch;
  srslte_dmrs_sch_t     dmrs_pdsch;

  srslte_dmrs_pdcch_estimator_t dmrs_pdcch;
  srslte_pdcch_nr_t             pdcch;
  srslte_dmrs_pdcch_ce_t*       pdcch_ce;
} srslte_ue_dl_nr_t;

SRSLTE_API int
srslte_ue_dl_nr_init(srslte_ue_dl_nr_t* q, cf_t* input[SRSLTE_MAX_PORTS], const srslte_ue_dl_nr_args_t* args);

SRSLTE_API int srslte_ue_dl_nr_set_carrier(srslte_ue_dl_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API int srslte_ue_dl_nr_set_coreset(srslte_ue_dl_nr_t* q, const srslte_coreset_t* coreset);

SRSLTE_API void srslte_ue_dl_nr_free(srslte_ue_dl_nr_t* q);

SRSLTE_API void srslte_ue_dl_nr_estimate_fft(srslte_ue_dl_nr_t* q, const srslte_dl_slot_cfg_t* slot_cfg);

SRSLTE_API int srslte_ue_dl_nr_find_dl_dci(srslte_ue_dl_nr_t*           q,
                                           const srslte_search_space_t* search_space,
                                           const srslte_dl_slot_cfg_t*  slot_cfg,
                                           uint16_t                     rnti,
                                           srslte_dci_dl_nr_t*          dci_dl_list,
                                           uint32_t                     nof_dci_msg);

SRSLTE_API int srslte_ue_dl_nr_decode_pdsch(srslte_ue_dl_nr_t*          q,
                                            const srslte_dl_slot_cfg_t* slot,
                                            const srslte_sch_cfg_nr_t*  cfg,
                                            srslte_pdsch_res_nr_t*      res);

SRSLTE_API int srslte_ue_dl_nr_pdsch_info(const srslte_ue_dl_nr_t*     q,
                                          const srslte_sch_cfg_nr_t*   cfg,
                                          const srslte_pdsch_res_nr_t  res[SRSLTE_MAX_CODEWORDS],
                                          char*                        str,
                                          uint32_t                     str_len);


#endif // SRSLTE_UE_DL_NR_H
