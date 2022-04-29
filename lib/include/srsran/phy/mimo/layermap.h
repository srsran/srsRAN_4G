/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         layermap.h
 *
 *  Description:  MIMO layer mapping and demapping.
 *                Single antenna, tx diversity and spatial multiplexing are
 *                supported.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.3.3
 *****************************************************************************/

#ifndef SRSRAN_LAYERMAP_H
#define SRSRAN_LAYERMAP_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

/* Generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 */
SRSRAN_API int srsran_layermap_single(cf_t* d, cf_t* x, int nof_symbols);

SRSRAN_API int srsran_layermap_diversity(cf_t* d, cf_t* x[SRSRAN_MAX_LAYERS], int nof_layers, int nof_symbols);

SRSRAN_API int srsran_layermap_multiplex(cf_t* d[SRSRAN_MAX_CODEWORDS],
                                         cf_t* x[SRSRAN_MAX_LAYERS],
                                         int   nof_cw,
                                         int   nof_layers,
                                         int   nof_symbols[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API int srsran_layermap_type(cf_t*              d[SRSRAN_MAX_CODEWORDS],
                                    cf_t*              x[SRSRAN_MAX_LAYERS],
                                    int                nof_cw,
                                    int                nof_layers,
                                    int                nof_symbols[SRSRAN_MAX_CODEWORDS],
                                    srsran_tx_scheme_t type);

/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 */
SRSRAN_API int srsran_layerdemap_single(cf_t* x, cf_t* d, int nof_symbols);

SRSRAN_API int srsran_layerdemap_diversity(cf_t* x[SRSRAN_MAX_LAYERS], cf_t* d, int nof_layers, int nof_layer_symbols);

SRSRAN_API int srsran_layerdemap_multiplex(cf_t* x[SRSRAN_MAX_LAYERS],
                                           cf_t* d[SRSRAN_MAX_CODEWORDS],
                                           int   nof_layers,
                                           int   nof_cw,
                                           int   nof_layer_symbols,
                                           int   nof_symbols[SRSRAN_MAX_CODEWORDS]);

SRSRAN_API int srsran_layerdemap_type(cf_t*              x[SRSRAN_MAX_LAYERS],
                                      cf_t*              d[SRSRAN_MAX_CODEWORDS],
                                      int                nof_layers,
                                      int                nof_cw,
                                      int                nof_layer_symbols,
                                      int                nof_symbols[SRSRAN_MAX_CODEWORDS],
                                      srsran_tx_scheme_t type);

SRSRAN_API int srsran_layermap_nr(cf_t** d, int nof_cw, cf_t** x, int nof_layers, uint32_t nof_re);

SRSRAN_API int srsran_layerdemap_nr(cf_t** d, int nof_cw, cf_t** x, int nof_layers, uint32_t nof_re);

#endif // SRSRAN_LAYERMAP_H
