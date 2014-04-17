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


#ifndef LTESEQ_
#define LTESEQ_

#include "lte/common/base.h"

typedef struct {
	char *c;
	int len;
}sequence_t;

int sequence_init(sequence_t *q, int len);
void sequence_free(sequence_t *q);

int sequence_LTEPRS(sequence_t *q, int len, int seed);

int sequence_pbch(sequence_t *seq, lte_cp_t cp, int cell_id);
int sequence_pcfich(sequence_t *seq, int nslot, int cell_id);
int sequence_phich(sequence_t *seq, int nslot, int cell_id);
int sequence_pdcch(sequence_t *seq, int nslot, int cell_id, int len);

#endif
