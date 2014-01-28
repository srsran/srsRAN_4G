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
