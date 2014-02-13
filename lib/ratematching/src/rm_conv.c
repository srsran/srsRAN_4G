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
#include "ratematching/rm_conv.h"

#define NCOLS 32
#define NROWS_MAX NCOLS
#define RATE 3

unsigned char RM_PERM_CC[NCOLS] =
		{ 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31, 0, 16, 8,
				24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30 };
unsigned char RM_PERM_CC_INV[NCOLS] = { 16, 0, 24, 8, 20, 4, 28, 12, 18, 2, 26,
		10, 22, 6, 30, 14, 17, 1, 25, 9, 21, 5, 29, 13, 19, 3, 27, 11, 23, 7,
		31, 15 };


/* Undoes Convolutional Code Rate Matching.
 * 3GPP TS 36.212 v10.1.0 section 5.1.4.2
 */
int rm_conv_rx(float *input, float *output, int in_len, int out_len) {

	int nrows, ndummy, K_p;
	int i, j, k;
	int d_i, d_j;

	float tmp[RATE * NCOLS * NROWS_MAX];

	nrows = (int) (out_len / RATE - 1) / NCOLS + 1;
	if (nrows > NROWS_MAX) {
		fprintf(stderr, "Output too large. Max output length is %d\n", RATE * NCOLS * NROWS_MAX);
		return -1;
	}
	K_p = nrows * NCOLS;

	ndummy = K_p - out_len / RATE;
	if (ndummy < 0) {
		ndummy = 0;
	}

	for (i = 0; i < RATE * K_p; i++) {
		tmp[i] = RX_NULL;
	}

	/* Undo bit collection. Account for dummy bits */
	k = 0;
	j = 0;
	while (k < in_len) {
		d_i = (j % K_p) / nrows;
		d_j = (j % K_p) % nrows;

		if (d_j * NCOLS + RM_PERM_CC[d_i] >= ndummy) {
			if (tmp[j] == RX_NULL) {
				tmp[j] = input[k];
			} else if (input[k] != RX_NULL) {
				tmp[j] += input[k]; /* soft combine LLRs */
			}
			k++;
		}
		j++;
		if (j == RATE * K_p) {
			j = 0;
		}
	}

	/* interleaving and bit selection */
	for (i = 0; i < out_len / RATE; i++) {
		d_i = (i + ndummy) / NCOLS;
		d_j = (i + ndummy) % NCOLS;
		for (j = 0; j < RATE; j++) {
			output[i * RATE + j] = tmp[K_p * j
					+ RM_PERM_CC_INV[d_j] * nrows + d_i];
		}
	}
	return 0;
}

/** High-level API */

int rm_conv_initialize(rm_conv_hl* h) {

	return 0;
}

/** This function can be called in a subframe (1ms) basis */
int rm_conv_work(rm_conv_hl* hl) {
	if (hl->init.direction) {
		//rm_conv_tx(hl->input, hl->output, hl->in_len, hl->ctrl_in.S);
		hl->out_len = hl->ctrl_in.S;
	} else {
		rm_conv_rx(hl->input, hl->output, hl->in_len, hl->ctrl_in.E);
		hl->out_len = hl->ctrl_in.E;
	}
	return 0;
}

int rm_conv_stop(rm_conv_hl* hl) {
	return 0;
}

