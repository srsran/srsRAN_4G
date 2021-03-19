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

#ifndef SRSRAN_POLAR_INTERLEAVER_H
#define SRSRAN_POLAR_INTERLEAVER_H

#include "srsran/config.h"
#include <stdbool.h>
#include <stdint.h>

#define SRSRAN_POLAR_INTERLEAVER_K_MAX_IL 164

/**
 *  * @brief Implements generic Polar code interleaver as described in TS 38.212 V15.9.0 Section 5.3.1.1
 *
 * @attention The input and output data cannot be the same.
 *
 * @param in Input data pointer
 * @param out Output data pointer
 * @param S Data element size in bytes
 * @param K Number of elements
 * @param dir Set to true for encoder and false for decoder
 */
SRSRAN_API void srsran_polar_interleaver_run(const void* in, void* out, uint32_t S, uint32_t K, bool dir);

#define SRSRAN_POLAR_INTERLEAVE_GEN(NAME, TYPE)                                                                        \
  static inline void srsran_polar_interleaver_run_##NAME(const TYPE* in, void* out, uint32_t K, bool dir)              \
  {                                                                                                                    \
    srsran_polar_interleaver_run(in, out, (uint32_t)sizeof(TYPE), K, dir);                                             \
  }

/**
 * @brief Implements Polar code interleaver as described in TS 38.212 V15.9.0 Section 5.3.1.1
 *
 * @attention The input and output data cannot be the same.
 *
 * @param in unsigned 16 bit Input data
 * @param out unsigned 16 bit Output data
 * @param K Number of elements
 * @param dir Set to true for encoder and false for decoder
 */
SRSRAN_POLAR_INTERLEAVE_GEN(u16, uint16_t)

/**
 * @brief Implements Polar code interleaver as described in TS 38.212 V15.9.0 Section 5.3.1.1
 *
 * @attention The input and output data cannot be the same.
 *
 * @param in unsigned 8 bit Input data
 * @param out unsigned 8 bit Output data
 * @param K Number of elements
 * @param dir Set to true for encoder and false for decoder
 */
SRSRAN_POLAR_INTERLEAVE_GEN(u8, uint8_t)

#undef SRSRAN_POLAR_INTERLEAVE_GEN

#endif // SRSRAN_POLAR_INTERLEAVER_H
