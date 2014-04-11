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
