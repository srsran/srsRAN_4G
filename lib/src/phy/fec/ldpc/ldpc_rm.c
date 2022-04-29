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

/*!
 * \file ldpc_rm.c
 * \brief Definition of the LDPC Rate Matcher and Rate Demacher (float-valued, int16_t and int8_t)
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "srsran/phy/fec/ldpc/ldpc_common.h" //FILLER_BIT definition
#include "srsran/phy/fec/ldpc/ldpc_rm.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/phy/utils/debug.h"

//#define debug
/*!
 * \brief Look-up table: k0 indices
 *
 * For each rv, the corresponding row contains the indices of the
 * two base graphs.
 */
static const uint32_t BASEK0[4][2] = {{0, 0}, {17, 13}, {33, 25}, {56, 43}};

/*!
 * \brief Look-up table: base codeword lengths N/LS
 *
 */
static const uint32_t BASEN[2] = {66, 50};

/*!
 * \brief Look-up table: base codeblock lengths K/LS
 *
 */
static const uint32_t BASEK[2] = {22, 10};

/*!
 * \brief Look-up table: Maximum number of coded bits available for transmission in a
 * transport block
 *If there is only one user
 * in the system using the maximum bandwidth, then
 * MAXE is smaller than nRB * nDS * nSC_RB * modOrd * nLayers
 * where nLayers is the number of layer (<=4), modOrder is the modulation order (<=8)
 * nSC_RB is the number of subcarriers per resource block (RB) (<=12), nDS is the number of symbols
 * to transmit data in a RB (13) and nRB is the maximum number of resource blocks in the system,
 * this depends on the channel bandwidth and subcarrier spacing and according to
 * Table 5.3.2-1: Maximum transmission bandwidth configuration NRB : FR1 ,
 * it is not larger than 273 (i.e. for subcarrier spacing 10 and bandwidth 100MHz)
 */
static const uint32_t MAXE = 273 * 13 * 12 * 8 * 4;

/*!
 * \brief Describes an rate matcher.
 */
struct pRM_tx {
  uint8_t* tmp_rm_codeword; /*!< \brief Pointer to a temporal buffer between bit-selection and interleaver. */
};

/*!
 * \brief Describes an rate dematcher (float version).
 */
struct pRM_rx_f {
  float*    tmp_rm_symbol; /*!< \brief Pointer to a temporal buffer between bit-selection and interleaver. */
  uint32_t* indices;       /*!< \brief Pointer to a temporal buffer with the indices for bit-selection. */
};

/*!
 * \brief Describes an rate dematcher (short version).
 */
struct pRM_rx_s {
  int16_t*  tmp_rm_symbol; /*!< \brief Pointer to a temporal buffer between bit-selection and interleaver. */
  uint32_t* indices;       /*!< \brief Pointer to a temporal buffer with the indices for bit-selection. */
};

/*!
 * \brief Describes an rate dematcher (char version).
 */
struct pRM_rx_c {
  int8_t*   tmp_rm_symbol; /*!< \brief Pointer to a temporal buffer between bit-selection and interleaver. */
  uint32_t* indices;       /*!< \brief Pointer to a temporal buffer with the indices for bit-selection. */
};

/*!
 * Initialize rate-matching parameters
 */
static int init_rm(srsran_ldpc_rm_t*  p,
                   const uint32_t     E,
                   const uint32_t     F,
                   const uint32_t     bg,
                   const uint32_t     ls,
                   const uint8_t      rv,
                   const srsran_mod_t mod_type,
                   const uint32_t     Nref)
{
  if (p == NULL) {
    return -1;
  }

  uint32_t basek0    = BASEK0[rv][bg];
  uint32_t mod_order = srsran_mod_bits_x_symbol(mod_type);
  uint32_t N         = ls * BASEN[bg];
  uint32_t K         = ls * BASEK[bg];

  // check E smaller than MAXE
  if ((E > MAXE) != 0) { //
    ERROR("Wrong RM codeword length (E) = %d. It must be smaller than %d for base graph %d", E, MAXE, bg + 1);
    return -1;
  }

  // Protect zero modulo
  if (mod_order == 0) {
    ERROR("Invalid modulation order");
    return -1;
  }

  // check out_len is multiple of mod_order
  if ((E % mod_order) != 0) { // N can only be a multiple of either BASEN[0] or BASEN[1], but not both
    ERROR("Wrong RM codeword length (E) = %d. It must be a multiple of modulation order = %d", E, mod_order);
    return -1;
  }

  // Set parameters
  p->N         = N;
  p->E         = E;
  p->K         = K;
  p->F         = F;
  p->ls        = ls;
  p->mod_order = mod_order;
  p->bg        = bg;
  // Determine Ncb and k0
  if (N <= Nref) {
    p->Ncb = N;
    p->k0  = ls * basek0;
  } else {
    p->Ncb = Nref;
    p->k0  = ls * ((basek0 * Nref) / N);
  }

  return 0;
}

/*!
 * Bit selection for the rate-matching block. Selects out_len bits, starting from
 * the k0th, ingoring filler bits, and consider an input buffer of length Ncb.
 */
static void bit_selection_rm_tx(const uint8_t* input,
                                uint8_t*       output,
                                const uint32_t out_len,
                                const uint32_t k0,
                                const uint32_t Ncb)
{
  uint32_t E = out_len;

  uint32_t k    = 0;
  uint32_t j    = 0;
  uint32_t icwd = 0;

  while (k < E) {
    icwd = (k0 + j) % Ncb;
    if (input[icwd] != FILLER_BIT) {
      output[k] = input[icwd];
      k         = k + 1;
    }
    j = j + 1;
  } // while
}

/*!
 * Undoes bit selection for the rate-dematching block.
 * The output has the codeword length N. It inserts filler bits as INFINITY symbols
 * (to indicate very reliable 0 bit), and set to 0 (completely unknown bit) all
 * missing symbol. Repeated symbols are added.
 * The input memory *output shall be either initialized to all zeros or to the
 * result of previous redundancy versions is available.
 */
static void bit_selection_rm_rx(const float*   input,
                                const uint32_t in_len,
                                float*         output,
                                uint32_t*      indices,
                                const uint32_t ini_exclude,
                                const uint32_t end_exclude,
                                const uint32_t k0,
                                const uint32_t Ncb)
{
  uint32_t E = in_len;

  uint32_t k    = 0;
  uint32_t j    = 0;
  uint32_t icwd = 0;
  while (k < E) {
    icwd = (k0 + j) % Ncb;
    if (!(icwd >= ini_exclude && icwd < end_exclude)) { // avoid filler bits
      indices[k] = icwd;
      k          = k + 1;
    }
    j = j + 1;
  } // while

  // set filler bits to INFINITY
  for (uint32_t i = ini_exclude; i < end_exclude; i++) {
    output[i] = INFINITY;
  }

  // Add soft bits, in case of repetition
  for (uint32_t i = 0; i < E; i++) {
    output[indices[i]] = output[indices[i]] + input[i];
  }
}

/*!
 * Undoes bit selection for the rate-dematching block.
 * The output has the codeword length N. It inserts filler bits as INFINITY symbols
 * (to indicate very reliable 0 bit), and set to 0 (completely unknown bit) all
 * missing symbol. Repeated symbols are added.
 * The input memory *output shall be either initialized to all zeros or to the
 * result of previous redundancy versions is available.
 */
static void bit_selection_rm_rx_s(const int16_t* input,
                                  const uint32_t in_len,
                                  int16_t*       output,
                                  uint32_t*      indices,
                                  const uint32_t ini_exclude,
                                  const uint32_t end_exclude,
                                  const uint32_t k0,
                                  const uint32_t Ncb)
{
  uint32_t E = in_len;

  uint32_t k    = 0;
  uint32_t j    = 0;
  uint32_t icwd = 0;
  while (k < E) {
    icwd = (k0 + j) % Ncb;
    if (!(icwd >= ini_exclude && icwd < end_exclude)) { // avoid filler bits
      indices[k] = icwd;
      k          = k + 1;
    }
    j = j + 1;
  } // while

  // set filler bits to INFINITY
  const long infinity16 = (1U << 15U) - 1; // Max positive value in 16-bit representation
  for (uint32_t i = ini_exclude; i < end_exclude; i++) {
    output[i] = infinity16;
  }

  // Add soft bits, in case of repetition
  const int16_t infinity15 =
      (1U << 14U) - 1; // Messages use a 15-bit quantization. Soft bits use the remaining bit to denote infinity.
  // input is assume to be quantized from -infinity15 to infinity15. Only filler bits can be infinity16
  long tmp = 0;
  for (uint32_t i = 0; i < E; i++) {
    tmp = (long)output[indices[i]] + input[i];
    if (tmp > infinity15) {
      tmp = infinity15;
    }
    if (tmp < -infinity15) {
      tmp = -infinity15;
    }
    output[indices[i]] = (int16_t)tmp;
  }
}

/*!
 * Undoes bit selection for the rate-dematching block (int8_t).
 * The output has the codeword length N. It inserts filler bits as INFINITY symbols
 * (to indicate very reliable 0 bit), and set to 0 (completely unknown bit) all
 * missing symbol. Repeated symbols are added.
 * The input memory *output shall be either initialized to all zeros or to the
 * result of previous redundancy versions is available.
 */
static void bit_selection_rm_rx_c(const int8_t*  input,
                                  const uint32_t in_len,
                                  int8_t*        output,
                                  uint32_t*      indices,
                                  const uint32_t ini_exclude,
                                  const uint32_t end_exclude,
                                  const uint32_t k0,
                                  const uint32_t Ncb)
{
  uint32_t E = in_len;

  uint32_t k    = 0;
  uint32_t j    = 0;
  uint32_t icwd = 0;
  while (k < E) {
    icwd = (k0 + j) % Ncb;
    if (!(icwd >= ini_exclude && icwd < end_exclude)) { // avoid filler bits
      indices[k] = icwd;
      k          = k + 1;
    }
    j = j + 1;
  } // while

  // set filler bits to INFINITY
  const long infinity8 = (1U << 7U) - 1; // Max positive value in 8-bit representation
  for (uint32_t i = ini_exclude; i < end_exclude; i++) {
    output[i] = infinity8;
  }

  // Add soft bits, in case of repetition
  const int16_t infinity7 =
      (1U << 6U) - 1; // Messages use a 15-bit quantization. Soft bits use the remaining bit to denote infinity.
  // input is assume to be quantized from -infinity15 to infinity15. Only filler bits can be infinity16
  long tmp = 0;
  for (uint32_t i = 0; i < E; i++) {
    tmp = (long)output[indices[i]] + input[i];
    if (tmp > infinity7) {
      tmp = infinity7;
    }
    if (tmp < -infinity7) {
      tmp = -infinity7;
    }
    output[indices[i]] = (int8_t)tmp;
  }
}

/*!
 * Bit interleaver
 */
static void
bit_interleaver_rm_tx(const uint8_t* input, uint8_t* output, const uint32_t in_out_len, const uint32_t mod_order)
{
  uint32_t cols = 0;
  uint32_t rows = 0;
  rows          = mod_order;
  cols          = in_out_len / rows;
  for (uint32_t j = 0; j < cols; j++) {
    for (uint32_t i = 0; i < rows; i++) {
      output[i + j * rows] = input[i * cols + j];
    }
  }
}

/*!
 * Bit deinterleaver (float)
 */
static void
bit_interleaver_rm_rx(const float* input, float* output, const uint32_t in_out_len, const uint32_t mod_order)
{
  uint32_t cols = 0;
  uint32_t rows = 0;
  rows          = mod_order;
  cols          = in_out_len / rows;
  for (uint32_t j = 0; j < cols; j++) {
    for (uint32_t i = 0; i < rows; i++) {
      output[i * cols + j] = input[j * rows + i];
    }
  }
}

/*!
 * Bit deinterleaver (short)
 */
static void
bit_interleaver_rm_rx_s(const int16_t* input, int16_t* output, const uint32_t in_out_len, const uint32_t mod_order)
{
  uint32_t cols = 0;
  uint32_t rows = 0;
  rows          = mod_order;
  cols          = in_out_len / rows;
  for (uint32_t j = 0; j < cols; j++) {
    for (uint32_t i = 0; i < rows; i++) {
      output[i * cols + j] = input[j * rows + i];
    }
  }
}

/*!
 * Bit deinterleaver (short)
 */
static void
bit_interleaver_rm_rx_c(const int8_t* input, int8_t* output, const uint32_t in_out_len, const uint32_t mod_order)
{
  uint32_t cols = 0;
  uint32_t rows = 0;
  rows          = mod_order;
  cols          = in_out_len / rows;
  for (uint32_t j = 0; j < cols; j++) {
    for (uint32_t i = 0; i < rows; i++) {
      output[i * cols + j] = input[j * rows + i];
    }
  }
}

int srsran_ldpc_rm_tx_init(srsran_ldpc_rm_t* p)
{
  if (p == NULL) {
    return -1;
  }

  struct pRM_tx* pp = NULL; // pointer to the rate matcher instance

  // allocate memory to the rate-matcher instance
  if ((pp = malloc(sizeof(struct pRM_tx))) == NULL) {
    return -1;
  }
  p->ptr = pp;

  // allocate memory to the rm_codeword after bit selection.
  if ((pp->tmp_rm_codeword = srsran_vec_u8_malloc(MAXE)) == NULL) {
    free(pp);
    return -1;
  }

  return 0;
}

int srsran_ldpc_rm_rx_init_f(srsran_ldpc_rm_t* p)
{
  if (p == NULL) {
    return -1;
  }

  struct pRM_rx_f* pp = NULL; // pointer to the rate matcher instance

  // allocate memory to ther rate-demacher instance
  if ((pp = malloc(sizeof(struct pRM_rx_f))) == NULL) {
    return -1;
  }
  p->ptr = pp;

  // allocate memory to the temporal buffer
  if ((pp->tmp_rm_symbol = srsran_vec_f_malloc(MAXE)) == NULL) {
    free(pp);
    return -1;
  }

  if ((pp->indices = srsran_vec_u32_malloc(MAXE)) == NULL) {
    free(pp->tmp_rm_symbol);
    free(pp);
    return -1;
  }
  return 0;
}

int srsran_ldpc_rm_rx_init_s(srsran_ldpc_rm_t* p)
{
  if (p == NULL) {
    return -1;
  }

  struct pRM_rx_s* pp = NULL; // pointer to the rate matcher instance

  // allocate memory to ther rate-demacher instance
  if ((pp = malloc(sizeof(struct pRM_rx_s))) == NULL) {
    return -1;
  }
  p->ptr = pp;

  // allocate memory to the temporal buffer
  if ((pp->tmp_rm_symbol = srsran_vec_i16_malloc(MAXE)) == NULL) {
    free(pp);
    return -1;
  }

  if ((pp->indices = srsran_vec_u32_malloc(MAXE)) == NULL) {
    free(pp->tmp_rm_symbol);
    free(pp);
    return -1;
  }

  return 0;
}
int srsran_ldpc_rm_rx_init_c(srsran_ldpc_rm_t* p)
{
  if (p == NULL) {
    return -1;
  }

  struct pRM_rx_c* pp = NULL; // pointer to the rate matcher instance

  // allocate memory to ther rate-demacher instance
  if ((pp = malloc(sizeof(struct pRM_rx_c))) == NULL) {
    return -1;
  }
  p->ptr = pp;

  // allocate memory to the temporal buffer
  if ((pp->tmp_rm_symbol = srsran_vec_i8_malloc(MAXE)) == NULL) {
    free(pp);
    return -1;
  }

  if ((pp->indices = srsran_vec_u32_malloc(MAXE)) == NULL) {
    free(pp->tmp_rm_symbol);
    free(pp);
    return -1;
  }

  return 0;
}

void srsran_ldpc_rm_tx_free(srsran_ldpc_rm_t* q)
{
  if (q != NULL) {
    struct pRM_tx* qq = q->ptr;
    if (qq != NULL) {
      if (qq->tmp_rm_codeword != NULL) {
        free(qq->tmp_rm_codeword);
      }
      free(qq);
    }
  }
}

void srsran_ldpc_rm_rx_free_f(srsran_ldpc_rm_t* q)
{
  if (q != NULL) {
    struct pRM_rx_f* qq = q->ptr;
    if (qq != NULL) {
      if (qq->tmp_rm_symbol != NULL) {
        free(qq->tmp_rm_symbol);
      }
      if (qq->indices != NULL) {
        free(qq->indices);
      }
      free(qq);
    }
  }
}

void srsran_ldpc_rm_rx_free_s(srsran_ldpc_rm_t* q)
{
  if (q != NULL) {
    struct pRM_rx_s* qq = q->ptr;
    if (qq != NULL) {
      if (qq->tmp_rm_symbol != NULL) {
        free(qq->tmp_rm_symbol);
      }
      if (qq->indices != NULL) {
        free(qq->indices);
      }
      free(qq);
    }
  }
}

void srsran_ldpc_rm_rx_free_c(srsran_ldpc_rm_t* q)
{
  if (q != NULL) {
    struct pRM_rx_c* qq = q->ptr;
    if (qq != NULL) {
      if (qq->tmp_rm_symbol != NULL) {
        free(qq->tmp_rm_symbol);
      }
      if (qq->indices != NULL) {
        free(qq->indices);
      }
      free(qq);
    }
  }
}

int srsran_ldpc_rm_tx(srsran_ldpc_rm_t*        q,
                      const uint8_t*           input,
                      uint8_t*                 output,
                      const uint32_t           E,
                      const srsran_basegraph_t bg,
                      const uint32_t           ls,
                      const uint8_t            rv,
                      const srsran_mod_t       mod_type,
                      const uint32_t           Nref)

{
  // initialize parameters. The filler bit is ignored
  if (init_rm(q, E, 0, bg, ls, rv, mod_type, Nref) != 0) {
    perror("rate matcher init");
    exit(-1);
  }

  struct pRM_tx* pp              = q->ptr;
  uint8_t*       tmp_rm_codeword = pp->tmp_rm_codeword;

  if (q->mod_order == 1) { // interleaver can be skipped
    bit_selection_rm_tx(input, output, q->E, q->k0, q->Ncb);
  } else {
    bit_selection_rm_tx(input, tmp_rm_codeword, q->E, q->k0, q->Ncb);
    bit_interleaver_rm_tx(tmp_rm_codeword, output, q->E, q->mod_order);
  }

  return 0;
}

int srsran_ldpc_rm_rx_f(srsran_ldpc_rm_t*        q,
                        const float*             input,
                        float*                   output,
                        const uint32_t           E,
                        const uint32_t           F,
                        const srsran_basegraph_t bg,
                        const uint32_t           ls,
                        const uint8_t            rv,
                        const srsran_mod_t       mod_type,
                        const uint32_t           Nref)
{
  if (init_rm(q, E, F, bg, ls, rv, mod_type, Nref) != 0) {
    perror("rate matcher init");
    exit(-1);
  }

  struct pRM_rx_f* pp            = q->ptr;
  float*           tmp_rm_symbol = pp->tmp_rm_symbol;
  uint32_t*        indices       = pp->indices;
  uint32_t         end_exclude   = q->K - 2 * q->ls;
  uint32_t         ini_exclude   = end_exclude - q->F;

  if (q->mod_order == 1) { // interleaver can be skipped
    bit_selection_rm_rx(input, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  } else {
    bit_interleaver_rm_rx(input, tmp_rm_symbol, q->E, q->mod_order);
    bit_selection_rm_rx(tmp_rm_symbol, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  }
  return 0;
}

int srsran_ldpc_rm_rx_s(srsran_ldpc_rm_t*        q,
                        const int16_t*           input,
                        int16_t*                 output,
                        const uint32_t           E,
                        const uint32_t           F,
                        const srsran_basegraph_t bg,
                        const uint32_t           ls,
                        const uint8_t            rv,
                        const srsran_mod_t       mod_type,
                        const uint32_t           Nref)
{
  if (init_rm(q, E, F, bg, ls, rv, mod_type, Nref) != 0) {
    perror("rate matcher init");
    exit(-1);
  }

  struct pRM_rx_f* pp            = q->ptr;
  int16_t*         tmp_rm_symbol = (int16_t*)pp->tmp_rm_symbol;
  uint32_t*        indices       = pp->indices;
  uint32_t         end_exclude   = q->K - 2 * q->ls;
  uint32_t         ini_exclude   = end_exclude - q->F;

  if (q->mod_order == 1) { // interleaver can be skipped
    bit_selection_rm_rx_s(input, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  } else {
    bit_interleaver_rm_rx_s(input, tmp_rm_symbol, q->E, q->mod_order);
    bit_selection_rm_rx_s(tmp_rm_symbol, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  }

  return 0;
}

int srsran_ldpc_rm_rx_c(srsran_ldpc_rm_t*        q,
                        const int8_t*            input,
                        int8_t*                  output,
                        const uint32_t           E,
                        const uint32_t           F,
                        const srsran_basegraph_t bg,
                        const uint32_t           ls,
                        const uint8_t            rv,
                        const srsran_mod_t       mod_type,
                        const uint32_t           Nref)
{
  if (init_rm(q, E, F, bg, ls, rv, mod_type, Nref) != 0) {
    perror("rate matcher init");
    exit(-1);
  }

  struct pRM_rx_c* pp            = q->ptr;
  int8_t*          tmp_rm_symbol = pp->tmp_rm_symbol;
  uint32_t*        indices       = pp->indices;
  uint32_t         end_exclude   = q->K - 2 * q->ls;
  uint32_t         ini_exclude   = end_exclude - q->F;

  if (q->mod_order == 1) { // interleaver can be skipped
    bit_selection_rm_rx_c(input, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  } else {
    bit_interleaver_rm_rx_c(input, tmp_rm_symbol, q->E, q->mod_order);
    bit_selection_rm_rx_c(tmp_rm_symbol, q->E, output, indices, ini_exclude, end_exclude, q->k0, q->Ncb);
  }

  // Return the number of useful LLR
  return (int)SRSRAN_MIN(q->k0 + q->E, q->Ncb);
}
