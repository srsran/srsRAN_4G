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

#ifndef PBCH_
#define PBCH_

#include "lte/base.h"
#include "modem/mod.h"
#include "modem/demod_soft.h"
#include "scrambling/scrambling.h"
#include "ratematching/rm_conv.h"
#include "fec/convcoder.h"
#include "fec/viterbi.h"
#include "fec/crc.h"

#define PBCH_RE_CPNORM		240
#define PBCH_RE_CPEXT		216

typedef _Complex float cf_t;

enum phich_length { NORMAL, EXTENDED};
enum phich_resources { R_1_6, R_1_2, R_1, R_2};

typedef struct {
	int nof_ports;
	int nof_prb;
	int sfn;
	enum phich_length phich_length;
	int phich_resources;
}pbch_mib_t;

/* PBCH receiver */
typedef struct {
	int cell_id;
	lte_cp_t cp;

	/* buffers */
	cf_t *pbch_symbols;
	float *pbch_llr;
	float *temp;
	float *pbch_rm;
	char *data;

	int frame_idx;

	/* tx & rx objects */
	modem_table_t mod;
	demod_soft_t demod;
	sequence_t seq_pbch;
	viterbi_t decoder;

}pbch_t;

int pbch_init(pbch_t *q, int cell_id, lte_cp_t cp);
void pbch_free(pbch_t *q);
int pbch_decode(pbch_t *q, cf_t *slot1_symbols, cf_t **ce, int nof_ports, int nof_prb, float ebno, pbch_mib_t *data);
void pbch_mib_fprint(FILE *stream, pbch_mib_t *mib);


bool pbch_exists(int nframe, int nslot);
int pbch_put(cf_t *pbch, cf_t *slot1_data, int nof_prb, lte_cp_t cp, int cell_id);
int pbch_get(cf_t *pbch, cf_t *slot1_data, int nof_prb, lte_cp_t cp, int cell_id);

#endif
