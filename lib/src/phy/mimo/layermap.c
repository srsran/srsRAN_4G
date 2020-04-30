/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

int srslte_layermap_single(cf_t* d, cf_t* x, int nof_symbols)
{
  memcpy(x, d, sizeof(cf_t) * nof_symbols);
  return nof_symbols;
}

int srslte_layermap_diversity(cf_t* d, cf_t* x[SRSLTE_MAX_LAYERS], int nof_layers, int nof_symbols)
{
  int i, j;
  for (i = 0; i < nof_symbols / nof_layers; i++) {
    for (j = 0; j < nof_layers; j++) {
      x[j][i] = d[nof_layers * i + j];
    }
  }
  return i;
}

int srslte_layermap_multiplex(cf_t* d[SRSLTE_MAX_CODEWORDS],
                              cf_t* x[SRSLTE_MAX_LAYERS],
                              int   nof_cw,
                              int   nof_layers,
                              int   nof_symbols[SRSLTE_MAX_CODEWORDS])
{
  if (nof_cw == nof_layers) {
    for (int i = 0; i < nof_cw; i++) {
      srslte_vec_cf_copy(x[i], d[i], (uint32_t)nof_symbols[0]);
    }
    return nof_symbols[0];
  } else if (nof_cw == 1) {
    return srslte_layermap_diversity(d[0], x, nof_layers, nof_symbols[0]);
  } else {
    int n[2];
    n[0] = nof_layers / nof_cw;
    n[1] = nof_layers - n[0];
    if (nof_symbols[0] / n[0] == nof_symbols[1] / n[1]) {

      srslte_layermap_diversity(d[0], x, n[0], nof_symbols[0]);
      srslte_layermap_diversity(d[1], &x[n[0]], n[1], nof_symbols[1]);
      return nof_symbols[0] / n[0];

    } else {
      ERROR("Number of symbols in codewords 0 and 1 is not consistent (%d, %d)\n", nof_symbols[0], nof_symbols[1]);
      return -1;
    }
  }
  return 0;
}

/* Layer mapping generates the vector of layer-mapped symbols "x" based on the vector of data symbols "d"
 * Based on 36.211 6.3.3
 * Returns the number of symbols per layer (M_symb^layer in the specs)
 */
int srslte_layermap_type(cf_t*              d[SRSLTE_MAX_CODEWORDS],
                         cf_t*              x[SRSLTE_MAX_LAYERS],
                         int                nof_cw,
                         int                nof_layers,
                         int                nof_symbols[SRSLTE_MAX_CODEWORDS],
                         srslte_tx_scheme_t type)
{

  if (nof_cw > SRSLTE_MAX_CODEWORDS) {
    ERROR("Maximum number of codewords is %d (nof_cw=%d)\n", SRSLTE_MAX_CODEWORDS, nof_cw);
    return -1;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    ERROR("Maximum number of layers is %d (nof_layers=%d)\n", SRSLTE_MAX_LAYERS, nof_layers);
    return -1;
  }
  if (nof_layers < nof_cw) {
    ERROR("Number of codewords must be lower or equal than number of layers\n");
    return -1;
  }

  switch (type) {
    case SRSLTE_TXSCHEME_PORT0:
      if (nof_cw == 1 && nof_layers == 1) {
        return srslte_layermap_single(d[0], x[0], nof_symbols[0]);
      } else {
        ERROR("Number of codewords and layers must be 1 for transmission on single antenna ports\n");
        return -1;
      }
      break;
    case SRSLTE_TXSCHEME_DIVERSITY:
      if (nof_cw == 1) {
        if (nof_layers == 2 || nof_layers == 4) {
          return srslte_layermap_diversity(d[0], x, nof_layers, nof_symbols[0]);
        } else {
          ERROR("Number of layers must be 2 or 4 for transmit diversity\n");
          return -1;
        }
      } else {
        ERROR("Number of codewords must be 1 for transmit diversity\n");
        return -1;
      }
      break;
    case SRSLTE_TXSCHEME_SPATIALMUX:
    case SRSLTE_TXSCHEME_CDD:
      return srslte_layermap_multiplex(d, x, nof_cw, nof_layers, nof_symbols);
      break;
  }
  return 0;
}

int srslte_layerdemap_single(cf_t* x, cf_t* d, int nof_symbols)
{
  memcpy(d, x, sizeof(cf_t) * nof_symbols);
  return nof_symbols;
}
int srslte_layerdemap_diversity(cf_t* x[SRSLTE_MAX_LAYERS], cf_t* d, int nof_layers, int nof_layer_symbols)
{
  int i, j;
  for (i = 0; i < nof_layer_symbols; i++) {
    for (j = 0; j < nof_layers; j++) {
      d[nof_layers * i + j] = x[j][i];
    }
  }
  return nof_layer_symbols * nof_layers;
}

int srslte_layerdemap_multiplex(cf_t* x[SRSLTE_MAX_LAYERS],
                                cf_t* d[SRSLTE_MAX_CODEWORDS],
                                int   nof_layers,
                                int   nof_cw,
                                int   nof_layer_symbols,
                                int   nof_symbols[SRSLTE_MAX_CODEWORDS])
{
  if (nof_cw == 1) {
    return srslte_layerdemap_diversity(x, d[0], nof_layers, nof_layer_symbols);
  } else {
    int n[2];
    n[0]           = nof_layers / nof_cw;
    n[1]           = nof_layers - n[0];
    nof_symbols[0] = n[0] * nof_layer_symbols;
    nof_symbols[1] = n[1] * nof_layer_symbols;

    nof_symbols[0] = srslte_layerdemap_diversity(x, d[0], n[0], nof_layer_symbols);
    nof_symbols[1] = srslte_layerdemap_diversity(&x[n[0]], d[1], n[1], nof_layer_symbols);
  }
  return 0;
}

/* Generates the vector of data symbols "d" based on the vector of layer-mapped symbols "x"
 * Based on 36.211 6.3.3
 * Returns 0 on ok and saves the number of symbols per codeword (M_symb^(q) in the specs) in
 * nof_symbols. Returns -1 on error
 */
int srslte_layerdemap_type(cf_t*              x[SRSLTE_MAX_LAYERS],
                           cf_t*              d[SRSLTE_MAX_CODEWORDS],
                           int                nof_layers,
                           int                nof_cw,
                           int                nof_layer_symbols,
                           int                nof_symbols[SRSLTE_MAX_CODEWORDS],
                           srslte_tx_scheme_t type)
{

  if (nof_cw > SRSLTE_MAX_CODEWORDS) {
    ERROR("Maximum number of codewords is %d (nof_cw=%d)\n", SRSLTE_MAX_CODEWORDS, nof_cw);
    return -1;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    ERROR("Maximum number of layers is %d (nof_layers=%d)\n", SRSLTE_MAX_LAYERS, nof_layers);
    return -1;
  }
  if (nof_layers < nof_cw) {
    ERROR("Number of codewords must be lower or equal than number of layers\n");
    return -1;
  }

  switch (type) {
    case SRSLTE_TXSCHEME_PORT0:
      if (nof_cw == 1 && nof_layers == 1) {
        nof_symbols[0] = srslte_layerdemap_single(x[0], d[0], nof_layer_symbols);
        nof_symbols[1] = 0;
      } else {
        ERROR("Number of codewords and layers must be 1 for transmission on single antenna ports\n");
        return -1;
      }
      break;
    case SRSLTE_TXSCHEME_DIVERSITY:
      if (nof_cw == 1) {
        if (nof_layers == 2 || nof_layers == 4) {
          nof_symbols[0] = srslte_layerdemap_diversity(x, d[0], nof_layers, nof_layer_symbols);
          nof_symbols[1] = 0;
        } else {
          ERROR("Number of layers must be 2 or 4 for transmit diversity\n");
          return -1;
        }
      } else {
        ERROR("Number of codewords must be 1 for transmit diversity\n");
        return -1;
      }
      break;
    case SRSLTE_TXSCHEME_SPATIALMUX:
    case SRSLTE_TXSCHEME_CDD:
      return srslte_layerdemap_multiplex(x, d, nof_layers, nof_cw, nof_layer_symbols, nof_symbols);
      break;
  }
  return 0;
}
