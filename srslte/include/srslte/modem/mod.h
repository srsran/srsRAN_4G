/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef MOD_
#define MOD_

#include <complex.h>
#include <stdint.h>

#include "srslte/config.h"
#include "modem_table.h"

typedef _Complex float cf_t;

SRSLTE_API int mod_modulate(modem_table_t* table, const uint8_t *bits, cf_t* symbols, uint32_t nbits);

/* High-level API */
typedef struct SRSLTE_API {
  modem_table_t obj;
  struct mod_init {
    lte_mod_t std;  // symbol mapping standard (see modem_table.h)
  } init;

  const uint8_t* input;
  int in_len;

  cf_t* output;
  int out_len;
}mod_hl;

SRSLTE_API int mod_initialize(mod_hl* hl);
SRSLTE_API int mod_work(mod_hl* hl);
SRSLTE_API int mod_stop(mod_hl* hl);

#endif // MOD_
