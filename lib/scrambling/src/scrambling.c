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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scrambling/scrambling.h"

/**
 * @ingroup Soft-bit Scrambling
 * Scrambles the input softbit-sequence (floats) with the scrambling
 * sequence (32-bit integers).
 *
 */
void scrambling_float(sequence_t *s, float *data) {
	scrambling_float_offset(s, data, 0, s->len);
}

int scrambling_float_offset(sequence_t *s, float *data, int offset, int len) {
	int i;
	if (len + offset > s->len) {
		return -1;
	}
	for (i = 0; i < len; i++) {
		data[i] = data[i]*(1-2*s->c[i+offset]);
	}
	return 0;
}

/**
 * @ingroup Bit Scrambling
 * Directly scrambles the input bit-sequence (char) with the scrambling
 * sequence.
 */
void scrambling_bit(sequence_t *s, char *data) {
	int i;

	for (i = 0; i < s->len; i++) {
		data[i] = (data[i] + s->c[i]) % 2;
	}
}

/** High-level API */

int compute_sequences(scrambling_hl* h) {

	switch (h->init.channel) {
	case PBCH:
		return sequence_pbch(&h->obj.seq[0], h->init.nof_symbols == CPNORM_NSYMB,
				h->init.cell_id);
	case PDSCH:
	case PCFICH:
	case PDCCH:
	case PMCH:
	case PUCCH:
		fprintf(stderr, "Not implemented\n");
		return -1;
	default:
		fprintf(stderr, "Invalid channel %d\n", h->init.channel);
		return -1;
	}
}

int scrambling_initialize(scrambling_hl* h) {

	bzero(&h->obj, sizeof(scrambling_t));

	return compute_sequences(h);
}

/** This function can be called in a subframe (1ms) basis for LTE */
int scrambling_work(scrambling_hl* hl) {
	int sf;
	if (hl->init.channel == PBCH) {
		sf = 0;
	} else {
		sf = hl->ctrl_in.subframe;
	}
	sequence_t *seq = &hl->obj.seq[sf];

	if (hl->init.hard) {
		memcpy(hl->output, hl->input, sizeof(char) * hl->in_len);
		scrambling_bit(seq, hl->output);
	} else {
		memcpy(hl->output, hl->input, sizeof(float) * hl->in_len);
		scrambling_float(seq, hl->output);
	}
	hl->out_len = hl->in_len;
	return 0;
}

int scrambling_stop(scrambling_hl* hl) {
	int i;
	for (i=0;i<NSUBFRAMES_X_FRAME;i++) {
		sequence_free(&hl->obj.seq[i]);
	}
	return 0;
}

