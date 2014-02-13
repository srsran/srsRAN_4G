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
#include <complex.h>

#ifndef CH_AWGN_
#define CH_AWGN_

typedef _Complex float cf;

void ch_awgn(const cf* input, cf* output, float variance, int buff_sz);

/* High-level API */

typedef struct {
	const cf* input;
	int in_len;
	struct ch_awgn_ctrl_in {
		float variance;			// Noise variance
	} ctrl_in;

	cf* output;
	int out_len;
}ch_awgn_hl;

int ch_awgn_initialize(ch_awgn_hl* hl);
int ch_awgn_work(ch_awgn_hl* hl);
int ch_awgn_stop(ch_awgn_hl* hl);

#endif
