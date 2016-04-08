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
  nof_symbols = mexutils_read_cf(INPUT, &input);
  if (nof_symbols < 0) {
    mexErrMsgTxt("Error reading input\n");
    return; 
  }
  // Read channel estimates
  uint32_t nof_symbols2 = mexutils_read_cf(HEST, &hest);
  if (nof_symbols < 0) {
    mexErrMsgTxt("Error reading hest\n");
    return; 
  }
  if ((nof_symbols2 % nof_symbols) != 0) {
    mexErrMsgTxt("Hest size must be multiple of input size\n");
    return; 
  }
  // Calculate number of ports
  uint32_t nof_ports = nof_symbols2/nof_symbols; 
  
  cf_t *x[8]; 
  cf_t *h[4];
  
  /* Allocate memory */
  output = srslte_vec_malloc(sizeof(cf_t)*nof_symbols);
  int i;
  for (i = 0; i < nof_ports; i++) {
    x[i] = srslte_vec_malloc(sizeof(cf_t)*nof_symbols);
    h[i] = &hest[i*nof_symbols];
  }
  for (;i<8;i++) {
    x[i] = NULL; 
  }
  for (i=nof_ports;i<4;i++) {
    h[i] = NULL; 
  }
  
  srslte_predecoding_diversity(input, h, x, nof_ports, nof_symbols); 
  srslte_layerdemap_diversity(x, output, nof_ports, nof_symbols / nof_ports);


  if (nlhs >= 1) { 
    mexutils_write_cf(output, &plhs[0], nof_symbols, 1);  
  }
  
  if (input) {
    free(input);
  }
  if (output) {
    free(output);
  }
  for (i=0;i<8;i++) {
    if (x[i]) {
      free(x[i]);      
    }
  }

  return;
}

