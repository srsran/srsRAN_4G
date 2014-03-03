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


/* Functions that generate the test data fed into the DSP modules being developed */
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

#include <skeleton.h>
#include <params.h>

#define INCLUDE_DEFS_ONLY
#include "template.h"

int offset=0;

/**
 *  Generates input signal. VERY IMPORTANT to fill length vector with the number of
 * samples that have been generated.
 * @param inp Input interface buffers. Data from other interfaces is stacked in the buffer.
 * Use in(ptr,idx) to access the address.
 *
 * @param lengths Save on n-th position the number of samples generated for the n-th interface
 */
int generate_input_signal(void *in, int *lengths)
{
	int i;
	input_t *input = in;
	int block_length;
	pmid_t blen_id;

	blen_id = param_id("block_length");
	if (!blen_id) {
		moderror("Parameter block_length not found\n");
		return -1;
	}
	if (!param_get_int(blen_id,&block_length)) {
		moderror("Getting integer parameter block_length\n");
		return -1;
	}

	modinfo_msg("Parameter block_length is %d\n",block_length);


	/** HERE INDICATE THE LENGTH OF THE SIGNAL */
	lengths[0] = block_length;

	for (i=0;i<block_length;i++) {
#ifdef GENERATE_COMPLEX
		__real__ input[i] = (float) ((i+offset)%(block_length));
		__imag__ input[i] = (float) ((block_length-i-1+offset)%(block_length));
#else
		input[i] = (i+offset)%(block_length);
#endif
	}
	offset++;
	return 0;
}
