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

#include <stdlib.h>
#include <strings.h>

#include "utils/bit.h"
#include "modem/mod.h"

/** Low-level API */

int mod_modulate(modem_table_t* q, const char *bits, cf* symbols, int nbits) {
	int i,j,idx;
	char *b_ptr=(char*) bits;
	j=0;
	for (i=0;i<nbits;i+=q->nbits_x_symbol) {
		idx = bit_unpack(&b_ptr,q->nbits_x_symbol);
		symbols[j] = q->symbol_table[idx];
		j++;
	}
	return j;
}


/* High-Level API */
int mod_initialize(mod_hl* hl) {
	modem_table_init(&hl->obj);
	if (modem_table_std(&hl->obj,hl->init.std,false)) {
		return -1;
	}

	return 0;
}

int mod_work(mod_hl* hl) {
	int ret = mod_modulate(&hl->obj,hl->input,hl->output,hl->in_len);
	hl->out_len = ret;
	return 0;
}

int mod_stop(mod_hl* hl) {
	modem_table_free(&hl->obj);
	return 0;
}


