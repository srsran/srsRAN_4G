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

#ifndef SRSRAN_GNB_DL_H
#define SRSRAN_GNB_DL_H

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
} srsran_gnb_dl_args_t;

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
} srsran_gnb_dl_t;

SRSRAN_API int srsran_gnb_dl_init(srsran_gnb_dl_t* q, cf_t* output[SRSRAN_MAX_PORTS], const srsran_gnb_dl_args_t* args);

SRSRAN_API int srsran_gnb_dl_set_carrier(srsran_gnb_dl_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_gnb_dl_set_pdcch_config(srsran_gnb_dl_t*             q,
                                              const srsran_pdcch_cfg_nr_t* cfg,
                                              const srsran_dci_cfg_nr_t*   dci_cfg);

SRSRAN_API void srsran_gnb_dl_free(srsran_gnb_dl_t* q);

SRSRAN_API int srsran_gnb_dl_base_zero(srsran_gnb_dl_t* q);

SRSRAN_API void srsran_gnb_dl_gen_signal(srsran_gnb_dl_t* q);

SRSRAN_API int
srsran_gnb_dl_pdcch_put_dl(srsran_gnb_dl_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_dl_nr_t* dci_dl);

SRSRAN_API int
srsran_gnb_dl_pdcch_put_ul(srsran_gnb_dl_t* q, const srsran_slot_cfg_t* slot_cfg, const srsran_dci_ul_nr_t* dci_ul);

SRSRAN_API int srsran_gnb_dl_pdsch_put(srsran_gnb_dl_t*           q,
                                       const srsran_slot_cfg_t*   slot,
                                       const srsran_sch_cfg_nr_t* cfg,
                                       uint8_t*                   data[SRSRAN_MAX_TB]);

SRSRAN_API float srsran_gnb_dl_get_maximum_signal_power_dBfs(uint32_t nof_prb);

SRSRAN_API int
srsran_gnb_dl_pdsch_info(const srsran_gnb_dl_t* q, const srsran_sch_cfg_nr_t* cfg, char* str, uint32_t str_len);

SRSRAN_API int
srsran_gnb_dl_pdcch_dl_info(const srsran_gnb_dl_t* q, const srsran_dci_dl_nr_t* dci, char* str, uint32_t str_len);

SRSRAN_API int
srsran_gnb_dl_pdcch_ul_info(const srsran_gnb_dl_t* q, const srsran_dci_ul_nr_t* dci, char* str, uint32_t str_len);

#endif // SRSRAN_GNB_DL_H
