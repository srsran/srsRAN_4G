/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_GNB_UL_H
#define SRSRAN_GNB_UL_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/pucch_nr.h"
#include "srsran/phy/phch/pusch_nr.h"

typedef struct SRSRAN_API {
  srsran_pusch_nr_args_t pusch;
  srsran_pucch_nr_args_t pucch;
  float                  pusch_min_snr_dB; ///< Minimum SNR threshold to decode PUSCH, set to 0 for default value
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
  float                 pusch_min_snr_dB; ///< Minimum measured DMRS SNR, below this threshold PUSCH is not decoded
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
                                       srsran_uci_value_nr_t*              uci_value,
                                       srsran_csi_trs_measurements_t*      meas);

SRSRAN_API uint32_t srsran_gnb_ul_pucch_info(srsran_gnb_ul_t*                     q,
                                             const srsran_pucch_nr_resource_t*    resource,
                                             const srsran_uci_data_nr_t*          uci_data,
                                             const srsran_csi_trs_measurements_t* csi,
                                             char*                                str,
                                             uint32_t                             str_len);

SRSRAN_API uint32_t srsran_gnb_ul_pusch_info(srsran_gnb_ul_t*             q,
                                             const srsran_sch_cfg_nr_t*   cfg,
                                             const srsran_pusch_res_nr_t* res,
                                             char*                        str,
                                             uint32_t                     str_len);

#endif // SRSRAN_GNB_UL_H
