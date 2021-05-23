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

#ifndef SRSRAN_REFSIGNAL_DL_SYNC_H_
#define SRSRAN_REFSIGNAL_DL_SYNC_H_

#include <srsran/phy/ch_estimation/refsignal_dl.h>
#include <srsran/phy/dft/ofdm.h>
#include <srsran/phy/utils/convolution.h>

typedef struct {
  srsran_refsignal_t   refsignal;
  srsran_ofdm_t        ifft;
  cf_t*                ifft_buffer_in;
  cf_t*                ifft_buffer_out;
  cf_t*                sequences[SRSRAN_NOF_SF_X_FRAME];
  cf_t*                correlation;
  srsran_conv_fft_cc_t conv_fft_cc;

  // Results
  bool     found;
  float    rsrp_dBfs;
  float    rssi_dBfs;
  float    rsrq_dB;
  float    cfo_Hz;
  uint32_t peak_index;
} srsran_refsignal_dl_sync_t;

SRSRAN_API int srsran_refsignal_dl_sync_init(srsran_refsignal_dl_sync_t* q, srsran_cp_t cp);

SRSRAN_API int srsran_refsignal_dl_sync_set_cell(srsran_refsignal_dl_sync_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_refsignal_dl_sync_free(srsran_refsignal_dl_sync_t* q);

SRSRAN_API int srsran_refsignal_dl_sync_run(srsran_refsignal_dl_sync_t* q, cf_t* buffer, uint32_t nsamples);

SRSRAN_API void srsran_refsignal_dl_sync_measure_sf(srsran_refsignal_dl_sync_t* q,
                                                    cf_t*                       buffer,
                                                    uint32_t                    sf_idx,
                                                    float*                      rsrp,
                                                    float*                      rssi,
                                                    float*                      cfo);

#endif // SRSRAN_REFSIGNAL_DL_SYNC_H_
