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

#ifndef SCRAMBLING_
#define SCRAMBLING_

#include "lte/sequence.h"
#include "lte/base.h"

/* Scrambling has no state */
void scrambling_bit(sequence_t *s, char *data);
void scrambling_float(sequence_t *s, float *data);
int scrambling_float_offset(sequence_t *s, float *data, int offset, int len);


/* High-level API */

/* channel integer values */
#define PDSCH			0	/* also PUSCH */
#define PCFICH			1
#define PDCCH			2
#define PBCH			3
#define PMCH			4
#define PUCCH			5

typedef struct {
	sequence_t seq[NSUBFRAMES_X_FRAME];
}scrambling_t;

typedef struct {
	scrambling_t obj;
	struct scrambling_init {
		int hard;
		int q;
		int cell_id;
		int nrnti;
		int nMBSFN;
		int channel;
		int nof_symbols;	// 7 normal 6 extended
	} init;
	void *input;			// input type may be char or float depending on hard
	int in_len;
	struct scrambling_ctrl_in {
		int subframe;
	} ctrl_in;
	void *output;
	int *out_len;
}scrambling_hl;

#endif
