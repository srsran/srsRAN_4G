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
 * \file polar_rm.c
 * \brief Definition of the LDPC Rate Matcher and Rate Dematcher (float-valued, int16_t and int8_t)
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include "srsran/phy/utils/vector.h"
#include <stddef.h>
#include <stdint.h>

#include "srsran/phy/fec/polar/polar_code.h"
#include "srsran/phy/fec/polar/polar_rm.h"

#include "srsran/phy/utils/debug.h"

/*!
 * \brief Describes an rate matcher.
 */
struct pRM_tx {
  uint8_t* y_e; /*!< \brief Pointer to a temporal buffer to store the block interleaved codeword (y), as well as the
                   rate-matched codewords (e). */
};

/*!
 * \brief Describes an rate dematcher (float version).
 */
struct pRM_rx_f {
  float* y_e; /*!< \brief Pointer to a temporal buffer to symbols before and after bit_selection_rx. */
  float* e;   /*!< \brief Pointer to a the position in the y_e buffer where the rate matched llr start.*/
};

/*!
 * \brief Describes an rate dematcher (int8_t version).
 */
struct pRM_rx_s {
  int16_t* y_e; /*!< \brief Pointer to a temporal buffer to symbols before and after bit_selection_rx. */
  int16_t* e;   /*!< \brief Pointer to a the position in the y_e buffer where the rate matched llr start.*/
};

/*!
 * \brief Describes an rate dematcher (int8_t version).
 */
struct pRM_rx_c {
  int8_t* y_e; /*!< \brief Pointer to a temporal buffer to symbols before and after bit_selection_rx. */
  int8_t* e;   /*!< \brief Pointer to a the position in the y_e buffer where the rate matched llr start.*/
};

/*!
 * generic interleaver
 */
static void interleaver_rm_tx(const uint8_t* input, uint8_t* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[j] = input[indices[j]];
  }
}

/*!
 * generic deinterleaver.
 */
static void interleaver_rm_rx(const float* input, float* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[indices[j]] = input[j];
  }
}

/*!
 * generic deinterleaver (int16_t).
 */
static void interleaver_rm_rx_s(const int16_t* input, int16_t* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[indices[j]] = input[j];
  }
}

/*!
 * generic deinterleaver (int8_t).
 */
static void interleaver_rm_rx_c(const int8_t* input, int8_t* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[indices[j]] = input[j];
  }
}

/*!
 * Bit selection for the polar rate-matching block. ye has length N, but there is EMAX memory allocated to it.
 */
static uint8_t* bit_selection_rm_tx(uint8_t* y, const uint32_t N, const uint32_t E, const uint32_t K)
{
  uint8_t* e   = NULL;
  uint32_t k_N = 0;

  e = y;
  if (E >= N) { // repetition
    for (uint32_t k = N; k < E; k++) {
      k_N  = k % N;
      e[k] = y[k_N];
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing the first N-E bits
      e = y + (N - E);
    } // else shortening the last N-E bits
  }
  return e;
}

/*!
 * Undoes bit selection for the rate-dematching block float).
 * The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
 * and 127 (to indicate very reliable 0 bit). Repeated symbols are added.
 */
static float* bit_selection_rm_rx(float* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  float*   y   = NULL;
  uint32_t k_N = 0;

  y = e;
  if (E >= N) { // add repetitions
    y = e;
    for (uint32_t k = N; k < E; k++) {
      k_N    = k % N;
      y[k_N] = y[k_N] + e[k];
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k < N - E; k++) {
        y[k] = 0;
      }

    } else { // shortening, bits are know to be 0. i.e., very high llrs
      for (uint32_t k = E; k < N; k++) {
        y[k] = 1e+20F; /* max value */
      }
    }
  }
  return y;
}

/*!
 * Undoes bit selection for the rate-dematching block (int16_t).
 * The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
 * and 127 (to indicate very reliable 0 bit). Repeated symbols are added.
 */
static int16_t* bit_selection_rm_rx_s(int16_t* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  int16_t* y   = NULL;
  uint32_t k_N = 0;
  long     tmp = 0;

  y = e;
  if (E >= N) { // add repetitions
    y = e;
    for (uint32_t k = N; k < E; k++) {
      k_N = k % N;
      tmp = (long)y[k_N] + e[k];
      // control saturation
      if (tmp > 32767) {
        tmp = 32767;
      }
      if (tmp < -32767) {
        tmp = -32767;
      }
      y[k_N] = (int16_t)tmp;
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k < N - E; k++) {
        y[k] = 0;
      }

    } else { // shortening, bits are know to be 0. i.e., very high llrs
      for (uint32_t k = E; k < N; k++) {
        y[k] = 32767; /* max value */
      }
    }
  }
  return y;
}

/*!
 * Undoes bit selection for the rate-dematching block (int8_t).
 * The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
 * and 127 (to indicate very reliable 0 bit). Repeated symbols are added.
 */
static int8_t* bit_selection_rm_rx_c(int8_t* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  int8_t*  y   = NULL;
  uint32_t k_N = 0;
  long     tmp = 0;

  y = e;
  if (E >= N) { // add repetitions
    y = e;
    for (uint32_t k = N; k < E; k++) {
      k_N = k % N;
      tmp = (long)y[k_N] + e[k];
      // control saturation
      if (tmp > 127) {
        tmp = 127;
      }
      if (tmp < -127) {
        tmp = -127;
      }
      y[k_N] = (int8_t)tmp;
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k < N - E; k++) {
        y[k] = 0;
      }

    } else { // shortening, bits are know to be 0. i.e., very high llrs
      for (uint32_t k = E; k < N; k++) {
        y[k] = 127; /* max value */
      }
    }
  }
  return y;
}

/*!
 * Channel interleaver.
 */
static void ch_interleaver_rm_tx(const uint8_t* e, uint8_t* f, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r < T; r++) {
    i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        f[i_out] = e[i_in];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

/*!
 * Channel deinterleaver.
 */
static void ch_interleaver_rm_rx(const float* f, float* e, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r < T; r++) {
    i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        e[i_in] = f[i_out];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

/*!
 * Channel deinterleaver (int16_t).
 */
static void ch_interleaver_rm_rx_s(const int16_t* f, int16_t* e, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r < T; r++) {
    i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        e[i_in] = f[i_out];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

/*!
 * Channel deinterleaver (int8_t).
 */
static void ch_interleaver_rm_rx_c(const int8_t* f, int8_t* e, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r < T; r++) {
    i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        e[i_in] = f[i_out];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

int srsran_polar_rm_tx_init(srsran_polar_rm_t* p)
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

  // allocate memory to the blk interleaved codeword
  if ((pp->y_e = srsran_vec_u8_malloc(EMAX)) == NULL) {
    free(pp);
    return -1;
  }
  return 0;
}

int srsran_polar_rm_rx_init_f(srsran_polar_rm_t* p)
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

  // allocate memory to the temporal buffer of chDeInterleaved llrs
  if ((pp->y_e = srsran_vec_f_malloc(EMAX + NMAX)) == NULL) {
    free(pp);
    return -1;
  }
  pp->e = pp->y_e + NMAX;

  return 0;
}

int srsran_polar_rm_rx_init_s(srsran_polar_rm_t* p)
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

  // allocate memory to the temporal buffer of chDeInterleaved llrs
  if ((pp->y_e = srsran_vec_i16_malloc(EMAX + NMAX)) == NULL) {
    free(pp);
    return -1;
  }
  pp->e = pp->y_e + NMAX;

  return 0;
}

int srsran_polar_rm_rx_init_c(srsran_polar_rm_t* p)
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

  // allocate memory to the temporal buffer of chDeInterleaved llrs
  if ((pp->y_e = srsran_vec_i8_malloc(EMAX + NMAX)) == NULL) {
    free(pp);
    return -1;
  }
  pp->e = pp->y_e + NMAX;

  return 0;
}

void srsran_polar_rm_tx_free(srsran_polar_rm_t* q)
{
  if (q == NULL) {
    return;
  }

  struct pRM_tx* qq = q->ptr;
  if (qq == NULL) {
    return;
  }

  if (qq->y_e) {
    free(qq->y_e);
  }

  free(qq);
}

void srsran_polar_rm_rx_free_f(srsran_polar_rm_t* q)
{
  if (q == NULL) {
    return;
  }

  struct pRM_rx_f* qq = q->ptr;
  if (qq == NULL) {
    return;
  }

  if (qq->y_e) {
    free(qq->y_e);
  }

  free(qq);
}

void srsran_polar_rm_rx_free_s(srsran_polar_rm_t* q)
{
  if (q == NULL) {
    return;
  }

  struct pRM_rx_s* qq = q->ptr;
  if (qq == NULL) {
    return;
  }

  if (qq->y_e) {
    free(qq->y_e);
  }

  free(qq);
}

void srsran_polar_rm_rx_free_c(srsran_polar_rm_t* q)
{
  if (q == NULL) {
    return;
  }

  struct pRM_rx_c* qq = q->ptr;
  if (qq == NULL) {
    return;
  }

  if (qq->y_e) {
    free(qq->y_e);
  }

  free(qq);
}

int srsran_polar_rm_tx(srsran_polar_rm_t* q,
                       const uint8_t*     input,
                       uint8_t*           output,
                       const uint8_t      n,
                       const uint32_t     E,
                       const uint32_t     K,
                       const uint8_t      ibil)

{
  const uint16_t* blk_interleaver = get_blk_interleaver(n);
  uint32_t        N               = (1U << n);

  struct pRM_tx* pp = q->ptr;
  uint8_t*       y  = pp->y_e;
  uint8_t*       e  = NULL;

  interleaver_rm_tx(input, y, blk_interleaver, N);

  e = bit_selection_rm_tx(y, N, E, K); // moves the pointer if puncturing e = y + (N-E), otherwise e = y;

  if (ibil == 0) {
    memcpy(output, e, E * sizeof(uint8_t));
  } else {
    ch_interleaver_rm_tx(e, output, E);
  }

  return 0;
}

int srsran_polar_rm_rx_f(srsran_polar_rm_t* q,
                         const float*       input,
                         float*             output,
                         const uint32_t     E,
                         const uint8_t      n,
                         const uint32_t     K,
                         const uint8_t      ibil)
{
  struct pRM_rx_f* pp = q->ptr;
  float*           y  = NULL;
  float*           e  = pp->e; // length E
  uint32_t         N  = (1U << n);

  const uint16_t* blk_interleaver = get_blk_interleaver(n);

  if (ibil == 0) {
    memcpy(e, input, E * sizeof(float));
  } else {
    ch_interleaver_rm_rx(input, e, E);
  }

  y = bit_selection_rm_rx(e, E, N, K);
  interleaver_rm_rx(y, output, blk_interleaver, N);

  return 0;
}

int srsran_polar_rm_rx_s(srsran_polar_rm_t* q,
                         const int16_t*     input,
                         int16_t*           output,
                         const uint32_t     E,
                         const uint8_t      n,
                         const uint32_t     K,
                         const uint8_t      ibil)
{
  struct pRM_rx_s* pp = q->ptr;
  int16_t*         y  = NULL;
  int16_t*         e  = pp->e;
  uint32_t         N  = (1U << n);

  const uint16_t* blk_interleaver = get_blk_interleaver(n);

  if (ibil == 0) {
    memcpy(e, input, E * sizeof(int16_t));
  } else {
    ch_interleaver_rm_rx_s(input, e, E);
  }

  y = bit_selection_rm_rx_s(e, E, N, K);
  interleaver_rm_rx_s(y, output, blk_interleaver, N);

  return 0;
}

int srsran_polar_rm_rx_c(srsran_polar_rm_t* q,
                         const int8_t*      input,
                         int8_t*            output,
                         const uint32_t     E,
                         const uint8_t      n,
                         const uint32_t     K,
                         const uint8_t      ibil)
{
  struct pRM_rx_c* pp = q->ptr;
  int8_t*          y  = NULL;
  int8_t*          e  = pp->e;
  uint32_t         N  = (1U << n);

  const uint16_t* blk_interleaver = get_blk_interleaver(n);

  if (ibil == 0) {
    memcpy(e, input, E * sizeof(int8_t));
  } else {
    ch_interleaver_rm_rx_c(input, e, E);
  }

  y = bit_selection_rm_rx_c(e, E, N, K);
  interleaver_rm_rx_c(y, output, blk_interleaver, N);

  return 0;
}
