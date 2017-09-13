/**
 *
 * \section COPYRIGHT
 *
* Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <string.h>
#include "srslte/srslte.h"
#include "srslte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the predecoder
 */

#define INPUT    prhs[0]
#define NLAYERS  prhs[1]
#define NPORTS   prhs[2]
#define TXSCHEME prhs[3]
#define NOF_INPUTS 3


void help()
{
  mexErrMsgTxt
    ("[output] = srslte_decoder(input, NLayers, NCellRefP, TxScheme)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  cf_t *input = NULL;
  cf_t *output = NULL; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  
  // Read input symbols
  int nof_symbols = mexutils_read_cf(INPUT, &input);
  if (nof_symbols < 0) {
    mexErrMsgTxt("Error reading input\n");
    return; 
  }
  uint32_t nof_layers   = mxGetScalar(NLAYERS); 
  uint32_t nof_tx_ports = mxGetScalar(NPORTS); 
  uint32_t nof_codewords = 1; 
  
  mexPrintf("nof_tx_ports=%d, nof_layers=%d, nof_symbols=%d\n", nof_tx_ports, nof_layers, nof_symbols);
  
  cf_t *y[SRSLTE_MAX_PORTS]; 
  cf_t *x[SRSLTE_MAX_LAYERS]; 
  cf_t *d[SRSLTE_MAX_CODEWORDS];

  d[0] = input; // Single codeword supported only 
  
  /* Allocate memory */
  for (int i = 0; i < nof_layers; i++) {
    x[i] = srslte_vec_malloc(sizeof(cf_t)*nof_symbols/nof_layers);
  }
  
  output = srslte_vec_malloc(sizeof(cf_t)*nof_symbols*nof_tx_ports);
  for (int i=0;i<nof_tx_ports;i++) {
    y[i] = &output[i*nof_symbols];
  }
  
  char *txscheme = "Port0";
  if (nrhs >= NOF_INPUTS) {
    txscheme = mxArrayToString(TXSCHEME);
  }  
  srslte_mimo_type_t type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA; 
  if (!strcmp(txscheme, "Port0")) {
    type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
  } else if (!strcmp(txscheme, "TxDiversity")) {
    type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
  } else if (!strcmp(txscheme, "CDD")) {    
    type = SRSLTE_MIMO_TYPE_CDD;
  } else if (!strcmp(txscheme, "SpatialMux")) {    
    type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
  } else {
    mexPrintf("Unsupported TxScheme=%s\n", txscheme);
    return; 
  }
  int symbols_layers[SRSLTE_MAX_LAYERS];
  for (int i=0;i<nof_layers;i++) {
    symbols_layers[i] = nof_symbols;
  }
  srslte_layermap_type(d, x, nof_codewords, nof_layers, symbols_layers, type);
  srslte_precoding_type(x, y, nof_layers, nof_tx_ports, nof_symbols/nof_layers, type);
  
  if (nlhs >= 1) { 
    switch (type) {
      case SRSLTE_MIMO_TYPE_CDD:
        mexutils_write_cf(output, &plhs[0], nof_symbols/nof_layers, nof_tx_ports);
        break;
      case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
      case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
      case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
      default:
        mexutils_write_cf(output, &plhs[0], (uint32_t) nof_symbols, nof_tx_ports);
        break;
    }
  }

  if (nlhs >= 2) {
    mexutils_write_cf(x[0], &plhs[1], nof_symbols / nof_layers, 1);
  }
  if (nlhs >= 3) {
    mexutils_write_cf(x[1], &plhs[2], nof_symbols / nof_layers, 1);
  }
  
  if (input) {
    free(input);
  }
  if (output) {
    free(output);
  }
  for (int i=0;i<nof_layers;i++) {
    if (x[i]) {
      free(x[i]);      
    }
  }

  return;
}

