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
#include "srslte/srslte.h"
#include "srslte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define ENBCFG     prhs[0]
#define PDSCHCFG   prhs[1]
#define TBS        prhs[2]
#define INPUT      prhs[3]
#define NOF_INPUTS 4

void help()
{
  mexErrMsgTxt
    ("[decoded_ok, llr, rm, bits, symbols] = srslte_pdsch(enbConfig, pdschConfig, trblklen, rxWaveform)\n\n");
}

extern int indices[2048];

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  srslte_cell_t cell; 
  srslte_pdsch_t pdsch;
  srslte_chest_dl_t chest; 
  srslte_ofdm_t fft; 
  uint32_t cfi, sf_idx; 
  cf_t *input_fft, *input_signal;
  int nof_re; 
  srslte_ra_mcs_t mcs;
  srslte_ra_dl_alloc_t dl_alloc;
  srslte_harq_t harq_process;
  uint32_t rv;
  uint32_t rnti32;

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  if (mexutils_read_uint32_struct(PDSCHCFG, "RNTI", &rnti32)) {
    mexErrMsgTxt("Field RNTI not found in pdsch config\n");
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

  if (srslte_pdsch_init(&pdsch, cell)) {
    mexErrMsgTxt("Error initiating PDSCH\n");
    return;
  }
  srslte_pdsch_set_rnti(&pdsch, (uint16_t) (rnti32 & 0xffff));

  if (srslte_harq_init(&harq_process, cell)) {
    mexErrMsgTxt("Error initiating HARQ process\n");
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
  
    
  nof_re = 2 * SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  mcs.tbs = mxGetScalar(TBS);
  if (mcs.tbs == 0) {
    mexErrMsgTxt("Error trblklen is zero\n");
    return;
  }

  if (mexutils_read_uint32_struct(PDSCHCFG, "RV", &rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }
  
  char *mod_str = mexutils_get_char_struct(PDSCHCFG, "Modulation");
  
  if (!strcmp(mod_str, "QPSK")) {
    mcs.mod = SRSLTE_MOD_QPSK;
  } else if (!strcmp(mod_str, "16QAM")) {
    mcs.mod = SRSLTE_MOD_16QAM;
  } else if (!strcmp(mod_str, "64QAM")) {
    mcs.mod = SRSLTE_MOD_64QAM;
  } else {
   mexErrMsgTxt("Unknown modulation\n");
   return;
  }

  mxFree(mod_str);
  
  float *prbset; 
  mxArray *p; 
  p = mxGetField(PDSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found\n");
    return;
  } 
  
  // Only localized PRB supported 
  dl_alloc.slot[0].nof_prb = mexutils_read_f(p, &prbset);

  for (i=0;i<cell.nof_prb;i++) {
    dl_alloc.slot[0].prb_idx[i] = false; 
    for (int j=0;j<dl_alloc.slot[0].nof_prb && !dl_alloc.slot[0].prb_idx[i];j++) {
      if ((int) prbset[j] == i) {
        dl_alloc.slot[0].prb_idx[i] = true;
      }
    }
  }
  memcpy(&dl_alloc.slot[1], &dl_alloc.slot[0], sizeof(srslte_ra_prb_slot_t));

  free(prbset);
  
  srslte_ra_dl_alloc_re(&dl_alloc, cell.nof_prb, cell.nof_ports, cell.nof_prb<10?(cfi+1):cfi, cell.cp);
  
  if (srslte_harq_setup_dl(&harq_process, mcs, rv, sf_idx, &dl_alloc)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
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
    cf_t *cearray = NULL; 
    nof_re = mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
    cf_t *cearray_ptr = cearray; 
    for (i=0;i<cell.nof_ports;i++) {
      for (int j=0;j<nof_re/cell.nof_ports;j++) {
        ce[i][j] = *cearray;
        cearray++;
      }
    }    
    if (cearray_ptr)
      free(cearray_ptr);
  } else {
    srslte_chest_dl_estimate(&chest, input_fft, ce, sf_idx);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else {
    noise_power = srslte_chest_dl_get_noise_estimate(&chest);
  }
  
  uint8_t *data = malloc(sizeof(uint8_t) * mcs.tbs);
  if (!data) {
    return;
  }

  int r = srslte_pdsch_decode(&pdsch, &harq_process, input_fft, ce, noise_power, data);

  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(r == 0);
  }
  if (nlhs >= 2) {
    mexutils_write_uint8(data, &plhs[1], mcs.tbs, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pdsch.symbols[0], &plhs[2], harq_process.dl_alloc.re_sf[sf_idx], 1);  
  }
  if (nlhs >= 4) {
    mexutils_write_cf(pdsch.d, &plhs[3], harq_process.dl_alloc.re_sf[sf_idx], 1);  
  }
  if (nlhs >= 5) {
    mexutils_write_f(pdsch.e, &plhs[4], harq_process.dl_alloc.re_sf[sf_idx] * srslte_mod_bits_x_symbol(mcs.mod), 1);  
  }
  
  srslte_chest_dl_free(&chest);
  srslte_ofdm_rx_free(&fft);
  srslte_pdsch_free(&pdsch);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(data);
  free(input_signal);
  free(input_fft);
  
  return;
}

