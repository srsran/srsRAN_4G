/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         nsss.h
 *
 *  Description:  Narrowband secondary synchronization signal (NSSS)
 *                generation and detection.
 *
 *
 *  Reference:    3GPP TS 36.211 version 13.2.0 Release 13 Sec. 10.2.7.2
 *****************************************************************************/

#ifndef SRSLTE_NSSS_H
#define SRSLTE_NSSS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"
#include "srslte/phy/utils/convolution.h"

#define SRSLTE_NSSS_NSYMB 11
#define SRSLTE_NSSS_NSC 12
#define SRSLTE_NSSS_LEN (SRSLTE_NSSS_NSYMB * SRSLTE_NSSS_NSC)
#define SRSLTE_NSSS_NUM_SEQ 4
#define SRSLTE_NSSS_TOT_LEN (SRSLTE_NSSS_NUM_SEQ * SRSLTE_NSSS_LEN)

#define SRSLTE_NSSS_CORR_FILTER_LEN 1508
#define SRSLTE_NSSS_CORR_OFFSET 412

#define SRSLTE_NSSS_PERIOD 2
#define SRSLTE_NSSS_NUM_SF_DETECT (SRSLTE_NSSS_PERIOD)

// b_q_m table from 3GPP TS 36.211 v13.2.0 table 10.2.7.2.1-1
static const int b_q_m[SRSLTE_NSSS_NUM_SEQ][128] = {
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
typedef struct SRSLTE_API {
  uint32_t             input_size;
  uint32_t             subframe_sz;
  uint32_t             fft_size, max_fft_size;
  srslte_conv_fft_cc_t conv_fft;

  cf_t*  nsss_signal_time[SRSLTE_NUM_PCI];
  cf_t*  tmp_input;
  cf_t*  conv_output;
  float* conv_output_abs;
  float  peak_values[SRSLTE_NUM_PCI];
  float  corr_peak_threshold;
} srslte_nsss_synch_t;

SRSLTE_API int srslte_nsss_synch_init(srslte_nsss_synch_t* q, uint32_t input_size, uint32_t fft_size);

SRSLTE_API void srslte_nsss_synch_free(srslte_nsss_synch_t* q);

SRSLTE_API int srslte_nsss_synch_resize(srslte_nsss_synch_t* q, uint32_t fft_size);

SRSLTE_API int srslte_nsss_sync_find(srslte_nsss_synch_t* q,
                                     cf_t*                input,
                                     float*               corr_peak_value,
                                     uint32_t*            cell_id,
                                     uint32_t*            sfn_partial);

void srslte_nsss_sync_find_pci(srslte_nsss_synch_t* q, cf_t* input, uint32_t cell_id);

SRSLTE_API int srslte_nsss_corr_init(srslte_nsss_synch_t* q);

SRSLTE_API void srslte_nsss_generate(cf_t* signal, uint32_t cell_id);

SRSLTE_API void srslte_nsss_put_subframe(srslte_nsss_synch_t* q,
                                         cf_t*                nsss,
                                         cf_t*                subframe,
                                         const int            nf,
                                         const uint32_t       nof_prb,
                                         const uint32_t       nbiot_prb_offset);

#endif // SRSLTE_NSSS_H