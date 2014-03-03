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

#include "lte.h"
#include "cuhd.h"

#define MHZ 			1000000
#define SAMP_FREQ 		1920000
#define RSSI_FS			1000000
#define FLEN 			9600
#define FLEN_PERIOD		0.005

#define RSSI_DECIM		20

#define IS_SIGNAL(i) (10*log10f(rssi[i]) + 30 > rssi_threshold)


int band, earfcn=-1;
float find_threshold = 40.0, track_threshold = 8.0;
int earfcn_start=-1, earfcn_end = -1;
float rssi_threshold = -30.0;
int max_track_lost=9;
int nof_frames_find=8, nof_frames_track=100, nof_samples_rssi=50000;
int track_len=500;

cf_t *input_buffer;
float *cfo_v;
int *idx_v, *idx_valid, *t;
float *p2a_v;
void *uhd;
int nof_bands;
float gain = 20.0;

#define MAX_EARFCN 1000
lte_earfcn_t channels[MAX_EARFCN];
float rssi[MAX_EARFCN];
float rssi_d[MAX_EARFCN/RSSI_DECIM];
float freqs[MAX_EARFCN];
float cfo[MAX_EARFCN];
float p2a[MAX_EARFCN];

enum sync_state {INIT, FIND, TRACK, DONE};

void print_to_matlab();

void usage(char *prog) {
	printf("Usage: %s [seRrFfTtgv] -b band\n", prog);
	printf("\t-s earfcn_start [Default All]\n");
	printf("\t-e earfcn_end [Default All]\n");
	printf("\t-R rssi_nof_samples [Default %d]\n", nof_samples_rssi);
	printf("\t-r rssi_threshold [Default %.2f dBm]\n", rssi_threshold);
	printf("\t-F pss_find_nof_frames [Default %d]\n", nof_frames_find);
	printf("\t-f pss_find_threshold [Default %.2f]\n", find_threshold);
	printf("\t-T pss_track_nof_frames [Default %d]\n", nof_frames_track);
	printf("\t-t pss_track_threshold [Default %.2f]\n", track_threshold);
	printf("\t-l pss_track_len [Default %d]\n", track_len);
	printf("\t-g gain [Default %.2f dB]\n", gain);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "bseRrFfTtgv")) != -1) {
		switch(opt) {
		case 'b':
			band = atoi(argv[optind]);
			break;
		case 's':
			earfcn_start = atoi(argv[optind]);
			break;
		case 'e':
			earfcn_end = atoi(argv[optind]);
			break;
		case 'R':
			nof_samples_rssi = atoi(argv[optind]);
			break;
		case 'r':
			rssi_threshold = -atof(argv[optind]);
			break;
		case 'F':
			nof_frames_find = atoi(argv[optind]);
			break;
		case 'f':
			find_threshold = atof(argv[optind]);
			break;
		case 'T':
			nof_frames_track = atoi(argv[optind]);
			break;
		case 't':
			track_threshold = atof(argv[optind]);
			break;
		case 'g':
			gain = atof(argv[optind]);
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

int base_init(int frame_length) {

	input_buffer = malloc(2 * frame_length * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		exit(-1);
	}

	idx_v = malloc(nof_frames_track * sizeof(int));
	if (!idx_v) {
		perror("malloc");
		exit(-1);
	}
	idx_valid = malloc(nof_frames_track * sizeof(int));
	if (!idx_valid) {
		perror("malloc");
		exit(-1);
	}
	t = malloc(nof_frames_track * sizeof(int));
	if (!t) {
		perror("malloc");
		exit(-1);
	}
	cfo_v = malloc(nof_frames_track * sizeof(float));
	if (!cfo_v) {
		perror("malloc");
		exit(-1);
	}
	p2a_v = malloc(nof_frames_track * sizeof(float));
	if (!p2a_v) {
		perror("malloc");
		exit(-1);
	}

	bzero(cfo, sizeof(float) * MAX_EARFCN);
	bzero(p2a, sizeof(float) * MAX_EARFCN);

	/* open UHD device */
	printf("Opening UHD device...\n");
	if (cuhd_open("",&uhd)) {
		fprintf(stderr, "Error opening uhd\n");
		exit(-1);
	}

	return 0;
}

void base_free() {

	cuhd_close(&uhd);
	free(input_buffer);
	free(idx_v);
	free(idx_valid);
	free(t);
	free(cfo_v);
	free(p2a_v);
}

float mean_valid(int *idx_v, float *x, int nof_frames) {
	int i;
	float mean = 0;
	int n = 0;
	for (i=0;i<nof_frames;i++) {
		if (idx_v[i] != -1) {
			mean += x[i];
			n++;
		}
	}
	if (n > 0) {
		return mean/n;
	} else {
		return 0.0;
	}
}

int preprocess_idx(int *in, int *out, int *period, int len) {
	int i, n;
	n=0;
	for (i=0;i<len;i++) {
		if (in[i] != -1) {
			out[n] = in[i];
			period[n] = i;
			n++;
		}
	}
	return n;
}

int rssi_scan() {
	int n=0;
	int i;

	if (nof_bands > 100) {
		/* scan every Mhz, that is 10 freqs */
		for (i=0;i<nof_bands;i+=RSSI_DECIM) {
			freqs[n] = channels[i].fd * MHZ;
			n++;
		}
		if (cuhd_rssi_scan(uhd, freqs, rssi_d, n, (double) RSSI_FS, nof_samples_rssi)) {
			fprintf(stderr, "Error while doing RSSI scan\n");
			return -1;
		}
		/* linearly interpolate the rssi vector */
		interp_linear_f(rssi_d, rssi, RSSI_DECIM, n);
	} else {
		for (i=0;i<nof_bands;i++) {
			freqs[i] = channels[i].fd * MHZ;
		}
		if (cuhd_rssi_scan(uhd, freqs, rssi, nof_bands, (double) RSSI_FS, nof_samples_rssi)) {
			fprintf(stderr, "Error while doing RSSI scan\n");
			return -1;
		}
		n = nof_bands;
	}

	return n;
}


int main(int argc, char **argv) {
	int frame_cnt, valid_frames;
	int freq;
	int cell_id;
	sync_t sfind, strack;
	float max_peak_to_avg;
	float sfo;
	int find_idx, track_idx, last_found;
	enum sync_state state;
	int n;
	filesink_t fs;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (base_init(FLEN)) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	if (sync_init(&sfind, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}
	sync_pss_det_peakmean(&sfind);

	if (sync_init(&strack, track_len)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}
	sync_pss_det_peakmean(&strack);

	nof_bands = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
	printf("RSSI scan: %d freqs in band %d, RSSI threshold %.2f dBm\n", nof_bands, band, rssi_threshold);

	n = rssi_scan();
	if (n == -1) {
		exit(-1);
	}

	printf("\nDone. Starting PSS search on %d channels\n", n);
	usleep(500000);
	INFO("Setting sampling frequency %.2f MHz\n", (float) SAMP_FREQ/MHZ);
	cuhd_set_rx_srate(uhd, SAMP_FREQ);

	cuhd_set_rx_gain(uhd, gain);

	print_to_matlab();

	filesink_init(&fs, "test.dat", COMPLEX_FLOAT_BIN);

	freq=0;
	state = INIT;
	find_idx = 0;
	max_peak_to_avg = 0;
	last_found = 0;
	frame_cnt = 0;
	while(freq<nof_bands) {
		/* scan only bands above rssi_threshold */
		if (!IS_SIGNAL(freq)) {
			INFO("[%3d/%d]: Skipping EARFCN %d %.2f MHz RSSI %.2f dB\n", freq, nof_bands,
								channels[freq].id, channels[freq].fd,10*log10f(rssi[freq]) + 30);
			freq++;
		} else {
			if (state == TRACK || state == FIND) {
				cuhd_recv(uhd, &input_buffer[FLEN], FLEN, 1);
			}
			switch(state) {
			case INIT:
				DEBUG("Stopping receiver...\n",0);
				cuhd_stop_rx_stream(uhd);

				/* set freq */
				cuhd_set_rx_freq(uhd, (double) channels[freq].fd * MHZ);
				cuhd_rx_wait_lo_locked(uhd);
				DEBUG("Set freq to %.3f MHz\n", (double) channels[freq].fd);

				DEBUG("Starting receiver...\n",0);
				cuhd_start_rx_stream(uhd);

				/* init variables */
				frame_cnt = 0;
				max_peak_to_avg = -99;
				cell_id = -1;

				/* receive first frame */
				cuhd_recv(uhd, input_buffer, FLEN, 1);

				/* set find_threshold and go to FIND state */
				sync_set_threshold(&sfind, find_threshold);
				sync_force_N_id_2(&sfind, -1);
				state = FIND;
				break;
			case FIND:
				/* find peak in all frame */
				find_idx = sync_run(&sfind, &input_buffer[FLEN]);
				DEBUG("[%3d/%d]: PAR=%.2f\n", freq, nof_bands, sync_get_peak_to_avg(&sfind));
				if (find_idx != -1) {
					/* if found peak, go to track and set lower threshold */
					frame_cnt = -1;
					last_found = 0;
					sync_set_threshold(&strack, track_threshold);
					sync_force_N_id_2(&strack, sync_get_N_id_2(&sfind));
					state = TRACK;
					INFO("[%3d/%d]: EARFCN %d Freq. %.2f MHz PSS found PAR %.2f dB\n", freq, nof_bands,
												channels[freq].id, channels[freq].fd,
												10*log10f(sync_get_peak_to_avg(&sfind)));
				} else {
					if (frame_cnt >= nof_frames_find) {
						state = INIT;
						printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz No PSS found\r", freq, nof_bands,
													channels[freq].id, channels[freq].fd, frame_cnt - last_found);
						if (VERBOSE_ISINFO()) {
							printf("\n");
						}
						freq++;
					}
				}
				break;
			case TRACK:
				INFO("Tracking PSS find_idx %d offset %d\n", find_idx, find_idx + track_len);

				filesink_write(&fs, &input_buffer[FLEN+find_idx+track_len], track_len);

				track_idx = sync_run(&strack, &input_buffer[FLEN + find_idx - track_len]);
				p2a_v[frame_cnt] = sync_get_peak_to_avg(&strack);

				/* save cell id for the best peak-to-avg */
				if (p2a_v[frame_cnt] > max_peak_to_avg) {
					max_peak_to_avg = p2a_v[frame_cnt];
					cell_id = sync_get_cell_id(&strack);
				}
				if (track_idx != -1) {
					cfo_v[frame_cnt] = sync_get_cfo(&strack);
					last_found = frame_cnt;
					find_idx += track_idx - track_len;
					idx_v[frame_cnt] = find_idx;
				} else {
					idx_v[frame_cnt] = -1;
					cfo_v[frame_cnt] = 0.0;
				}
				/* if we missed to many PSS it is not a cell, next freq */
				if (frame_cnt - last_found > max_track_lost) {
					INFO("\n[%3d/%d]: EARFCN %d Freq. %.2f MHz %d frames lost\n", freq, nof_bands,
							channels[freq].id, channels[freq].fd, frame_cnt - last_found);

					state = INIT;
					freq++;
				} else if (frame_cnt >= nof_frames_track) {
					state = DONE;
				}
				break;
			case DONE:

				cfo[freq] = mean_valid(idx_v, cfo_v, frame_cnt);
				p2a[freq] = mean_valid(idx_v, p2a_v, frame_cnt);
				valid_frames = preprocess_idx(idx_v, idx_valid, t, frame_cnt);
				sfo = sfo_estimate_period(idx_valid, t, valid_frames, FLEN_PERIOD);

				printf("\n[%3d/%d]: FOUND EARFCN %d Freq. %.2f MHz. "
						"PAR %2.2f dB, CFO=%+.2f KHz, SFO=%+2.3f KHz, CELL_ID=%3d\n", freq, nof_bands,
								channels[freq].id, channels[freq].fd,
								10*log10f(p2a[freq]), cfo[freq] * 15, sfo / 1000, cell_id);
				state = INIT;
				freq++;
				break;
			}
			if (state == TRACK || (state == FIND && frame_cnt)) {
				memcpy(input_buffer, &input_buffer[FLEN], FLEN * sizeof(cf_t));
			}
			frame_cnt++;
		}
	}

	print_to_matlab();

	sync_free(&sfind);
	base_free();

	printf("\n\nDone\n");
	exit(0);
}

void print_to_matlab() {
	int i;

	FILE *f = fopen("output.m", "w");
	if (!f) {
		perror("fopen");
		exit(-1);
	}
	fprintf(f, "fd=[");
	for (i=0;i<nof_bands;i++) {
		fprintf(f, "%g, ", channels[i].fd);
	}
	fprintf(f, "];\n");

	fprintf(f, "rssi=[");
	for (i=0;i<nof_bands;i++) {
		fprintf(f, "%g, ", rssi[i]);
	}
	fprintf(f, "];\n");


	fprintf(f, "rssi_d=[");
	for (i=0;i<nof_bands/RSSI_DECIM;i++) {
		fprintf(f, "%g, ", rssi_d[i]);
	}
	fprintf(f, "];\n");

	/*
	fprintf(f, "cfo=[");
	for (i=0;i<nof_bands;i++) {
		if (IS_SIGNAL(i)) {
			fprintf(f, "%g, ", cfo[i]);
		} else {
			fprintf(f, "NaN, ");
		}
	}
	fprintf(f, "];\n");
*/
	fprintf(f, "p2a=[");
	for (i=0;i<nof_bands;i++) {
		if (IS_SIGNAL(i)) {
			fprintf(f, "%g, ", p2a[i]);
		} else {
			fprintf(f, "0, ");
		}
	}
	fprintf(f, "];\n");
	fprintf(f, "clf;\n\n");
	fprintf(f, "subplot(1,2,1)\n");
	fprintf(f, "plot(fd, 10*log10(rssi)+30)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel('RSSI [dBm]');\n");
	fprintf(f, "title('RSSI Estimation')\n");

	fprintf(f, "subplot(1,2,2)\n");
	fprintf(f, "plot(fd, p2a)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel('Peak-to-Avg [dB]');\n");
	fprintf(f, "title('PSS Correlation')\n");
/*
	fprintf(f, "subplot(1,3,3)\n");
	fprintf(f, "plot(fd, cfo)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel(''); axis([min(fd) max(fd) -0.5 0.5]);\n");
	fprintf(f, "title('CFO Estimation')\n");
	*/
	fprintf(f, "drawnow;\n");
	fclose(f);
}
