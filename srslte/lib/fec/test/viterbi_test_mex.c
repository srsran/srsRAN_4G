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

#define INPUT   prhs[0]
#define NOF_INPUTS 1


void help()
{
  mexErrMsgTxt
    ("[decoded_bits] = srslte_viterbi(input_llr, type)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_viterbi_t viterbi;
  float *input_llr;
  uint8_t *output_data; 
  int nof_bits; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
    
  // Read input symbols
  nof_bits = mexutils_read_f(INPUT, &input_llr);
  
  output_data = srslte_vec_malloc(nof_bits * sizeof(uint8_t));

  int poly[3] = { 0x6D, 0x4F, 0x57 };
  if (srslte_viterbi_init(&viterbi, SRSLTE_VITERBI_37, poly, nof_bits/3, true)) {
    return;
  }
  
  if (nrhs >= 2) {
    float gain_quant = mxGetScalar(prhs[1]);
    srslte_viterbi_set_gain_quant(&viterbi, gain_quant);  
  }

  srslte_viterbi_decode_f(&viterbi, input_llr, output_data, nof_bits/3);
  
  if (nlhs >= 1) { 
    mexutils_write_uint8(output_data, &plhs[0], nof_bits/3, 1);  
  }
  if (nlhs >= 2) {
    mexutils_write_uint8(viterbi.symbols_uc, &plhs[1], nof_bits/3, 1);  
  }

  srslte_viterbi_free(&viterbi);

  free(input_llr);
  free(output_data);

  return;
}

