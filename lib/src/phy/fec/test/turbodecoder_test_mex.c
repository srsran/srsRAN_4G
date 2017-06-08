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
#define NITERS  prhs[1]
#define NOF_INPUTS 1


void help()
{
  mexErrMsgTxt
    ("[decoded_bits] = srslte_turbodecoder(input_llr, nof_iterations)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_tdec_gen_t tdec;
  float *input_llr;
  uint8_t *output_data; 
  uint32_t nof_bits; 
  uint32_t nof_iterations; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  
  // Read input symbols
  uint32_t nof_symbols = mexutils_read_f(INPUT, &input_llr);
  if (nof_symbols < 40) {
    mexErrMsgTxt("Minimum block size is 40\n");
    return; 
  }
  nof_bits = (nof_symbols-12)/3;
  
  if (!srslte_cbsegm_cbsize_isvalid(nof_bits)) {
    mexErrMsgTxt("Invalid codeblock size\n");
    return; 
  }


  // read number of iterations 
  if (nrhs > NOF_INPUTS) {
    nof_iterations = (uint32_t) mxGetScalar(prhs[1]);
    if (nof_iterations > 50) {
      mexErrMsgTxt("Maximum number of iterations is 50\n");
      return; 
    }
  } else {
    nof_iterations = 5; // set the default nof iterations to 5 as in matlab 
  }
  
  // allocate memory for output bits
  output_data = srslte_vec_malloc(nof_bits * sizeof(uint8_t));

  if (srslte_tdec_gen_init(&tdec, nof_bits)) {
    mexErrMsgTxt("Error initiating Turbo decoder\n");
    return;
  }

  srslte_tdec_gen_run_all(&tdec, input_llr, output_data, nof_iterations, nof_bits);

  if (nlhs >= 1) { 
    mexutils_write_uint8(output_data, &plhs[0], nof_bits, 1);  
  }

  srslte_tdec_gen_free(&tdec);

  free(input_llr);
  free(output_data);

  return;
}

