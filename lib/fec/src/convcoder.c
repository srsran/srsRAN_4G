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

/**@TODO frontend to FEC library if installed
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "fec/convcoder.h"
#include "parity.h"
#include "viterbi37.h"

#define DEB 0

int decode37(void *o, float *symbols, char *data) {
	viterbi_t *q = o;
	int i;
	int len = q->tail_biting ? q->framebits : (q->framebits + q->K - 1);
	float amp = 0;

	for (i=0;i<3*len;i++) {
		if (fabsf(symbols[i] > amp)) {
			amp = symbols[i];
		}
	}

	/* Decode it and make sure we get the right answer */
	/* Initialize Viterbi decoder */
	init_viterbi37_port(q->ptr, q->tail_biting?-1:0);

	/* Decode block */
	update_viterbi37_blk_port(q->ptr, symbols,q->framebits + q->K - 1, amp, len);

	/* Do Viterbi chainback */
	chainback_viterbi37_port(q->ptr, data, q->framebits, 0);

	return q->framebits;
}

void free37(void *o) {
	viterbi_t *q = o;
	delete_viterbi37_port(q->ptr);
}

int init37(viterbi_t *q, int poly[3], int framebits, bool tail_biting) {
	q->K = 7;
	q->R = 3;
	q->framebits = framebits;
	q->tail_biting = tail_biting;
	q->decode = decode37;
	q->free = free37;

	if ((q->ptr = create_viterbi37_port(poly, framebits, tail_biting)) == NULL) {
		fprintf(stderr, "create_viterbi37 failed\n");
		return -1;
	} else {
		return 0;
	}
}

int viterbi_init(viterbi_t *q, viterbi_type_t type, int poly[3], int framebits, bool tail_bitting) {
	switch(type) {
	case CONVCODER_37:
		return init37(q, poly, framebits, tail_bitting);
	default:
		fprintf(stderr, "Decoder not implemented\n");
		return -1;
	}
}

void viterbi_free(viterbi_t *q) {
	q->free(q);
}

/* symbols are real-valued */
int viterbi_decode(viterbi_t *q, float *symbols, char *data) {
	return q->decode(q, symbols, data);
}


int conv_encode(convcoder_t *q, char *input, char *output) {
	unsigned int sr;
	int i,j;
	int len = q->tail_biting ? q->framelength : (q->framelength + q->K - 1);

	if (q->tail_biting) {
		sr = 0;
		for (i=q->framelength - q->K + 1; i<q->framelength; i++) {
			if (DEB) printf("%3d: sr=%3d, bit=%d\n",i,sr&7,input[i]);
			sr = (sr << 1) | (input[i] & 1);
		}
	} else {
		sr = 0;
	}

	if (DEB) printf("state st=%d\n",sr&7);
	for (i = 0; i < len; i++) {
		int bit = (i < q->framelength) ? (input[i] & 1) : 0;
		sr = (sr << 1) | bit;
		if (DEB) printf("%d, ",input[i]);
		for (j=0;j<q->R;j++) {
			output[q->R * i + j] = parity(sr & q->poly[j]);
		}
	}
	if (DEB) printf("\n");
	if (DEB) printf("state fin=%u\n",sr&7);
	return q->R*len;
}

