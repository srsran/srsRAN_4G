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

#include "srslte/config.h"

#define MAKE_CALL(a) CONCAT2(a, type_name)
#define MAKE_VEC(a) CONCAT2(a, vec_suffix)
#define PRINT CONCAT2(srslte_vec_fprint, print_suffix)

#ifdef LLR_IS_8BIT
#define llr_t int8_t
#define type_name _8bit
#define vec_suffix _bbb
#define print_suffix _bs
#define decptr h->dec8[h->current_dec]
#define dechdlr h->dec8_hdlr[h->current_dec]
#define input_is_interleaved 1
#else
#ifdef LLR_IS_16BIT
#define llr_t int16_t
#define vec_suffix _sss
#define print_suffix _s
#define decptr h->dec16[h->current_dec]
#define dechdlr h->dec16_hdlr[h->current_dec]
#define input_is_interleaved (h->current_dec > 0)
#define type_name _16bit
#else
#pragma message "Unsupported LLR mode"
#endif
#endif

#define debug_enabled_iter 0
#define debug_len 20

#define debug_vec(a)                                                                                                   \
  if (debug_enabled_iter) {                                                                                            \
    printf("%s it=%d: ", STRING(a), n_iter);                                                                           \
    PRINT(stdout, a, debug_len);                                                                                       \
  }

static void MAKE_CALL(
    extract_input_tail_sb)(llr_t* input, llr_t* syst, llr_t* app2, llr_t* parity0, llr_t* parity1, uint32_t long_cb)
{
  for (int i = long_cb; i < long_cb + 3; i++) {
    syst[i]    = input[3 * (long_cb + 32) + 2 * (i - long_cb)];
    parity0[i] = input[3 * (long_cb + 32) + 2 * (i - long_cb) + 1];

    app2[i]    = input[3 * (long_cb + 32) + 6 + 2 * (i - long_cb)];
    parity1[i] = input[3 * (long_cb + 32) + 6 + 2 * (i - long_cb) + 1];
  }
}

/* Runs 1 turbo decoder iteration */
void MAKE_CALL(run_tdec_iteration)(srslte_tdec_t* h, llr_t* input)
{

  if (h->current_cbidx >= 0) {
    uint16_t* inter   = h->interleaver[h->current_inter_idx][h->current_cbidx].forward;
    uint16_t* deinter = h->interleaver[h->current_inter_idx][h->current_cbidx].reverse;
    llr_t*    syst    = (llr_t*)h->syst0;
    llr_t*    parity0 = (llr_t*)h->parity0;
    llr_t*    parity1 = (llr_t*)h->parity1;

    llr_t* app1 = (llr_t*)h->app1;
    llr_t* app2 = (llr_t*)h->app2;
    llr_t* ext1 = (llr_t*)h->ext1;
    llr_t* ext2 = (llr_t*)h->ext2;

    uint32_t long_cb = h->current_long_cb;
    uint32_t n_iter  = h->n_iter;

    if (SRSLTE_TDEC_EXPECT_INPUT_SB && !h->force_not_sb && input_is_interleaved) {
      syst = input;
      // align to 32 bytes (warning: must be same alignment as in rm_turbo.c)
      parity0 = &input[long_cb + 32];
      parity1 = &input[2 * (long_cb + 32)];
      if (n_iter == 0) {
        MAKE_CALL(extract_input_tail_sb)(input, syst, app2, parity0, parity1, long_cb);
      }
    } else {
      if (n_iter == 0) {
        decptr->tdec_extract_input(input, syst, app2, parity0, parity1, long_cb);
      }
    }

    if ((n_iter % 2) == 0) {

      // Add apriori information to decoder 1
      if (n_iter) {
        MAKE_VEC(srslte_vec_sub)(app1, ext1, app1, long_cb);
      }

      // Run MAP DEC #1
      decptr->tdec_dec(dechdlr, syst, n_iter ? app1 : NULL, parity0, ext1, long_cb);
    }
    // Interleave extrinsic output of DEC1 to form apriori info for decoder 2
    if (n_iter % 2) {
      // Convert aposteriori information into extrinsic information
      if (n_iter > 1) {
        MAKE_VEC(srslte_vec_sub)(ext1, app1, ext1, long_cb);
      }

      MAKE_VEC(srslte_vec_lut)(ext1, deinter, app2, long_cb);

      // Run MAP DEC #2. 2nd decoder uses apriori information as systematic bits
      decptr->tdec_dec(dechdlr, app2, NULL, parity1, ext2, long_cb);

      // Deinterleaved extrinsic bits become apriori info for decoder 1
      MAKE_VEC(srslte_vec_lut)(ext2, inter, app1, long_cb);
    }

    if (h->n_iter == 0) {
      debug_vec(syst);
      debug_vec(parity0);
      debug_vec(parity1);
    }
    debug_vec(ext1);
    debug_vec(ext2);
    debug_vec(app1);
    debug_vec(app2);

    h->n_iter++;
  } else {
    ERROR("Error CB index not set (call srslte_tdec_new_cb() first\n");
  }
}

#undef debug_enabled
#undef debug_len
#undef debug_vec
#undef llr_t
#undef vec_suffix
#undef print_suffix
#undef decptr
#undef dechdlr
#undef type_name
#undef input_is_interleaved