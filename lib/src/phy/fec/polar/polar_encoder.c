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
 * \file polar_encoder.c
 * \brief Definition of the polar encoder.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * 5G uses a polar encoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */
#include "srsran/phy/fec/polar/polar_encoder.h"
#include "polar_encoder_avx2.h"
#include "polar_encoder_pipelined.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifdef LV_HAVE_AVX2

/*! AVX2 polar encoder */
static int encode_avx2(void* o, const uint8_t* input, uint8_t* output, const uint8_t code_size_log)
{
  srsran_polar_encoder_t* q = o;

  polar_encoder_encode_avx2(q->ptr, input, output, code_size_log);
  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the AVX2 encoder. */
static void free_avx2(void* o)
{
  srsran_polar_encoder_t* q = o;
  delete_polar_encoder_avx2(q->ptr);
}

/*! Initializes a polar encoder structure to use the AVX2 polar encoder algorithm*/
static int init_avx2(srsran_polar_encoder_t* q, const uint8_t code_size_log)
{
  q->encode = encode_avx2;
  q->free   = free_avx2;
  if ((q->ptr = create_polar_encoder_avx2(code_size_log)) == NULL) {
    free_avx2(q);
    return -1;
  }
  return 0;
}
#endif // LV_HAVE_AVX2

/*! Pipelined polar encoder */
static int encode_pipelined(void* o, const uint8_t* input, uint8_t* output, const uint8_t code_size_log)
{
  srsran_polar_encoder_t* q = o;

  polar_encoder_encode_pipelined(q->ptr, input, output, code_size_log);
  return 0;
}

/*! Carries out the actual destruction of the memory allocated to the pipelined encoder. */
static void free_pipelined(void* o)
{
  srsran_polar_encoder_t* q = o;
  delete_polar_encoder_pipelined(q->ptr);
}

/*! Initializes a polar encoder structure to use the pipeline polar encoder algorithm*/
static int init_pipelined(srsran_polar_encoder_t* q, const uint8_t code_size_log)
{
  q->encode = encode_pipelined;
  q->free   = free_pipelined;
  if ((q->ptr = create_polar_encoder_pipelined(code_size_log)) == NULL) {
    free_pipelined(q);
    return -1;
  }
  return 0;
}

int srsran_polar_encoder_init(srsran_polar_encoder_t* q, srsran_polar_encoder_type_t type, const uint8_t code_size_log)
{
  switch (type) { // NOLINT
    case SRSRAN_POLAR_ENCODER_PIPELINED:
      return init_pipelined(q, code_size_log);
#ifdef LV_HAVE_AVX2
    case SRSRAN_POLAR_ENCODER_AVX2:
      return init_avx2(q, code_size_log);
#endif // LV_HAVE_AVX2
    default:
      return -1;
  }
  return 0;
}

void srsran_polar_encoder_free(srsran_polar_encoder_t* q)
{
  if (q->free) {
    q->free(q);
  }
  memset(q, 0, sizeof(srsran_polar_encoder_t));
}

int srsran_polar_encoder_encode(srsran_polar_encoder_t* q,
                                const uint8_t*          input,
                                uint8_t*                output,
                                const uint8_t           code_size_log)
{
  return q->encode(q, input, output, code_size_log);
}
