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
#include "liblte/phy/phy.h"
#include "liblte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define ENBCFG  prhs[0]
#define RNTI    prhs[1]
#define INPUT   prhs[2]
#define NOF_INPUTS 3

#define MAX_CANDIDATES 64


dci_format_t ue_formats[] = {Format1A,Format1}; // Format1B should go here also
const uint32_t nof_ue_formats = 2; 

dci_format_t common_formats[] = {Format1A,Format1C};
const uint32_t nof_common_formats = 2; 


void help()
{
  mexErrMsgTxt
    ("[decoded_ok, llr, rm, bits, symbols] = liblte_pdcch(enbConfig, RNTI, rxWaveform)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  lte_cell_t cell; 
  pdcch_t pdcch;
  chest_dl_t chest; 
  lte_fft_t fft; 
  regs_t regs;
  dci_location_t locations[MAX_CANDIDATES];
  uint32_t cfi, sf_idx; 
  uint16_t rnti; 
  cf_t *input_fft, *input_signal;
  int nof_re; 
  uint32_t nof_formats; 
  dci_format_t *formats = NULL; 
  
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

  if (chest_dl_init(&chest, cell)) {
    fprintf(stderr, "Error initializing equalizer\n");
    return;
  }

  if (lte_fft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return;
  }
  
  rnti = (uint16_t) mxGetScalar(RNTI);
    
  if (regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    exit(-1);
  }
  
  if (pdcch_init(&pdcch, &regs, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
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
    nof_re = mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<nof_re;j++) {
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
    
  pdcch_extract_llr(&pdcch, input_fft, ce, noise_power, sf_idx, cfi);
  
  uint32_t nof_locations;
  if (rnti == SIRNTI) {
    nof_locations = pdcch_common_locations(&pdcch, locations, MAX_CANDIDATES, cfi);
    formats = common_formats;
    nof_formats = nof_common_formats;
  } else {
    nof_locations = pdcch_ue_locations(&pdcch, locations, MAX_CANDIDATES, sf_idx, cfi, rnti); 
    formats = ue_formats; 
    nof_formats = nof_ue_formats;
  }
  uint16_t crc_rem=0;   
  dci_msg_t dci_msg; 
  bzero(&dci_msg, sizeof(dci_msg_t));
  
  for (int f=0;f<nof_formats;f++) {
    for (i=0;i<nof_locations && crc_rem != rnti;i++) {
      if (pdcch_decode_msg(&pdcch, &dci_msg, &locations[i], formats[f], &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return;
      }
    }
  }    
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(crc_rem == rnti);
  }
  int nof_bits = (regs_pdcch_nregs(&regs, cfi) / 9) * 72;
  if (nlhs >= 2) {
    mexutils_write_f(pdcch.pdcch_llr, &plhs[1], nof_bits, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pdcch.pdcch_symbols[0], &plhs[2], 36*pdcch.nof_cce, 1);  
  }
  
  chest_dl_free(&chest);
  lte_fft_free(&fft);
  pdcch_free(&pdcch);
  regs_free(&regs);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_signal);
  free(input_fft);
  
  return;
}

