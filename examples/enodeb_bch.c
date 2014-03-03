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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

#define ENABLE_UHD

#ifdef ENABLE_UHD
#include "cuhd.h"
void *uhd;
#endif

char *output_file_name = NULL;
int nof_frames=-1;
int cell_id = 1;
int nof_prb = 6;


filesink_t fsink;
lte_fft_t ifft;
pbch_t pbch;

cf_t *slot_buffer = NULL, *output_buffer = NULL;
int slot_n_re, slot_n_samples;

#define cuhd_FREQ		2680000000
#define cuhd_SAMP_FREQ	1920000
#define cuhd_GAIN		10
#define cuhd_AMP			0.25
#define cuhd_ARGS		"addr=192.168.10.3"

void usage(char *prog) {
	printf("Usage: %s [ncvp]\n", prog);
	printf("\t-o output_file [Default USRP]\n");
	printf("\t-n number of frames [Default %d]\n", nof_frames);
	printf("\t-c cell id [Default %d]\n", cell_id);
	printf("\t-p nof_prb [Default %d]\n", nof_prb);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "oncpv")) != -1) {
		switch(opt) {
		case 'o':
			output_file_name = argv[optind];
			break;
		case 'n':
			nof_frames = atoi(argv[optind]);
			break;
		case 'p':
			nof_prb = atoi(argv[optind]);
			break;
		case 'c':
			cell_id = atoi(argv[optind]);
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
#ifndef ENABLE_UHD
	if (!output_file_name) {
		usage(argv[0]);
		exit(-1);
	}
#endif
}

void base_init() {
	/* init memory */
	slot_buffer = malloc(sizeof(cf_t) * slot_n_re);
	if (!slot_buffer) {
		perror("malloc");
		exit(-1);
	}
	output_buffer = malloc(sizeof(cf_t) * slot_n_samples);
	if (!output_buffer) {
		perror("malloc");
		exit(-1);
	}
	/* open file or USRP */
	if (output_file_name) {
		if (filesink_init(&fsink, output_file_name, COMPLEX_FLOAT_BIN)) {
			fprintf(stderr, "Error opening file %s\n", output_file_name);
			exit(-1);
		}
	} else {
#ifdef ENABLE_UHD
		printf("Opening UHD device...\n");
		if (cuhd_open(cuhd_ARGS,&uhd)) {
			fprintf(stderr, "Error opening uhd\n");
			exit(-1);
		}
#else
		exit(-1); // not supposed to be here
#endif
	}
	/* create ifft object */
	if (lte_ifft_init(&ifft, CPNORM, nof_prb)) {
		fprintf(stderr, "Error creating iFFT object\n");
		exit(-1);
	}
	if (pbch_init(&pbch, cell_id, CPNORM)) {
		fprintf(stderr, "Error creating PBCH object\n");
		exit(-1);
	}
#ifdef ENABLE_MATLAB
	fmatlab = fopen("output.m", "w");
	if (!fmatlab) {
		perror("fopen");
		exit(-1);
	}
#endif
}

void base_free() {

	pbch_free(&pbch);

	lte_ifft_free(&ifft);

	if (slot_buffer) {
		free(slot_buffer);
	}
	if (output_buffer) {
		free(output_buffer);
	}
	if (output_file_name) {
		filesink_free(&fsink);
	} else {
#ifdef ENABLE_UHD
		cuhd_close(&uhd);
#endif
	}
#ifdef ENABLE_MATLAB
	fclose(fmatlab);
#endif
}

int main(int argc, char **argv) {
	int nf, ns, N_id_2;
	cf_t pss_signal[PSS_LEN];
	float sss_signal0[SSS_LEN]; // for subframe 0
	float sss_signal5[SSS_LEN]; // for subframe 5
	pbch_mib_t mib;
	refsignal_t refs[NSLOTS_X_FRAME];
	int i;
	cf_t *slot1_symbols[MAX_PORTS_CTRL];


#ifndef ENABLE_UHD
	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}
#endif

	parse_args(argc,argv);

	N_id_2 = cell_id%3;
	slot_n_re = CPNORM_NSYMB * nof_prb * RE_X_RB;
	slot_n_samples = SLOT_LEN_CPNORM(lte_symbol_sz(nof_prb));

	/* this *must* be called after setting slot_len_* */
	base_init();

	/* Generate PSS/SSS signals */
	pss_generate(pss_signal, N_id_2);
	sss_generate(sss_signal0, sss_signal5, cell_id);

	/* Generate CRS signals */
	for (i=0;i<NSLOTS_X_FRAME;i++) {
		if (refsignal_init_LTEDL(&refs[i], 0, i, cell_id, CPNORM, nof_prb)) {
			fprintf(stderr, "Error initiating CRS slot=%d\n", i);
			return -1;
		}
	}

	mib.nof_ports = 1;
	mib.nof_prb = 6;
	mib.phich_length = NORMAL;
	mib.phich_resources = R_1;
	mib.sfn = 0;

	for (i=0;i<MAX_PORTS_CTRL;i++) { // now there's only 1 port
		slot1_symbols[i] = slot_buffer;
	}

#ifdef ENABLE_UHD
	if (!output_file_name) {
		printf("Set TX rate: %.2f MHz\n", cuhd_set_tx_srate(uhd, cuhd_SAMP_FREQ)/1000000);
		printf("Set TX gain: %.1f dB\n", cuhd_set_tx_gain(uhd, cuhd_GAIN));
		printf("Set TX freq: %.2f MHz\n", cuhd_set_tx_freq(uhd, cuhd_FREQ)/1000000);
		cuhd_start_tx_stream(uhd);
	}
#endif

	nf = 0;

	while(nf<nof_frames || nof_frames == -1) {
		for (ns=0;ns<NSLOTS_X_FRAME;ns++) {
			bzero(slot_buffer, sizeof(cf_t) * slot_n_re);

			switch(ns) {
			case 0: // tx pss/sss
			case 10: // tx pss/sss
				pss_put_slot(pss_signal, slot_buffer, nof_prb, CPNORM);
				sss_put_slot(ns?sss_signal5:sss_signal0, slot_buffer, nof_prb, CPNORM);
				break;
			case 1: // tx pbch
				pbch_encode(&pbch, &mib, slot1_symbols, nof_prb, 1);
				break;
			default: // transmit zeros
				break;
			}

			refsignal_put(&refs[ns], slot_buffer);

			/* Transform to OFDM symbols */
			lte_ifft_run(&ifft, slot_buffer, output_buffer);

			/* send to file or usrp */
			if (output_file_name) {
				filesink_write(&fsink, output_buffer, slot_n_samples);
			} else {
#ifdef ENABLE_UHD
				vec_sc_prod_cfc(output_buffer, cuhd_AMP, output_buffer, slot_n_samples);
				cuhd_send(uhd, output_buffer, slot_n_samples, 1);
#endif
			}
		}
		mib.sfn=(mib.sfn+1)%1024;
		printf("SFN: %4d\r", mib.sfn);fflush(stdout);
		nf++;
	}

	base_free();

	printf("Done\n");
	exit(0);
}
