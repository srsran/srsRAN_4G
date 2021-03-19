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

#ifndef SRSRAN_RF_UTILS_H
#define SRSRAN_RF_UTILS_H

#include "srsran/phy/rf/rf.h"
#include "srsran/srsran.h"

typedef struct SRSRAN_API {
  uint32_t max_frames_pbch;      // timeout in number of 5ms frames for MIB decoding
  uint32_t max_frames_pss;       // timeout in number of 5ms frames for synchronization
  uint32_t nof_valid_pss_frames; // number of required synchronized frames
  float    init_agc;             // 0 or negative to disable AGC
  bool     force_tdd;
} cell_search_cfg_t;

SRSRAN_API int rf_rssi_scan(srsran_rf_t* rf, float* freqs, float* rssi, int nof_bands, double fs, int nsamp);

SRSRAN_API int
rf_mib_decoder(srsran_rf_t* rf, uint32_t nof_rf_channels, cell_search_cfg_t* config, srsran_cell_t* cell, float* cfo);

SRSRAN_API int rf_cell_search(srsran_rf_t*       rf,
                              uint32_t           nof_rf_channels,
                              cell_search_cfg_t* config,
                              int                force_N_id_2,
                              srsran_cell_t*     cell,
                              float*             cfo);

SRSRAN_API int rf_search_and_decode_mib(srsran_rf_t*       rf,
                                        uint32_t           nof_rf_channels,
                                        cell_search_cfg_t* config,
                                        int                force_N_id_2,
                                        srsran_cell_t*     cell,
                                        float*             cfo);

SRSRAN_API int rf_cell_search_nbiot(srsran_rf_t* rf, cell_search_cfg_t* config, srsran_nbiot_cell_t* cell, float* cfo);

#endif // SRSRAN_RF_UTILS_H
