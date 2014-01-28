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
#include <strings.h>
#include <stdlib.h>

#include "lte/base.h"
#include "utils/dft.h"
#include "lte/fft.h"

int lte_fft_init_(lte_fft_t *q, lte_cp_t cp_type, int symbol_sz, dft_dir_t dir) {

	if (dft_plan_c2c(symbol_sz, dir, &q->fft_plan)) {
		return -1;
	}
	q->fft_plan.options = DFT_DC_OFFSET | DFT_MIRROR_POS | DFT_NORMALIZE;
	q->nof_symbols = CP_NSYMB(cp_type);
	q->symbol_sz = symbol_sz;
	q->cp_type = cp_type;
	return 0;
}
void lte_fft_free_(lte_fft_t *q) {
	dft_plan_free(&q->fft_plan);
	bzero(q, sizeof(lte_fft_t));
}

int lte_fft_init(lte_fft_t *q, lte_cp_t cp_type, int symbol_sz) {
	return lte_fft_init_(q, cp_type, symbol_sz, FORWARD);
}
void lte_fft_free(lte_fft_t *q) {
	lte_fft_free_(q);
}
int lte_ifft_init(lte_fft_t *q, lte_cp_t cp_type, int symbol_sz) {
	return lte_fft_init_(q, cp_type, symbol_sz, BACKWARD);
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
		dft_run_c2c(&q->fft_plan, input, output);
		input += q->symbol_sz;
		output += q->symbol_sz;
	}
}

/* Transforms input OFDM symbols into output samples.
 * Performs FFT on a each symbol and adds CP.
 */
void lte_ifft_run(lte_fft_t *q, cf_t *input, cf_t *output) {
	fprintf(stderr, "Error: Not implemented\n");
}

