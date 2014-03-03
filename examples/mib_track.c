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
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#ifndef DISABLE_UHD
#include "cuhd.h"
#endif

#include "lte.h"
#include "plot.h"

#define MHZ 			1000000
#define SAMP_FREQ 		1920000
#define FLEN 			9600
#define FLEN_PERIOD		0.005

#define NOF_PORTS 2

float find_threshold = 40.0, track_threshold = 8.0;
int max_track_lost = 9, nof_frames = -1;
int track_len=300;
char *input_file_name = NULL;
int disable_plots = 0;

float uhd_freq = 2400000000.0, uhd_gain = 20.0;
char *uhd_args = "";

filesource_t fsrc;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS_CTRL];
pbch_t pbch;
lte_fft_t fft;
chest_t chest;
sync_t sfind, strack;
cfo_t cfocorr;

plot_real_t poutfft;
plot_complex_t pce;
plot_scatter_t pscatrecv, pscatequal;

void *uhd;

enum sync_state {FIND, TRACK};

void usage(char *prog) {
	printf("Usage: %s [iagfndv]\n", prog);
	printf("\t-i input_file [Default use USRP]\n");
	printf("\t-a UHD args [Default %s]\n", uhd_args);
	printf("\t-g UHD RX gain [Default %.2f dB]\n", uhd_gain);
	printf("\t-f UHD RX frequency [Default %.1f MHz]\n", uhd_freq/1000000);
	printf("\t-n nof_frames [Default %d]\n", nof_frames);
	printf("\t-d disable plots [Default enabled]\n");
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "iagfndv")) != -1) {
		switch(opt) {
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'a':
			uhd_args = argv[optind];
			break;
		case 'g':
			uhd_gain = atof(argv[optind]);
			break;
		case 'f':
			uhd_freq = atof(argv[optind]);
			break;
		case 'n':
			nof_frames = atoi(argv[optind]);
			break;
		case 'd':
			disable_plots = 1;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
}

void init_plots() {
	plot_init();
	plot_real_init(&poutfft);
	plot_real_setTitle(&poutfft, "Output FFT - Magnitude");
	plot_real_setLabels(&poutfft, "Index", "dB");
	plot_real_setYAxisScale(&poutfft, -60, 0);
	plot_real_setXAxisScale(&poutfft, 1, 504);

	plot_complex_init(&pce);
	plot_complex_setTitle(&pce, "Channel Estimates");
	plot_complex_setYAxisScale(&pce, Ip, -0.01, 0.01);
	plot_complex_setYAxisScale(&pce, Q, -0.01, 0.01);
	plot_complex_setYAxisScale(&pce, Magnitude, 0, 0.01);
	plot_complex_setYAxisScale(&pce, Phase, -M_PI, M_PI);

	plot_scatter_init(&pscatrecv);
	plot_scatter_setTitle(&pscatrecv, "Received Symbols");
	plot_scatter_setXAxisScale(&pscatrecv, -0.01, 0.01);
	plot_scatter_setYAxisScale(&pscatrecv, -0.01, 0.01);

	plot_scatter_init(&pscatequal);
	plot_scatter_setTitle(&pscatequal, "Equalized Symbols");
	plot_scatter_setXAxisScale(&pscatequal, -1, 1);
	plot_scatter_setYAxisScale(&pscatequal, -1, 1);

}

int base_init(int frame_length) {
	int i;

	if (!disable_plots) {
		init_plots();
	}

	if (input_file_name) {
		if (filesource_init(&fsrc, input_file_name, COMPLEX_FLOAT_BIN)) {
			return -1;
		}
	} else {
		/* open UHD device */
	#ifndef DISABLE_UHD
		printf("Opening UHD device...\n");
		if (cuhd_open(uhd_args,&uhd)) {
			fprintf(stderr, "Error opening uhd\n");
			return -1;
		}
	#else
		printf("Error UHD not configured. Select an input file\n");
		return -1;
	#endif
	}

	input_buffer = (cf_t*) malloc(frame_length * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		return -1;
	}

	fft_buffer = (cf_t*) malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
	if (!fft_buffer) {
		perror("malloc");
		return -1;
	}

	for (i=0;i<MAX_PORTS_CTRL;i++) {
		ce[i] = (cf_t*) malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
		if (!ce[i]) {
			perror("malloc");
			return -1;
		}
	}
	if (sync_init(&sfind, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		return -1;
	}
	if (sync_init(&strack, track_len)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		return -1;
	}
	if (chest_init(&chest, LINEAR, CPNORM, 6, NOF_PORTS)) {
		fprintf(stderr, "Error initializing equalizer\n");
		return -1;
	}

	if (cfo_init(&cfocorr, FLEN)) {
		fprintf(stderr, "Error initiating CFO\n");
		return -1;
	}

	if (lte_fft_init(&fft, CPNORM, 6)) {
		fprintf(stderr, "Error initializing FFT\n");
		return -1;
	}

	return 0;
}

void base_free() {
	int i;

	if (input_file_name) {
		filesource_free(&fsrc);
	} else {
	#ifndef DISABLE_UHD
		cuhd_close(&uhd);
	#endif
	}

	sync_free(&sfind);
	sync_free(&strack);
	lte_fft_free(&fft);
	chest_free(&chest);
	cfo_free(&cfocorr);

	free(input_buffer);
	free(fft_buffer);
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		free(ce[i]);
	}
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
	int i, n;
	lte_fft_run(&fft, input, fft_buffer);
	float tmp[72*7];

	/* Get channel estimates for each port */
	for (i=0;i<NOF_PORTS;i++) {
		chest_ce_slot_port(&chest, fft_buffer, ce[i], 1, i);
	}

	DEBUG("Decoding PBCH\n", 0);
	n = pbch_decode(&pbch, fft_buffer, ce, 6, 1, mib);

	if (!disable_plots) {
		for (i=0;i<72*7;i++) {
			tmp[i] = 10*log10f(cabsf(fft_buffer[i]));
		}
		plot_real_setNewData(&poutfft, tmp, 72*7);
		plot_complex_setNewData(&pce, ce[0], 72*7);
		plot_scatter_setNewData(&pscatrecv, pbch.pbch_symbols[0], pbch.nof_symbols);
		if (n) {
			plot_scatter_setNewData(&pscatequal, pbch.pbch_d, pbch.nof_symbols);
		}
	}

	return n;
}


int main(int argc, char **argv) {
	int frame_cnt;
	int cell_id;
	int find_idx, track_idx, last_found;
	enum sync_state state;
	int nslot;
	pbch_mib_t mib;
	float cfo;
	int n;
	int nof_found_mib = 0;

	parse_args(argc,argv);

	if (base_init(FLEN)) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	sync_pss_det_peakmean(&sfind);
	sync_pss_det_peakmean(&strack);

	if (!input_file_name) {
	#ifndef DISABLE_UHD
		INFO("Setting sampling frequency %.2f MHz\n", (float) SAMP_FREQ/MHZ);
		cuhd_set_rx_srate(uhd, SAMP_FREQ);
		cuhd_set_rx_gain(uhd, uhd_gain);
		/* set uhd_freq */
		cuhd_set_rx_freq(uhd, (double) uhd_freq);
		cuhd_rx_wait_lo_locked(uhd);
		DEBUG("Set uhd_freq to %.3f MHz\n", (double) uhd_freq);

		DEBUG("Starting receiver...\n",0);
		cuhd_start_rx_stream(uhd);
	#endif
	}

	state = FIND;
	nslot = 0;
	find_idx = 0;
	cfo = 0;
	mib.sfn = -1;
	frame_cnt = 0;
	last_found = 0;
	sync_set_threshold(&sfind, find_threshold);
	sync_force_N_id_2(&sfind, -1);

	while(frame_cnt < nof_frames || nof_frames==-1) {
		INFO(" -----   RECEIVING %d SAMPLES ---- \n", FLEN);
		if (input_file_name) {
			n = filesource_read(&fsrc, input_buffer, FLEN);
			if (n == -1) {
				fprintf(stderr, "Error reading file\n");
				exit(-1);
			} else if (n < FLEN) {
				filesource_seek(&fsrc, 0);
				filesource_read(&fsrc, input_buffer, FLEN);
			}
		} else {
		#ifndef DISABLE_UHD
			cuhd_recv(uhd, input_buffer, FLEN, 1);
		#endif
		}

		switch(state) {
		case FIND:
			/* find peak in all frame */
			find_idx = sync_run(&sfind, input_buffer);
			INFO("FIND %3d:\tPAR=%.2f\n", frame_cnt, sync_get_peak_to_avg(&sfind));
			if (find_idx != -1) {
				/* if found peak, go to track and set track threshold */
				frame_cnt = -1;
				last_found = 0;
				sync_set_threshold(&strack, track_threshold);
				sync_force_N_id_2(&strack, sync_get_N_id_2(&sfind));
				cell_id = sync_get_cell_id(&sfind);
				mib_decoder_init(cell_id);
				nslot = sync_get_slot_id(&sfind);
				nslot=(nslot+10)%20;
				cfo = 0;
				printf("\n");
				state = TRACK;
			}
			if (verbose == VERBOSE_NONE) {
				printf("Tracking... PAR=%.2f\r", sync_get_peak_to_avg(&sfind));
			}
			break;
		case TRACK:
			/* Find peak around known position find_idx */
			INFO("TRACK %3d: PSS find_idx %d offset %d\n", frame_cnt, find_idx, find_idx - track_len);
			track_idx = sync_run(&strack, &input_buffer[find_idx - track_len]);

			if (track_idx != -1) {
				/* compute cumulative moving average CFO */
				cfo = (sync_get_cfo(&strack) + frame_cnt * cfo) / (frame_cnt + 1);
				last_found = frame_cnt;
				find_idx += track_idx - track_len;
				if (nslot != sync_get_slot_id(&strack)) {
					INFO("Expected slot %d but got %d\n", nslot, sync_get_slot_id(&strack));
					state = TRACK;
				}
			}

			/* if we missed too many PSS go back to track */
			if (frame_cnt - last_found > max_track_lost) {
				INFO("%d frames lost. Going back to TRACK\n", frame_cnt - last_found);
			}

			// Correct CFO
			INFO("Correcting CFO=%.4f\n", cfo);

			cfo_correct(&cfocorr, input_buffer, -cfo/128);

			if (nslot == 0) {
				INFO("Finding MIB at idx %d\n", find_idx);
				if (mib_decoder_run(&input_buffer[find_idx], &mib)) {
					INFO("MIB detected attempt=%d\n", frame_cnt);
					last_found = frame_cnt;
					if (verbose == VERBOSE_NONE) {
						if (!nof_found_mib) {
							pbch_mib_fprint(stdout, &mib);
						}
					}
					nof_found_mib++;
				} else {
					INFO("MIB not found attempt %d\n",frame_cnt);
				}
				if (frame_cnt) {
					printf("SFN: %4d\tCFO: %+.4f KHz\tTimeOffset: %4d\tErrors: %4d/%4d\tErrorRate: %.1e\r", mib.sfn,
							cfo*15, find_idx, frame_cnt-2*(nof_found_mib-1), frame_cnt,
							(float) (frame_cnt-2*(nof_found_mib-1))/frame_cnt);
					fflush(stdout);
				}
			}
			if (input_file_name) {
				usleep(5000);
			}
			nslot = (nslot+10)%20;
			break;
		}
		frame_cnt++;
	}

	base_free();

	printf("\n\nDone\n");
	exit(0);
}

