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
#include <time.h>
#include <stdbool.h>

#include "lte.h"

int cell_id = -1, offset = 0;

#define FLEN	9600

void usage(char *prog) {
	printf("Usage: %s [co]\n", prog);
	printf("\t-c cell_id [Default check for all]\n");
	printf("\t-o offset [Default %d]\n", offset);
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "co")) != -1) {
		switch (opt) {
		case 'c':
			cell_id = atoi(argv[optind]);
			break;
		case 'o':
			offset = atoi(argv[optind]);
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
}

int main(int argc, char **argv) {
	int N_id_2, ns, find_ns;
	cf_t *buffer, *fft_buffer;
	cf_t pss_signal[PSS_LEN];
	float sss_signal0[SSS_LEN]; // for subframe 0
	float sss_signal5[SSS_LEN]; // for subframe 5
	int cid, max_cid, find_idx;
	sync_t sync;
	lte_fft_t ifft;

	parse_args(argc, argv);

	buffer = malloc(sizeof(cf_t) * FLEN);
	if (!buffer) {
		perror("malloc");
		exit(-1);
	}

	fft_buffer = malloc(sizeof(cf_t) * 2 * FLEN);
	if (!fft_buffer) {
		perror("malloc");
		exit(-1);
	}

	if (lte_ifft_init(&ifft, CPNORM, 6)) {
		fprintf(stderr, "Error creating iFFT object\n");
		exit(-1);
	}

	if (sync_init(&sync, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		return -1;
	}

	sync_set_threshold(&sync, 20);
	sync_force_N_id_2(&sync, -1);

	if (cell_id == -1) {
		cid = 0;
		max_cid = 149;
	} else {
		cid = cell_id;
		max_cid = cell_id;
	}
	while(cid <= max_cid) {
		N_id_2 = cid%3;

		/* Generate PSS/SSS signals */
		pss_generate(pss_signal, N_id_2);
		sss_generate(sss_signal0, sss_signal5, cid);

		for (ns=0;ns<2;ns++) {
			memset(buffer, 0, sizeof(cf_t) * FLEN);
			pss_put_slot(pss_signal, buffer, 6, CPNORM);
			sss_put_slot(ns?sss_signal5:sss_signal0, buffer, 6, CPNORM);

			/* Transform to OFDM symbols */
			memset(fft_buffer, 0, sizeof(cf_t) * 2 * FLEN);
			lte_ifft_run(&ifft, buffer, &fft_buffer[offset]);

			find_idx = sync_run(&sync, fft_buffer);
			find_ns = sync_get_slot_id(&sync);
			printf("cell_id: %d find: %d, offset: %d, ns=%d find_ns=%d\n", cid, find_idx, offset,
					ns, find_ns);
			if (find_idx != offset + 960) {
				printf("offset != find_offset: %d != %d\n", find_idx, offset + 960);
				exit(-1);
			}
			if (ns*10 != find_ns) {
				printf("ns != find_ns\n", 10 * ns, find_ns);
				exit(-1);
			}
		}
		cid++;
	}

	free(fft_buffer);
	free(buffer);

	sync_free(&sync);
	lte_ifft_free(&ifft);

	fftwf_cleanup();

	printf("Ok\n");
	exit(0);
}
