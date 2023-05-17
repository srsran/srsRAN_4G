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

/*!
 * \file polar_encoder_pipelined.c
 * \brief Definition of the pipelined polar encoder.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * The pipelined polar encoder is described in
 * Erdal Arikan, "Polar code: A pipelined implementation" presented at "4th International Symposium on Broadband
 * Communication (ISBC 2010) July 11-14, 2010, Melaka, Malaysia"
 * 5G uses a polar encoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */

#include "srsran/phy/fec/polar/polar_encoder.h"
#include "srsran/phy/utils/vector.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/*!
 * \brief Describes an PIPELINED polar encoder.
 */
struct pPIPELINED {
  uint16_t code_size;       /*!< \brief Number of bits of the encoder input/output vector. */
  uint8_t  code_size_log;   /*!< \brief The \f$ log_2\f$ of the maximum supported number of bits of the encoder
                               input/output vector. */
  uint16_t  code_half_size; /*!< \brief Half of the number of bits of the encoder input/output vector. */
  uint16_t* i_even;         /*!< \brief Pointer to the even positions of the encoder input/output vector. */
  uint16_t* i_odd;          /*!< \brief Pointer to the odd positions of the encoder input/output vector. */
  uint8_t*  tmp;            /*!< \brief Pointer to a temporary buffer. */
};

void delete_polar_encoder_pipelined(void* o)
{
  struct pPIPELINED* q = o;
  if (q->i_even) {
    free(q->i_even);
  }
  if (q->i_odd) {
    free(q->i_odd);
  }
  if (q->tmp) {
    free(q->tmp);
  }
  free(q);
}

void* create_polar_encoder_pipelined(const uint8_t code_size_log)
{
  struct pPIPELINED* q = NULL; // pointer to the polar encoder instance

  // allocate memory to the polar decoder instance
  if ((q = malloc(sizeof(struct pPIPELINED))) == NULL) {
    return NULL;
  }

  uint16_t code_size      = 1U << code_size_log;
  uint16_t code_half_size = code_size / 2;

  q->i_odd = srsran_vec_u16_malloc(code_half_size);
  if (!q->i_odd) {
    free(q);
    perror("malloc");
    return NULL;
  }

  q->i_even = srsran_vec_u16_malloc(code_half_size);
  if (!q->i_even) {
    free(q->i_odd);
    free(q);
    perror("malloc");
    return NULL;
  }

  q->tmp = srsran_vec_u8_malloc(code_size);
  if (!q->tmp) {
    free(q->i_even);
    free(q->i_odd);
    free(q);
    perror("malloc");
    return NULL;
  }

  for (uint16_t i = 0; i < code_size / 2; i++) {
    q->i_even[i] = 2 * i;
    q->i_odd[i]  = 2 * i + 1;
  }

  q->code_size      = code_size;
  q->code_size_log  = code_size_log;
  q->code_half_size = code_half_size;

  return q;
}

int polar_encoder_encode_pipelined(void* p, const uint8_t* input, uint8_t* output, const uint8_t code_size_log)
{

  struct pPIPELINED* q = p;

  if (q == NULL) {
    return -1;
  }

  // first stage also initializes output vector
  uint16_t code_half_size = 1U << (code_size_log - 1U);
  if (code_half_size > q->code_half_size) {
    printf("ERROR: max code size %d, current code size %d.\n", 2 * q->code_half_size, 2 * code_half_size);
    return -1;
  }

  for (uint16_t j = 0; j < code_half_size; j++) {
    q->tmp[j]                  = input[q->i_even[j]];
    q->tmp[j + code_half_size] = input[q->i_odd[j]];
  }

  for (uint16_t j = 0; j < code_half_size; j++) {
    output[q->i_odd[j]]  = q->tmp[q->i_odd[j]];
    output[q->i_even[j]] = q->tmp[q->i_even[j]] ^ q->tmp[q->i_odd[j]]; // bitXor
  }

  // remaining stages
  for (uint16_t i = 1; i < code_size_log; i++) {

    for (uint16_t j = 0; j < code_half_size; j++) {
      q->tmp[j]                  = output[q->i_even[j]];
      q->tmp[j + code_half_size] = output[q->i_odd[j]];
    }

    for (uint16_t j = 0; j < code_half_size; j++) {
      output[q->i_odd[j]]  = q->tmp[q->i_odd[j]];
      output[q->i_even[j]] = q->tmp[q->i_even[j]] ^ q->tmp[q->i_odd[j]]; // bitXor
    }
  }

  return 0;
}
