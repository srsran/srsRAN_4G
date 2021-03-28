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

#ifndef SRSRAN_PRIMES_H
#define SRSRAN_PRIMES_H

#include "srsran/config.h"
#include <stdint.h>

/**
 * @brief Finds the smallest prime number greater than n
 * @param[in] n Provide the number
 * @return A prime number below 1193, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_prime_greater_than(uint32_t n);

/**
 * @brief Finds the biggest prime number lesser than n
 * @attention the maximum prime number it can return is 1193
 * @param[in] n Provide the number
 * @return A prime number below 1193, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_prime_lower_than(uint32_t n);

#endif // SRSRAN_PRIMES_H
