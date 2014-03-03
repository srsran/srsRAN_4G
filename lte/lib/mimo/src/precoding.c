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



#include <stdio.h>
#include <assert.h>
#include <complex.h>
#include <string.h>

#include "lte/common/base.h"
#include "lte/mimo/precoding.h"
#include "lte/utils/vector.h"

/* 36.211 v10.3.0 Section 6.3.4 */
void precoding_decode(cf_t *y[MAX_PORTS], cf_t *ce[MAX_PORTS],
		cf_t *x[MAX_LAYERS], int nof_ports, int nof_symbols, mimo_type_t type) {

	int i;
	cf_t h0, h1, r0, r1;
	float hh;

	switch(nof_ports) {
	case 1:
		vec_div_ccc(y[0], ce[0], x[0], nof_symbols);
		break;
	case 2:
		switch(type) {
		case TX_DIVERSITY:
			/* FIXME: Use VOLK here */
			// 6.3.4.3
			for (i=0;i<nof_symbols/2;i++) {
				h0 = ce[0][2*i];
				h1 = ce[1][2*i];
				hh = cabs(h0)*cabs(h0)+cabs(h1)*cabs(h1);
				r0 = y[0][2*i];
				r1 = y[0][2*i+1];
				x[0][i] = (conj(h0)*r0 + h1*conj(r1))/hh;
				x[1][i] = (h0*conj(r1) - h1*conj(r0))/hh;
			}
			break;
		default:
			printf("Error: Unsupported transmit mode\n");
		}
		break;
	default:
		printf("Error: Unsupported nof_ports=%d\n", nof_ports);
		return;
	}
}

void precoding_encode(cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_ports, int nof_symbols,
		mimo_type_t type) {
	switch(nof_ports) {
	case 1:
		memcpy(y[0], x[0], nof_symbols * sizeof(cf_t));
		break;
	default:
		printf("Error: Unsupported nof_ports=%d\n", nof_ports);
		return;
	}
}
