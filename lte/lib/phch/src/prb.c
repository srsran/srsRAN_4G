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


#include <stdbool.h>
#include <string.h>

#include "prb.h"
#include "lte/common/base.h"

void prb_cp_ref(cf_t **input, cf_t **output, int offset, int nof_refs,
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

void prb_cp(cf_t **input, cf_t **output, int nof_prb) {
	memcpy(*output, *input, sizeof(cf_t) * RE_X_RB * nof_prb);
	*input += nof_prb * RE_X_RB;
	*output += nof_prb * RE_X_RB;
}


void prb_cp_half(cf_t **input, cf_t **output, int nof_prb) {
	memcpy(*output, *input, sizeof(cf_t) * RE_X_RB * nof_prb / 2);
	*input += nof_prb * RE_X_RB / 2;
	*output += nof_prb * RE_X_RB / 2;
}

void prb_put_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb) {
	prb_cp_ref(input, output, offset, nof_refs, nof_prb, false);
}

void prb_get_ref_(cf_t **input, cf_t **output, int offset, int nof_refs,
		int nof_prb) {
	prb_cp_ref(input, output, offset, nof_refs, nof_prb, true);
}

