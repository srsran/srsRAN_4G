/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#ifndef SRSLTE_LAYERMAP_H
#define SRSLTE_LAYERMAP_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

/* Generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 */
SRSLTE_API int srslte_layermap_single(cf_t* d, cf_t* x, int nof_symbols);

SRSLTE_API int srslte_layermap_diversity(cf_t* d, cf_t* x[SRSLTE_MAX_LAYERS], int nof_layers, int nof_symbols);

SRSLTE_API int srslte_layermap_multiplex(cf_t* d[SRSLTE_MAX_CODEWORDS],
                                         cf_t* x[SRSLTE_MAX_LAYERS],
                                         int   nof_cw,
                                         int   nof_layers,
                                         int   nof_symbols[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_layermap_type(cf_t*              d[SRSLTE_MAX_CODEWORDS],
                                    cf_t*              x[SRSLTE_MAX_LAYERS],
                                    int                nof_cw,
                                    int                nof_layers,
                                    int                nof_symbols[SRSLTE_MAX_CODEWORDS],
                                    srslte_tx_scheme_t type);

/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 */
SRSLTE_API int srslte_layerdemap_single(cf_t* x, cf_t* d, int nof_symbols);

SRSLTE_API int srslte_layerdemap_diversity(cf_t* x[SRSLTE_MAX_LAYERS], cf_t* d, int nof_layers, int nof_layer_symbols);

SRSLTE_API int srslte_layerdemap_multiplex(cf_t* x[SRSLTE_MAX_LAYERS],
                                           cf_t* d[SRSLTE_MAX_CODEWORDS],
                                           int   nof_layers,
                                           int   nof_cw,
                                           int   nof_layer_symbols,
                                           int   nof_symbols[SRSLTE_MAX_CODEWORDS]);

SRSLTE_API int srslte_layerdemap_type(cf_t*              x[SRSLTE_MAX_LAYERS],
                                      cf_t*              d[SRSLTE_MAX_CODEWORDS],
                                      int                nof_layers,
                                      int                nof_cw,
                                      int                nof_layer_symbols,
                                      int                nof_symbols[SRSLTE_MAX_CODEWORDS],
                                      srslte_tx_scheme_t type);

SRSLTE_API int srslte_layermap_nr(cf_t** d, int nof_cw, cf_t** x, int nof_layers, uint32_t nof_re);

SRSLTE_API int srslte_layerdemap_nr(cf_t** d, int nof_cw, cf_t** x, int nof_layers, uint32_t nof_re);

#endif // SRSLTE_LAYERMAP_H
