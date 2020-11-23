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

#ifndef SRSLTE_ENB_DL_NR_H
#define SRSLTE_ENB_DL_NR_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/phch/pdsch_nr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SRSLTE_API {
  srslte_pdsch_args_t pdsch;
  uint32_t            nof_tx_antennas;
  uint32_t            nof_max_prb;
} srslte_enb_dl_nr_args_t;

typedef struct SRSLTE_API {
  uint32_t            max_prb;
  uint32_t            nof_tx_antennas;
  srslte_carrier_nr_t carrier;

  srslte_ofdm_t fft[SRSLTE_MAX_PORTS];

  cf_t*               sf_symbols[SRSLTE_MAX_PORTS];
  srslte_pdsch_nr_t   pdsch;
  srslte_dmrs_pdsch_t dmrs;
} srslte_enb_dl_nr_t;

SRSLTE_API int
srslte_enb_dl_nr_init(srslte_enb_dl_nr_t* q, cf_t* output[SRSLTE_MAX_PORTS], const srslte_enb_dl_nr_args_t* args);

SRSLTE_API int srslte_enb_dl_nr_set_carrier(srslte_enb_dl_nr_t* q, const srslte_carrier_nr_t* carrier);

SRSLTE_API void srslte_enb_dl_nr_free(srslte_enb_dl_nr_t* q);

SRSLTE_API void srslte_enb_dl_nr_gen_signal(srslte_enb_dl_nr_t* q);

SRSLTE_API int srslte_enb_dl_nr_pdsch_put(srslte_enb_dl_nr_t*            q,
                                          const srslte_dl_slot_cfg_t*    slot,
                                          const srslte_pdsch_cfg_nr_t*   cfg,
                                          const srslte_pdsch_grant_nr_t* grant,
                                          uint8_t*                       data[SRSLTE_MAX_TB]);

SRSLTE_API int srslte_enb_dl_nr_pdsch_info(const srslte_enb_dl_nr_t*      q,
                                           const srslte_pdsch_cfg_nr_t*   cfg,
                                           const srslte_pdsch_grant_nr_t* grant,
                                           char*                          str,
                                           uint32_t                       str_len);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_ENB_DL_NR_H
