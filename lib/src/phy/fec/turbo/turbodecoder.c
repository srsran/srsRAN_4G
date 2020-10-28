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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/srslte.h"

#define debug_enabled 0

/* Generic (no SSE) implementation */
#include "srslte/phy/fec/turbodecoder_gen.h"
srslte_tdec_16bit_impl_t gen_impl = {tdec_gen_init,
                                     tdec_gen_free,
                                     tdec_gen_dec,
                                     tdec_gen_extract_input,
                                     tdec_gen_decision_byte};

/* SSE no-window implementation */
#ifdef LV_HAVE_SSE
#include "srslte/phy/fec/turbodecoder_sse.h"
srslte_tdec_16bit_impl_t sse_impl = {tdec_sse_init,
                                     tdec_sse_free,
                                     tdec_sse_dec,
                                     tdec_sse_extract_input,
                                     tdec_sse_decision_byte};

/* SSE window implementation */

#define WINIMP_IS_SSE16
#include "srslte/phy/fec/turbodecoder_win.h"
#undef WINIMP_IS_SSE16

srslte_tdec_16bit_impl_t sse16_win_impl = {tdec_winsse16_init,
                                           tdec_winsse16_free,
                                           tdec_winsse16_dec,
                                           tdec_winsse16_extract_input,
                                           tdec_winsse16_decision_byte};
#endif

/* AVX window implementation */
#ifdef LV_HAVE_AVX2
#define WINIMP_IS_AVX16
#include "srslte/phy/fec/turbodecoder_win.h"
#undef WINIMP_IS_AVX16
srslte_tdec_16bit_impl_t avx16_win_impl = {tdec_winavx16_init,
                                           tdec_winavx16_free,
                                           tdec_winavx16_dec,
                                           tdec_winavx16_extract_input,
                                           tdec_winavx16_decision_byte};
#endif

/* SSE window implementation */
#ifdef LV_HAVE_SSE
#define WINIMP_IS_SSE8
#include "srslte/phy/fec/turbodecoder_win.h"
#undef WINIMP_IS_SSE8

srslte_tdec_8bit_impl_t sse8_win_impl = {tdec_winsse8_init,
                                         tdec_winsse8_free,
                                         tdec_winsse8_dec,
                                         tdec_winsse8_extract_input,
                                         tdec_winsse8_decision_byte};
#endif

/* AVX window implementation */
#ifdef LV_HAVE_AVX2
#define WINIMP_IS_AVX8
#include "srslte/phy/fec/turbodecoder_win.h"
#undef WINIMP_IS_AVX8
srslte_tdec_8bit_impl_t avx8_win_impl = {tdec_winavx8_init,
                                         tdec_winavx8_free,
                                         tdec_winavx8_dec,
                                         tdec_winavx8_extract_input,
                                         tdec_winavx8_decision_byte};
#endif

#ifdef HAVE_NEON
#define WINIMP_IS_NEON16
#include "srslte/phy/fec/turbodecoder_win.h"
#undef WINIMP_IS_NEON16

srslte_tdec_16bit_impl_t arm16_win_impl = {tdec_winarm16_init,
                                           tdec_winarm16_free,
                                           tdec_winarm16_dec,
                                           tdec_winarm16_extract_input,
                                           tdec_winarm16_decision_byte};
#endif

#define AUTO_16_SSE 0
#define AUTO_16_SSEWIN 1
#define AUTO_16_AVXWIN 2
#define AUTO_8_SSEWIN 0
#define AUTO_8_AVXWIN 1
#define AUTO_16_GEN 0
#define AUTO_16_NEONWIN 1

// Include interfaces for 8 and 16 bit decoder implementations
#define LLR_IS_8BIT
#include "srslte/phy/fec/turbodecoder_iter.h"
#undef LLR_IS_8BIT

#define LLR_IS_16BIT
#include "srslte/phy/fec/turbodecoder_iter.h"
#undef LLR_IS_16BIT

int srslte_tdec_init(srslte_tdec_t* h, uint32_t max_long_cb)
{
  return srslte_tdec_init_manual(h, max_long_cb, SRSLTE_TDEC_AUTO);
}

uint32_t interleaver_idx(uint32_t nof_subblocks)
{
  switch (nof_subblocks) {
    case 32:
      return 3;
    case 16:
      return 2;
    case 8:
      return 1;
    case 1:
      return 0;
    default:
      return 0;
  }
}

/* Initializes the turbo decoder object */
int srslte_tdec_init_manual(srslte_tdec_t* h, uint32_t max_long_cb, srslte_tdec_impl_type_t dec_type)
{
  int ret = -1;
  bzero(h, sizeof(srslte_tdec_t));
  uint32_t len = max_long_cb + SRSLTE_TCOD_TOTALTAIL;

  h->dec_type = dec_type;

  // Set manual
  switch (dec_type) {
    case SRSLTE_TDEC_AUTO:
      break;
#ifdef LV_HAVE_SSE
    case SRSLTE_TDEC_SSE:
      h->dec16[0]         = &sse_impl;
      h->current_llr_type = SRSLTE_TDEC_16;
      break;
    case SRSLTE_TDEC_SSE_WINDOW:
      h->dec16[0]         = &sse16_win_impl;
      h->current_llr_type = SRSLTE_TDEC_16;
      break;
    case SRSLTE_TDEC_SSE8_WINDOW:
      h->dec8[0]          = &sse8_win_impl;
      h->current_llr_type = SRSLTE_TDEC_8;
      break;
#endif /* LV_HAVE_SSE */
#ifdef HAVE_NEON
    case SRSLTE_TDEC_NEON_WINDOW:
      h->dec16[0]         = &arm16_win_impl;
      h->current_llr_type = SRSLTE_TDEC_16;
      break;
#else  /* HAVE_NEON */
    case SRSLTE_TDEC_GENERIC:
      h->dec16[0]         = &gen_impl;
      h->current_llr_type = SRSLTE_TDEC_16;
      break;
#endif /* HAVE_NEON */
#ifdef LV_HAVE_AVX2
    case SRSLTE_TDEC_AVX_WINDOW:
      h->dec16[0]         = &avx16_win_impl;
      h->current_llr_type = SRSLTE_TDEC_16;
      break;
    case SRSLTE_TDEC_AVX8_WINDOW:
      h->dec8[0]          = &avx8_win_impl;
      h->current_llr_type = SRSLTE_TDEC_8;
      break;
#endif /* LV_HAVE_AVX2 */
    default:
      ERROR("Error decoder %d not supported\n", dec_type);
      goto clean_and_exit;
  }

  h->max_long_cb = max_long_cb;

  h->app1 = srslte_vec_i16_malloc(len);
  if (!h->app1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->app2 = srslte_vec_i16_malloc(len);
  if (!h->app2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->ext1 = srslte_vec_i16_malloc(len);
  if (!h->ext1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->ext2 = srslte_vec_i16_malloc(len);
  if (!h->ext2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->syst0 = srslte_vec_i16_malloc(len);
  if (!h->syst0) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity0 = srslte_vec_i16_malloc(len);
  if (!h->parity0) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity1 = srslte_vec_i16_malloc(len);
  if (!h->parity1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->input_conv = srslte_vec_i16_malloc(len * 3 + 32 * 3);
  if (!h->input_conv) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }

  if (dec_type == SRSLTE_TDEC_AUTO) {
#ifdef HAVE_NEON
    h->dec16[AUTO_16_GEN]     = &gen_impl;
    h->dec16[AUTO_16_NEONWIN] = &arm16_win_impl;
#elif LV_HAVE_SSE
    h->dec16[AUTO_16_SSE]    = &gen_impl;
    h->dec16[AUTO_16_SSEWIN] = &sse16_win_impl;
    h->dec8[AUTO_8_SSEWIN]   = &sse8_win_impl;
#ifdef LV_HAVE_AVX2
    h->dec16[AUTO_16_AVXWIN] = &avx16_win_impl;
    h->dec8[AUTO_8_AVXWIN]   = &avx8_win_impl;
#endif /* LV_HAVE_AVX2 */
#else  /* HAVE_NEON | LV_HAVE_SSE */
    h->dec16[AUTO_16_SSE]    = &gen_impl;
    h->dec16[AUTO_16_SSEWIN] = &gen_impl;
#endif /* HAVE_NEON | LV_HAVE_SSE */

    for (int td = 0; td < SRSLTE_TDEC_NOF_AUTO_MODES_16; td++) {
      if (h->dec16[td]) {
        if ((h->nof_blocks16[td] = h->dec16[td]->tdec_init(&h->dec16_hdlr[td], h->max_long_cb)) < 0) {
          goto clean_and_exit;
        }
      }
    }
    for (int td = 0; td < SRSLTE_TDEC_NOF_AUTO_MODES_8; td++) {
      if (h->dec8[td]) {
        if ((h->nof_blocks8[td] = h->dec8[td]->tdec_init(&h->dec8_hdlr[td], h->max_long_cb)) < 0) {
          goto clean_and_exit;
        }
      }
    }

    // Compute 1 interleaver for each possible nof_subblocks (1, 8, 16 or 32)
    for (int s = 0; s < 4; s++) {
      for (int i = 0; i < SRSLTE_NOF_TC_CB_SIZES; i++) {
        if (srslte_tc_interl_init(&h->interleaver[s][i], srslte_cbsegm_cbsize(i)) < 0) {
          goto clean_and_exit;
        }
        srslte_tc_interl_LTE_gen_interl(&h->interleaver[s][i], srslte_cbsegm_cbsize(i), s ? (8 << (s - 1)) : 1);
      }
    }
  } else {
    uint32_t nof_subblocks;
    if (dec_type < SRSLTE_TDEC_SSE8_WINDOW) {
      if ((h->nof_blocks16[0] = h->dec16[0]->tdec_init(&h->dec16_hdlr[0], h->max_long_cb)) < 0) {
        goto clean_and_exit;
      }
      nof_subblocks = h->nof_blocks16[0];
    } else {
      if ((h->nof_blocks8[0] = h->dec8[0]->tdec_init(&h->dec8_hdlr[0], h->max_long_cb)) < 0) {
        goto clean_and_exit;
      }
      nof_subblocks = h->nof_blocks8[0];
    }
    for (int i = 0; i < SRSLTE_NOF_TC_CB_SIZES; i++) {
      if (srslte_tc_interl_init(&h->interleaver[interleaver_idx(nof_subblocks)][i], srslte_cbsegm_cbsize(i)) < 0) {
        goto clean_and_exit;
      }
      srslte_tc_interl_LTE_gen_interl(
          &h->interleaver[interleaver_idx(nof_subblocks)][i], srslte_cbsegm_cbsize(i), nof_subblocks);
    }
  }

  h->current_cbidx = -1;
  ret              = 0;

clean_and_exit:
  if (ret == -1) {
    srslte_tdec_free(h);
  }
  return ret;
}

void srslte_tdec_free(srslte_tdec_t* h)
{
  if (h->app1) {
    free(h->app1);
  }
  if (h->app2) {
    free(h->app2);
  }
  if (h->ext1) {
    free(h->ext1);
  }
  if (h->ext2) {
    free(h->ext2);
  }
  if (h->syst0) {
    free(h->syst0);
  }
  if (h->parity0) {
    free(h->parity0);
  }
  if (h->parity1) {
    free(h->parity1);
  }
  if (h->input_conv) {
    free(h->input_conv);
  }

  for (int td = 0; td < SRSLTE_TDEC_NOF_AUTO_MODES_8; td++) {
    if (h->dec8[td] && h->dec8_hdlr[td]) {
      h->dec8[td]->tdec_free(h->dec8_hdlr[td]);
    }
  }
  for (int td = 0; td < SRSLTE_TDEC_NOF_AUTO_MODES_16; td++) {
    if (h->dec16[td] && h->dec16_hdlr[td]) {
      h->dec16[td]->tdec_free(h->dec16_hdlr[td]);
    }
  }
  for (int s = 0; s < 4; s++) {
    for (int i = 0; i < SRSLTE_NOF_TC_CB_SIZES; i++) {
      srslte_tc_interl_free(&h->interleaver[s][i]);
    }
  }

  bzero(h, sizeof(srslte_tdec_t));
}

void srslte_tdec_force_not_sb(srslte_tdec_t* h)
{
  h->force_not_sb = true;
}

static void tdec_decision_byte(srslte_tdec_t* h, uint8_t* output)
{
  if (h->current_llr_type == SRSLTE_TDEC_16) {
    h->dec16[h->current_dec]->tdec_decision_byte(!(h->n_iter % 2) ? h->app1 : h->ext1, output, h->current_long_cb);
  } else {
    h->dec8[h->current_dec]->tdec_decision_byte(
        !(h->n_iter % 2) ? (int8_t*)h->app1 : (int8_t*)h->ext1, output, h->current_long_cb);
  }
}

/* Returns number of subblocks in automatic mode for this long_cb */
uint32_t srslte_tdec_autoimp_get_subblocks(uint32_t long_cb)
{
#ifdef LV_HAVE_AVX2
  if (!(long_cb % 16) && long_cb > 800) {
    return 16;
  } else
#endif
      if (!(long_cb % 8) && long_cb > 400) {
    return 8;
  } else {
    return 0;
  }
}

static int tdec_sb_idx(uint32_t long_cb)
{
  uint32_t nof_sb = srslte_tdec_autoimp_get_subblocks(long_cb);
  switch (nof_sb) {
    case 16:
      return AUTO_16_AVXWIN;
    case 8:
      return AUTO_16_SSEWIN;
    case 0:
      return AUTO_16_SSE;
  }
  ERROR("Error in tdec_sb_idx() invalid nof_sb=%d\n", nof_sb);
  return 0;
}

uint32_t srslte_tdec_autoimp_get_subblocks_8bit(uint32_t long_cb)
{
#ifdef LV_HAVE_AVX2
  if (!(long_cb % 32) && long_cb > 2048) {
    return 32;
  } else
#endif
      if (!(long_cb % 16) && long_cb > 800) {
    return 16;
  } else if (!(long_cb % 8) && long_cb > 400) {
    return 8;
  } else {
    return 0;
  }
}

static int tdec_sb_idx_8(uint32_t long_cb)
{
  uint32_t nof_sb = srslte_tdec_autoimp_get_subblocks_8bit(long_cb);
  switch (nof_sb) {
    case 32:
      return AUTO_8_AVXWIN;
    case 16:
      return AUTO_8_SSEWIN;
    case 8:
      return 10 + AUTO_16_SSEWIN;
    case 0:
      return 10 + AUTO_16_SSE;
  }
  ERROR("Error in tdec_sb_idx_8() invalid nof_sb=%d\n", nof_sb);
  return 0;
}

// TODO: Implement SSE version. Don't really a problem since this only called at very low rates
static void convert_8_to_16(int8_t* in, int16_t* out, uint32_t len)
{
  for (int i = 0; i < len; i++) {
    out[i] = (int16_t)in[i];
  }
}

static void convert_16_to_8(int16_t* in, int8_t* out, uint32_t len)
{
  for (int i = 0; i < len; i++) {
    out[i] = (int8_t)in[i];
  }
}

static void tdec_iteration_8(srslte_tdec_t* h, int8_t* input)
{
  // Select decoder if in auto mode
  if (h->dec_type == SRSLTE_TDEC_AUTO) {
    h->current_llr_type  = SRSLTE_TDEC_8;
    h->current_dec       = tdec_sb_idx_8(h->current_long_cb);
    h->current_inter_idx = interleaver_idx(h->nof_blocks8[h->current_dec]);

    // If long_cb is not multiple of any 8-bit decoder, use a 16-bit decoder and do type conversion
    if (h->current_dec >= 10) {
      h->current_llr_type = SRSLTE_TDEC_16;
      h->current_dec -= 10;
      h->current_inter_idx = interleaver_idx(h->nof_blocks16[h->current_dec]);
    }
  } else {
    h->current_dec = 0;
  }

  if (h->current_llr_type == SRSLTE_TDEC_16) {
    if (!h->n_iter) {
      convert_8_to_16(input, h->input_conv, 3 * h->current_long_cb + 12);
    }
    run_tdec_iteration_16bit(h, h->input_conv);
  } else {
    run_tdec_iteration_8bit(h, input);
  }
}

static void tdec_iteration_16(srslte_tdec_t* h, int16_t* input)
{
  // Select decoder if in auto mode
  if (h->dec_type == SRSLTE_TDEC_AUTO) {
    h->current_llr_type = SRSLTE_TDEC_16;
    h->current_dec      = tdec_sb_idx(h->current_long_cb);
  } else {
    h->current_dec = 0;
  }
  h->current_inter_idx = interleaver_idx(h->nof_blocks16[h->current_dec]);

  if (h->current_llr_type == SRSLTE_TDEC_8) {

    h->current_inter_idx = interleaver_idx(h->nof_blocks8[h->current_dec]);

    if (!h->n_iter) {
      convert_16_to_8(input, h->input_conv, 3 * h->current_long_cb + 12);
    }
    run_tdec_iteration_8bit(h, h->input_conv);
  } else {
    run_tdec_iteration_16bit(h, input);
  }
}

/* Resets the decoder and sets the codeblock length */
int srslte_tdec_new_cb(srslte_tdec_t* h, uint32_t long_cb)
{
  if (long_cb > h->max_long_cb) {
    ERROR("TDEC was initialized for max_long_cb=%d\n", h->max_long_cb);
    return -1;
  }

  h->n_iter          = 0;
  h->current_long_cb = long_cb;
  h->current_cbidx   = srslte_cbsegm_cbindex(long_cb);
  if (h->current_cbidx < 0) {
    ERROR("Invalid CB length %d\n", long_cb);
    return -1;
  }
  return 0;
}

void srslte_tdec_iteration(srslte_tdec_t* h, int16_t* input, uint8_t* output)
{
  if (h->current_cbidx >= 0) {
    tdec_iteration_16(h, input);
    tdec_decision_byte(h, output);
  }
}

/* Runs nof_iterations iterations and decides the output bits */
int srslte_tdec_run_all(srslte_tdec_t* h, int16_t* input, uint8_t* output, uint32_t nof_iterations, uint32_t long_cb)
{
  if (srslte_tdec_new_cb(h, long_cb)) {
    return SRSLTE_ERROR;
  }

  do {
    tdec_iteration_16(h, input);
  } while (h->n_iter < nof_iterations);

  tdec_decision_byte(h, output);

  return SRSLTE_SUCCESS;
}

void srslte_tdec_iteration_8bit(srslte_tdec_t* h, int8_t* input, uint8_t* output)
{
  if (h->current_cbidx >= 0) {
    tdec_iteration_8(h, input);
    tdec_decision_byte(h, output);
  }
}

/* Runs nof_iterations iterations and decides the output bits */
int srslte_tdec_run_all_8bit(srslte_tdec_t* h,
                             int8_t*        input,
                             uint8_t*       output,
                             uint32_t       nof_iterations,
                             uint32_t       long_cb)
{
  if (srslte_tdec_new_cb(h, long_cb)) {
    return SRSLTE_ERROR;
  }

  do {
    tdec_iteration_8(h, input);
  } while (h->n_iter < nof_iterations);

  tdec_decision_byte(h, output);

  return SRSLTE_SUCCESS;
}

int srslte_tdec_get_nof_iterations(srslte_tdec_t* h)
{
  return h->n_iter;
}
