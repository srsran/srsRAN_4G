/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*!
 * \file polar_decoder.c
 * \brief Definition of the polar decoder.
 * \author Jesus Gomez (CTTC)
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
#include "srslte/phy/fec/polar/polar_decoder.h"
#include "srslte/phy/utils/debug.h"

/*! SSC Polar decoder with float LLR inputs. */
static int decode_ssc_f(void* o, const float* symbols, uint8_t* data)
{

  srslte_polar_decoder_t* q = o;

  init_polar_decoder_ssc_f(q->ptr, symbols, data);

  polar_decoder_ssc_f(q->ptr, data);

  return 0;
}

/*! SSC Polar decoder with int16_t LLR inputs. */
static int decode_ssc_s(void* o, const int16_t* symbols, uint8_t* data)
{
  srslte_polar_decoder_t* q = o;

  init_polar_decoder_ssc_s(q->ptr, symbols, data);

  polar_decoder_ssc_s(q->ptr, data);

  return 0;
}

/*! SSC Polar decoder with int8_t LLR inputs. */
static int decode_ssc_c(void* o, const int8_t* symbols, uint8_t* data)
{
  srslte_polar_decoder_t* q = o;

  init_polar_decoder_ssc_c(q->ptr, symbols, data);

  polar_decoder_ssc_c(q->ptr, data);

  return 0;
}

#ifdef LV_HAVE_AVX2
/*! SSC Polar decoder AVX2 with int8_t LLR inputs . */
static int decode_ssc_c_avx2(void* o, const int8_t* symbols, uint8_t* data)
{
  srslte_polar_decoder_t* q = o;

  init_polar_decoder_ssc_c_avx2(q->ptr, symbols, data);

  polar_decoder_ssc_c_avx2(q->ptr, data);

  return 0;
}
#endif // LV_HAVE_AVX2

/*! Destructor of a (float) SSC polar decoder. */
static void free_ssc_f(void* o)
{
  srslte_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_f(q->ptr);
}

/*! Destructor of a (int16_t) SSC polar decoder. */
static void free_ssc_s(void* o)
{
  srslte_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_s(q->ptr);
}

/*! Destructor of a (int8_t) SSC polar decoder. */
static void free_ssc_c(void* o)
{
  srslte_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_c(q->ptr);
}

#ifdef LV_HAVE_AVX2
/*! Destructor of a (int8_t, avx2) SSC polar decoder. */
static void free_ssc_c_avx2(void* o)
{
  srslte_polar_decoder_t* q = o;
  delete_polar_decoder_ssc_c_avx2(q->ptr);
}
#endif

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with float LLR inputs. */
static int init_ssc_f(srslte_polar_decoder_t* q, uint16_t* frozen_set, uint16_t code_size_log, uint16_t frozen_set_size)
{
  q->decode_f = decode_ssc_f;
  q->free     = free_ssc_f;

  if ((q->ptr = create_polar_decoder_ssc_f(frozen_set, code_size_log, frozen_set_size)) == NULL) {
    ERROR("create_polar_decoder_ssc_f failed\n");
    free_ssc_f(q);
    return -1;
  }
  return 0;
}

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint16_t LLR inputs. */
static int init_ssc_s(srslte_polar_decoder_t* q, uint16_t* frozen_set, uint16_t code_size_log, uint16_t frozen_set_size)
{
  q->decode_s = decode_ssc_s;
  q->free     = free_ssc_s;

  if ((q->ptr = create_polar_decoder_ssc_s(frozen_set, code_size_log, frozen_set_size)) == NULL) {
    ERROR("create_polar_decoder_ssc_s failed\n");
    free_ssc_s(q);
    return -1;
  }
  return 0;
}

/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint8_t LLR inputs. */
static int init_ssc_c(srslte_polar_decoder_t* q, uint16_t* frozen_set, uint16_t code_size_log, uint16_t frozen_set_size)
{
  q->decode_c = decode_ssc_c;
  q->free     = free_ssc_c;

  if ((q->ptr = create_polar_decoder_ssc_c(frozen_set, code_size_log, frozen_set_size)) == NULL) {
    ERROR("create_polar_decoder_ssc_c failed\n");
    free_ssc_c(q);
    return -1;
  }
  return 0;
}

#ifdef LV_HAVE_AVX2
/*! Initializes a polar decoder structure to use the SSC polar decoder algorithm with uint8_t LLR inputs and AVX2
 * instructions. */
static int
init_ssc_c_avx2(srslte_polar_decoder_t* q, uint16_t* frozen_set, uint16_t code_size_log, uint16_t frozen_set_size)
{
  q->decode_c = decode_ssc_c_avx2;
  q->free     = free_ssc_c_avx2;

  if ((q->ptr = create_polar_decoder_ssc_c_avx2(frozen_set, code_size_log, frozen_set_size)) == NULL) {
    ERROR("create_polar_decoder_ssc_c failed\n");
    free_ssc_c_avx2(q);
    return -1;
  }
  return 0;
}
#endif

int srslte_polar_decoder_init(srslte_polar_decoder_t*     q,
                              srslte_polar_decoder_type_t type,
                              uint16_t                    code_size_log,
                              uint16_t*                   frozen_set,
                              uint16_t                    frozen_set_size)
{
  switch (type) {
    case SRSLTE_POLAR_DECODER_SSC_F:
      return init_ssc_f(q, frozen_set, code_size_log, frozen_set_size);
    case SRSLTE_POLAR_DECODER_SSC_S:
      return init_ssc_s(q, frozen_set, code_size_log, frozen_set_size);
    case SRSLTE_POLAR_DECODER_SSC_C:
      return init_ssc_c(q, frozen_set, code_size_log, frozen_set_size);
#ifdef LV_HAVE_AVX2
    case SRSLTE_POLAR_DECODER_SSC_C_AVX2:
      return init_ssc_c_avx2(q, frozen_set, code_size_log, frozen_set_size);
#endif
    default:
      ERROR("Decoder not implemented\n");
      return -1;
  }
  return 0;
}

void srslte_polar_decoder_free(srslte_polar_decoder_t* q)
{
  if (q->free) {
    q->free(q);
  }
  memset(q, 0, sizeof(srslte_polar_decoder_t));
}

int srslte_polar_decoder_decode_f(srslte_polar_decoder_t* q, const float* llr, uint8_t* data_decoded)
{
  return q->decode_f(q, llr, data_decoded);
}

int srslte_polar_decoder_decode_s(srslte_polar_decoder_t* q, const int16_t* llr, uint8_t* data_decoded)
{
  return q->decode_s(q, llr, data_decoded);
}

int srslte_polar_decoder_decode_c(srslte_polar_decoder_t* q, const int8_t* llr, uint8_t* data_decoded)
{
  return q->decode_c(q, llr, data_decoded);
}
