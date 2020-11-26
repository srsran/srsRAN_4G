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

#ifndef SRSLTE_UE_DL_NR_H
#define SRSLTE_UE_DL_NR_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/pdsch_nr.h"

typedef struct SRSLTE_API {
  srslte_pdsch_args_t pdsch;
  uint32_t            nof_rx_antennas;
} srslte_ue_dl_nr_args_t;

typedef struct SRSLTE_API {
  uint32_t            max_prb;
  uint32_t            nof_rx_antennas;
  srslte_carrier_nr_t carrier;

  srslte_ofdm_t fft[SRSLTE_MAX_PORTS];

  cf_t*                 sf_symbols[SRSLTE_MAX_PORTS];
  srslte_chest_dl_res_t chest;
  srslte_pdsch_nr_t     pdsch;
  srslte_dmrs_pdsch_t   dmrs;
} srslte_ue_dl_nr_t;

SRSLTE_API int
srslte_ue_dl_nr_init(srslte_ue_dl_nr_t* q, cf_t* input[SRSLTE_MAX_PORTS], const srslte_ue_dl_nr_args_t* args);

SRSLTE_API int
srslte_ue_dl_nr_set_carrier(srslte_ue_dl_nr_t* q, const srslte_carrier_nr_t* carrier, const srslte_sch_cfg_t* common);

SRSLTE_API void srslte_ue_dl_nr_free(srslte_ue_dl_nr_t* q);

SRSLTE_API void srslte_ue_dl_nr_estimate_fft(srslte_ue_dl_nr_t* q);

SRSLTE_API int srslte_ue_dl_nr_pdsch_get(srslte_ue_dl_nr_t*             q,
                                         const srslte_dl_slot_cfg_t*    slot,
                                         const srslte_pdsch_cfg_nr_t*   cfg,
                                         const srslte_pdsch_grant_nr_t* grant,
                                         srslte_pdsch_res_nr_t*         res);

#endif // SRSLTE_UE_DL_NR_H
