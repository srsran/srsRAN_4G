/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef _TC_INTERL_H
#define _TC_INTERL_H

#include "liblte/config.h"

typedef struct LIBLTE_API {
  uint32_t *forward;
  uint32_t *reverse;
  uint32_t max_long_cb;
} tc_interl_t;

LIBLTE_API int tc_interl_LTE_gen(tc_interl_t *h, uint32_t long_cb);
LIBLTE_API int tc_interl_UMTS_gen(tc_interl_t *h, uint32_t long_cb);

LIBLTE_API int tc_interl_init(tc_interl_t *h, uint32_t max_long_cb);
LIBLTE_API void tc_interl_free(tc_interl_t *h);

#endif
