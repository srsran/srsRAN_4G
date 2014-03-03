/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "lte/fec/convcoder.h"
#include "parity.h"

int convcoder_encode(convcoder_t *q, char *input, char *output) {
	unsigned int sr;
	int i,j;
	int len = q->tail_biting ? q->framelength : (q->framelength + q->K - 1);

	if (q->tail_biting) {
		sr = 0;
		for (i=q->framelength - q->K + 1; i<q->framelength; i++) {
			sr = (sr << 1) | (input[i] & 1);
		}
	} else {
		sr = 0;
	}
	//printf("Start state %d\n", sr);
	for (i = 0; i < len; i++) {
		int bit = (i < q->framelength) ? (input[i] & 1) : 0;
		sr = (sr << 1) | bit;
		for (j=0;j<q->R;j++) {
			output[q->R * i + j] = parity(sr & q->poly[j]);
		}
		//printf("%3d - sr=%u\n", i, sr%64);
	}

	return q->R*len;
}



int convcoder_initialize(convcoder_hl* h) {
	return 0;
}

int convcoder_work(convcoder_hl* hl) {

	hl->obj.K = hl->ctrl_in.constraint_length;
	hl->obj.R = hl->ctrl_in.rate;
	hl->obj.framelength = hl->in_len;
	hl->obj.poly[0] = hl->ctrl_in.generator_0;
	hl->obj.poly[1] = hl->ctrl_in.generator_1;
	hl->obj.poly[2] = hl->ctrl_in.generator_2;
	hl->obj.tail_biting = hl->ctrl_in.tail_bitting?true:false;
	hl->out_len = convcoder_encode(&hl->obj, hl->input, hl->output);
	return 0;
}

int convcoder_stop(convcoder_hl* h) {
	return 0;
}
