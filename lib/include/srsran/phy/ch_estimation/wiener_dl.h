/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSRAN_WIENER_DL_H_
#define SRSRAN_WIENER_DL_H_

#include <srsran/config.h>
#include <srsran/phy/common/phy_common.h>
#include <srsran/phy/dft/dft.h>
#include <srsran/phy/utils/random.h>

// Constant static parameters
#define SRSRAN_WIENER_DL_HLS_FIFO_SIZE (8U)
#define SRSRAN_WIENER_DL_MIN_PRB (4U)
#define SRSRAN_WIENER_DL_MIN_RE (SRSRAN_WIENER_DL_MIN_PRB * SRSRAN_NRE)
#define SRSRAN_WIENER_DL_MIN_REF (SRSRAN_WIENER_DL_MIN_PRB * 2U)
#define SRSRAN_WIENER_DL_TFIFO_SIZE (2U)
#define SRSRAN_WIENER_DL_XFIFO_SIZE (400U)
#define SRSRAN_WIENER_DL_TIMEFIFO_SIZE (32U)
#define SRSRAN_WIENER_DL_CXFIFO_SIZE (400U)

typedef struct {
  cf_t*    hls_fifo_1[SRSRAN_WIENER_DL_HLS_FIFO_SIZE]; // Least square channel estimates on odd pilots
  cf_t*    hls_fifo_2[SRSRAN_WIENER_DL_HLS_FIFO_SIZE]; // Least square channel estimates on even pilots
  cf_t*    tfifo[SRSRAN_WIENER_DL_TFIFO_SIZE];         // memory for time domain channel linear interpolation
  cf_t*    xfifo[SRSRAN_WIENER_DL_XFIFO_SIZE];         // fifo for averaging the frequency correlation vectors
  cf_t     cV[SRSRAN_WIENER_DL_MIN_RE];                // frequency correlation vector among all subcarriers
  float    deltan;                                     // step within time domain linear interpolation
  uint32_t nfifosamps;   // number of samples inside the fifo for averaging the correlation vectors
  float    invtpilotoff; // step for time domain linear interpolation
  cf_t*    timefifo;     // fifo for storing single frequency channel time domain evolution
  cf_t*    cxfifo[SRSRAN_WIENER_DL_CXFIFO_SIZE]; // fifo for averaging time domain channel correlation vector
  uint32_t sumlen; // length of dynamic average window for time domain channel correlation vector
  uint32_t skip;   // pilot OFDM symbols to skip when training Wiener matrices (skip = 1,..,4)
  uint32_t cnt;    // counter for skipping pilot OFDM symbols
} srsran_wiener_dl_state_t;

typedef struct {
  // Maximum allocated number of...
  uint32_t max_prb;      // Resource Blocks
  uint32_t max_ref;      // Reference signals
  uint32_t max_re;       // Resource Elements (equivalent to sub-carriers)
  uint32_t max_tx_ports; // Tx Ports
  uint32_t max_rx_ant;   // Rx Antennas

  // Configured number of...
  uint32_t nof_prb;      // Resource Blocks
  uint32_t nof_ref;      // Reference signals
  uint32_t nof_re;       // Resource Elements (equivalent to sub-carriers)
  uint32_t nof_tx_ports; // Tx Ports
  uint32_t nof_rx_ant;   // Rx Antennas

  // One state per possible channel (allocated in init)
  srsran_wiener_dl_state_t* state[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];

  // Wiener matrices
  cf_t wm1[SRSRAN_WIENER_DL_MIN_RE][SRSRAN_WIENER_DL_MIN_REF];
  cf_t wm2[SRSRAN_WIENER_DL_MIN_RE][SRSRAN_WIENER_DL_MIN_REF];
  bool wm_computed;
  bool ready;

  // Calculation support
  cf_t hlsv[SRSRAN_WIENER_DL_MIN_RE];
  cf_t hlsv_sum[SRSRAN_WIENER_DL_MIN_RE];
  cf_t acV[SRSRAN_WIENER_DL_MIN_RE];

  union {
    cf_t m[SRSRAN_WIENER_DL_MIN_REF][SRSRAN_WIENER_DL_MIN_REF];
    cf_t v[SRSRAN_WIENER_DL_MIN_REF * SRSRAN_WIENER_DL_MIN_REF];
  } RH;
  union {
    cf_t m[SRSRAN_WIENER_DL_MIN_REF][SRSRAN_WIENER_DL_MIN_REF];
    cf_t v[SRSRAN_WIENER_DL_MIN_REF * SRSRAN_WIENER_DL_MIN_REF];
  } invRH;
  cf_t hH1[SRSRAN_WIENER_DL_MIN_RE][SRSRAN_WIENER_DL_MIN_REF];
  cf_t hH2[SRSRAN_WIENER_DL_MIN_RE][SRSRAN_WIENER_DL_MIN_REF];

  // Temporal vector
  cf_t* tmp;

  // Random generator
  srsran_random_t random;

  // FFT/iFFT
  srsran_dft_plan_t fft;
  srsran_dft_plan_t ifft;
  cf_t              filter[SRSRAN_WIENER_DL_MIN_RE];

  // Matrix inverter
  void* matrix_inverter;
} srsran_wiener_dl_t;

SRSRAN_API int
srsran_wiener_dl_init(srsran_wiener_dl_t* q, uint32_t max_prb, uint32_t max_tx_ports, uint32_t max_rx_ant);

SRSRAN_API int srsran_wiener_dl_set_cell(srsran_wiener_dl_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_wiener_dl_reset(srsran_wiener_dl_t* q);

SRSRAN_API int srsran_wiener_dl_run(srsran_wiener_dl_t* q,
                                    uint32_t            tx,
                                    uint32_t            rx,
                                    uint32_t            m,
                                    uint32_t            shift,
                                    cf_t*               pilots,
                                    cf_t*               estimated,
                                    float               snr_lin);

SRSRAN_API void srsran_wiener_dl_free(srsran_wiener_dl_t* q);

#endif // SRSRAN_WIENER_DL_H_
