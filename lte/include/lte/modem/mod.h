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


#ifndef MOD_
#define MOD_

#include <complex.h>
#include <stdint.h>

#include "modem_table.h"

typedef _Complex float cf_t;

int mod_modulate(modem_table_t* table, const char *bits, cf_t* symbols, int nbits);

/* High-level API */
typedef struct {
	modem_table_t obj;
	struct mod_init {
		enum modem_std std;	// symbol mapping standard (see modem_table.h)
	} init;

	const char* input;
	int in_len;

	cf_t* output;
	int out_len;
}mod_hl;

int mod_initialize(mod_hl* hl);
int mod_work(mod_hl* hl);
int mod_stop(mod_hl* hl);

#endif
