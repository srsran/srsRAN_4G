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

#define INPUT prhs[0]
#define HEST  prhs[1]
#define NEST  prhs[2]
#define NOF_INPUTS 2


void help()
{
  mexErrMsgTxt
    ("[output] = srslte_predecoder(input, hest, nest)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  cf_t *input = NULL;
  cf_t *hest = NULL; 
  cf_t *output = NULL; 
  uint32_t nof_symbols = 0; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  
  // Read input symbols
  if (mexutils_read_cf(INPUT, &input) < 0) {
    mexErrMsgTxt("Error reading input\n");
    return; 
  }
  uint32_t nof_tx_ports = 1; 
  uint32_t nof_rx_ants  = 1; 
  const mwSize *dims = mxGetDimensions(INPUT);
  mwSize ndims = mxGetNumberOfDimensions(INPUT);
  nof_symbols = dims[0];
  
  if (ndims >= 2) {
    nof_rx_ants = dims[1];
  }
  
  // Read channel estimates
  if (mexutils_read_cf(HEST, &hest) < 0) {
    mexErrMsgTxt("Error reading hest\n");
    return; 
  }
  dims = mxGetDimensions(HEST);
  ndims = mxGetNumberOfDimensions(HEST);

  if (ndims == 3) {
    nof_tx_ports = dims[2];        
  }
  
  mexPrintf("nof_tx_ports=%d, nof_rx_ants=%d, nof_symbols=%d\n", nof_tx_ports, nof_rx_ants, nof_symbols);

  // Read noise estimate
  float noise_estimate = 0; 
  if (nrhs >= NOF_INPUTS) {
    noise_estimate = mxGetScalar(NEST);
  }
  
  cf_t *x[SRSLTE_MAX_LAYERS]; 
  cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  cf_t *y[SRSLTE_MAX_PORTS];

  for (int i=0;i<SRSLTE_MAX_LAYERS;i++) {
    x[i] = NULL; 
  }
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    for (int j=0;j<SRSLTE_MAX_PORTS;j++) {
      h[i][j] = NULL; 
    }
  }
  
  /* Allocate memory */
  output = srslte_vec_malloc(sizeof(cf_t)*nof_symbols);
  for (int i = 0; i < nof_tx_ports; i++) {
    x[i] = srslte_vec_malloc(sizeof(cf_t)*nof_symbols);
    for (int j=0;j<nof_rx_ants;j++) {
      h[i][j] = &hest[i*nof_symbols*nof_rx_ants + j*nof_symbols];
    }
  }
  
  for (int j=0;j<nof_rx_ants;j++) {
    y[j] = &input[j*nof_symbols];
  }
    
  if (nof_tx_ports > 1) {
    srslte_predecoding_diversity_multi(y, h, x, nof_rx_ants, nof_tx_ports, nof_symbols); 
    srslte_layerdemap_diversity(x, output, nof_tx_ports, nof_symbols / nof_tx_ports);
  } else {
    srslte_predecoding_single_multi(y, h[0], output, nof_rx_ants, nof_symbols, noise_estimate);
  }
  

  if (nlhs >= 1) { 
    mexutils_write_cf(output, &plhs[0], nof_symbols, 1);  
  }
  
  if (input) {
    free(input);
  }
  if (output) {
    free(output);
  }
  for (int i=0;i<SRSLTE_MAX_LAYERS;i++) {
    if (x[i]) {
      free(x[i]);      
    }
  }

  return;
}

