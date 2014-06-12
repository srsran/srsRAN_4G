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

#define MSE_THRESHOLD	0.00001

int nof_symbols = 1000;
int nof_layers = 1, nof_ports = 1;
char *mimo_type_name = NULL;

void usage(char *prog) {
	printf("Usage: %s -m [single|diversity|multiplex] -l [nof_layers] -p [nof_ports]\n", prog);
	printf("\t-n num_symbols [Default %d]\n", nof_symbols);
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "mpln")) != -1) {
		switch (opt) {
		case 'n':
			nof_symbols = atoi(argv[optind]);
			break;
		case 'p':
			nof_ports = atoi(argv[optind]);
			break;
		case 'l':
			nof_layers = atoi(argv[optind]);
			break;
		case 'm':
			mimo_type_name = argv[optind];
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
	if (!mimo_type_name) {
		usage(argv[0]);
		exit(-1);
	}
}

int main(int argc, char **argv) {
	int i, j;
	float mse;
	cf_t *x[MAX_LAYERS], *r[MAX_PORTS], *y[MAX_PORTS], *h[MAX_PORTS], *xr[MAX_LAYERS];
	lte_mimo_type_t type;

	parse_args(argc, argv);

	if (nof_ports > MAX_PORTS || nof_layers > MAX_LAYERS) {
		fprintf(stderr, "Invalid number of layers or ports\n");
		exit(-1);
	}

	if (lte_str2mimotype(mimo_type_name, &type)) {
		fprintf(stderr, "Invalid MIMO type %s\n", mimo_type_name);
		exit(-1);
	}

	for (i=0;i<nof_layers;i++) {
		x[i] = malloc(sizeof(cf_t) * nof_symbols);
		if (!x[i]) {
			perror("malloc");
			exit(-1);
		}
		xr[i] = malloc(sizeof(cf_t) * nof_symbols);
		if (!xr[i]) {
			perror("malloc");
			exit(-1);
		}
	}
	for (i=0;i<nof_ports;i++) {
		y[i] = malloc(sizeof(cf_t) * nof_symbols * nof_layers);
		// TODO: The number of symbols per port is different in spatial multiplexing.
 		if (!y[i]) {
			perror("malloc");
			exit(-1);
		}
		h[i] = malloc(sizeof(cf_t) * nof_symbols * nof_layers);
		if (!h[i]) {
			perror("malloc");
			exit(-1);
		}
	}

	/* only 1 receiver antenna supported now */
	r[0] = malloc(sizeof(cf_t) * nof_symbols * nof_layers);
	if (!r[0]) {
		perror("malloc");
		exit(-1);
	}

	/* generate random data */
	for (i=0;i<nof_layers;i++) {
		for (j=0;j<nof_symbols;j++) {
			x[i][j] = 100 * ((float) rand()/RAND_MAX + (float) I*rand()/RAND_MAX);
		}
	}

	/* precoding */
	if (precoding_type(x, y, nof_layers, nof_ports, nof_symbols, type) < 0) {
		fprintf(stderr, "Error layer mapper encoder\n");
		exit(-1);
	}

	/* generate channel */
	for (i=0;i<nof_ports;i++) {
		for (j=0;j<nof_symbols * nof_layers;j++) {
			float hc = -1+(float) i/nof_ports;
			h[i][j] = (3+hc) * cexpf(I * hc);
		}
	}

	/* pass signal through channel
	   (we are in the frequency domain so it's a multiplication) */
	/* there's only one receiver antenna, signals from different transmitter
	 * ports are simply combined at the receiver
	 */
	for (j=0;j<nof_symbols * nof_layers;j++) {
		r[0][j] = 0;
		for (i=0;i<nof_ports;i++) {
			r[0][j] += y[i][j] * h[i][j];
		}
	}

	/* predecoding / equalization */
	if (predecoding_type(r, h, xr, nof_ports, nof_layers, nof_symbols * nof_layers, type) < 0) {
		fprintf(stderr, "Error layer mapper encoder\n");
		exit(-1);
	}

	/* check errors */
	mse = 0;
	for (i=0;i<nof_layers;i++) {
		for (j=0;j<nof_symbols;j++) {
			mse += cabsf(xr[i][j] - x[i][j])/nof_layers/nof_symbols;
		}
	}

	for (i=0;i<nof_layers;i++) {
		free(x[i]);
		free(xr[i]);
	}
	for (i=0;i<nof_ports;i++) {
		free(y[i]);
		free(h[i]);
	}

	free(r[0]);

	if (mse > MSE_THRESHOLD) {
		printf("MSE: %f\n", mse);
		exit(-1);
	} else {
		printf("Ok\n");
		exit(0);
	}
}
