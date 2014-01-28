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

#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "lte/base.h"

void phch_cp_prb_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb, bool advance_output) {
	int i;

	int ref_interval = ((RE_X_RB / nof_refs) - 1);
	memcpy(*output, *input, offset * sizeof(cf_t));
	*input += offset;
	*output += offset;
	for (i = 0; i < nof_refs * nof_prb - 1; i++) {
		if (advance_output) {
			(*output)++;
		} else {
			(*input)++;
		}
		memcpy(*output, *input, ref_interval * sizeof(cf_t));
		*output += ref_interval;
		*input += ref_interval;
	}
	if (ref_interval - offset > 0) {
		if (advance_output) {
			(*output)++;
		} else {
			(*input)++;
		}
		memcpy(*output, *input, (ref_interval - offset) * sizeof(cf_t));
		*output += (ref_interval - offset);
		*input += (ref_interval - offset);
	}
}

void phch_cp_prb(cf_t **input, cf_t **output, int nof_prb) {
	memcpy(*output, *input, sizeof(cf_t) * RE_X_RB * nof_prb);
	*input += nof_prb * RE_X_RB;
	*output += nof_prb * RE_X_RB;
}

void phch_put_prb_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb) {
	phch_cp_prb_ref(input, output, offset, nof_refs, nof_prb, false);
}

void phch_get_prb_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb) {
	phch_cp_prb_ref(input, output, offset, nof_refs, nof_prb, true);
}

