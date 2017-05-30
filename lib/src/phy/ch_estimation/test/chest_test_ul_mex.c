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

#define UECFG    prhs[0]
#define PUSCHCFG prhs[1]
#define INPUT    prhs[2]
#define NOF_INPUTS 3

void help()
{
  mexErrMsgTxt
    ("[estChannel, noiseEst, eq_output] = srslte_chest_ul(ue_cfg, pusch_cfg, inputSignal, [w_coeff])\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  srslte_cell_t cell; 
  srslte_chest_ul_t chest;
  
  cf_t *input_signal = NULL, *output_signal = NULL; 
  cf_t *ce = NULL; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }

  if (mexutils_read_uint32_struct(UECFG, "NCellID", &cell.id)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  if (mexutils_read_uint32_struct(UECFG, "NULRB", &cell.nof_prb)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  cell.cp = SRSLTE_CP_NORM;
  cell.nof_ports = 1; 

  uint32_t sf_idx=0; 
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    help();
    return;
  }

  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg;
  pusch_cfg.group_hopping_en = false;
  pusch_cfg.sequence_hopping_en = false;
  char *tmp = mexutils_get_char_struct(UECFG, "Hopping");
  if (tmp) {
    if (!strcmp(tmp, "Group")) {
      pusch_cfg.group_hopping_en = true;
    } else if (!strcmp(tmp, "Sequence")) {
      pusch_cfg.sequence_hopping_en = true;
    }
    mxFree(tmp);    
  }
  
  
  if (mexutils_read_uint32_struct(UECFG, "SeqGroup", &pusch_cfg.delta_ss)) {
    pusch_cfg.delta_ss = 0; 
  }
  if (mexutils_read_uint32_struct(UECFG, "CyclicShift", &pusch_cfg.cyclic_shift)) {
    pusch_cfg.cyclic_shift = 0; 
  }
  float *prbset; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found in PUSCH config\n");
    return;
  } 
  uint32_t nof_prb = mexutils_read_f(p, &prbset); 
  uint32_t n_prb[2]; 
  n_prb[0] = prbset[0];
  n_prb[1] = prbset[0];
  
  
  uint32_t cyclic_shift_for_dmrs = 0; 
  if (mexutils_read_uint32_struct(PUSCHCFG, "DynCyclicShift", &cyclic_shift_for_dmrs)) {
    cyclic_shift_for_dmrs = 0; 
  } 
  
  if (srslte_chest_ul_init(&chest, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
    return;
  }
  
  srslte_chest_ul_set_cfg(&chest, &pusch_cfg, NULL, NULL);
    
  /** Allocate input buffers */
  int nof_re = 2*SRSLTE_CP_NSYMB(cell.cp)*cell.nof_prb*SRSLTE_NRE;
  ce = srslte_vec_malloc(nof_re * sizeof(cf_t));  
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
    srslte_chest_ul_set_smooth_filter3_coeff(&chest, w);
  } else {
    srslte_chest_ul_set_smooth_filter(&chest, NULL, 0);
  }
      
  // Perform channel estimation 
  if (srslte_chest_ul_estimate(&chest, input_signal, ce, nof_prb, sf_idx, cyclic_shift_for_dmrs, n_prb)) {
    mexErrMsgTxt("Error running channel estimator\n");
    return;
  }    
  
  // Get noise power estimation 
  float noise_power = srslte_chest_ul_get_noise_estimate(&chest);
          
  // Perform channel equalization 
  srslte_predecoding_single(input_signal, ce, output_signal, nof_re, noise_power);            
    
  /* Write output values */
  if (nlhs >= 1) {
    mexutils_write_cf(ce, &plhs[0], mxGetM(INPUT), mxGetN(INPUT));  
  }  
  if (nlhs >= 2) {
    plhs[1] = mxCreateDoubleScalar(noise_power);
  }
  if (nlhs >= 3) {
    mexutils_write_cf(output_signal, &plhs[2], mxGetM(INPUT), mxGetN(INPUT));
  }
  
  // Free all memory 
  srslte_chest_ul_free(&chest);
  
  if (ce) {
    free(ce);    
  }
  if (input_signal) {
    free(input_signal);
  }
  if (output_signal) {
    free(output_signal);
  }

  return;
}

