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

#ifndef DEMOD_SOFT_
#define DEMOD_SOFT_

#include <complex.h>
#include <stdint.h>

#include "modem_table.h"

enum alg { EXACT, APPROX };

typedef struct {
	float sigma;			// noise power
	enum alg alg_type;		// soft demapping algorithm (EXACT or APPROX)
	modem_table_t *table;	// symbol mapping table (see modem_table.h)
}demod_soft_t;

void demod_soft_init(demod_soft_t *q);
void demod_soft_table_set(demod_soft_t *q, modem_table_t *table);
void demod_soft_alg_set(demod_soft_t *q, enum alg alg_type);
void demod_soft_sigma_set(demod_soft_t *q, float sigma);
int demod_soft_demodulate(demod_soft_t *q, const cf* symbols, float* llr, int nsymbols);


/* High-level API */
typedef struct {
	demod_soft_t obj;
	modem_table_t table;

	struct demod_soft_init {
		enum modem_std std;		// symbol mapping standard (see modem_table.h)
	} init;

	const cf* input;
	int in_len;

	struct demod_soft_ctrl_in {
		float sigma;			// Estimated noise variance
		enum alg alg_type;		// soft demapping algorithm (EXACT or APPROX)
	}ctrl_in;

	float* output;
	int *out_len;

}demod_soft_hl;

int demod_soft_initialize(demod_soft_hl* hl);
int demod_soft_work(demod_soft_hl* hl);
int demod_soft_stop(demod_soft_hl* hl);


#endif
