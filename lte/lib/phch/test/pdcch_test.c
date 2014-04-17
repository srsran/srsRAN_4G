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
int nof_ports = 1;

void usage(char *prog) {
	printf("Usage: %s [cpv]\n", prog);
	printf("\t-c cell id [Default %d]\n", cell_id);
	printf("\t-p nof_ports [Default %d]\n", nof_ports);
	printf("\t-n nof_prb [Default %d]\n", nof_prb);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "cpnv")) != -1) {
		switch(opt) {
		case 'p':
			nof_ports = atoi(argv[optind]);
			break;
		case 'n':
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
	pdcch_t pdcch;
	dci_t dci_tx, dci_rx;
	dci_format1_t dci_msg;
	regs_t regs;
	int i, j;
	cf_t *ce[MAX_PORTS_CTRL];
	int nof_re;
	cf_t *slot1_symbols[MAX_PORTS_CTRL];
	int nof_dcis;
	int ret = -1;

	parse_args(argc,argv);

	nof_re = CPNORM_NSYMB * nof_prb * RE_X_RB;

	/* init memory */
	for (i=0;i<MAX_PORTS_CTRL;i++) {
		ce[i] = malloc(sizeof(cf_t) * nof_re);
		if (!ce[i]) {
			perror("malloc");
			exit(-1);
		}
		for (j=0;j<nof_re;j++) {
			ce[i][j] = 1;
		}
		slot1_symbols[i] = 	malloc(sizeof(cf_t) * nof_re);
		if (!slot1_symbols[i]) {
			perror("malloc");
			exit(-1);
		}
	}

	if (regs_init(&regs, cell_id, nof_prb, nof_ports, R_1, PHICH_NORM, CPNORM)) {
		fprintf(stderr, "Error initiating regs\n");
		exit(-1);
	}

	if (pdcch_init(&pdcch, &regs, nof_prb, nof_ports, cell_id, CPNORM)) {
		fprintf(stderr, "Error creating PBCH object\n");
		exit(-1);
	}

	dci_init(&dci_tx, 1);
	dci_format1_add(&dci_tx, &dci_msg);

	pdcch_encode(&pdcch, &dci_tx, slot1_symbols, 0);

	/* combine outputs */
	for (i=1;i<nof_ports;i++) {
		for (j=0;j<nof_re;j++) {
			slot1_symbols[0][j] += slot1_symbols[i][j];
		}
	}

	pdcch_init_search_ue(&pdcch, 1234);

	dci_init(&dci_rx, 1);
	nof_dcis = pdcch_decode(&pdcch, slot1_symbols[0], ce, &dci_rx, 0, 1);
	if (nof_dcis < 0) {
		printf("Error decoding\n");
	} else if (nof_dcis == dci_tx.nof_dcis) {
		for (i=0;i<nof_dcis;i++) {

			if (dci_tx.msg[i].location.L != dci_rx.msg[i].location.L
					|| dci_tx.msg[i].location.ncce != dci_rx.msg[i].location.ncce
					|| dci_tx.msg[i].location.nof_bits != dci_rx.msg[i].location.nof_bits
					|| dci_tx.msg[i].location.rnti != dci_rx.msg[i].location.rnti) {
				printf("Error in DCI %d: Received location does not match\n", i);
				dci_candidate_fprint(stdout, &dci_tx.msg[i].location);
				dci_candidate_fprint(stdout, &dci_rx.msg[i].location);
				goto quit;
			}

			if (memcmp(dci_tx.msg[i].data, dci_rx.msg[i].data, dci_tx.msg[i].location.nof_bits)) {
				printf("Error in DCI %d: Received data does not match\n", i);
				goto quit;
			}
			/* check more things ... */
		}
	} else {
		printf("Transmitted %d DCIs but got %d\n", dci_tx.nof_dcis, nof_dcis);
		goto quit;
	}
	ret = 0;
quit:
	pdcch_free(&pdcch);
	regs_free(&regs);
	dci_free(&dci_tx);
	dci_free(&dci_rx);

	for (i=0;i<MAX_PORTS_CTRL;i++) {
		free(ce[i]);
		free(slot1_symbols[i]);
	}
	if (ret) {
		printf("Error\n");
	} else {
		printf("Ok\n");
	}
	exit(ret);
}
