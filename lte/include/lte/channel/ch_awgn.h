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


#include <complex.h>

#ifndef CH_AWGN_
#define CH_AWGN_

typedef _Complex float cf_t;

void ch_awgn_c(const cf_t* input, cf_t* output, float variance, int buff_sz);
void ch_awgn_f(const float* x, float* y, float variance, int buff_sz);

/* High-level API */

typedef struct {
	const cf_t* input;
	int in_len;
	struct ch_awgn_ctrl_in {
		float variance;			// Noise variance
	} ctrl_in;

	cf_t* output;
	int out_len;
}ch_awgn_hl;

int ch_awgn_initialize(ch_awgn_hl* hl);
int ch_awgn_work(ch_awgn_hl* hl);
int ch_awgn_stop(ch_awgn_hl* hl);

#endif
