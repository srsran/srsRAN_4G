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

char *input_file_name = NULL;
int nof_frames=100;
float corr_peak_threshold=30;
int force_N_id_2=-1;

FILE *fmatlab;

#define NOF_PORTS 2
#define FLEN	9600

filesource_t fsrc;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS_CTRL];
pbch_t pbch;
lte_fft_t fft;
chest_t chest;
sync_t synch;
cfo_t cfocorr;

void usage(char *prog) {
	printf("Usage: %s [onlt] -i input_file\n", prog);
	printf("\t-n number of frames [Default %d]\n", nof_frames);
	printf("\t-t correlation threshold [Default %g]\n", corr_peak_threshold);
	printf("\t-v [set verbose to debug, default none]\n");
	printf("\t-f force_N_id_2 [Default %d]\n", force_N_id_2);
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "intvf")) != -1) {
		switch(opt) {
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'n':
			nof_frames = atoi(argv[optind]);
			break;
		case 't':
			corr_peak_threshold = atof(argv[optind]);
			break;
		case 'v':
			verbose++;
			break;
		case 'f':
			force_N_id_2 = atoi(argv[optind]);
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
	if (!input_file_name) {
		usage(argv[0]);
		exit(-1);
	}
}

int base_init() {
	int i;

	if (filesource_init(&fsrc, input_file_name, COMPLEX_FLOAT_BIN)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		exit(-1);
	}

	fmatlab = fopen("output.m", "w");
	if (!fmatlab) {
		perror("fopen");
		return -1;
	}

	input_buffer = malloc(FLEN * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		exit(-1);
	}

	fft_buffer = malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
	if (!fft_buffer) {
		perror("malloc");
		return -1;
	}

	for (i=0;i<MAX_PORTS_CTRL;i++) {
		ce[i] = malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
		if (!ce[i]) {
			perror("malloc");
			return -1;
		}
	}

	if (cfo_init(&cfocorr, FLEN)) {
		fprintf(stderr, "Error initiating CFO\n");
		return -1;
	}

	if (chest_init(&chest, LINEAR, CPNORM, 6, NOF_PORTS)) {
		fprintf(stderr, "Error initializing equalizer\n");
		return -1;
	}

	if (lte_fft_init(&fft, CPNORM, 6)) {
		fprintf(stderr, "Error initializing FFT\n");
		return -1;
	}

	DEBUG("Memory init OK\n",0);
	return 0;
}

void base_close() {
	int i;

	filesource_free(&fsrc);
	fclose(fmatlab);

	free(input_buffer);
	free(fft_buffer);

	sync_free(&synch);
	filesource_free(&fsrc);
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		free(ce[i]);
	}
	chest_free(&chest);
	lte_fft_free(&fft);

}



int mib_decoder_init(int cell_id) {

	if (chest_ref_LTEDL(&chest, cell_id)) {
		fprintf(stderr, "Error initializing reference signal\n");
		return -1;
	}

	if (pbch_init(&pbch, cell_id, CPNORM)) {
		fprintf(stderr, "Error initiating PBCH\n");
		return -1;
	}
	DEBUG("PBCH initiated cell_id=%d\n", cell_id);
	return 0;
}

int mib_decoder_run(cf_t *input, pbch_mib_t *mib) {
	int i, j;

	lte_fft_run(&fft, input, fft_buffer);

	fprintf(fmatlab, "outfft=");
	vec_sc_prod_cfc(fft_buffer, 1000.0, fft_buffer, 72 * 7);
	vec_fprint_c(fmatlab, fft_buffer, CP_NSYMB(CPNORM) * 6 * RE_X_RB);
	fprintf(fmatlab, ";\n");
	vec_sc_prod_cfc(fft_buffer, 0.001, fft_buffer, 	72 * 7);

	/* Get channel estimates for each port */
	for (i=0;i<NOF_PORTS;i++) {
		chest_ce_slot_port(&chest, fft_buffer, ce[i], 1, i);
		chest_fprint(&chest, fmatlab, 1, i);
		for (j=0;j<CP_NSYMB(CPNORM);j++) {
			fprintf(fmatlab, "ce%d(%d,:)=", i, j + 1);
			vec_fprint_c(fmatlab, &ce[i][j * 6 * RE_X_RB], 6 * RE_X_RB);
		}
	}

	INFO("Decoding PBCH\n", 0);
	return pbch_decode(&pbch, fft_buffer, ce, 6, 1, mib);
}

int get_samples(int length, int offset) {
	int n = 0;
	if (length != -1 && offset != -1) {
		while(n < length) {
			INFO("Reading %d samples offset=%d\n", length - n, offset + n);
			n = filesource_read(&fsrc, &input_buffer[offset + n], length - n);
			if (n == -1) {
				fprintf(stderr, "Error reading %d samples from file\n", length - n);
				break;
			} else if (n == 0) {
				printf("End of file\n");
				return -1;
			}
		}
		return n;
	} else {
		return -1;
	}
}

enum radio_state { DONE, SYNC, MIB};

int main(int argc, char **argv) {
	enum radio_state state;
	int mib_idx;
	int mib_attempts;
	pbch_mib_t mib;
	int cell_id, slot_id;
	int frame_cnt;
	float cfo;
	int read_samples;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (base_init()) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	if (sync_init(&synch, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}

	sync_force_N_id_2(&synch, force_N_id_2);
	sync_set_threshold(&synch, corr_peak_threshold);
	sync_pss_det_peakmean(&synch);

	state = SYNC;
	mib_idx = 0;
	mib_attempts = 0;
	frame_cnt = -1;
	cfo = 0.0;
	read_samples = FLEN;
	slot_id = 0;

	printf("\n\n-- Initiating MIB search --\n\n");

	while(frame_cnt < nof_frames) {
		if (get_samples(read_samples, 0) == -1) {
			fprintf(stderr, "Error reading %d samples\n", FLEN);
			break;
		}

		INFO("Correcting CFO=%.4f\n", cfo);
		cfo_correct(&cfocorr, input_buffer, -cfo/128);

		switch(state) {
		case SYNC:
			INFO("State Sync, Slot idx=%d\n", frame_cnt);
			mib_idx = sync_run(&synch, input_buffer);
			if (mib_idx != -1) {
				cell_id = sync_get_cell_id(&synch);
				cfo = sync_get_cfo(&synch);
				slot_id = sync_get_slot_id(&synch);
				read_samples = 0;
				state = MIB;
				if (mib_decoder_init(cell_id)) {
					fprintf(stderr, "Error initiating MIB decoder\n");
					exit(-1);
				}
				INFO("SYNC done, cell_id=%d mib_idx=%d frame_idx=%d\n", cell_id, mib_idx, frame_cnt);
			}
			break;
		case MIB:
			if (!read_samples) {
				read_samples = FLEN;
			}
			INFO("State MIB, frame idx=%d MIB offset=%d slot_id=%d\n", frame_cnt, mib_idx, slot_id);
			if (slot_id == 0) {
				if (mib_decoder_run(&input_buffer[mib_idx], &mib)) {
					INFO("MIB detected attempt=%d\n", mib_attempts+1);
					state = DONE;
				} else {
					INFO("MIB not detected attempt=%d\n", mib_attempts+1);
				}
				mib_attempts++;
				slot_id = 10;
			} else {
				slot_id = 0;
			}
			break;
		case DONE:
			INFO("State Done after %d frames\n", frame_cnt);
			pbch_mib_fprint(stdout, &mib);
			frame_cnt = nof_frames;
			printf("Done\n");
			break;
		}
		frame_cnt++;
	}

	base_close();

	printf("Exit\n");
	exit(0);
}
