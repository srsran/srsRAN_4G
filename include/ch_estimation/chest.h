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

#ifndef CHEST_
#define CHEST_

#include <stdio.h>

#include "ch_estimation/refsignal.h"
#include "filter/filter2d.h"
#include "lte/base.h"

typedef _Complex float cf_t; /* this is only a shortcut */

/** This is an OFDM channel estimator.
 * It works with any reference signal pattern, provided by the object
 * refsignal_t
 * A 2-D filter is used for freq and time channel interpolation.
 *
 */

/* Low-level API */
typedef struct {
	int nof_ports;
	int nof_symbols;
	int nof_prb;
	int symbol_sz;
	lte_cp_t cp;
	refsignal_t refsignal[MAX_PORTS][NSLOTS_X_FRAME];
}chest_t;

int chest_init(chest_t *q, lte_cp_t cp, int nof_prb, int nof_ports);
void chest_free(chest_t *q);

int chest_ref_LTEDL_slot_port(chest_t *q, int port, int nslot, int cell_id);
int chest_ref_LTEDL_slot(chest_t *q, int nslot, int cell_id);
int chest_ref_LTEDL(chest_t *q, int cell_id);

void chest_ce_ref(chest_t *q, cf_t *input, int nslot, int port_id, int nref);
void chest_ce_slot_port(chest_t *q, cf_t *input, cf_t *ce, int nslot, int port_id);
void chest_ce_slot(chest_t *q, cf_t *input, cf_t **ce, int nslot);

void chest_fprint(chest_t *q, FILE *stream, int nslot, int port_id);
void chest_ref_fprint(chest_t *q, FILE *stream, int nslot, int port_id);
void chest_recvsig_fprint(chest_t *q, FILE *stream, int nslot, int port_id);
void chest_ce_fprint(chest_t *q, FILE *stream, int nslot, int port_id);
int chest_ref_symbols(chest_t *q, int port_id, int nslot, int l[2]);

/* High-level API */

/** TODO: The high-level API has N interfaces, one for each port */

typedef struct {
	chest_t obj;
	struct chest_init {
		int nof_symbols; 		// 7 for normal cp, 6 for extended
		int port_id;
		int nof_ports;
		int cell_id;
		int nof_prb;
		int ntime;
		int nfreq;
	} init;
	cf_t *input;
	int in_len;
	struct chest_ctrl_in {
		int slot_id;	// slot id in the 10ms frame
	} ctrl_in;
	cf_t *output;
	int *out_len;
}chest_hl;

#define DEFAULT_FRAME_SIZE		2048

int chest_initialize(chest_hl* h);
int chest_work(chest_hl* hl);
int chest_stop(chest_hl* hl);

#endif
