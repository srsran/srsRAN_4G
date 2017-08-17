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
#define HEST     prhs[1]
#define NEST     prhs[2]
#define NLAYERS  prhs[3]
#define NCW      prhs[4]
#define TXSCHEME prhs[5]
#define CODEBOOK prhs[6]
#define NOF_INPUTS 7


void help()
{
  mexErrMsgTxt
    ("[output] = srslte_predecoder(input, hest, nest, Nl, TxScheme)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  const mwSize *dims = mxGetDimensions(INPUT);
  mwSize ndims;
  cf_t *input = NULL;
  cf_t *hest = NULL; 
  cf_t *output = NULL; 
  uint32_t nof_symbols = 0; 
  uint32_t nof_rx_ants  = 1;
  uint32_t nof_layers;
  uint32_t nof_tx_ports = 1;
  uint32_t nof_codewords = 1;
  uint32_t codebook_idx = 0;
  float noise_estimate = 0;
  cf_t *x[SRSLTE_MAX_LAYERS];
  cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  cf_t *y[SRSLTE_MAX_PORTS];
  int symbols_layers[SRSLTE_MAX_LAYERS];
  int i, j;
  srslte_mimo_type_t type;
  
  /* Print help if number of inputs does not match with expected */
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  
  /* Read input symbols */
  if (mexutils_read_cf(INPUT, &input) < 0) {
    mexErrMsgTxt("Error reading input\n");
    return; 
  }

  /* Read number of layers */
  nof_layers = (uint32_t) mxGetScalar(NLAYERS);

  /* Read number of codewords */
  nof_codewords = (uint32_t) mxGetScalar(NCW);

  if (nof_layers > SRSLTE_MAX_LAYERS) {
    mexErrMsgTxt("Too many layers\n");
    return;
  }

  /* Read number of symbols and Rx antennas */
  ndims = mxGetNumberOfDimensions(INPUT);
  nof_symbols = (uint32_t) dims[0];

  if (ndims >= 2) {
    nof_rx_ants = (uint32_t) dims[1];
  }
  
  /* Read channel estimates */
  if (mexutils_read_cf(HEST, &hest) < 0) {
    mexErrMsgTxt("Error reading hest\n");
    return; 
  }

  /* Get number of tx ports */
  dims = mxGetDimensions(HEST);
  ndims = mxGetNumberOfDimensions(HEST);

  if (ndims == 3) {
    nof_tx_ports = (uint32_t) dims[2];
  }

  /* Print parameters trace */
  mexPrintf("nof_tx_ports=%d, nof_rx_ants=%d, nof_layers=%d, nof_codewords=%d, codebook_idx=%d, nof_symbols=%d\n",
            nof_tx_ports, nof_rx_ants, nof_layers, nof_codewords, codebook_idx, nof_symbols);

  /* Read noise estimate */
  if (nrhs >= NOF_INPUTS) {
    noise_estimate = (float) mxGetScalar(NEST);
  }
  
  /* Initialise x, h & y pointers */
  for (i=0;i<SRSLTE_MAX_LAYERS;i++) {
    x[i] = NULL; 
  }
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    for (j=0;j<SRSLTE_MAX_PORTS;j++) {
      h[i][j] = NULL; 
    }
  }
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    y[i] = NULL;
  }

  /* Allocate memory for ouput */
  output = srslte_vec_malloc(sizeof(cf_t)*nof_symbols*nof_layers);

  /* Allocate memory for intermediate data */
  for (i = 0; i < nof_tx_ports; i++) {
    x[i] = srslte_vec_malloc(sizeof(cf_t) * nof_symbols*nof_layers);
  }

  /* Allocate memory for channel estimate */
  for (i = 0; i < nof_tx_ports; i++) {
    for (j=0; j<nof_rx_ants; j++) {
      h[i][j] = &hest[(i*nof_rx_ants + j)*nof_symbols];
    }
  }

  /* Allocate memory for input */
  for (j = 0; j < nof_rx_ants; j++) {
    y[j] = &input[j*nof_symbols];
  }

  /* Parse Tx scheme */
  char txscheme[32] = "Port0";
  if (nrhs >= NOF_INPUTS) {
    mxGetString_700(TXSCHEME, txscheme, 32);
  }

  codebook_idx = (uint32_t) mxGetScalar(CODEBOOK);

  if (srslte_str2mimotype(txscheme, &type)) {
    mexPrintf("Unsupported TxScheme=%s\n", txscheme);
    return; 
  }

  /* Populate symbols in layers */
  for (i = 0; i < nof_layers; i++) {
    symbols_layers[i] = nof_symbols; 
  }

  /* Set output pointer */
  cf_t *d[SRSLTE_MAX_CODEWORDS];
  for (i = 0; i<nof_codewords; i++) {
    d[i] = &output[i*nof_symbols*nof_layers/nof_codewords];
  }

  /* Pre-decode */
  srslte_predecoding_type_multi(y, h, x, nof_rx_ants, nof_tx_ports, nof_layers, codebook_idx, nof_symbols, type,
                                noise_estimate);

  /* Layer de-mapper */
  srslte_layerdemap_type(x, d, nof_layers, nof_codewords, nof_symbols, symbols_layers, type);
  
  /* Write output */
  if (nlhs >= 1) {
    mexutils_write_cf(output, &plhs[0], nof_symbols, nof_codewords);
  }

  /* Free memory */
  if (input) {
    free(input);
  }
  if (hest) {
    free(hest);
  }
  if (output) {
    free(output);
  }
  for (i=0;i<SRSLTE_MAX_LAYERS;i++) {
    if (x[i]) {
      free(x[i]);      
    }
  }

  return;
}

