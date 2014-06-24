/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef PRACH_
#define PRACH_

#include <stdint.h>
#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include "liblte/config.h"
#include "liblte/phy/utils/dft.h"

typedef _Complex float cf_t;
#define PRACH_DETECT_THRESH 0.3 // Should be a value in [0,1]

/** Generation and detection of RACH signals for uplink.
 *  Currently only supports preamble formats 0-3.
 *  Does not currently support high speed flag.
 *  Based on 3GPP TS 36.211 version 10.7.0 Release 10.
 */

typedef struct LIBLTE_API{
  // Parameters from higher layers (extracted from SIB2)
  uint32_t f;               // preamble format
  uint32_t rsi;             // rootSequenceIndex
  bool hs;                  // highSpeedFlag
  uint32_t zczc;            // zeroCorrelationZoneConfig
  uint32_t N_ifft_ul;       // IFFT size for uplink
  uint32_t N_ifft_prach;    // IFFT size for PRACH generation

  // Working parameters
  uint32_t N_zc;  // PRACH sequence length
  uint32_t N_cs;  // Cyclic shift size
  uint32_t N_seq; // Preamble length
  uint32_t N_cp;  // Cyclic prefix length

  // Generated tables
  cf_t seqs[64][839];         // Our set of 64 preamble sequences
  cf_t dft_seqs[64][839];     // DFT-precoded seqs
  uint32_t root_seqs_idx[64]; // Indices of root seqs in seqs table
  uint32_t N_roots;           // Number of root sequences used in this configuration

  // Containers
  cf_t *ifft_in;
  cf_t *ifft_out;
  cf_t *prach_bins;
  cf_t *corr_spec;
  float *corr;

  // PRACH IFFT
  dft_plan_t *fft;
  dft_plan_t *ifft;

  // ZC-sequence FFT and IFFT
  dft_plan_t *zc_fft;
  dft_plan_t *zc_ifft;


}prach_t;

LIBLTE_API int prach_init(prach_t *p,
                          uint32_t N_ifft_ul,
                          uint32_t preamble_format,
                          uint32_t root_seq_index,
                          bool high_speed_flag,
                          uint32_t zero_corr_zone_config);

LIBLTE_API int prach_gen(prach_t *p,
                         uint32_t seq_index,
                         uint32_t freq_offset,
                         cf_t *signal);

LIBLTE_API int prach_detect(prach_t *p,
                            uint32_t freq_offset,
                            cf_t *signal,
                            uint32_t sig_len,
                            uint32_t *indices,
                            uint32_t *ind_len);

LIBLTE_API int prach_free(prach_t *p);

LIBLTE_API int prach_print_seqs(prach_t *p);

#endif // PRACH_
