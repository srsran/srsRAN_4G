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
    ("[offset,corr] = srslte_pss(enbConfig, inputSignal)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_cell_t cell; 
  srslte_pss_synch_t pss; 
  cf_t *input_symbols;
  int frame_len; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
    
  srslte_use_standard_symbol_size(true);  
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  /* Allocate input buffers */
  frame_len = mexutils_read_cf(INPUT, &input_symbols);
  if (frame_len < 0) {
    mexErrMsgTxt("Error reading input symbols\n");
    return;
  }
  
  if (nrhs == NOF_INPUTS+1) {
    frame_len = (int) mxGetScalar(prhs[NOF_INPUTS]);
  }
  
  if (srslte_pss_synch_init_fft(&pss, frame_len, srslte_symbol_sz(cell.nof_prb))) {
    fprintf(stderr, "Error initiating PSS\n");
    exit(-1);
  }
  if (srslte_pss_synch_set_N_id_2(&pss, cell.id%3)) {
    fprintf(stderr, "Error setting N_id_2=%d\n",cell.id%3);
    exit(-1);
  }
  srslte_pss_synch_set_ema_alpha(&pss, 1.0);     
      
  int peak_idx = srslte_pss_synch_find_pss(&pss, input_symbols, NULL);

  if (nlhs >= 1) { 
    plhs[0] = mxCreateDoubleScalar(peak_idx);
  }
  if (nlhs >= 2) {
    mexutils_write_cf(pss.conv_output, &plhs[1], frame_len, 1);  
  }
    
  srslte_pss_synch_free(&pss);
  free(input_symbols);

  return;
}

