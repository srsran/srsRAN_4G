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
  cf_t *input_fft;
  int nof_re; 
  srslte_pdsch_cfg_t cfg;
  srslte_softbuffer_rx_t softbuffer; 
  uint32_t rnti32;
  uint32_t cfi; 

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }

  srslte_verbose = SRSLTE_VERBOSE_INFO;
  bzero(&cfg, sizeof(srslte_pdsch_cfg_t));

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
  if (mexutils_read_uint32_struct(ENBCFG, "NSubframe", &cfg.sf_idx)) {
    help();
    return;
  }

  if (srslte_pdsch_init(&pdsch, cell)) {
    mexErrMsgTxt("Error initiating PDSCH\n");
    return;
  }
  srslte_pdsch_set_rnti(&pdsch, (uint16_t) (rnti32 & 0xffff));

  if (srslte_softbuffer_rx_init(&softbuffer, cell.nof_prb)) {
    mexErrMsgTxt("Error initiating soft buffer\n");
    return;
  }
  
  if (srslte_chest_dl_init(&chest, cell)) {
    mexErrMsgTxt("Error initializing equalizer\n");
    return;
  }

  
  nof_re = 2 * SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;

  srslte_ra_dl_grant_t grant; 
  grant.mcs.tbs = mxGetScalar(TBS);
  if (grant.mcs.tbs == 0) {
    mexErrMsgTxt("Error trblklen is zero\n");
    return;
  }
  if (srslte_cbsegm(&cfg.cb_segm, grant.mcs.tbs)) {
    mexErrMsgTxt("Error computing CB segmentation\n");
    return; 
  }

  if (mexutils_read_uint32_struct(PDSCHCFG, "RV", &cfg.rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }
  
  char *mod_str = mexutils_get_char_struct(PDSCHCFG, "Modulation");
  
  if (!strcmp(mod_str, "QPSK")) {
    grant.mcs.mod = SRSLTE_MOD_QPSK;
  } else if (!strcmp(mod_str, "16QAM")) {
    grant.mcs.mod = SRSLTE_MOD_16QAM;
  } else if (!strcmp(mod_str, "64QAM")) {
    grant.mcs.mod = SRSLTE_MOD_64QAM;
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
  grant.nof_prb = mexutils_read_f(p, &prbset);

  for (i=0;i<cell.nof_prb;i++) {
    grant.prb_idx[0][i] = false; 
    for (int j=0;j<grant.nof_prb && !grant.prb_idx[0][i];j++) {
      if ((int) prbset[j] == i) {
        grant.prb_idx[0][i] = true;
      }
    }
    grant.prb_idx[1][i] = grant.prb_idx[0][i];
  }

  free(prbset);
  
  /* Configure rest of pdsch_cfg parameters */
  grant.Qm = srslte_mod_bits_x_symbol(grant.mcs.mod);
  if (srslte_pdsch_cfg(&cfg, cell, &grant, cfi, cfg.sf_idx, cfg.rv)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }
      
  /** Allocate input buffers */
  if (mexutils_read_cf(INPUT, &input_fft) < 0) {
    mexErrMsgTxt("Error reading input signal\n");
    return; 
  }
  
  cf_t *ce[SRSLTE_MAX_PORTS];
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  }
  
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
    srslte_chest_dl_estimate(&chest, input_fft, ce, cfg.sf_idx);    
  }
  float noise_power;
  if (nrhs > NOF_INPUTS + 1) {
    noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
  } else {
    noise_power = 0;
  }
  
  uint8_t *data_bytes = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs.tbs/8);
  if (!data_bytes) {
    return;
  }

  int r = srslte_pdsch_decode(&pdsch, &cfg, &softbuffer, input_fft, ce, noise_power, data_bytes);

  free(data_bytes);
  
  uint8_t *data = malloc(grant.mcs.tbs);
  srslte_bit_unpack_vector(data_bytes, data, grant.mcs.tbs);
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(r == 0);
  }
  if (nlhs >= 2) {
    mexutils_write_uint8(data, &plhs[1], grant.mcs.tbs, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pdsch.symbols[0], &plhs[2], cfg.nbits.nof_re, 1);  
  }
  if (nlhs >= 4) {
    mexutils_write_cf(pdsch.d, &plhs[3], cfg.nbits.nof_re, 1);  
  }
  if (nlhs >= 5) {
    mexutils_write_s(pdsch.e, &plhs[4], cfg.nbits.nof_bits, 1);  
  }
  if (nlhs >= 6) {
    mexutils_write_s(softbuffer.buffer_f[9], &plhs[5], 16908, 1);  
  }
  
  srslte_chest_dl_free(&chest);
  srslte_pdsch_free(&pdsch);
  
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(data);
  free(input_fft);
  
  return;
}

