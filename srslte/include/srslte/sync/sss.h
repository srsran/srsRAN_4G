/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef SSS_
#define SSS_

#include <stdint.h>
#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/dft.h"

typedef _Complex float cf_t; /* this is only a shortcut */


#define N_SSS      31
#define SSS_LEN    2*N_SSS

struct sss_tables{
  int z1[N_SSS][N_SSS];
  int c[2][N_SSS];
  int s[N_SSS][N_SSS];
};

/* Allocate 32 complex to make it multiple of 32-byte AVX instructions alignment requirement.
 * Should use vec_malloc() to make it platform agnostic.
 */
struct fc_tables{
  float z1[N_SSS][N_SSS];
  float c[2][N_SSS];
  float s[N_SSS][N_SSS];
  float sd[N_SSS][N_SSS-1];
};


/* Low-level API */
typedef struct SRSLTE_API {

  dft_plan_t dftp_input;
  
  uint32_t fft_size;

  float corr_peak_threshold;
  uint32_t symbol_sz;
  uint32_t subframe_sz;
  uint32_t N_id_2;
  
  uint32_t N_id_1_table[30][30];
  struct fc_tables fc_tables[3]; // one for each N_id_2

  float corr_output_m0[N_SSS];
  float corr_output_m1[N_SSS];

}sss_synch_t;


/* Basic functionality */
SRSLTE_API int sss_synch_init(sss_synch_t *q,
                              uint32_t fft_size);

SRSLTE_API int sss_synch_realloc(sss_synch_t *q, 
                                 uint32_t fft_size); 

SRSLTE_API void sss_synch_free(sss_synch_t *q);

SRSLTE_API void sss_generate(float *signal0, 
                             float *signal5, 
                             uint32_t cell_id);

SRSLTE_API void sss_put_slot(float *sss, 
                             cf_t *symbol, 
                             uint32_t nof_prb, 
                             lte_cp_t cp);

SRSLTE_API int sss_synch_set_N_id_2(sss_synch_t *q, 
                                    uint32_t N_id_2);

SRSLTE_API int sss_synch_m0m1_partial(sss_synch_t *q, 
                                      cf_t *input, 
                                      uint32_t M, 
                                      cf_t ce[2*N_SSS],
                                      uint32_t *m0, 
                                      float *m0_value, 
                                      uint32_t *m1, 
                                      float *m1_value);

SRSLTE_API int sss_synch_m0m1_diff_coh(sss_synch_t *q, 
                                       cf_t *input, 
                                       cf_t ce[2*N_SSS],
                                       uint32_t *m0, 
                                       float *m0_value, 
                                       uint32_t *m1, 
                                       float *m1_value);

SRSLTE_API int sss_synch_m0m1_diff(sss_synch_t *q, 
                                   cf_t *input, 
                                   uint32_t *m0, 
                                   float *m0_value, 
                                   uint32_t *m1, 
                                   float *m1_value);


SRSLTE_API uint32_t sss_synch_subframe(uint32_t m0, 
                                       uint32_t m1);

SRSLTE_API int sss_synch_N_id_1(sss_synch_t *q, 
                                uint32_t m0, 
                                uint32_t m1);

SRSLTE_API int sss_synch_frame(sss_synch_t *q, 
                               cf_t *input, 
                               uint32_t *subframe_idx, 
                               uint32_t *N_id_1);

SRSLTE_API void sss_synch_set_threshold(sss_synch_t *q, 
                                        float threshold);

SRSLTE_API void sss_synch_set_symbol_sz(sss_synch_t *q, 
                                        uint32_t symbol_sz);

SRSLTE_API void sss_synch_set_subframe_sz(sss_synch_t *q, 
                                          uint32_t subframe_sz);


/* High-level API */

typedef struct SRSLTE_API {
  sss_synch_t obj;
  struct sss_synch_init {
    uint32_t N_id_2;
  } init;
  cf_t *input;
  uint32_t in_len;
  struct sss_synch_ctrl_in {
    uint32_t symbol_sz;
    uint32_t subframe_sz;
    uint32_t correlation_threshold;
  } ctrl_in;
  struct sss_synch_ctrl_out {
    uint32_t subframe_idx;
    uint32_t N_id_1;
  } ctrl_out;
}sss_synch_hl;

#define DEFAULT_FRAME_SIZE    2048

SRSLTE_API int sss_synch_initialize(sss_synch_hl* h);
SRSLTE_API int sss_synch_work(sss_synch_hl* hl);
SRSLTE_API int sss_synch_stop(sss_synch_hl* hl);

#endif // SSS_

