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

#define INPUT     prhs[0]
#define TRBLKLEN  prhs[1]
#define RV        prhs[2]
#define NOF_INPUTS 3


void help()
{
  mexErrMsgTxt
    ("[out] = srslte_rm_turbo_rx(in, trblkin, rv)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  float *input;
  float *output; 
  uint32_t in_len, trblklen, cblen, rvidx; 
  float *w_buff_f; 
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
  
  // Read input symbols
  in_len = mexutils_read_f(INPUT, &input);
  if (in_len < 0) {
    mexErrMsgTxt("Error reading input bits\n");
    return; 
  }
  
  trblklen = (uint32_t) mxGetScalar(TRBLKLEN);
  rvidx = (uint32_t) mxGetScalar(RV);
  
  srslte_cbsegm_t cbsegm; 
  srslte_cbsegm(&cbsegm, trblklen);
  cblen = 3*cbsegm.K1+12;

  w_buff_f = calloc(1,sizeof(float) * cblen * 10);
  if (!w_buff_f) {
    perror("malloc");
    exit(-1);
  }

  // allocate memory for output bits
  output = srslte_vec_malloc(cblen * sizeof(float));

  srslte_rm_turbo_rx(w_buff_f, cblen * 10, input, in_len, output, cblen,
      rvidx,cbsegm.F);

  if (nlhs >= 1) { 
    mexutils_write_f(output, &plhs[0], cblen, 1);  
  }
  if (nlhs >= 2) { 
    mexutils_write_f(input, &plhs[1], in_len, 1);  
  }

  free(input);
  free(output);
  free(w_buff_f);

  return;
}

