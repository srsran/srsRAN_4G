/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"

typedef _Complex float cf_t;

/* Generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 */
SRSLTE_API int layermap_single(cf_t *d, cf_t *x, int nof_symbols);
SRSLTE_API int layermap_diversity(cf_t *d, cf_t *x[SRSLTE_MAX_LAYERS], int nof_layers, int nof_symbols);
SRSLTE_API int layermap_multiplex(cf_t *d[SRSLTE_MAX_CODEWORDS], cf_t *x[SRSLTE_MAX_LAYERS], int nof_cw, int nof_layers,
    int nof_symbols[SRSLTE_MAX_CODEWORDS]);
SRSLTE_API int layermap_type(cf_t *d[SRSLTE_MAX_CODEWORDS], cf_t *x[SRSLTE_MAX_LAYERS], int nof_cw, int nof_layers,
    int nof_symbols[SRSLTE_MAX_CODEWORDS], srslte_mimo_type_t type);


/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 */
SRSLTE_API int layerdemap_single(cf_t *x, cf_t *d, int nof_symbols);
SRSLTE_API int layerdemap_diversity(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *d, int nof_layers, int nof_layer_symbols);
SRSLTE_API int layerdemap_multiplex(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *d[SRSLTE_MAX_CODEWORDS], int nof_layers, int nof_cw,
    int nof_layer_symbols, int nof_symbols[SRSLTE_MAX_CODEWORDS]);
SRSLTE_API int layerdemap_type(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *d[SRSLTE_MAX_CODEWORDS], int nof_layers, int nof_cw,
    int nof_layer_symbols, int nof_symbols[SRSLTE_MAX_CODEWORDS], srslte_mimo_type_t type);

#endif // LAYERMAP_H_
