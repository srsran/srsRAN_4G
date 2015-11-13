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
#define ALGO    prhs[2]
#define NOF_INPUTS 2


void help()
{
  mexErrMsgTxt
    ("[N_id_1,sf_idx,corr_output_m0,corr_output_m1] = srslte_sss(enbConfig, inputSignal, [Algorithm])\n"
      "\tinputSignal must be aligned to the subframe. CP length is assumed Normal.\n"
      "\tAlgorithm is an optional parameter: Can be 'partial','diff','full'\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_cell_t cell; 
  srslte_sss_synch_t sss; 
  cf_t *input_symbols;
  int frame_len; 
  uint32_t m0, m1;
  float m0_value, m1_value; 
  char alg[64];

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  if (nrhs > NOF_INPUTS) {
    mxGetString(ALGO, alg, (mwSize)sizeof(alg));       
  } else {
    strcpy(alg, "full");
  }
  
  /** Allocate input buffers */
  frame_len = mexutils_read_cf(INPUT, &input_symbols);
  if (frame_len < 0) {
    mexErrMsgTxt("Error reading input symbols\n");
    return;
  }
  
  if (srslte_sss_synch_init(&sss, srslte_symbol_sz(cell.nof_prb))) {
    mexErrMsgTxt("Error initializing SSS object\n");
    return;
  }

  srslte_sss_synch_set_N_id_2(&sss, cell.id%3);
      
  // Find SSS 
  uint32_t sss_idx = SRSLTE_SLOT_IDX_CPNORM(5,srslte_symbol_sz(cell.nof_prb));     
  if (sss_idx > frame_len) {
    mexErrMsgTxt("Error too few samples provided.\n");
    return;
  }
  //mexPrintf("SSS begins at %d/%d. Running algorithm %s\n", sss_idx, frame_len, alg);
  if (!strcmp(alg, "partial")) {
    srslte_sss_synch_m0m1_partial(&sss, &input_symbols[sss_idx], 3, NULL, &m0, &m0_value, &m1, &m1_value);      
  } else if (!strcmp(alg, "diff")) {
    srslte_sss_synch_m0m1_diff(&sss, &input_symbols[sss_idx], &m0, &m0_value, &m1, &m1_value);      
  } else if (!strcmp(alg, "full")) {
    srslte_sss_synch_m0m1_partial(&sss, &input_symbols[sss_idx], 1, NULL, &m0, &m0_value, &m1, &m1_value);      
  } else {
    mexErrMsgTxt("Unsupported algorithm type\n");
    return;
  }
  
  //mexPrintf("m0: %d, m1: %d, N_id_1: %d\n", m0, m1, srslte_sss_synch_N_id_1(&sss, m0, m1));
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateDoubleScalar(srslte_sss_synch_N_id_1(&sss, m0, m1));
  }
  if (nlhs >= 2) {
    plhs[1] = mxCreateDoubleScalar(srslte_sss_synch_subframe(m0, m1));
  }
  if (nlhs >= 3) {
    mexutils_write_f(sss.corr_output_m0, &plhs[2], SRSLTE_SSS_N, 1);  
  }
  if (nlhs >= 4) {
    mexutils_write_f(sss.corr_output_m1, &plhs[3], SRSLTE_SSS_N, 1);  
  }
  srslte_sss_synch_free(&sss);
  free(input_symbols);

  return;
}

