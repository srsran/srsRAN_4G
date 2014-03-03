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
#include <string.h>

#include "lte/common/base.h"
#include "lte/mimo/layermap.h"

/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 */
void layermap_decode(cf_t *x[MAX_LAYERS], cf_t *d[MAX_CODEWORDS], int nof_layers, int nof_cw,
		int nof_layer_symbols, mimo_type_t type) {

	int i;

	switch(nof_layers) {
	case 1:
		memcpy(d[0], x[0], nof_layer_symbols * sizeof(cf_t));
		break;
	case 2:
		switch(type) {
		case TX_DIVERSITY:
			for (i=0;i<nof_layer_symbols;i++) {
				d[0][2*i] = x[0][i];
				d[0][2*i+1] = x[1][i];
			}
			break;
		default:
			printf("Error: Unsupported transmit mode\n");
		}
		break;
	default:
		printf("Error: Unsupported nof_ports=%d\n", nof_layers);
		return;
	}
}

/* Generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 */
void layermap_encode(cf_t *d[MAX_CODEWORDS], cf_t *x[MAX_LAYERS], int nof_layers, int nof_cw,
		int nof_symbols, mimo_type_t type) {
	switch(nof_layers) {
	case 1:
		memcpy(x[0], d[0], nof_symbols * sizeof(cf_t));
		break;
	default:
		printf("Error: Unsupported nof_ports=%d\n", nof_layers);
		return;
	}
}

