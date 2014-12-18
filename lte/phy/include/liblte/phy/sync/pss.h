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


#ifndef PSS_
#define PSS_

#include <stdint.h>
#include <stdbool.h>

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/convolution.h"

typedef _Complex float cf_t; /* this is only a shortcut */

#define CONVOLUTION_FFT

#define PSS_LEN     62
#define PSS_RE      6*12


/* PSS processing options */

#define PSS_ACCUMULATE_ABS   // If enabled, accumulates the correlation absolute value on consecutive calls to pss_synch_find_pss

#define PSS_ABS_SQUARE   // If enabled, compute abs square, otherwise computes absolute value only 

#define PSS_RETURN_PSR  // If enabled returns peak to side-lobe ratio, otherwise returns absolute peak value

/**
 * The pss_synch_t object provides functions for fast computation of the crosscorrelation
 * between the PSS and received signal and CFO estimation. Also, the function pss_synch_periodic() is designed
 * to be called periodically every subframe, taking care of the correct data alignment with respect
 * to the PSS sequence.
 *
 * The object is designed to work with signals sampled at 1.92 Mhz centered at the carrier frequency.
 * Thus, downsampling is required if the signal is sampled at higher frequencies.
 *
 */


/* Low-level API */
typedef struct LIBLTE_API {
  
  dft_plan_t dftp_input; 
  
#ifdef CONVOLUTION_FFT
  conv_fft_cc_t conv_fft;
#endif

  uint32_t frame_size;
  uint32_t N_id_2;
  uint32_t fft_size;

  cf_t pss_signal_time[3][PSS_LEN];
  cf_t *pss_signal_freq[3]; // One sequence for each N_id_2
  cf_t *tmp_input;
  cf_t *conv_output;
  float *conv_output_abs;
  float ema_alpha; 
  float *conv_output_avg;
  float peak_value;
}pss_synch_t;

typedef enum { PSS_TX, PSS_RX } pss_direction_t;

/* Basic functionality */
LIBLTE_API int pss_synch_init_fft(pss_synch_t *q, 
                                  uint32_t frame_size, 
                                  uint32_t fft_size);

LIBLTE_API int pss_synch_init(pss_synch_t *q, 
                              uint32_t frame_size);

LIBLTE_API void pss_synch_free(pss_synch_t *q);

LIBLTE_API void pss_synch_reset(pss_synch_t *q); 

LIBLTE_API int pss_generate(cf_t *signal, 
                            uint32_t N_id_2);

LIBLTE_API void pss_put_slot(cf_t *pss_signal, 
                             cf_t *slot, 
                             uint32_t nof_prb, 
                             lte_cp_t cp);

LIBLTE_API void pss_synch_set_ema_alpha(pss_synch_t *q, 
                                        float alpha); 

LIBLTE_API int pss_synch_set_N_id_2(pss_synch_t *q, 
                                    uint32_t N_id_2);

LIBLTE_API int pss_synch_find_pss(pss_synch_t *q, 
                                  cf_t *input, 
                                  float *corr_peak_value);

LIBLTE_API int pss_synch_chest(pss_synch_t *q, 
                               cf_t *input, 
                               cf_t ce[PSS_LEN]); 

LIBLTE_API float pss_synch_cfo_compute(pss_synch_t* q, 
                                       cf_t *pss_recv);


/* High-level API */

typedef struct LIBLTE_API {
  pss_synch_t obj;
  struct pss_synch_init {
    int frame_size;        // if 0, 2048
    int unsync_nof_pkts;
    int N_id_2;
    int do_cfo;
  } init;
  cf_t *input;
  int in_len;
  struct pss_synch_ctrl_in {
    int correlation_threshold;
    float manual_cfo;
  } ctrl_in;
  cf_t *output;
  int out_len;
}pss_synch_hl;

#define DEFAULT_FRAME_SIZE    2048

LIBLTE_API int pss_synch_initialize(pss_synch_hl* h);
LIBLTE_API int pss_synch_work(pss_synch_hl* hl);
LIBLTE_API int pss_synch_stop(pss_synch_hl* hl);


#endif // PSS_
