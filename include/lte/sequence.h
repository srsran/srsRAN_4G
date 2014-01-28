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

#ifndef LTESEQ_
#define LTESEQ_

#include "lte/base.h"

typedef struct {
	char *c;
	int len;
}sequence_t;

int sequence_init(sequence_t *q, int len);
void sequence_free(sequence_t *q);

int sequence_LTEPRS(sequence_t *q, int len, int seed);

int sequence_pbch(sequence_t *seq, lte_cp_t cp, int cell_id);
int sequence_pbch_crc(sequence_t *seq, int nof_ports);

#endif
