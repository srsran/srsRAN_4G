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
    ("[decoded_ok, symbols, bits] = srslte_pbch(enbConfig, rxWaveform)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  int i;
  lte_cell_t cell; 
  pbch_t pbch;
  chest_dl_t chest; 
  lte_fft_t fft; 
  cf_t *input_symbols, *input_fft;
  int nof_re; 
  cf_t *ce[MAX_PORTS], *ce_slot[MAX_PORTS];

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }

  // Read input symbols
  mexutils_read_cf(INPUT, &input_symbols);
    
  nof_re = SF_LEN_RE(cell.nof_prb, cell.cp);

  // Allocate memory
  input_fft = vec_malloc(nof_re * sizeof(cf_t));
  for (i=0;i<MAX_PORTS;i++) {
    ce[i] = vec_malloc(nof_re * sizeof(cf_t));       
  }  
  
  if (chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return;
  }

  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return;
  }

  if (pbch_init(&pbch, cell)) {
    fprintf(stderr, "Error initiating PBCH\n");
    return;
  }
  
  lte_fft_run_sf(&fft, input_symbols, input_fft);
  
  if (nrhs > NOF_INPUTS) {
    cf_t *cearray; 
    mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<nof_re;j++) {
        ce[i][j] = *cearray;
        cearray++;
      }
    }
  } else {
    chest_dl_estimate(&chest, input_fft, ce, 0);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else {
    noise_power = chest_dl_get_noise_estimate(&chest);
  }
  
  for (int i=0;i<MAX_PORTS;i++) {
    ce_slot[i] = &ce[i][SLOT_LEN_RE(cell.nof_prb, cell.cp)];
  }

  uint32_t nof_ports; 
  int n = pbch_decode(&pbch, &input_fft[SLOT_LEN_RE(cell.nof_prb, cell.cp)], 
                  ce_slot, noise_power, 
                  NULL, &nof_ports, NULL);
  
  if (nlhs >= 1) { 
    if (n == 1) {
      plhs[0] = mxCreateDoubleScalar(nof_ports);      
    } else {
      plhs[0] = mxCreateDoubleScalar(0);
    }
  }
  if (nlhs >= 2) {
    mexutils_write_cf(pbch.pbch_d, &plhs[1], pbch.nof_symbols, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_f(pbch.pbch_llr, &plhs[2], 2*pbch.nof_symbols, 1);  
  }
  if (nlhs >= 4) {
    mexutils_write_cf(ce[0], &plhs[3], SF_LEN_RE(cell.nof_prb,cell.cp)/14, 14);  
  }
  if (nlhs >= 5) {
    mexutils_write_cf(ce[1], &plhs[4], SF_LEN_RE(cell.nof_prb,cell.cp)/14, 14);  
  }
  if (nlhs >= 6) {
    mexutils_write_cf(pbch.pbch_symbols[0], &plhs[5], pbch.nof_symbols, 1);  
  }
  if (nlhs >= 7) {
    mexutils_write_cf(pbch.ce[0], &plhs[6], pbch.nof_symbols, 1);  
  }
  
  chest_dl_free(&chest);
  lte_fft_free(&fft);
  pbch_free(&pbch);

  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_symbols);
  free(input_fft);

  return;
}

