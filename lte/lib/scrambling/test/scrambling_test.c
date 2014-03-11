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

char *sequence_name = NULL;
bool do_floats = false;
lte_cp_t cp = CPNORM;
int cell_id = -1;

void usage(char *prog) {
	printf("Usage: %s [ef] -c cell_id -s [PBCH, PDSCH, PDCCH, PMCH, PUCCH]\n", prog);
	printf("\t -e CP extended [Default CP Normal]\n");
	printf("\t -f scramble floats [Default bits]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "csef")) != -1) {
		switch (opt) {
		case 'c':
			cell_id = atoi(argv[optind]);
			break;
		case 'e':
			cp = CPEXT;
			break;
		case 'f':
			do_floats = true;
			break;
		case 's':
			sequence_name = argv[optind];
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
	if (cell_id == -1) {
		usage(argv[0]);
		exit(-1);
	}
	if (!sequence_name) {
		usage(argv[0]);
		exit(-1);
	}
}

int init_sequence(sequence_t *seq, char *name) {
	if (!strcmp(name, "PBCH")) {
		return sequence_pbch(seq, cp, cell_id);
	} else {
		fprintf(stderr, "Unsupported sequence name %s\n", name);
		return -1;
	}
}


int main(int argc, char **argv) {
	int i;
	sequence_t seq;
	char *input_b, *scrambled_b;
	float *input_f, *scrambled_f;

	parse_args(argc, argv);

	if (init_sequence(&seq, sequence_name) == -1) {
		fprintf(stderr, "Error initiating sequence %s\n", sequence_name);
		exit(-1);
	}

	if (!do_floats) {
		input_b = malloc(sizeof(char) * seq.len);
		if (!input_b) {
			perror("malloc");
			exit(-1);
		}
		scrambled_b = malloc(sizeof(char) * seq.len);
		if (!scrambled_b) {
			perror("malloc");
			exit(-1);
		}

		for (i=0;i<seq.len;i++) {
			input_b[i] = rand()%2;
			scrambled_b[i] = input_b[i];
		}

		scrambling_bit(&seq, scrambled_b);
		scrambling_bit(&seq, scrambled_b);

		for (i=0;i<seq.len;i++) {
			if (scrambled_b[i] != input_b[i]) {
				printf("Error in %d\n", i);
				exit(-1);
			}
		}
		free(input_b);
		free(scrambled_b);
	} else {
		input_f = malloc(sizeof(float) * seq.len);
		if (!input_f) {
			perror("malloc");
			exit(-1);
		}
		scrambled_f = malloc(sizeof(float) * seq.len);
		if (!scrambled_f) {
			perror("malloc");
			exit(-1);
		}

		for (i=0;i<seq.len;i++) {
			input_f[i] = 100*(rand()/RAND_MAX);
			scrambled_f[i] = input_f[i];
		}

		scrambling_float(&seq, scrambled_f);
		scrambling_float(&seq, scrambled_f);

		for (i=0;i<seq.len;i++) {
			if (scrambled_f[i] != input_f[i]) {
				printf("Error in %d\n", i);
				exit(-1);
			}
		}

		free(input_f);
		free(scrambled_f);
	}
	printf("Ok\n");
	sequence_free(&seq);
	exit(0);
}
