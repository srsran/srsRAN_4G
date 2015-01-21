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


#ifndef SYNC_
#define SYNC_

#include <stdbool.h>
#include <math.h>

#include "liblte/config.h"
#include "liblte/phy/sync/pss.h"
#include "liblte/phy/sync/sss.h"
#include "liblte/phy/sync/cfo.h"

#define FFT_SIZE_MIN    64
#define FFT_SIZE_MAX    2048

/**
 *
 * This object performs time and frequency synchronization using the PSS and SSS signals.
 * 
 * The object is designed to work with signals sampled at 1.92 Mhz centered at the carrier frequency.
 * Thus, downsampling is required if the signal is sampled at higher frequencies.
 *
 * Correlation peak is detected comparing the maximum at the output of the correlator with a threshold.
 * The comparison accepts two modes: absolute value or peak-to-mean ratio, which are configured with the
 * functions sync_pss_det_absolute() and sync_pss_det_peakmean().
 */

typedef enum {SSS_DIFF=0, SSS_PARTIAL_3=2, SSS_FULL=1} sss_alg_t; 

typedef struct LIBLTE_API {
  pss_synch_t pss; 
  sss_synch_t sss;
  float threshold;
  float peak_value;
  float mean_peak_value;
  uint32_t N_id_2;
  uint32_t N_id_1;
  uint32_t sf_idx;
  uint32_t fft_size;
  uint32_t frame_size;
  float mean_cfo;
  cfo_t cfocorr;
  sss_alg_t sss_alg; 
  bool detect_cp;
  bool sss_en;
  bool correct_cfo; 
  lte_cp_t cp;
  uint32_t m0;
  uint32_t m1;
  float m0_value;
  float m1_value;
  float M_norm_avg; 
  float M_ext_avg; 

}sync_t;


LIBLTE_API int sync_init(sync_t *q, 
                         uint32_t frame_size, 
                         uint32_t fft_size);

LIBLTE_API void sync_free(sync_t *q);

LIBLTE_API void sync_reset(sync_t *q); 

/* Finds a correlation peak in the input signal around position find_offset */
LIBLTE_API int sync_find(sync_t *q, 
                         cf_t *input,
                         uint32_t find_offset,
                         uint32_t *peak_position);

/* Estimates the CP length */
LIBLTE_API lte_cp_t sync_detect_cp(sync_t *q, 
                                   cf_t *input, 
                                   uint32_t peak_pos);

/* Sets the threshold for peak comparison */
LIBLTE_API void sync_set_threshold(sync_t *q, 
                                   float threshold);

/* Gets the subframe idx (0 or 5) */
LIBLTE_API uint32_t sync_get_sf_idx(sync_t *q);

/* Gets the last peak value */
LIBLTE_API float sync_get_last_peak_value(sync_t *q);

/* Gets the mean peak value */
LIBLTE_API float sync_get_peak_value(sync_t *q);

/* Choose SSS detection algorithm */
LIBLTE_API void sync_set_sss_algorithm(sync_t *q, 
                                       sss_alg_t alg); 

/* Sets PSS exponential averaging alpha weight */
LIBLTE_API void sync_set_em_alpha(sync_t *q, 
                                  float alpha);

/* Sets the N_id_2 to search for */
LIBLTE_API int sync_set_N_id_2(sync_t *q, 
                               uint32_t N_id_2);

/* Gets the Physical CellId from the last call to synch_run() */
LIBLTE_API int sync_get_cell_id(sync_t *q);

/* Gets the CFO estimation from the last call to synch_run() */
LIBLTE_API float sync_get_cfo(sync_t *q);

/* Gets the CP length estimation from the last call to synch_run() */
LIBLTE_API lte_cp_t sync_get_cp(sync_t *q);

/* Sets the CP length estimation (must do it if disabled) */
LIBLTE_API void sync_set_cp(sync_t *q, lte_cp_t cp);

/* Enables/Disables SSS detection  */
LIBLTE_API void sync_sss_en(sync_t *q, 
                            bool enabled);

LIBLTE_API bool sync_sss_detected(sync_t *q);

LIBLTE_API bool sync_sss_is_en(sync_t *q); 

/* Enables/Disables CP detection  */
LIBLTE_API void sync_cp_en(sync_t *q, 
                           bool enabled);

LIBLTE_API void sync_correct_cfo(sync_t *q, 
                                 bool enabled);

#endif // SYNC_

