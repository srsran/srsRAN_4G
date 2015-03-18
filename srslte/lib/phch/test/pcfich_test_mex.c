/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include "srslte/phy/phy.h"
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
  lte_cell_t cell; 
  pcfich_t pcfich;
  chest_dl_t chest; 
  lte_fft_t fft; 
  regs_t regs;
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

  if (chest_dl_init(&chest, cell)) {
    mexErrMsgTxt("Error initializing equalizer\n");
    return;
  }

  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    mexErrMsgTxt("Error initializing FFT\n");
    return;
  }
  
  if (regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (pcfich_init(&pcfich, &regs, cell)) {
    mexErrMsgTxt("Error creating PBCH object\n");
    return;
  }
      
  /** Allocate input buffers */
  if (mexutils_read_cf(INPUT, &input_signal) < 0) {
    mexErrMsgTxt("Error reading input signal\n");
    return; 
  }
  input_fft = vec_malloc(SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  
  // Set Channel estimates to 1.0 (ignore fading) 
  cf_t *ce[MAX_PORTS];
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = vec_malloc(SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  }
  
  lte_fft_run_sf(&fft, input_signal, input_fft);

  if (nrhs > NOF_INPUTS) {
    cf_t *cearray; 
    mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
        ce[i][j] = *cearray;
        cearray++;
      }
    }
  } else {
    chest_dl_estimate(&chest, input_fft, ce, sf_idx);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else {
    noise_power = chest_dl_get_noise_estimate(&chest);
  }
    
    
  uint32_t cfi;
  float corr_res; 
  int n = pcfich_decode(&pcfich, input_fft, ce, noise_power,  sf_idx, &cfi, &corr_res);

  if (nlhs >= 1) { 
    if (n < 0) {      
      plhs[0] = mxCreateDoubleScalar(-1);
    } else {
      plhs[0] = mxCreateDoubleScalar(cfi);      
    }
  }
  if (nlhs >= 2) {
    mexutils_write_cf(pcfich.pcfich_d, &plhs[1], 16, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pcfich.pcfich_symbols[0], &plhs[2], 16, 1);  
  }
  
  chest_dl_free(&chest);
  lte_fft_free(&fft);
  pcfich_free(&pcfich);
  regs_free(&regs);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_signal);
  free(input_fft);
  
  return;
}

