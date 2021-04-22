/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/*!
 * \file ldpc_encoder.c
 * \brief Definition of the LDPC encoder.
 * \author David Gregoratti and Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include "../utils_avx2.h"
#include "../utils_avx512.h"
#include "ldpc_enc_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/ldpc/ldpc_encoder.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/*! Carries out the actual destruction of the memory allocated to the encoder. */
static void free_enc_c(void* o)
{
  srsran_ldpc_encoder_t* q = o;
  if (q->pcm) {
    free(q->pcm);
  }
  if (q->ptr) {
    free(q->ptr);
  }
}

/*! Carries out the actual encoding with a non-optimized encoder. */
static int encode_c(void* o, const uint8_t* input, uint8_t* output, uint32_t input_length, uint32_t cdwd_rm_length)
{
  srsran_ldpc_encoder_t* q = o;

  if (input_length / q->bgK != q->ls) {
    ERROR("Dimension mismatch.");
    return -1;
  }
  // it must be smaller than the codeword size
  if (cdwd_rm_length > q->liftN - 2 * q->ls) {
    cdwd_rm_length = q->liftN - 2 * q->ls;
  }
  // We need at least q->bgK + 4 variable nodes to cover the high-rate region. However,
  // 2 variable nodes are systematically punctured by the encoder.
  if (cdwd_rm_length < (q->bgK + 2) * q->ls) {
    // ERROR("The rate-matched codeword should have a length at least equal to the high-rate region.");
    cdwd_rm_length = (q->bgK + 2) * q->ls;
    // return -1;
  }
  if (cdwd_rm_length % q->ls) {
    cdwd_rm_length = (cdwd_rm_length / q->ls + 1) * q->ls;
    // ERROR("The rate-matched codeword length should be a multiple of the lifting size.");
    // return -1;
  }

  // systematic bits
  int skip_in = 2 * q->ls;
  for (int k = 0; k < (q->bgK - 2) * q->ls; k++) {
    output[k] = input[skip_in + k];
  }

  preprocess_systematic_bits(q, input);

  q->encode_high_rate(q, output);

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  encode_ext_region(q, output, n_layers);

  return 0;
}

/*! Initializes a non-optimized encoder. */
static int init_c(srsran_ldpc_encoder_t* q)
{
  int ls_index = get_ls_index(q->ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", q->ls);
    return -1;
  }

  if (q->bg == BG1 && ls_index != 6) {
    q->encode_high_rate = encode_high_rate_case1;
  } else if (q->bg == BG1 && ls_index == 6) {
    q->encode_high_rate = encode_high_rate_case2;
  } else if (q->bg == BG2 && ls_index != 3 && ls_index != 7) {
    q->encode_high_rate = encode_high_rate_case3;
  } else if (q->bg == BG2 && (ls_index == 3 || ls_index == 7)) {
    q->encode_high_rate = encode_high_rate_case4;
  } else {
    ERROR("Invalid lifting size %d and/or Base Graph %d", q->ls, q->bg + 1);
    return -1;
  }

  q->free = free_enc_c;

  q->ptr = srsran_vec_u8_malloc(q->bgM * q->ls);
  if (!q->ptr) {
    perror("malloc");
    free_enc_c(q);
    return -1;
  }

  q->encode = encode_c;

  return 0;
}

#ifdef LV_HAVE_AVX2
/*! Carries out the actual destruction of the memory allocated to the encoder. */
static void free_enc_avx2(void* o)
{
  srsran_ldpc_encoder_t* q = o;
  if (q->pcm) {
    free(q->pcm);
  }
  if (q->ptr) {
    delete_ldpc_enc_avx2(q->ptr);
  }
}

/*! Carries out the actual encoding with an optimized encoder. */
static int encode_avx2(void* o, const uint8_t* input, uint8_t* output, uint32_t input_length, uint32_t cdwd_rm_length)
{
  srsran_ldpc_encoder_t* q = o;

  if (input_length / q->bgK != q->ls) {
    ERROR("Dimension mismatch.");
    return -1;
  }

  // it must be smaller than the codeword size
  if (cdwd_rm_length > q->liftN - 2 * q->ls) {
    cdwd_rm_length = q->liftN - 2 * q->ls;
  }
  // We need at least q->bgK + 4 variable nodes to cover the high-rate region. However,
  // 2 variable nodes are systematically punctured by the encoder.
  if (cdwd_rm_length < (q->bgK + 2) * q->ls) {
    // ERROR("The rate-matched codeword should have a length at least equal to the high-rate region.");
    cdwd_rm_length = (q->bgK + 2) * q->ls;
    // return -1;
  }
  if (cdwd_rm_length % q->ls) {
    cdwd_rm_length = (cdwd_rm_length / q->ls + 1) * q->ls;
    // ERROR("The rate-matched codeword length should be a multiple of the lifting size.");
    // return -1;
  }

  load_avx2(q->ptr, input, q->bgK, q->bgN, q->ls);

  preprocess_systematic_bits_avx2(q);

  q->encode_high_rate_avx2(q);

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  encode_ext_region_avx2(q, n_layers);

  return_codeword_avx2(q->ptr, output, n_layers + q->bgK, q->ls);

  return 0;
}

/*! Initializes an optimized encoder. */
static int init_avx2(srsran_ldpc_encoder_t* q)
{
  int ls_index = get_ls_index(q->ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", q->ls);
    return -1;
  }

  if (q->bg == BG1 && ls_index != 6) {
    q->encode_high_rate_avx2 = encode_high_rate_case1_avx2;
  } else if (q->bg == BG1 && ls_index == 6) {
    q->encode_high_rate_avx2 = encode_high_rate_case2_avx2;
  } else if (q->bg == BG2 && ls_index != 3 && ls_index != 7) {
    q->encode_high_rate_avx2 = encode_high_rate_case3_avx2;
  } else if (q->bg == BG2 && (ls_index == 3 || ls_index == 7)) {
    q->encode_high_rate_avx2 = encode_high_rate_case4_avx2;
  } else {
    ERROR("Invalid lifting size %d and/or Base Graph %d", q->ls, q->bg + 1);
    return -1;
  }

  q->free = free_enc_avx2;

  if ((q->ptr = create_ldpc_enc_avx2(q)) == NULL) {
    ERROR("Create_ldpc_enc");
    free_enc_avx2(q);
    return -1;
  }

  q->encode = encode_avx2;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the encoder. */
static void free_enc_avx2long(void* o)
{
  srsran_ldpc_encoder_t* q = o;
  if (q->pcm) {
    free(q->pcm);
  }
  if (q->ptr) {
    delete_ldpc_enc_avx2long(q->ptr);
  }
}

/*! Carries out the actual encoding with an optimized encoder. */
static int
encode_avx2long(void* o, const uint8_t* input, uint8_t* output, uint32_t input_length, uint32_t cdwd_rm_length)
{
  srsran_ldpc_encoder_t* q = o;

  if (input_length / q->bgK != q->ls) {
    ERROR("Dimension mismatch.");
    return -1;
  }

  // it must be smaller than the codeword size
  if (cdwd_rm_length > q->liftN - 2 * q->ls) {
    cdwd_rm_length = q->liftN - 2 * q->ls;
  }
  // We need at least q->bgK + 4 variable nodes to cover the high-rate region. However,
  // 2 variable nodes are systematically punctured by the encoder.
  if (cdwd_rm_length < (q->bgK + 2) * q->ls) {
    // ERROR("The rate-matched codeword should have a length at least equal to the high-rate region.");
    cdwd_rm_length = (q->bgK + 2) * q->ls;
    // return -1;
  }
  if (cdwd_rm_length % q->ls) {
    cdwd_rm_length = (cdwd_rm_length / q->ls + 1) * q->ls;
    // ERROR("The rate-matched codeword length should be a multiple of the lifting size.");
    // return -1;
  }
  load_avx2long(q->ptr, input, q->bgK, q->bgN, q->ls);

  preprocess_systematic_bits_avx2long(q);

  q->encode_high_rate_avx2(q);

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  encode_ext_region_avx2long(q, n_layers);

  return_codeword_avx2long(q->ptr, output, n_layers + q->bgK, q->ls);

  return 0;
}

/*! Initializes an optimized encoder. */
static int init_avx2long(srsran_ldpc_encoder_t* q)
{
  int ls_index = get_ls_index(q->ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", q->ls);
    return -1;
  }

  if (q->bg == BG1 && ls_index != 6) {
    q->encode_high_rate_avx2 = encode_high_rate_case1_avx2long;
  } else if (q->bg == BG1 && ls_index == 6) {
    q->encode_high_rate_avx2 = encode_high_rate_case2_avx2long;
  } else if (q->bg == BG2 && ls_index != 3 && ls_index != 7) {
    q->encode_high_rate_avx2 = encode_high_rate_case3_avx2long;
  } else if (q->bg == BG2 && (ls_index == 3 || ls_index == 7)) {
    q->encode_high_rate_avx2 = encode_high_rate_case4_avx2long;
  } else {
    ERROR("Invalid lifting size %d and/or Base Graph %d", q->ls, q->bg + 1);
    return -1;
  }

  q->free = free_enc_avx2long;

  if ((q->ptr = create_ldpc_enc_avx2long(q)) == NULL) {
    ERROR("Create_ldpc_enc");
    free_enc_avx2long(q);
    return -1;
  }

  q->encode = encode_avx2long;

  return 0;
}

#endif

#ifdef LV_HAVE_AVX512

/*! Carries out the actual destruction of the memory allocated to the encoder. */
static void free_enc_avx512(void* o)
{
  srsran_ldpc_encoder_t* q = o;
  if (q->pcm) {
    free(q->pcm);
  }
  if (q->ptr) {
    delete_ldpc_enc_avx512(q->ptr);
  }
}

/*! Carries out the actual encoding with an optimized encoder. */
static int encode_avx512(void* o, const uint8_t* input, uint8_t* output, uint32_t input_length, uint32_t cdwd_rm_length)
{
  srsran_ldpc_encoder_t* q = o;

  if (input_length / q->bgK != q->ls) {
    ERROR("Dimension mismatch.");
    return -1;
  }

  // it must be smaller than the codeword size
  if (cdwd_rm_length > q->liftN - 2 * q->ls) {
    cdwd_rm_length = q->liftN - 2 * q->ls;
  }
  // We need at least q->bgK + 4 variable nodes to cover the high-rate region. However,
  // 2 variable nodes are systematically punctured by the encoder.
  if (cdwd_rm_length < (q->bgK + 2) * q->ls) {
    // ERROR("The rate-matched codeword should have a length at least equal to the high-rate region.");
    cdwd_rm_length = (q->bgK + 2) * q->ls;
    // return -1;
  }
  if (cdwd_rm_length % q->ls) {
    cdwd_rm_length = (cdwd_rm_length / q->ls + 1) * q->ls;
    // ERROR("The rate-matched codeword length should be a multiple of the lifting size.");
    // return -1;
  }

  load_avx512(q->ptr, input, q->bgK, q->bgN, q->ls);

  preprocess_systematic_bits_avx512(q);

  q->encode_high_rate_avx512(q);

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  encode_ext_region_avx512(q, n_layers);

  return_codeword_avx512(q->ptr, output, n_layers + q->bgK, q->ls);

  return 0;
}

/*! Initializes an optimized encoder. */
static int init_avx512(srsran_ldpc_encoder_t* q)
{
  int ls_index = get_ls_index(q->ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", q->ls);
    return -1;
  }

  if (q->bg == BG1 && ls_index != 6) {
    q->encode_high_rate_avx512 = encode_high_rate_case1_avx512;
  } else if (q->bg == BG1 && ls_index == 6) {
    q->encode_high_rate_avx512 = encode_high_rate_case2_avx512;
  } else if (q->bg == BG2 && ls_index != 3 && ls_index != 7) {
    q->encode_high_rate_avx512 = encode_high_rate_case3_avx512;
  } else if (q->bg == BG2 && (ls_index == 3 || ls_index == 7)) {
    q->encode_high_rate_avx512 = encode_high_rate_case4_avx512;
  } else {
    ERROR("Invalid lifting size %d and/or Base Graph %d", q->ls, q->bg + 1);
    return -1;
  }

  q->free = free_enc_avx512;

  if ((q->ptr = create_ldpc_enc_avx512(q)) == NULL) {
    ERROR("Create_ldpc_enc");
    free_enc_avx512(q);
    return -1;
  }

  q->encode = encode_avx512;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the encoder. */
static void free_enc_avx512long(void* o)
{
  srsran_ldpc_encoder_t* q = o;
  if (q->pcm) {
    free(q->pcm);
  }
  if (q->ptr) {
    delete_ldpc_enc_avx512long(q->ptr);
  }
}

/*! Carries out the actual encoding with an optimized encoder. */
static int
encode_avx512long(void* o, const uint8_t* input, uint8_t* output, uint32_t input_length, uint32_t cdwd_rm_length)
{
  srsran_ldpc_encoder_t* q = o;

  if (input_length / q->bgK != q->ls) {
    ERROR("Dimension mismatch.");
    return -1;
  }

  // it must be smaller than the codeword size
  if (cdwd_rm_length > q->liftN - 2 * q->ls) {
    cdwd_rm_length = q->liftN - 2 * q->ls;
  }
  // We need at least q->bgK + 4 variable nodes to cover the high-rate region. However,
  // 2 variable nodes are systematically punctured by the encoder.
  if (cdwd_rm_length < (q->bgK + 2) * q->ls) {
    // ERROR("The rate-matched codeword should have a length at least equal to the high-rate region.");
    cdwd_rm_length = (q->bgK + 2) * q->ls;
    // return -1;
  }
  if (cdwd_rm_length % q->ls) {
    cdwd_rm_length = (cdwd_rm_length / q->ls + 1) * q->ls;
    // ERROR("The rate-matched codeword length should be a multiple of the lifting size.");
    // return -1;
  }
  load_avx512long(q->ptr, input, q->bgK, q->bgN, q->ls);

  preprocess_systematic_bits_avx512long(q);

  q->encode_high_rate_avx512(q);

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  encode_ext_region_avx512long(q, n_layers);

  return_codeword_avx512long(q->ptr, output, n_layers + q->bgK, q->ls);

  return 0;
}

/*! Initializes an optimized encoder. */
static int init_avx512long(srsran_ldpc_encoder_t* q)
{
  int ls_index = get_ls_index(q->ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", q->ls);
    return -1;
  }

  if (q->bg == BG1 && ls_index != 6) {
    q->encode_high_rate_avx512 = encode_high_rate_case1_avx512long;
  } else if (q->bg == BG1 && ls_index == 6) {
    q->encode_high_rate_avx512 = encode_high_rate_case2_avx512long;
  } else if (q->bg == BG2 && ls_index != 3 && ls_index != 7) {
    q->encode_high_rate_avx512 = encode_high_rate_case3_avx512long;
  } else if (q->bg == BG2 && (ls_index == 3 || ls_index == 7)) {
    q->encode_high_rate_avx512 = encode_high_rate_case4_avx512long;
  } else {
    ERROR("Invalid lifting size %d and/or Base Graph %d", q->ls, q->bg + 1);
    return -1;
  }

  q->free = free_enc_avx512long;

  if ((q->ptr = create_ldpc_enc_avx512long(q)) == NULL) {
    ERROR("Create_ldpc_enc");
    free_enc_avx512long(q);
    return -1;
  }

  q->encode = encode_avx512long;

  return 0;
}

#endif

int srsran_ldpc_encoder_init(srsran_ldpc_encoder_t*     q,
                             srsran_ldpc_encoder_type_t type,
                             srsran_basegraph_t         bg,
                             uint16_t                   ls)
{
  switch (bg) {
    case BG1:
      q->bgN = BG1Nfull;
      q->bgM = BG1M;
      break;
    case BG2:
      q->bgN = BG2Nfull;
      q->bgM = BG2M;
      break;
    default:
      ERROR("Base Graph BG%d does not exist", bg + 1);
      return -1;
  }
  q->bg  = bg;
  q->bgK = q->bgN - q->bgM;

  q->ls    = ls;
  q->liftK = ls * q->bgK;
  q->liftM = ls * q->bgM;
  q->liftN = ls * q->bgN;

  q->pcm = srsran_vec_u16_malloc(q->bgM * q->bgN);
  if (!q->pcm) {
    perror("malloc");
    return -1;
  }
  if (create_compact_pcm(q->pcm, NULL, q->bg, q->ls) != 0) {
    perror("Create PCM");
    return -1;
  }

  switch (type) {
    case SRSRAN_LDPC_ENCODER_C:
      return init_c(q);
#ifdef LV_HAVE_AVX2
    case SRSRAN_LDPC_ENCODER_AVX2:
      if (ls <= SRSRAN_AVX2_B_SIZE) {
        return init_avx2(q);
      } else {
        return init_avx2long(q);
      }
#endif // LV_HAVE_AVX2
#ifdef LV_HAVE_AVX512
    case SRSRAN_LDPC_ENCODER_AVX512:
      if (ls <= SRSRAN_AVX512_B_SIZE) {
        return init_avx512(q);
      } else {
        return init_avx512long(q);
      }
#endif // LV_HAVE_AVX512
    default:
      return -1;
  }
}

void srsran_ldpc_encoder_free(srsran_ldpc_encoder_t* q)
{
  if (q->free) {
    q->free(q);
  }
  bzero(q, sizeof(srsran_ldpc_encoder_t));
}

int srsran_ldpc_encoder_encode(srsran_ldpc_encoder_t* q, const uint8_t* input, uint8_t* output, uint32_t input_length)
{
  return q->encode(q, input, output, input_length, q->liftN - 2 * q->ls);
}

int srsran_ldpc_encoder_encode_rm(srsran_ldpc_encoder_t* q,
                                  const uint8_t*         input,
                                  uint8_t*               output,
                                  uint32_t               input_length,
                                  uint32_t               cdwd_rm_length)
{
  return q->encode(q, input, output, input_length, cdwd_rm_length);
}
