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


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "lte/phch/dci.h"
#include "lte/common/base.h"
#include "lte/utils/bit.h"
#include "lte/utils/vector.h"
#include "lte/utils/debug.h"


int dci_init(dci_t *q, int nof_dcis) {
	q->msg = calloc(sizeof(dci_msg_t), nof_dcis);
	if (!q->msg) {
		perror("malloc");
		return -1;
	}
	q->nof_dcis = nof_dcis;
	return 0;
}

void dci_free(dci_t *q) {
	if (q->msg) {
		free(q->msg);
	}
}

void dci_candidate_fprint(FILE *f, dci_candidate_t *q) {
	fprintf(f, "L: %d, nCCE: %d, RNTI: 0x%x, nBits: %d\n",
			q->L, q->ncce, q->rnti, q->nof_bits);
}

int dci_format1_add(dci_t *q, dci_format1_t *msg) {
	int i, j;
	i=0;
	while(i<q->nof_dcis && q->msg[i].location.nof_bits)
		i++;
	if (i == q->nof_dcis) {
		fprintf(stderr, "No more space in DCI container\n");
		return -1;
	}
	q->msg[i].location.L = 0;
	q->msg[i].location.ncce = 0;
	q->msg[i].location.nof_bits = dci_format1_sizeof();
	q->msg[i].location.rnti = 1234;
	for (j=0;j<q->msg[i].location.nof_bits;j++) {
		q->msg[i].data[j] = rand()%2;
	}
	return 0;
}

int dci_format0_add(dci_t *q, dci_format0_t *msg, int L, int nCCE, unsigned short rnti) {
	int i, j;
	i=0;
	while(i<q->nof_dcis && q->msg[i].location.nof_bits)
		i++;
	if (i == q->nof_dcis) {
		fprintf(stderr, "No more space in DCI container\n");
		return -1;
	}
	q->msg[i].location.L = L;
	q->msg[i].location.ncce = nCCE;
	q->msg[i].location.nof_bits = dci_format0_sizeof(msg->n_rb_ul);
	q->msg[i].location.rnti = rnti;
	for (j=0;j<q->msg[i].location.nof_bits;j++) {
		q->msg[i].data[j] = rand()%2;
	}
	return 0;
}

int dci_format0_sizeof(int nof_prb) {
	return 1+1+(int) ceilf(log2f(nof_prb*(nof_prb+1)/2))+2+3+1;
}

int dci_format1_sizeof(int nof_prb, int P) {
	return (nof_prb>10)?1:0+(int) ceilf(log2f(nof_prb/P))+5+3+1+2+2;
}

int dci_format1A_sizeof(int nof_prb, bool random_access_initiated) {
	if (random_access_initiated) {
		return 1+(int) ceilf(log2f(nof_prb*(nof_prb+1)/2))+6+4;
	} else {
		return 1+(int) ceilf(log2f(nof_prb*(nof_prb+1)/2))+5+3+1+2+2;
	}
}

int dci_format1C_sizeof() {
	return 10;
}
