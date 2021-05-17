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

SRSRAN_API int srsran_refsignal_dl_sync_init(srsran_refsignal_dl_sync_t* q);

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
