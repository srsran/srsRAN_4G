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
#define HIRES   prhs[1]
#define INPUT   prhs[2]
#define NOF_INPUTS 3

void help()
{
  mexErrMsgTxt
    ("[hi, symbols] = srslte_phich(enbConfig, hires, input_signal, [hest, nest])\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  srslte_cell_t cell; 
  srslte_phich_t phich;
  srslte_chest_dl_t chest; 
  srslte_ofdm_t ofdm_rx; 
  srslte_regs_t regs;
  uint32_t sf_idx; 
  cf_t *input_fft, *input_signal;
  
  if (nrhs < NOF_INPUTS) {
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

  if (srslte_ofdm_rx_init(&ofdm_rx, cell.cp, cell.nof_prb)) {
    mexErrMsgTxt("Error initializing FFT\n");
    return;
  }
  
  if (srslte_regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (srslte_phich_init(&phich, &regs, cell)) {
    mexErrMsgTxt("Error creating PHICH object\n");
    return;
  }
      
// Read input signal 
  input_signal = NULL; 
  int insignal_len = mexutils_read_cf(INPUT, &input_signal);
  if (insignal_len < 0) {
    mexErrMsgTxt("Error reading input signal\n");
    return; 
  }
  if (insignal_len == SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)) {
    input_fft = input_signal; 
  } else {
    input_fft = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));  
    srslte_ofdm_rx_sf(&ofdm_rx, input_signal, input_fft);
    free(input_signal);
  }  
  
  cf_t *ce[SRSLTE_MAX_PORTS];
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  }
  
  if (nrhs > NOF_INPUTS) {
    cf_t *cearray = NULL; 
    mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    cf_t *cearray_ptr = cearray; 
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
        ce[i][j] = *cearray_ptr;
        cearray_ptr++;
      }
    }
    if (cearray) {
      free(cearray);
    }
  } else {
    srslte_chest_dl_estimate(&chest, input_fft, ce, sf_idx);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else if (nrhs > NOF_INPUTS) {
    noise_power = 0; 
  } else {
    noise_power = srslte_chest_dl_get_noise_estimate(&chest);
  } 
  
  // Read hires values 
  float *hires = NULL; 
  int nhires = mexutils_read_f(HIRES, &hires); 
  if (nhires != 2) {
    mexErrMsgTxt("Expecting 2 values for hires parameter\n");
    return; 
  }
  uint32_t ngroup = (uint32_t) hires[0]; 
  uint32_t nseq   = (uint32_t) hires[1]; 
  uint8_t ack;
  float corr_res; 
  int n = srslte_phich_decode(&phich, input_fft, ce, noise_power, ngroup, nseq, sf_idx, &ack, &corr_res);

  if (nlhs >= 1) { 
    if (n < 0) {      
      plhs[0] = mxCreateDoubleScalar(-1);
    } else {
      plhs[0] = mxCreateDoubleScalar(ack);      
    }
  }
  if (nlhs >= 2) {
    mexutils_write_cf(phich.z, &plhs[1], 1, SRSLTE_PHICH_NBITS);  
  }
  
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&ofdm_rx);
  srslte_phich_free(&phich);
  srslte_regs_free(&regs);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_fft);
  
  return;
}

