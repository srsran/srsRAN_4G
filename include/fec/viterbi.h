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

#ifndef VITERBI_
#define VITERBI_

#include <stdbool.h>

typedef enum {
	viterbi_27, viterbi_29, viterbi_37, viterbi_39
}viterbi_type_t;

typedef struct {
	void *ptr;
	int R;
	int K;
	unsigned int framebits;
	bool tail_biting;
	int poly[3];
	int (*decode) (void*, float*, char*);
	void (*free) (void*);
}viterbi_t;

int viterbi_init(viterbi_t *q, viterbi_type_t type, int poly[3], int framebits, bool tail_bitting);
void viterbi_free(viterbi_t *q);
int viterbi_decode(viterbi_t *q, float *symbols, char *data);


/* High-level API */
typedef struct {
	viterbi_t obj;
	struct viterbi_init {
		int rate;
		int constraint_length;
		int tail_bitting;
		int generator_0;
		int generator_1;
		int generator_2;
		int frame_length;
	} init;
	float *input;
	int in_len;
	char *output;
	int out_len;
}viterbi_hl;

int viterbi_initialize(viterbi_hl* h);
int viterbi_work(viterbi_hl* hl);
int viterbi_stop(viterbi_hl* h);

#endif
