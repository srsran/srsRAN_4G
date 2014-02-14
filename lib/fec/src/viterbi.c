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

#include "fec/viterbi.h"
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
	case viterbi_37:
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


int viterbi_initialize(viterbi_hl* h) {
	int poly[3];
	viterbi_type_t type;
	if (h->init.rate == 2) {
		if (h->init.constraint_length == 7) {
			type = viterbi_27;
		} else if (h->init.constraint_length == 9) {
			type = viterbi_29;
		} else {
			fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
					h->init.constraint_length);
			return -1;
		}
	} else if (h->init.rate == 3) {
		if (h->init.constraint_length == 7) {
			type = viterbi_37;
		} else if (h->init.constraint_length == 9) {
			type = viterbi_39;
		} else {
			fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
					h->init.constraint_length);
			return -1;
		}
	} else {
		fprintf(stderr, "Unsupported decoder %d/%d\n", h->init.rate,
				h->init.constraint_length);
		return -1;
	}
	poly[0] = h->init.generator_0;
	poly[1] = h->init.generator_1;
	poly[2] = h->init.generator_2;
	return viterbi_init(&h->obj, type, poly, h->init.frame_length,
			h->init.tail_bitting?true:false);
}

int viterbi_work(viterbi_hl* hl) {
	if (hl->in_len != hl->init.frame_length) {
		fprintf(stderr, "Expected input length %d but got %d\n", hl->init.frame_length, hl->in_len);
		return -1;
	}
	return viterbi_decode(&hl->obj, hl->input, hl->output);
}

int viterbi_stop(viterbi_hl* h) {
	viterbi_free(&h->obj);
	return 0;
}
