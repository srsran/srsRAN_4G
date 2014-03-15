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

int cell_id = 1;
int nof_prb = 6;


void usage(char *prog) {
	printf("Usage: %s [cpv]\n", prog);
	printf("\t-c cell id [Default %d]\n", cell_id);
	printf("\t-p nof_prb [Default %d]\n", nof_prb);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "cpv")) != -1) {
		switch(opt) {
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
}


int main(int argc, char **argv) {
	pbch_t pbch;
	cf_t *buffer = NULL;
	pbch_mib_t mib_tx, mib_rx;
	int i, j;
	cf_t *ce[MAX_PORTS_CTRL];
	int nof_re;
	cf_t *slot1_symbols[MAX_PORTS_CTRL];

	parse_args(argc,argv);

	nof_re = CPNORM_NSYMB * nof_prb * RE_X_RB;

	/* init memory */
	buffer = malloc(sizeof(cf_t) * nof_re);
	if (!buffer) {
		perror("malloc");
		exit(-1);
	}
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		ce[i] = malloc(sizeof(cf_t) * nof_re);
		if (!ce[i]) {
			perror("malloc");
			exit(-1);
		}
		for (j=0;j<nof_re;j++) {
			ce[i][j] = 1;
		}
		slot1_symbols[i] = buffer;
	}
	if (pbch_init(&pbch, cell_id, CPNORM)) {
		fprintf(stderr, "Error creating PBCH object\n");
		exit(-1);
	}

	mib_tx.nof_ports = 1;
	mib_tx.nof_prb = 50;
	mib_tx.phich_length = EXTENDED;
	mib_tx.phich_resources = R_1_6;
	mib_tx.sfn = 124;

	pbch_encode(&pbch, &mib_tx, slot1_symbols, nof_prb, 1);
	pbch_decode_reset(&pbch);
	if (1 != pbch_decode(&pbch, buffer, ce, nof_prb, 1, &mib_rx)) {
		printf("Error decoding\n");
		exit(-1);
	}

	pbch_free(&pbch);

	if (buffer) {
		free(buffer);
	}

	if (!memcmp(&mib_tx, &mib_rx, sizeof(pbch_mib_t))) {
		printf("OK\n");
		exit(0);
	} else {
		pbch_mib_fprint(stdout, &mib_rx);
		exit(-1);
	}
}
