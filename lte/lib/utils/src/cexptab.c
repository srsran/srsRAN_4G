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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <complex.h>

#include "lte/utils/cexptab.h"

int cexptab_init(cexptab_t *h, int size) {
	int i;

	h->size = size;
	h->tab = malloc(sizeof(cf_t) * size);
	if (h->tab) {
		for (i = 0; i < size; i++) {
			h->tab[i] = cexpf(_Complex_I * 2 * M_PI * (float) i / size);
		}
		return 0;
	} else {
		return -1;
	}
}

void cexptab_free(cexptab_t *h) {
	if (h->tab) {
		free(h->tab);
	}
	bzero(h, sizeof(cexptab_t));
}

void cexptab_gen(cexptab_t *h, cf_t *x, float freq, int len) {
	int i;
	unsigned int idx;
	float phase_inc = freq * h->size;
	float phase=0;

	for (i = 0; i < len; i++) {
		idx = (unsigned int) phase;
		x[i] = h->tab[idx];
		phase += phase_inc;
		if (phase >= (float) h->size) {
			phase -= (float) h->size;
		}
	}
}

void cexptab_gen_direct(cf_t *x, float freq, int len) {
	int i;
	for (i = 0; i < len; i++) {
		x[i] = cexpf(_Complex_I * 2 * M_PI * freq * i);
	}
}

