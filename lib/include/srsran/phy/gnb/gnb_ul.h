/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_GNB_UL_H
#define SRSRAN_GNB_UL_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/pucch_nr.h"
#include "srsran/phy/phch/pusch_nr.h"

typedef struct SRSRAN_API {
  srsran_pusch_nr_args_t pusch;
  srsran_pucch_nr_args_t pucch;
  uint32_t               nof_max_prb;
} srsran_gnb_ul_args_t;

typedef struct SRSRAN_API {
  uint32_t            max_prb;
  srsran_carrier_nr_t carrier;

  srsran_ofdm_t fft;

  cf_t*                 sf_symbols[SRSRAN_MAX_PORTS];
  srsran_pusch_nr_t     pusch;
  srsran_pucch_nr_t     pucch;
  srsran_dmrs_sch_t     dmrs;
  srsran_chest_dl_res_t chest_pusch;
  srsran_chest_ul_res_t chest_pucch;
} srsran_gnb_ul_t;

SRSRAN_API int srsran_gnb_ul_init(srsran_gnb_ul_t* q, cf_t* input, const srsran_gnb_ul_args_t* args);

SRSRAN_API void srsran_gnb_ul_free(srsran_gnb_ul_t* q);

SRSRAN_API int srsran_gnb_ul_set_carrier(srsran_gnb_ul_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_gnb_ul_fft(srsran_gnb_ul_t* q);

SRSRAN_API int srsran_gnb_ul_get_pusch(srsran_gnb_ul_t*             q,
                                       const srsran_slot_cfg_t*     slot_cfg,
                                       const srsran_sch_cfg_nr_t*   cfg,
                                       const srsran_sch_grant_nr_t* grant,
                                       srsran_pusch_res_nr_t*       data);

SRSRAN_API int srsran_gnb_ul_get_pucch(srsran_gnb_ul_t*                    q,
                                       const srsran_slot_cfg_t*            slot_cfg,
                                       const srsran_pucch_nr_common_cfg_t* cfg,
                                       const srsran_pucch_nr_resource_t*   resource,
                                       const srsran_uci_cfg_nr_t*          uci_cfg,
                                       srsran_uci_value_nr_t*              uci_value);

#endif // SRSRAN_GNB_UL_H
