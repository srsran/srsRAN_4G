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

#ifndef SRSRAN_SLIV_H
#define SRSRAN_SLIV_H

#include "srsran/config.h"
#include <inttypes.h>

SRSRAN_API void srsran_sliv_to_s_and_l(uint32_t N, uint32_t v, uint32_t* S, uint32_t* L);

SRSRAN_API uint32_t srsran_sliv_from_s_and_l(uint32_t N, uint32_t S, uint32_t L);

#endif // SRSRAN_SLIV_H
