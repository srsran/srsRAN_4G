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
#define RNTI    prhs[1]
#define INPUT   prhs[2]
#define NOF_INPUTS 3

#define MAX_CANDIDATES 64


srslte_dci_format_t ue_formats[] = {SRSLTE_DCI_FORMAT1A,SRSLTE_DCI_FORMAT1}; // SRSLTE_DCI_FORMAT1B should go here also
const uint32_t nof_ue_formats = 2; 

srslte_dci_format_t common_formats[] = {SRSLTE_DCI_FORMAT1A,SRSLTE_DCI_FORMAT1C};
const uint32_t nof_common_formats = 2; 


void help()
{
  mexErrMsgTxt
    ("[decoded_ok, llr, rm, bits, symbols] = srslte_pdcch(enbConfig, RNTI, rxWaveform)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  srslte_cell_t cell; 
  srslte_pdcch_t pdcch;
  srslte_chest_dl_t chest; 
  srslte_ofdm_t fft; 
  srslte_regs_t regs;
  srslte_dci_location_t locations[MAX_CANDIDATES];
  uint32_t cfi, sf_idx; 
  uint16_t rnti; 
  cf_t *input_fft, *input_signal;
  int nof_re; 
  uint32_t nof_formats; 
  srslte_dci_format_t *formats = NULL; 
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  if (mexutils_read_uint32_struct(ENBCFG, "CFI", &cfi)) {
    help();
    return;
  }
  if (mexutils_read_uint32_struct(ENBCFG, "NSubframe", &sf_idx)) {
    help();
    return;
  }

  if (srslte_chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return;
  }

  if (srslte_ofdm_rx_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return;
  }
  
  rnti = (uint16_t) mxGetScalar(RNTI);
    
  if (srslte_regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (srslte_regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    exit(-1);
  }
  
  if (srslte_pdcch_init(&pdcch, &regs, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
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
    nof_re = mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<nof_re;j++) {
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
    
  srslte_pdcch_extract_llr(&pdcch, input_fft, ce, noise_power, sf_idx, cfi);
  
  uint32_t nof_locations;
  if (rnti == SRSLTE_SIRNTI) {
    nof_locations = srslte_pdcch_common_locations(&pdcch, locations, MAX_CANDIDATES, cfi);
    formats = common_formats;
    nof_formats = nof_common_formats;
  } else {
    nof_locations = srslte_pdcch_ue_locations(&pdcch, locations, MAX_CANDIDATES, sf_idx, cfi, rnti); 
    formats = ue_formats; 
    nof_formats = nof_ue_formats;
  }
  uint16_t crc_rem=0;   
  srslte_dci_msg_t dci_msg; 
  bzero(&dci_msg, sizeof(srslte_dci_msg_t));
  
  for (int f=0;f<nof_formats;f++) {
    for (i=0;i<nof_locations && crc_rem != rnti;i++) {
      if (srslte_pdcch_decode_msg(&pdcch, &dci_msg, &locations[i], formats[f], &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return;
      }
    }
  }    
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(crc_rem == rnti);
  }
  int nof_bits = (srslte_regs_pdcch_nregs(&regs, cfi) / 9) * 72;
  if (nlhs >= 2) {
    mexutils_write_f(pdcch.llr, &plhs[1], nof_bits, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pdcch.symbols[0], &plhs[2], 36*pdcch.nof_cce, 1);  
  }
  
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);
  srslte_pdcch_free(&pdcch);
  srslte_regs_free(&regs);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_signal);
  free(input_fft);
  
  return;
}

