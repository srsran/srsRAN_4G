/*
 * This file has been automatically generated from binsource
 */

#include <stdio.h>
#include <oesr.h>
#include <params.h>
#include <skeleton.h>

#include "binsource.h"

binsource_hl binsource;

pmid_t nbits_id;

int out_len[NOF_OUTPUT_ITF];

int initialize() {

	/* Initialization Parameters */
	if (param_get_int_name("cache_seq_nbits", &binsource.init.cache_seq_nbits)) {
		binsource.init.cache_seq_nbits = 2;
	}
	if (param_get_int_name("seed", &binsource.init.seed)) {
		binsource.init.seed = 0;
	}

	/* Input Control Parameters */
	nbits_id = param_id("nbits");

	/* Initialization function */
	return binsource_initialize(&binsource);
}


int work(void **inp, void **out) {
	int i,n;
#if NOF_INPUTS>1
	for (i=0;i<NOF_INPUT_ITF;i++) {
		binsource.input[i] = inp[i];
		binsource.in_len[i] = get_input_samples(i)
	}
#elif NOF_INPUTS == 1
	binsource.input = inp[0];
	binsource.in_len = get_input_samples(0);
#endif

#if NOF_OUTPUTS>1
	for (i=0;i<NOF_OUTPUT_ITF;i++) {
		binsource.output[i] = out[i];
		binsource.out_len = &out_len[i];
	}
#elif NOF_OUTPUTS == 1
	binsourceoutput = out[0];
	binsource.out_len = &out_len[0];
#endif

	/* Get input parameters */
	if (param_get_int(nbits_id, &binsource.ctrl_in.nbits) != 1) {
		moderror("Error getting parameter nbits\n");
		return -1;
	}

	/* call work */
	n = binsource_work(&binsource);

	/* Set output nof_samples */
	for (i=0;i<NOF_OUTPUT_ITF;i++) {
		set_output_samples(i,out_len[i]);
		binsource.out_len = &out_len[i];
	}

	/* Set output parameters */

	return n;
}

int stop() {
	binsource_destroy(&binsource);
	return 0;
}
