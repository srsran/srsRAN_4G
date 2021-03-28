/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*!
 * \file ldpc_decoder.c
 * \brief Definition of the LDPC decoder.
 * \author David Gregoratti
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <stdint.h>

#include "../utils_avx2.h"
#include "../utils_avx512.h"
#include "ldpc_dec_all.h"
#include "srsran/phy/fec/ldpc/base_graph.h"
#include "srsran/phy/fec/ldpc/ldpc_decoder.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define MAX_ITERATIONS 10 /*!< \brief Iterations of the BP algorithm. */

/*! Carries out the actual destruction of the memory allocated to the decoder, float-LLR case. */
static void free_dec_f(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_f(q->ptr);
}

/*! Carries out the decoding with real-valued LLRs. */
static int decode_f(void* o, const float* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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

  init_ldpc_dec_f(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_f(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_f(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_f(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_f(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with real valued LLRs. */
static int init_f(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_f;

  if ((q->ptr = create_ldpc_dec_f(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_f(q);
    return -1;
  }

  q->decode_f = decode_f;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 16-bit-LLR case. */
static void free_dec_s(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_s(q->ptr);
}

/*! Carries out the decoding with 16-bit integer-valued LLRs. */
static int decode_s(void* o, const int16_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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

  init_ldpc_dec_s(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_s(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_s(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_s(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_s(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 16-bit integer-valued LLRs. */
static int init_s(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_s;

  if ((q->ptr = create_ldpc_dec_s(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_s(q);
    return -1;
  }

  q->decode_s = decode_s;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case. */
static void free_dec_c(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs. */
static int decode_c(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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

  init_ldpc_dec_c(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_c(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_c(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs. */
static int init_c(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c;

  if ((q->ptr = create_ldpc_dec_c(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c(q);
    return -1;
  }

  q->decode_c = decode_c;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR flooded case. */
static void free_dec_c_flood(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_flood(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs, flooded scheduling. */
static int decode_c_flood(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_flood(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < 2 * MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_flood(q->ptr, i_layer);
    }

    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_flood(q->ptr, i_layer, this_pcm, these_var_indices);
    }
    update_ldpc_soft_bits_c_flood(q->ptr, q->var_indices);
  }

  extract_ldpc_message_c_flood(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs. */
static int init_c_flood(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_flood;

  if ((q->ptr = create_ldpc_dec_c_flood(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_flood(q);
    return -1;
  }

  q->decode_c = decode_c_flood;

  return 0;
}

#ifdef LV_HAVE_AVX2
/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case (AVX2 implementation). */
static void free_dec_c_avx2(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx2(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (AVX2 implementation). */
static int decode_c_avx2(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx2(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx2(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx2(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_c_avx2(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_c_avx2(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs (AVX2 implementation). */
static int init_c_avx2(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx2;

  if ((q->ptr = create_ldpc_dec_c_avx2(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx2(q);
    return -1;
  }

  q->decode_c = decode_c_avx2;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case (AVX2 implementation,
 * large lifting size). */
static void free_dec_c_avx2long(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx2long(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (AVX2 implementation, large lifting size). */
static int decode_c_avx2long(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx2long(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx2long(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx2long(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_c_avx2long(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_c_avx2long(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs (AVX2 implementation, large lifting size). */
static int init_c_avx2long(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx2long;

  if ((q->ptr = create_ldpc_dec_c_avx2long(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx2long(q);
    return -1;
  }

  q->decode_c = decode_c_avx2long;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case (AVX2 implementation,
 * flooded scheduling). */
static void free_dec_c_avx2_flood(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx2_flood(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (AVX2 implementation, flooded scheduling). */
static int decode_c_avx2_flood(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx2_flood(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < 2 * MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx2_flood(q->ptr, i_layer);
    }

    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx2_flood(q->ptr, i_layer, this_pcm, these_var_indices);
    }
    update_ldpc_soft_bits_c_avx2_flood(q->ptr, q->var_indices);
  }

  extract_ldpc_message_c_avx2_flood(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs (AVX2 implementation, flooded scheduling). */
static int init_c_avx2_flood(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx2_flood;

  if ((q->ptr = create_ldpc_dec_c_avx2_flood(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx2_flood(q);
    return -1;
  }

  q->decode_c = decode_c_avx2_flood;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case
 * (flooded scheduling, AVX2 implementation, large lifting size). */
static void free_dec_c_avx2long_flood(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx2long_flood(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (flooded scheduling, AVX2 implementation, large lifting
 * size). */
static int decode_c_avx2long_flood(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx2long_flood(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < 2 * MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx2long_flood(q->ptr, i_layer);
    }

    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx2long_flood(q->ptr, i_layer, this_pcm, these_var_indices);
    }

    update_ldpc_soft_bits_c_avx2long_flood(q->ptr, q->var_indices);
  }

  extract_ldpc_message_c_avx2long_flood(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs
 * (flooded scheduling, AVX2 implementation, large lifting size). */
static int init_c_avx2long_flood(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx2long_flood;

  if ((q->ptr = create_ldpc_dec_c_avx2long_flood(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx2long(q);
    return -1;
  }

  q->decode_c = decode_c_avx2long_flood;

  return 0;
}
#endif // LV_HAVE_AVX2

// AVX512 Declarations

#ifdef LV_HAVE_AVX512

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case (AVX512 implementation).
 */
static void free_dec_c_avx512(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx512(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (AVX512 implementation). */
static int decode_c_avx512(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx512(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx512(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx512(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_c_avx512(q->ptr, i_layer, these_var_indices);
    }
  }

  extract_ldpc_message_c_avx512(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs (AVX512 implementation). */
static int init_c_avx512(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx512;

  if ((q->ptr = create_ldpc_dec_c_avx512(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx512(q);
    return -1;
  }

  q->decode_c = decode_c_avx512;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case (AVX512 implementation,
 * large lifting size). */
static void free_dec_c_avx512long(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx512long(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (AVX512 implementation, large lifting size). */
static int decode_c_avx512long(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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

  init_ldpc_dec_c_avx512long(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx512long(q->ptr, i_layer);

      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx512long(q->ptr, i_layer, this_pcm, these_var_indices);

      update_ldpc_soft_bits_c_avx512long(q->ptr, i_layer, these_var_indices);
    }
  }
  extract_ldpc_message_c_avx512long(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs (AVX512 implementation, large lifting size). */
static int init_c_avx512long(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx512long;

  if ((q->ptr = create_ldpc_dec_c_avx512long(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx512long(q);
    return -1;
  }

  q->decode_c = decode_c_avx512long;

  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the decoder, 8-bit-LLR case
 * (flooded scheduling, AVX512 implementation, large lifting size). */
static void free_dec_c_avx512long_flood(void* o)
{
  srsran_ldpc_decoder_t* q = o;
  if (q->var_indices) {
    free(q->var_indices);
  }
  if (q->pcm) {
    free(q->pcm);
  }
  delete_ldpc_dec_c_avx512long_flood(q->ptr);
}

/*! Carries out the decoding with 8-bit integer-valued LLRs (flooded scheduling, AVX512 implementation, large lifting
 * size). */
static int decode_c_avx512long_flood(void* o, const int8_t* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  srsran_ldpc_decoder_t* q = o;

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
  init_ldpc_dec_c_avx512long_flood(q->ptr, llrs, q->ls);

  uint16_t* this_pcm                   = NULL;
  int8_t(*these_var_indices)[MAX_CNCT] = NULL;

  // When computing the number of layers, we need to recall that the standard always removes
  // the first two variable nodes from the final codeword.
  uint8_t n_layers = cdwd_rm_length / q->ls - q->bgK + 2;

  for (int i_iteration = 0; i_iteration < 2 * MAX_ITERATIONS; i_iteration++) {
    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      update_ldpc_var_to_check_c_avx512long_flood(q->ptr, i_layer);
    }

    for (int i_layer = 0; i_layer < n_layers; i_layer++) {
      this_pcm          = q->pcm + i_layer * q->bgN;
      these_var_indices = q->var_indices + i_layer;

      update_ldpc_check_to_var_c_avx512long_flood(q->ptr, i_layer, this_pcm, these_var_indices);
    }

    update_ldpc_soft_bits_c_avx512long_flood(q->ptr, q->var_indices);
  }

  extract_ldpc_message_c_avx512long_flood(q->ptr, message, q->liftK);

  return 0;
}

/*! Initializes the decoder to work with 8-bit integer-valued LLRs
 * (flooded scheduling, AVX512 implementation, large lifting size). */
static int init_c_avx512long_flood(srsran_ldpc_decoder_t* q)
{
  q->free = free_dec_c_avx512long_flood;

  if ((q->ptr = create_ldpc_dec_c_avx512long_flood(q->bgN, q->bgM, q->ls, q->scaling_fctr)) == NULL) {
    ERROR("Create_ldpc_dec failed");
    free_dec_c_avx512long_flood(q);
    return -1;
  }

  q->decode_c = decode_c_avx512long_flood;

  return 0;
}

#endif // LV_HAVE_AVX512

int srsran_ldpc_decoder_init(srsran_ldpc_decoder_t*     q,
                             srsran_ldpc_decoder_type_t type,
                             srsran_basegraph_t         bg,
                             uint16_t                   ls,
                             float                      scaling_fctr)
{
  int ls_index = get_ls_index(ls);

  if (ls_index == VOID_LIFTSIZE) {
    ERROR("Invalid lifting size %d", ls);
    return -1;
  }

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

  q->var_indices = srsran_vec_malloc(q->bgM * sizeof(int8_t[MAX_CNCT]));
  if (!q->var_indices) {
    free(q->pcm);
    perror("malloc");
    return -1;
  }

  if (create_compact_pcm(q->pcm, q->var_indices, q->bg, q->ls) != 0) {
    perror("Create PCM");
    free(q->var_indices);
    free(q->pcm);
    return -1;
  }

  if ((scaling_fctr <= 0) || (scaling_fctr > 1)) {
    perror("The scaling factor of the min-sum algorithm should be larger than 0 and not larger than 1.");
    free(q->var_indices);
    free(q->pcm);
    return -1;
  }
  q->scaling_fctr = scaling_fctr;

  switch (type) {
    case SRSRAN_LDPC_DECODER_F:
      return init_f(q);
    case SRSRAN_LDPC_DECODER_S:
      return init_s(q);
    case SRSRAN_LDPC_DECODER_C:
      return init_c(q);
    case SRSRAN_LDPC_DECODER_C_FLOOD:
      return init_c_flood(q);
#ifdef LV_HAVE_AVX2
    case SRSRAN_LDPC_DECODER_C_AVX2:
      if (ls <= SRSRAN_AVX2_B_SIZE) {
        return init_c_avx2(q);
      } else {
        return init_c_avx2long(q);
      }
    case SRSRAN_LDPC_DECODER_C_AVX2_FLOOD:
      if (ls <= SRSRAN_AVX2_B_SIZE) {
        return init_c_avx2_flood(q);
      } else {
        return init_c_avx2long_flood(q);
      }
#endif // LV_HAVE_AVX2
#ifdef LV_HAVE_AVX512
    case SRSRAN_LDPC_DECODER_C_AVX512:
      if (ls <= SRSRAN_AVX512_B_SIZE) {
        return init_c_avx512(q);
      } else {
        return init_c_avx512long(q);
      }
    case SRSRAN_LDPC_DECODER_C_AVX512_FLOOD:
      return init_c_avx512long_flood(q);
#endif // LV_HAVE_AVX2

    default:
      ERROR("Unknown decoder.");
      return -1;
  }
}

void srsran_ldpc_decoder_free(srsran_ldpc_decoder_t* q)
{
  if (q->free) {
    q->free(q);
  }
  bzero(q, sizeof(srsran_ldpc_decoder_t));
}

int srsran_ldpc_decoder_decode_f(srsran_ldpc_decoder_t* q, const float* llrs, uint8_t* message, uint32_t cdwd_rm_length)
{
  return q->decode_f(q, llrs, message, cdwd_rm_length);
}

int srsran_ldpc_decoder_decode_s(srsran_ldpc_decoder_t* q,
                                 const int16_t*         llrs,
                                 uint8_t*               message,
                                 uint32_t               cdwd_rm_length)
{
  return q->decode_s(q, llrs, message, cdwd_rm_length);
}

int srsran_ldpc_decoder_decode_c(srsran_ldpc_decoder_t* q, const int8_t* llrs, uint8_t* message)
{
  return q->decode_c(q, llrs, message, q->liftN - 2 * q->ls);
}

int srsran_ldpc_decoder_decode_rm_c(srsran_ldpc_decoder_t* q,
                                    const int8_t*          llrs,
                                    uint8_t*               message,
                                    uint32_t               cdwd_rm_length)
{
  return q->decode_c(q, llrs, message, cdwd_rm_length);
}
