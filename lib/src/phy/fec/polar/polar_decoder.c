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
 * \file polar_decoder.c
 * \brief Definition of the polar decoder.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * 5G uses a polar decoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */

#include <stdint.h>
#include <stdio.h>

#include <math.h>
#include <string.h>

#include "polar_decoder_ssc_c.h"
#include "polar_decoder_ssc_c_avx2.h"
#include "polar_decoder_ssc_f.h"
#include "polar_decoder_ssc_s.h"
#include "srsran/phy/fec/polar/polar_decoder.h"
#include "srsran/phy/utils/debug.h"

/*! SSC Polar decoder with float LLR inputs. */
static int decode_ssc_f(void*           o,
                        const float*    symbols,
                        uint8_t*        data,
                        const uint8_t   n,
                        const uint16_t* frozen_set,
                        const uint16_t  frozen_set_size)
{
  srsran_polar_decoder_t* q = o;

  init_polar_decoder_ssc_f(q->ptr, symbols, data, n, frozen_set, frozen_set_size);

  polar_decoder_ssc_f(q->ptr, data);

  return 0;
}

/*! SSC Polar decoder with int16_t LLR inputs. */
static int decode_ssc_s(void*           o,
                        const int16_t*  symbols,
                        uint8_t*        data,
                        const uint8_t   n,
                        const uint16_t* frozen_set,
                        const uint16_t  frozen_set_size)
{
  srsran_polar_decoder_t* q = o;

  init_polar_decoder_ssc_s(q->ptr, symbols, data, n, frozen_set, frozen_set_size);

  polar_decoder_ssc_s(q->ptr, data);

  return 0;
}

/*! SSC Polar decoder with int8_t LLR inputs. */
static int decode_ssc_c(void*           o,
                        const int8_t*   symbols,
                        uint8_t*        data,
                        const uint8_t   n,
                        const uint16_t* frozen_set,
                        const uint16_t  frozen_set_size)
{
  srsran_polar_decoder_t* q = o;

  init_polar_decoder_ssc_c(q->ptr, symbols, data, n, frozen_set, frozen_set_size);

  polar_decoder_ssc_c(q->ptr, data);

  return 0;
}

#ifdef LV_HAVE_AVX2
/*! SSC Polar decoder AVX2 with int8_t LLR inputs . */
static int decode_ssc_c_avx2(void*           o,
                             const int8_t*   symbols,
                             uint8_t*        data,
                             const uint8_t   n,
                             const uint16_t* frozen_set,
                             const uint16_t  frozen_set_size)
{
  srsran_polar_decoder_t* q = o;

  init_polar_decoder_ssc_c_avx2(q->ptr, symbols, data, n, frozen_set, frozen_set_size);

  polar_decoder_ssc_c_avx2(q->ptr, data);

  return 0;
}
#endif // LV_HAVE_AVX2

/*! Destructor of a (float) SSC polar decoder. */
static void free_ssc_f(void* o)
{
  srsran_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_f(q->ptr);
}

/*! Destructor of a (int16_t) SSC polar decoder. */
static void free_ssc_s(void* o)
{
  srsran_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_s(q->ptr);
}

/*! Destructor of a (int8_t) SSC polar decoder. */
static void free_ssc_c(void* o)
{
  srsran_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_c(q->ptr);
}

#ifdef LV_HAVE_AVX2
/*! Destructor of a (int8_t, avx2) SSC polar decoder. */
static void free_ssc_c_avx2(void* o)
{
  srsran_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_c_avx2(q->ptr);
}
#endif

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with float LLR inputs. */
static int init_ssc_f(srsran_polar_decoder_t* q)
{
  q->decode_f = decode_ssc_f;
  q->free     = free_ssc_f;

  if ((q->ptr = create_polar_decoder_ssc_f(q->nMax)) == NULL) {
    ERROR("create_polar_decoder_ssc_f failed");
    free_ssc_f(q);
    return -1;
  }
  return 0;
}

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint16_t LLR inputs. */
static int init_ssc_s(srsran_polar_decoder_t* q)
{
  q->decode_s = decode_ssc_s;
  q->free     = free_ssc_s;

  if ((q->ptr = create_polar_decoder_ssc_s(q->nMax)) == NULL) {
    ERROR("create_polar_decoder_ssc_s failed");
    free_ssc_s(q);
    return -1;
  }
  return 0;
}

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint8_t LLR inputs. */
static int init_ssc_c(srsran_polar_decoder_t* q)
{
  q->decode_c = decode_ssc_c;
  q->free     = free_ssc_c;

  if ((q->ptr = create_polar_decoder_ssc_c(q->nMax)) == NULL) {
    ERROR("create_polar_decoder_ssc_c failed");
    free_ssc_c(q);
    return -1;
  }
  return 0;
}

#ifdef LV_HAVE_AVX2
/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint8_t LLR inputs and AVX2
 * instructions. */
static int init_ssc_c_avx2(srsran_polar_decoder_t* q)
{
  q->decode_c = decode_ssc_c_avx2;
  q->free     = free_ssc_c_avx2;

  if ((q->ptr = create_polar_decoder_ssc_c_avx2(q->nMax)) == NULL) {
    ERROR("create_polar_decoder_ssc_c failed");
    free_ssc_c_avx2(q);
    return -1;
  }
  return 0;
}
#endif

int srsran_polar_decoder_init(srsran_polar_decoder_t* q, srsran_polar_decoder_type_t type, const uint8_t nMax)
{
  q->nMax = nMax;
  switch (type) {
    case SRSRAN_POLAR_DECODER_SSC_F:
      return init_ssc_f(q);
    case SRSRAN_POLAR_DECODER_SSC_S:
      return init_ssc_s(q);
    case SRSRAN_POLAR_DECODER_SSC_C:
      return init_ssc_c(q);
#ifdef LV_HAVE_AVX2
    case SRSRAN_POLAR_DECODER_SSC_C_AVX2:
      return init_ssc_c_avx2(q);
#endif
    default:
      ERROR("Decoder not implemented");
      return -1;
  }
  return 0;
}

void srsran_polar_decoder_free(srsran_polar_decoder_t* q)
{
  if (q->free) {
    q->free(q);
  }
  memset(q, 0, sizeof(srsran_polar_decoder_t));
}

int srsran_polar_decoder_decode_f(srsran_polar_decoder_t* q,
                                  const float*            llr,
                                  uint8_t*                data_decoded,
                                  const uint8_t           n,
                                  const uint16_t*         frozen_set,
                                  const uint16_t          frozen_set_size)
{
  if (q->nMax >= n) {
    return q->decode_f(q, llr, data_decoded, n, frozen_set, frozen_set_size);
  }

  return -1;
}

int srsran_polar_decoder_decode_s(srsran_polar_decoder_t* q,
                                  const int16_t*          llr,
                                  uint8_t*                data_decoded,
                                  const uint8_t           n,
                                  const uint16_t*         frozen_set,
                                  const uint16_t          frozen_set_size)
{
  if (q->nMax >= n) {
    return q->decode_s(q, llr, data_decoded, n, frozen_set, frozen_set_size);
  }

  return -1;
}

int srsran_polar_decoder_decode_c(srsran_polar_decoder_t* q,
                                  const int8_t*           llr,
                                  uint8_t*                data_decoded,
                                  const uint8_t           n,
                                  const uint16_t*         frozen_set,
                                  const uint16_t          frozen_set_size)
{
  if (q->nMax >= n) {
    return q->decode_c(q, llr, data_decoded, n, frozen_set, frozen_set_size);
  }

  return -1;
}
