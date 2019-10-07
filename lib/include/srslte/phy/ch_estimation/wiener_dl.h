/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_WIENER_DL_H_
#define SRSLTE_WIENER_DL_H_

#include <srslte/config.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/dft/dft.h>
#include <srslte/phy/utils/random.h>

// Constant static parameters
#define SRSLTE_WIENER_DL_HLS_FIFO_SIZE (8U)
#define SRSLTE_WIENER_DL_MIN_PRB (4U)
#define SRSLTE_WIENER_DL_MIN_RE (SRSLTE_WIENER_DL_MIN_PRB * SRSLTE_NRE)
#define SRSLTE_WIENER_DL_MIN_REF (SRSLTE_WIENER_DL_MIN_PRB * 2U)
#define SRSLTE_WIENER_DL_TFIFO_SIZE (2U)
#define SRSLTE_WIENER_DL_XFIFO_SIZE (400U)
#define SRSLTE_WIENER_DL_TIMEFIFO_SIZE (32U)
#define SRSLTE_WIENER_DL_CXFIFO_SIZE (400U)

typedef struct {
  cf_t*    hls_fifo_1[SRSLTE_WIENER_DL_HLS_FIFO_SIZE]; // Least square channel estimates on odd pilots
  cf_t*    hls_fifo_2[SRSLTE_WIENER_DL_HLS_FIFO_SIZE]; // Least square channel estimates on even pilots
  cf_t*    tfifo[SRSLTE_WIENER_DL_TFIFO_SIZE];         // memory for time domain channel linear interpolation
  cf_t*    xfifo[SRSLTE_WIENER_DL_XFIFO_SIZE];         // fifo for averaging the frequency correlation vectors
  cf_t     cV[SRSLTE_WIENER_DL_MIN_RE];                // frequency correlation vector among all subcarriers
  float    deltan;                                     // step within time domain linear interpolation
  uint32_t nfifosamps;   // number of samples inside the fifo for averaging the correlation vectors
  float    invtpilotoff; // step for time domain linear interpolation
  cf_t*    timefifo;     // fifo for storing single frequency channel time domain evolution
  cf_t*    cxfifo[SRSLTE_WIENER_DL_CXFIFO_SIZE]; // fifo for averaging time domain channel correlation vector
  uint32_t sumlen; // length of dynamic average window for time domain channel correlation vector
  uint32_t skip;   // pilot OFDM symbols to skip when training Wiener matrices (skip = 1,..,4)
  uint32_t cnt;    // counter for skipping pilot OFDM symbols
} srslte_wiener_dl_state_t;

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
  srslte_wiener_dl_state_t* state[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];

  // Wiener matrices
  cf_t wm1[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF];
  cf_t wm2[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF];
  bool wm_computed;
  bool ready;

  // Calculation support
  cf_t hlsv[SRSLTE_WIENER_DL_MIN_RE];
  cf_t hlsv_sum[SRSLTE_WIENER_DL_MIN_RE];
  cf_t acV[SRSLTE_WIENER_DL_MIN_RE];

  union {
    cf_t m[SRSLTE_WIENER_DL_MIN_REF][SRSLTE_WIENER_DL_MIN_REF];
    cf_t v[SRSLTE_WIENER_DL_MIN_REF * SRSLTE_WIENER_DL_MIN_REF];
  } RH;
  union {
    cf_t m[SRSLTE_WIENER_DL_MIN_REF][SRSLTE_WIENER_DL_MIN_REF];
    cf_t v[SRSLTE_WIENER_DL_MIN_REF * SRSLTE_WIENER_DL_MIN_REF];
  } invRH;
  cf_t hH1[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF];
  cf_t hH2[SRSLTE_WIENER_DL_MIN_RE][SRSLTE_WIENER_DL_MIN_REF];

  // Temporal vector
  cf_t* tmp;

  // Random generator
  srslte_random_t random;

  // FFT/iFFT
  srslte_dft_plan_t fft;
  srslte_dft_plan_t ifft;
  cf_t              filter[SRSLTE_WIENER_DL_MIN_RE];

  // Matrix inverter
  void* matrix_inverter;
} srslte_wiener_dl_t;

SRSLTE_API int
srslte_wiener_dl_init(srslte_wiener_dl_t* q, uint32_t max_prb, uint32_t max_tx_ports, uint32_t max_rx_ant);

SRSLTE_API int srslte_wiener_dl_set_cell(srslte_wiener_dl_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_wiener_dl_reset(srslte_wiener_dl_t* q);

SRSLTE_API int srslte_wiener_dl_run(srslte_wiener_dl_t* q,
                                    uint32_t            tx,
                                    uint32_t            rx,
                                    uint32_t            m,
                                    uint32_t            shift,
                                    cf_t*               pilots,
                                    cf_t*               estimated,
                                    float               snr_lin);

SRSLTE_API void srslte_wiener_dl_free(srslte_wiener_dl_t* q);

#endif // SRSLTE_WIENER_DL_H_
