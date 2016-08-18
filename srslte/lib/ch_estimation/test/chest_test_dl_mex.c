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

#define ENBCFG prhs[0]
#define INPUT  prhs[1]
#define NOF_INPUTS 2

void help()
{
  mexErrMsgTxt
    ("[estChannel, noiseEst, eq_output] = srslte_chest_dl(enb, inputSignal, [w_coeff])\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  int i;
  srslte_cell_t cell; 
  srslte_chest_dl_t chest;
  
  cf_t *input_signal = NULL, *output_signal = NULL, *tmp_x[SRSLTE_MAX_LAYERS]; 
  cf_t *ce[SRSLTE_MAX_PORTS]; 
  
  for (int i=0;i<SRSLTE_MAX_LAYERS;i++) {
    tmp_x[i] = NULL; 
  }
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = NULL; 
  }
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }

  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }

  uint32_t sf_idx=0; 
  if (mexutils_read_uint32_struct(ENBCFG, "NSubframe", &sf_idx)) {
    help();
    return;
  }

  if (srslte_chest_dl_init(&chest, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
    return;
  }
    
  /** Allocate input buffers */
  int nof_re = 2*SRSLTE_CP_NSYMB(cell.cp)*cell.nof_prb*SRSLTE_NRE;
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce[i] = srslte_vec_malloc(nof_re * sizeof(cf_t));
  }
  for (i=0;i<SRSLTE_MAX_LAYERS;i++) {
    tmp_x[i] = srslte_vec_malloc(nof_re * sizeof(cf_t));
  }
  output_signal = srslte_vec_malloc(nof_re * sizeof(cf_t));
  
  // Read input signal 
  int insignal_len = mexutils_read_cf(INPUT, &input_signal);
  if (insignal_len < 0) {
    mexErrMsgTxt("Error reading input signal\n");
    return; 
  }
  
  // Read optional value smooth filter coefficient
  if (nrhs > NOF_INPUTS) {
    float w = (float) mxGetScalar(prhs[NOF_INPUTS]);
    srslte_chest_dl_set_smooth_filter3_coeff(&chest, w);
  } else {
    srslte_chest_dl_set_smooth_filter(&chest, NULL, 0);
  }
      
  // Perform channel estimation 
  if (srslte_chest_dl_estimate(&chest, input_signal, ce, sf_idx)) {
    mexErrMsgTxt("Error running channel estimator\n");
    return;
  }    
  
  // Get noise power estimation 
  float noise_power = srslte_chest_dl_get_noise_estimate(&chest);
          
  // Perform channel equalization 
  if (cell.nof_ports == 1) {
    srslte_predecoding_single(input_signal, ce[0], output_signal, nof_re, noise_power);            
  } else {
    srslte_predecoding_diversity(input_signal, ce, tmp_x, cell.nof_ports, nof_re);
    srslte_layerdemap_diversity(tmp_x, output_signal, cell.nof_ports, nof_re/cell.nof_ports);
  }
    
  /* Write output values */
  if (nlhs >= 1) {
    mexutils_write_cf(ce[0], &plhs[0], mxGetM(INPUT), mxGetN(INPUT));  
  }  
  if (nlhs >= 2) {
    plhs[1] = mxCreateLogicalScalar(noise_power);
  }
  if (nlhs >= 3) {
    mexutils_write_cf(output_signal, &plhs[2], mxGetM(INPUT), mxGetN(INPUT));
  }
  
  // Free all memory 
  srslte_chest_dl_free(&chest);
  
  for (i=0;i<SRSLTE_MAX_LAYERS;i++) {
    if (tmp_x[i]) {
      free(tmp_x[i]);
    }
  }
  for (i=0;i<SRSLTE_MAX_PORTS;i++) {
    if (ce[i]) {
      free(ce[i]);
    }
  }
  if (input_signal) {
    free(input_signal);
  }
  if (output_signal) {
    free(output_signal);
  }

  return;
}

