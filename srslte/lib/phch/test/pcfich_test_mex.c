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
    ("[cfi] = srslte_pdcch(enbConfig, rxWaveform)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  srslte_cell_t cell; 
  srslte_pcfich_t pcfich;
  srslte_chest_dl_t chest; 
  srslte_ofdm_t fft; 
  srslte_regs_t regs;
  uint32_t sf_idx; 
  cf_t *input_fft, *input_signal;
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  if (mexutils_read_uint32_struct(ENBCFG, "NSubframe", &sf_idx)) {
    help();
    return;
  }

  if (srslte_chest_dl_init(&chest, cell)) {
    mexErrMsgTxt("Error initializing equalizer\n");
    return;
  }

  if (srslte_ofdm_rx_init(&fft, cell.cp, cell.nof_prb)) {
    mexErrMsgTxt("Error initializing FFT\n");
    return;
  }
  
  if (srslte_regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (srslte_pcfich_init(&pcfich, &regs, cell)) {
    mexErrMsgTxt("Error creating PBCH object\n");
    return;
  }
      
  /** Allocate input buffers */
  if (mexutils_read_cf(INPUT, &input_signal) < 0) {
    mexErrMsgTxt("Error reading input signal\n");
    return; 
  }
  input_fft = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  
  // Set Channel estimates to 1.0 (ignore fading) 
  cf_t *ce[SRSLTE_MAX_PORTS];
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  }
  
  srslte_ofdm_rx_sf(&fft, input_signal, input_fft);

  if (nrhs > NOF_INPUTS) {
    cf_t *cearray; 
    mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
        ce[i][j] = *cearray;
        cearray++;
      }
    }
  } else {
    srslte_chest_dl_estimate(&chest, input_fft, ce, sf_idx);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else {
    noise_power = srslte_chest_dl_get_noise_estimate(&chest);
  }
    
    
  uint32_t cfi;
  float corr_res; 
  int n = srslte_pcfich_decode(&pcfich, input_fft, ce, noise_power,  sf_idx, &cfi, &corr_res);

  if (nlhs >= 1) { 
    if (n < 0) {      
      plhs[0] = mxCreateDoubleScalar(-1);
    } else {
      plhs[0] = mxCreateDoubleScalar(cfi);      
    }
  }
  if (nlhs >= 2) {
    mexutils_write_cf(pcfich.d, &plhs[1], 16, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pcfich.symbols[0], &plhs[2], 16, 1);  
  }
  
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);
  srslte_pcfich_free(&pcfich);
  srslte_regs_free(&regs);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_signal);
  free(input_fft);
  
  return;
}

