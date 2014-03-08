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
#include <strings.h>
#include <unistd.h>
#include <complex.h>

#include "lte.h"

int cell_id = -1;
int nof_prb = 6;
lte_cp_t cp = CPNORM;

char *output_matlab = NULL;

void usage(char *prog) {
	printf("Usage: %s [recov]\n", prog);

	printf("\t-r nof_prb [Default %d]\n", nof_prb);
	printf("\t-e extended cyclic prefix [Default normal]\n");

	printf("\t-c cell_id (-1 tests all). [Default %d]\n", cell_id);

	printf("\t-o output matlab file [Default %s]\n",output_matlab?output_matlab:"None");
	printf("\t-v increase verbosity\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "recov")) != -1) {
		switch(opt) {
		case 'r':
			nof_prb = atoi(argv[optind]);
			break;
		case 'e':
			cp = CPEXT;
			break;
		case 'c':
			cell_id = atoi(argv[optind]);
			break;
		case 'o':
			output_matlab = argv[optind];
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

int check_mse(float mod, float arg, int n_port) {
	INFO("mod=%.4f, arg=%.4f, n_port=%d\n", mod, arg, n_port);
	switch(n_port) {
	case 0:
		if (mod > 0.029) {
			return -1;
		}
		if (arg > 0.029) {
			return -1;
		}
		break;
	case 1:
		if (mod > 0.012) {
			return -1;
		}
		if (arg > 0.012) {
			return -1;
		}
		break;
	case 2:
	case 3:
		if (mod > 3.33) {
			return -1;
		}
		if (arg > 0.63) {
			return -1;
		}
		break;
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) {
	chest_t eq;
	cf_t *input = NULL, *ce = NULL, *h = NULL;
	refsignal_t refs;
	int i, j, n_port, n_slot, cid, num_re;
	int ret = -1;
	int max_cid;
	FILE *fmatlab = NULL;
	float mse_mag, mse_phase;

	parse_args(argc,argv);

	if (output_matlab) {
		fmatlab=fopen(output_matlab, "w");
		if (!fmatlab) {
			perror("fopen");
			goto do_exit;
		}
	}

	num_re = nof_prb * RE_X_RB * CP_NSYMB(cp);

	input = malloc(num_re * sizeof(cf_t));
	if (!input) {
		perror("malloc");
		goto do_exit;
	}
	h = malloc(num_re * sizeof(cf_t));
	if (!h) {
		perror("malloc");
		goto do_exit;
	}
	ce = malloc(num_re * sizeof(cf_t));
	if (!ce) {
		perror("malloc");
		goto do_exit;
	}

	if (cell_id == -1) {
		cid = 0;
		max_cid = 149;
	} else {
		cid = cell_id;
		max_cid = cell_id;
	}
	while(cid <= max_cid) {
		if (chest_init(&eq, LINEAR, cp, nof_prb, MAX_PORTS)) {
			fprintf(stderr, "Error initializing equalizer\n");
			goto do_exit;
		}

		if (chest_ref_LTEDL(&eq, cid)) {
			fprintf(stderr, "Error initializing reference signal\n");
			goto do_exit;
		}

		for (n_slot=0;n_slot<NSLOTS_X_FRAME;n_slot++) {
			for (n_port=0;n_port<MAX_PORTS;n_port++) {

				if (refsignal_init_LTEDL(&refs, n_port, n_slot, cid, cp, nof_prb)) {
					fprintf(stderr, "Error initiating CRS slot=%d\n", i);
					return -1;
				}

				bzero(input, sizeof(cf_t) * num_re);
				for (i=0;i<num_re;i++) {
					input[i] = 0.5-rand()/RAND_MAX+I*(0.5-rand()/RAND_MAX);
				}

				bzero(ce, sizeof(cf_t) * num_re);
				bzero(h, sizeof(cf_t) * num_re);

				refsignal_put(&refs, input);

				refsignal_free(&refs);

				for (i=0;i<CP_NSYMB(cp);i++) {
					for (j=0;j<nof_prb * RE_X_RB;j++) {
						float x = -1+(float) i/CP_NSYMB(cp) + cosf(2 * M_PI * (float) j/nof_prb/RE_X_RB);
						h[i*nof_prb * RE_X_RB+j] = (3+x) * cexpf(I * x);
						input[i*nof_prb * RE_X_RB+j] *= h[i*nof_prb * RE_X_RB+j];
					}
				}

				chest_ce_slot_port(&eq, input, ce, n_slot, n_port);

				mse_mag = mse_phase = 0;
				for (i=0;i<num_re;i++) {
					mse_mag += (cabsf(h[i]) - cabsf(ce[i])) * (cabsf(h[i]) - cabsf(ce[i])) / num_re;
					mse_phase += (cargf(h[i]) - cargf(ce[i])) * (cargf(h[i]) - cargf(ce[i])) / num_re;
				}

				if (check_mse(mse_mag, mse_phase, n_port)) {
					goto do_exit;
				}

				if (fmatlab) {
					fprintf(fmatlab, "input=");
					vec_fprint_c(fmatlab, input, num_re);
					fprintf(fmatlab, ";\n");
					fprintf(fmatlab, "h=");
					vec_fprint_c(fmatlab, h, num_re);
					fprintf(fmatlab, ";\n");
					fprintf(fmatlab, "ce=");
					vec_fprint_c(fmatlab, ce, num_re);
					fprintf(fmatlab, ";\n");
					chest_fprint(&eq, fmatlab, n_slot, n_port);
				}
			}
		}
		chest_free(&eq);
		cid++;
		INFO("cid=%d\n", cid);
	}


	ret = 0;

do_exit:

	if (ce) {
		free(ce);
	}
	if (input) {
		free(input);
	}
	if (h) {
		free(h);
	}

	if (!ret) {
		printf("OK\n");
	} else {
		printf("Error at cid=%d, slot=%d, port=%d\n",cid, n_slot, n_port);
	}

	exit(ret);
}
