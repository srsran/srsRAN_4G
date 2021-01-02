/**
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

#ifndef SRSLTE_POLAR_INTERLEAVER_H
#define SRSLTE_POLAR_INTERLEAVER_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

#define SRSLTE_POLAR_INTERLEAVER_K_MAX_IL 164

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
SRSLTE_API void srslte_polar_interleaver_run(const void* in, void* out, uint32_t S, uint32_t K, bool dir);

#define SRSLTE_POLAR_INTERLEAVE_GEN(NAME, TYPE)                                                                        \
  static inline void srslte_polar_interleaver_run_##NAME(const TYPE* in, void* out, uint32_t K, bool dir)              \
  {                                                                                                                    \
    srslte_polar_interleaver_run(in, out, (uint32_t)sizeof(TYPE), K, dir);                                             \
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
SRSLTE_POLAR_INTERLEAVE_GEN(u16, uint16_t)

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
SRSLTE_POLAR_INTERLEAVE_GEN(u8, uint8_t)

#undef SRSLTE_POLAR_INTERLEAVE_GEN

#endif // SRSLTE_POLAR_INTERLEAVER_H
