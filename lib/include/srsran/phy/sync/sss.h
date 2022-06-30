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

/******************************************************************************
 *  File:         sss.h
 *
 *  Description:  Secondary synchronization signal (SSS) generation and detection.
 *
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.11.2
 *****************************************************************************/

#ifndef SRSRAN_SSS_H
#define SRSRAN_SSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"

#define SRSRAN_SSS_N 31
#define SRSRAN_SSS_LEN 2 * SRSRAN_SSS_N

typedef struct SRSRAN_API {
  int z1[SRSRAN_SSS_N][SRSRAN_SSS_N];
  int c[2][SRSRAN_SSS_N];
  int s[SRSRAN_SSS_N][SRSRAN_SSS_N];
} srsran_sss_tables_t;

/* Allocate 32 complex to make it multiple of 32-byte AVX instructions alignment requirement.
 * Should use srsran_vec_malloc() to make it platform agnostic.
 */
typedef struct SRSRAN_API {
  float z1[SRSRAN_SSS_N][SRSRAN_SSS_N];
  float c[2][SRSRAN_SSS_N];
  float s[SRSRAN_SSS_N][SRSRAN_SSS_N];
  float sd[SRSRAN_SSS_N][SRSRAN_SSS_N - 1];
} srsran_sss_fc_tables_t;

/* Low-level API */
typedef struct SRSRAN_API {
  srsran_dft_plan_t dftp_input;

  uint32_t fft_size;
  uint32_t max_fft_size;

  float    corr_peak_threshold;
  uint32_t symbol_sz;
  uint32_t subframe_sz;
  uint32_t N_id_2;

  uint32_t               N_id_1_table[30][30];
  srsran_sss_fc_tables_t fc_tables[3]; // one for each N_id_2

  float corr_output_m0[SRSRAN_SSS_N];
  float corr_output_m1[SRSRAN_SSS_N];

} srsran_sss_t;

/* Basic functionality */
SRSRAN_API int srsran_sss_init(srsran_sss_t* q, uint32_t fft_size);

SRSRAN_API int srsran_sss_resize(srsran_sss_t* q, uint32_t fft_size);

SRSRAN_API void srsran_sss_free(srsran_sss_t* q);

SRSRAN_API void srsran_sss_generate(float* signal0, float* signal5, uint32_t cell_id);

SRSRAN_API void srsran_sss_put_slot(float* sss, cf_t* symbol, uint32_t nof_prb, srsran_cp_t cp);

SRSRAN_API void srsran_sss_put_symbol(float* sss, cf_t* symbol, uint32_t nof_prb);

SRSRAN_API int srsran_sss_set_N_id_2(srsran_sss_t* q, uint32_t N_id_2);

SRSRAN_API int srsran_sss_m0m1_partial(srsran_sss_t* q,
                                       const cf_t*   input,
                                       uint32_t      M,
                                       cf_t          ce[2 * SRSRAN_SSS_N],
                                       uint32_t*     m0,
                                       float*        m0_value,
                                       uint32_t*     m1,
                                       float*        m1_value);

SRSRAN_API int srsran_sss_m0m1_diff_coh(srsran_sss_t* q,
                                        const cf_t*   input,
                                        cf_t          ce[2 * SRSRAN_SSS_N],
                                        uint32_t*     m0,
                                        float*        m0_value,
                                        uint32_t*     m1,
                                        float*        m1_value);

SRSRAN_API int
srsran_sss_m0m1_diff(srsran_sss_t* q, const cf_t* input, uint32_t* m0, float* m0_value, uint32_t* m1, float* m1_value);

SRSRAN_API uint32_t srsran_sss_subframe(uint32_t m0, uint32_t m1);

SRSRAN_API int srsran_sss_N_id_1(srsran_sss_t* q, uint32_t m0, uint32_t m1, float corr);

SRSRAN_API int srsran_sss_frame(srsran_sss_t* q, cf_t* input, uint32_t* subframe_idx, uint32_t* N_id_1);

SRSRAN_API void srsran_sss_set_threshold(srsran_sss_t* q, float threshold);

SRSRAN_API void srsran_sss_set_symbol_sz(srsran_sss_t* q, uint32_t symbol_sz);

SRSRAN_API void srsran_sss_set_subframe_sz(srsran_sss_t* q, uint32_t subframe_sz);

#endif // SRSRAN_SSS_H
