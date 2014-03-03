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


#ifndef LAYERMAP_H_
#define LAYERMAP_H_

typedef _Complex float cf_t;

/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 */
void layermap_decode(cf_t *x[MAX_LAYERS], cf_t *d[MAX_CODEWORDS], int nof_layers, int nof_cw,
		int nof_layer_symbols, mimo_type_t type);

/* Generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 */
void layermap_encode(cf_t *d[MAX_CODEWORDS], cf_t *x[MAX_LAYERS], int nof_layers, int nof_cw,
		int nof_symbols, mimo_type_t type);

#endif
