/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MOD_
#define MOD_

#include <complex.h>
#include <stdint.h>

#include "modem_table.h"

typedef _Complex float cf;

int mod_modulate(modem_table_t* table, const char *bits, cf* symbols, int nbits);

/* High-level API */
typedef struct {
	modem_table_t obj;
	struct mod_init {
		enum modem_std std;	// symbol mapping standard (see modem_table.h)
	} init;

	const char* input;
	int in_len;

	cf* output;
	int *out_len;
}mod_hl;

int mod_initialize(mod_hl* hl);
int mod_work(mod_hl* hl);
int mod_stop(mod_hl* hl);

#endif
