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

#ifndef SRSLTE_RF_UTILS_H
#define SRSLTE_RF_UTILS_H

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

typedef struct SRSLTE_API {
  uint32_t max_frames_pbch;      // timeout in number of 5ms frames for MIB decoding
  uint32_t max_frames_pss;       // timeout in number of 5ms frames for synchronization
  uint32_t nof_valid_pss_frames; // number of required synchronized frames
  float    init_agc;             // 0 or negative to disable AGC
  bool     force_tdd;
} cell_search_cfg_t;

SRSLTE_API int rf_rssi_scan(srslte_rf_t* rf, float* freqs, float* rssi, int nof_bands, double fs, int nsamp);

SRSLTE_API int
rf_mib_decoder(srslte_rf_t* rf, uint32_t nof_rf_channels, cell_search_cfg_t* config, srslte_cell_t* cell, float* cfo);

SRSLTE_API int rf_cell_search(srslte_rf_t*       rf,
                              uint32_t           nof_rf_channels,
                              cell_search_cfg_t* config,
                              int                force_N_id_2,
                              srslte_cell_t*     cell,
                              float*             cfo);

SRSLTE_API int rf_search_and_decode_mib(srslte_rf_t*       rf,
                                        uint32_t           nof_rf_channels,
                                        cell_search_cfg_t* config,
                                        int                force_N_id_2,
                                        srslte_cell_t*     cell,
                                        float*             cfo);

SRSLTE_API int rf_cell_search_nbiot(srslte_rf_t* rf, cell_search_cfg_t* config, srslte_nbiot_cell_t* cell, float* cfo);

#endif // SRSLTE_RF_UTILS_H
