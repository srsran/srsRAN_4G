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

typedef enum {
	CONVCODER_27, CONVCODER_29, CONVCODER_37, CONVCODER_39
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

typedef struct {
	int R;
	int K;
	int poly[3];
	int framelength;
	bool tail_biting;
}convcoder_t;

int conv_encode(convcoder_t *q, char *input, char *output);

#endif
