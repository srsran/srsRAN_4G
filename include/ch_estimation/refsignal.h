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

#ifndef REFSIGNAL_
#define REFSIGNAL_


/* Object to manage reference signals for OFDM channel equalization.
 *
 * It generates the reference signals for LTE.
 *
 */

#include "lte/base.h"

typedef _Complex float cf_t;

typedef struct {
	int time_idx;
	int freq_idx;
	cf_t simbol;
	cf_t recv_simbol;
}ref_t;

typedef struct {
	int nof_refs;		// number of reference signals
	int *symbols_ref; 	// symbols with at least one reference
	int nsymbols;		// number of symbols with at least one reference
	int voffset;		// offset of the first reference in the freq domain
	ref_t *refs;
	cf_t *ch_est;
} refsignal_t;

int refsignal_init_LTEDL(refsignal_t *q, int port_id, int nslot,
		int cell_id, lte_cp_t cp, int nof_prb);

void refsignal_free(refsignal_t *q);

#endif
