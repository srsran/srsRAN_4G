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

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define ENBCFG  prhs[0]
#define INPUT   prhs[1]
#define NOF_INPUTS 2


void help()
{
  mexErrMsgTxt
    ("[offset,corr] = srslte_cp_synch(enbConfig, inputSignal)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_cell_t cell; 
  srslte_cp_synch_t cp_synch; 
  cf_t *input_symbols;
  int frame_len; 
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  /** Allocate input buffers */
  frame_len = mexutils_read_cf(INPUT, &input_symbols);
  if (frame_len < 0) {
    mexErrMsgTxt("Error reading input symbols\n");
    return;
  }
  
  uint32_t symbol_sz = srslte_symbol_sz(cell.nof_prb);
  if (srslte_cp_synch_init(&cp_synch, symbol_sz)) {
    fprintf(stderr, "Error initiating CP\n");
    return;
  }
  
  uint32_t cp_len = SRSLTE_CP_LEN_NORM(1, symbol_sz);
  uint32_t nsymbols = frame_len/(symbol_sz+cp_len)-1;
  uint32_t peak_idx = srslte_cp_synch(&cp_synch, input_symbols, symbol_sz, nsymbols, cp_len);
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateDoubleScalar(peak_idx);
  }
  if (nlhs >= 2) {
    mexutils_write_cf(cp_synch.corr, &plhs[1], symbol_sz, 1);  
  }
    
  srslte_cp_synch_free(&cp_synch);
  free(input_symbols);

  return;
}

