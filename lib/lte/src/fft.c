/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "lte/base.h"
#include "utils/dft.h"
#include "lte/fft.h"
#include "utils/debug.h"

int lte_fft_init_(lte_fft_t *q, lte_cp_t cp_type, int nof_prb, dft_dir_t dir) {
	int symbol_sz = lte_symbol_sz(nof_prb);

	if (symbol_sz == -1) {
		fprintf(stderr, "Error: Invalid nof_prb=%d\n", nof_prb);
		return -1;
	}
	if (dft_plan_c2c(symbol_sz, dir, &q->fft_plan)) {
		fprintf(stderr, "Error: Creating DFT plan\n");
		return -1;
	}
	q->tmp = malloc(symbol_sz * sizeof(cf_t));
	if (!q->tmp) {
		perror("malloc");
		return -1;
	}

	q->fft_plan.options = DFT_DC_OFFSET | DFT_MIRROR_POS | DFT_NORMALIZE;
	q->symbol_sz = symbol_sz;
	q->nof_symbols = CP_NSYMB(cp_type);
	q->cp_type = cp_type;
	q->nof_re = nof_prb * RE_X_RB;
	q->nof_guards = ((symbol_sz - q->nof_re) / 2);
	DEBUG("Init %s symbol_sz=%d, nof_symbols=%d, cp_type=%s, nof_re=%d, nof_guards=%d\n",
			dir==FORWARD?"FFT":"iFFT", q->symbol_sz, q->nof_symbols,
					q->cp_type==CPNORM?"Normal":"Extended", q->nof_re, q->nof_guards);
	return 0;
}

void lte_fft_free_(lte_fft_t *q) {
	dft_plan_free(&q->fft_plan);
	bzero(q, sizeof(lte_fft_t));
}

int lte_fft_init(lte_fft_t *q, lte_cp_t cp_type, int nof_prb) {
	return lte_fft_init_(q, cp_type, nof_prb, FORWARD);
}

void lte_fft_free(lte_fft_t *q) {
	lte_fft_free_(q);
}

int lte_ifft_init(lte_fft_t *q, lte_cp_t cp_type, int nof_prb) {
	return lte_fft_init_(q, cp_type, nof_prb, BACKWARD);
}

void lte_ifft_free(lte_fft_t *q) {
	lte_fft_free_(q);
}

/* Transforms input samples into output OFDM symbols.
 * Performs FFT on a each symbol and removes CP.
 */
void lte_fft_run(lte_fft_t *q, cf_t *input, cf_t *output) {
	int i;
	for (i=0;i<q->nof_symbols;i++) {
		input += CP_ISNORM(q->cp_type)?CP_NORM(i, q->symbol_sz):CP_EXT(q->symbol_sz);
		dft_run_c2c(&q->fft_plan, input, q->tmp);
		memcpy(output, &q->tmp[q->nof_guards], q->nof_re * sizeof(cf_t));
		input += q->symbol_sz;
		output += q->nof_re;
	}
}

/* Transforms input OFDM symbols into output samples.
 * Performs FFT on a each symbol and adds CP.
 */
void lte_ifft_run(lte_fft_t *q, cf_t *input, cf_t *output) {
	fprintf(stderr, "Error: Not implemented\n");
}

