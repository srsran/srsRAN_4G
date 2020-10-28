/*
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

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"

#ifndef SRSLTE_CBSEGM_H
#define SRSLTE_CBSEGM_H

#define SRSLTE_NOF_TC_CB_SIZES 188

typedef struct SRSLTE_API {
  uint32_t F;
  uint32_t C;
  uint32_t K1;
  uint32_t K2;
  uint32_t K1_idx;
  uint32_t K2_idx;
  uint32_t C1;
  uint32_t C2;
  uint32_t tbs;
} srslte_cbsegm_t;

SRSLTE_API int srslte_cbsegm(srslte_cbsegm_t* s, uint32_t tbs);

SRSLTE_API int srslte_cbsegm_cbsize(uint32_t index);

SRSLTE_API bool srslte_cbsegm_cbsize_isvalid(uint32_t size);

SRSLTE_API int srslte_cbsegm_cbindex(uint32_t long_cb);

#endif // SRSLTE_CBSEGM_H
