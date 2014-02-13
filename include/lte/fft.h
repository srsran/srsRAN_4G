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

#ifndef LTEFFT_
#define LTEFFT_


#include <strings.h>
#include <stdlib.h>

#include "lte/base.h"
#include "utils/dft.h"

typedef _Complex float cf_t; /* this is only a shortcut */

/* This is common for both directions */
typedef struct {
	dft_plan_t fft_plan;
	int nof_symbols;
	int symbol_sz;
	int nof_guards;
	int nof_re;
	lte_cp_t cp_type;
	cf_t *tmp; // for removing zero padding
}lte_fft_t;

int lte_fft_init(lte_fft_t *q, lte_cp_t cp_type, int nof_prb);
void lte_fft_free(lte_fft_t *q);
void lte_fft_run(lte_fft_t *q, cf_t *input, cf_t *output);

int lte_ifft_init(lte_fft_t *q, lte_cp_t cp_type, int nof_prb);
void lte_ifft_free(lte_fft_t *q);
void lte_ifft_run(lte_fft_t *q, cf_t *input, cf_t *output);

#endif
