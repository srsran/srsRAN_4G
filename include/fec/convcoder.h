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

#ifndef CONVCODER_
#define CONVCODER_

#include <stdbool.h>



typedef struct {
	int R;
	int K;
	int poly[3];
	int framelength;
	bool tail_biting;
}convcoder_t;

int convcoder_encode(convcoder_t *q, char *input, char *output);


/* High-level API */
typedef struct {
	convcoder_t obj;
	struct convcoder_ctrl_in {
		int rate;
		int constraint_length;
		int tail_bitting;
		int generator_0;
		int generator_1;
		int generator_2;
		int frame_length;
	} ctrl_in;
	char *input;
	int in_len;
	char *output;
	int out_len;
}convcoder_hl;

int convcoder_initialize(convcoder_hl* h);
int convcoder_work(convcoder_hl* hl);
int convcoder_stop(convcoder_hl* h);

#endif
