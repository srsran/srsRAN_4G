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

/*
 * This file has been automatically generated from -name-
 */

#include <stdio.h>
#include <oesr.h>
#include <params.h>
#include <skeleton.h>

#include "-name-.h"

-name-_hl -name-;

--input_parameters_handlers--

int out_len[NOF_OUTPUT_ITF];

int initialize() {

	/* Initialization Parameters */
	--init_parameters--

	/* Input Control Parameters */
	--input_parameters_getid--

	/* Initialization function */
	return -name-_initialize(&-name-);
}


int work(void **inp, void **out) {
	int i,n;
#if NOF_INPUTS>1
	for (i=0;i<NOF_INPUT_ITF;i++) {
		-name-.input[i] = inp[i];
		-name-.in_len[i] = get_input_samples(i)
	}
#elif NOF_INPUTS == 1
	-name-.input = inp[0];
	-name-.in_len = get_input_samples(0);
#endif

#if NOF_OUTPUTS>1
	for (i=0;i<NOF_OUTPUT_ITF;i++) {
		-name-.output[i] = out[i];
		-name-.out_len = &out_len[i];
	}
#elif NOF_OUTPUTS == 1
	-name-output = out[0];
	-name-.out_len = &out_len[0];
#endif

	/* Get input parameters */
	--input_parameters--

	/* call work */
	n = -name-_work(&-name-);

	/* Set output nof_samples */
	for (i=0;i<NOF_OUTPUT_ITF;i++) {
		set_output_samples(i,out_len[i]);
		-name-.out_len = &out_len[i];
	}

	/* Set output parameters */

	return n;
}

int stop() {
	-name-_destroy(&-name-);
	return 0;
}
