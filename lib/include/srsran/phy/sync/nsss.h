/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 *  File:         nsss.h
 *
 *  Description:  Narrowband secondary synchronization signal (NSSS)
 *                generation and detection.
 *
 *
 *  Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.7.2
 *****************************************************************************/

#ifndef SRSRAN_NSSS_H
#define SRSRAN_NSSS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/utils/convolution.h"

#define SRSRAN_NSSS_NSYMB 11
#define SRSRAN_NSSS_NSC 12
#define SRSRAN_NSSS_LEN (SRSRAN_NSSS_NSYMB * SRSRAN_NSSS_NSC)
#define SRSRAN_NSSS_NUM_SEQ 4
#define SRSRAN_NSSS_TOT_LEN (SRSRAN_NSSS_NUM_SEQ * SRSRAN_NSSS_LEN)

#define SRSRAN_NSSS_CORR_FILTER_LEN 1508
#define SRSRAN_NSSS_CORR_OFFSET 412

#define SRSRAN_NSSS_PERIOD 2
#define SRSRAN_NSSS_NUM_SF_DETECT (SRSRAN_NSSS_PERIOD)

// b_q_m table from 3GPP TS 36.211 v13.2.0 table 10.2.7.2.1-1
static const int b_q_m[SRSRAN_NSSS_NUM_SEQ][128] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1, 1, 1, -1,
     1, -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1, 1, 1, -1,
     1, -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1, 1, 1, -1,
     1, -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1, 1, 1, -1},
    {1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1,
     -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,
     -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1,
     -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,
     1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1},
    {1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1,
     -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,
     -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,
     1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1,
     -1, 1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  1,  -1}};

/* Low-level API */
typedef struct SRSRAN_API {
  uint32_t             input_size;
  uint32_t             subframe_sz;
  uint32_t             fft_size, max_fft_size;
  srsran_conv_fft_cc_t conv_fft;

  cf_t*  nsss_signal_time[SRSRAN_NUM_PCI];
  cf_t*  tmp_input;
  cf_t*  conv_output;
  float* conv_output_abs;
  float  peak_values[SRSRAN_NUM_PCI];
  float  corr_peak_threshold;
} srsran_nsss_synch_t;

SRSRAN_API int srsran_nsss_synch_init(srsran_nsss_synch_t* q, uint32_t input_size, uint32_t fft_size);

SRSRAN_API void srsran_nsss_synch_free(srsran_nsss_synch_t* q);

SRSRAN_API int srsran_nsss_synch_resize(srsran_nsss_synch_t* q, uint32_t fft_size);

SRSRAN_API int srsran_nsss_sync_find(srsran_nsss_synch_t* q,
                                     cf_t*                input,
                                     float*               corr_peak_value,
                                     uint32_t*            cell_id,
                                     uint32_t*            sfn_partial);

void srsran_nsss_sync_find_pci(srsran_nsss_synch_t* q, cf_t* input, uint32_t cell_id);

SRSRAN_API int srsran_nsss_corr_init(srsran_nsss_synch_t* q);

SRSRAN_API void srsran_nsss_generate(cf_t* signal, uint32_t cell_id);

SRSRAN_API void srsran_nsss_put_subframe(srsran_nsss_synch_t* q,
                                         cf_t*                nsss,
                                         cf_t*                subframe,
                                         const int            nf,
                                         const uint32_t       nof_prb,
                                         const uint32_t       nbiot_prb_offset);

#endif // SRSRAN_NSSS_H