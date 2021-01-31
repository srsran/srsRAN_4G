/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSLTE_PRIMES_H
#define SRSLTE_PRIMES_H

#include "srslte/config.h"
#include <stdint.h>

/**
 * @brief Finds the smallest prime number greater than n
 * @param[in] n Provide the number
 * @return A prime number below 1193, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_prime_greater_than(uint32_t n);

/**
 * @brief Finds the biggest prime number lesser than n
 * @attention the maximum prime number it can return is 1193
 * @param[in] n Provide the number
 * @return A prime number below 1193, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_prime_lower_than(uint32_t n);

#endif // SRSLTE_PRIMES_H
