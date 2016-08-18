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

#define UECFG      prhs[0]
#define PUSCHCFG   prhs[1]
#define TBS        prhs[2]
#define INPUT      prhs[3]
#define NOF_INPUTS 4

void help()
{
  mexErrMsgTxt
    ("[decoded_ok, cqi_data, ri_data, ack_data] = srslte_pusch(ueConfig, puschConfig, trblklen, rxWaveform)\n\n");
}

extern int indices[2048];

int rv_seq[4] = {0, 2, 3, 1};

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  srslte_ofdm_t ofdm_rx; 
  srslte_pusch_t pusch;
  srslte_chest_ul_t chest; 
  cf_t *input_fft;
  srslte_softbuffer_rx_t softbuffer; 
  uint32_t rnti32;

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }

  srslte_verbose = SRSLTE_VERBOSE_INFO;

  srslte_cell_t cell;     
  bzero(&cell, sizeof(srslte_cell_t));
  cell.nof_ports = 1; 
  if (mexutils_read_uint32_struct(UECFG, "NCellID", &cell.id)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  if (mexutils_read_uint32_struct(UECFG, "NULRB", &cell.nof_prb)) {
    mexErrMsgTxt("Field NULRB not found in UE config\n");
    return;
  }

  srslte_pusch_cfg_t cfg;
  bzero(&cfg, sizeof(srslte_pusch_cfg_t));
  if (mexutils_read_uint32_struct(UECFG, "RNTI", &rnti32)) {
    mexErrMsgTxt("Field RNTI not found in pdsch config\n");
    return;
  }
  
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &cfg.sf_idx)) {
    help();
    return;
  }

  if (srslte_ofdm_rx_init(&ofdm_rx, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initializing FFT\n");
    return;
  }
  srslte_ofdm_set_normalize(&ofdm_rx, true);
  srslte_ofdm_set_freq_shift(&ofdm_rx, 0.5);

  if (srslte_pusch_init(&pusch, cell)) {
    mexErrMsgTxt("Error initiating PDSCH\n");
    return;
  }
  srslte_pusch_set_rnti(&pusch, (uint16_t) (rnti32 & 0xffff));

  if (srslte_softbuffer_rx_init(&softbuffer, cell.nof_prb)) {
    mexErrMsgTxt("Error initiating soft buffer\n");
    return;
  }
  
  if (srslte_chest_ul_init(&chest, cell)) {
    mexErrMsgTxt("Error initializing equalizer\n");
    return;
  }

  srslte_ra_ul_grant_t grant; 
  bzero(&grant, sizeof(srslte_ra_ul_grant_t));

  char *mod_str = mexutils_get_char_struct(PUSCHCFG, "Modulation");  
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
  
  grant.mcs.tbs = mxGetScalar(TBS);
  if (grant.mcs.tbs == 0) {
    mexErrMsgTxt("Error trblklen is zero\n");
    return;
  }
    
  uint32_t N_srs = 0; 
  mexutils_read_uint32_struct(PUSCHCFG, "Shortened", &N_srs);
  if (N_srs == 1) {
    pusch.shortened = true; 
  }

  float *prbset = NULL; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found\n");
    return;
  } 

  grant.L_prb = mexutils_read_f(p, &prbset);
  grant.n_prb[0] = prbset[0];
  grant.n_prb[1] = prbset[0];
  free(prbset);
  
  grant.M_sc = grant.L_prb*SRSLTE_NRE;
  grant.M_sc_init = grant.M_sc; // FIXME: What should M_sc_init be? 
  grant.Qm = srslte_mod_bits_x_symbol(grant.mcs.mod);

  if (srslte_cbsegm(&cfg.cb_segm, grant.mcs.tbs)) {
    mexErrMsgTxt("Error computing CB segmentation\n");
    return; 
  }

  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &cfg.rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }

  uint32_t max_iterations = 5;
  mexutils_read_uint32_struct(PUSCHCFG, "NTurboDecIts", &max_iterations);
  
  /* Configure rest of pdsch_cfg parameters */
  if (srslte_pusch_cfg(&pusch, &cfg, &grant, NULL, NULL, NULL, cfg.sf_idx, cfg.rv, 0)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }

  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));

  mexutils_read_uint32_struct(PUSCHCFG, "OCQI", &uci_data.uci_cqi_len);
  mexutils_read_uint32_struct(PUSCHCFG, "ORI", &uci_data.uci_ri_len);
  mexutils_read_uint32_struct(PUSCHCFG, "OACK", &uci_data.uci_ack_len);
    
  float beta; 
  if (mexutils_read_float_struct(PUSCHCFG, "BetaCQI", &beta)) {
    cfg.uci_cfg.I_offset_cqi = 7; 
  } else {
    cfg.uci_cfg.I_offset_cqi = srslte_sch_find_Ioffset_cqi(beta);
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaRI", &beta)) {
    cfg.uci_cfg.I_offset_ri = 2; 
  } else {
    cfg.uci_cfg.I_offset_ri = srslte_sch_find_Ioffset_ri(beta);
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaACK", &beta)) {
    cfg.uci_cfg.I_offset_ack = 0; 
  } else {
    cfg.uci_cfg.I_offset_ack = srslte_sch_find_Ioffset_ack(beta);
  }
  mexPrintf("TRBL_len: %d, CQI_len: %2d, ACK_len: %d (%d), RI_len: %d (%d)\n", grant.mcs.tbs, 
            uci_data.uci_cqi_len, uci_data.uci_ack_len, uci_data.uci_ack, uci_data.uci_ri_len, uci_data.uci_ri);

  mexPrintf("I_cqi: %2d, I_ri: %2d, I_ack=%2d\n", cfg.uci_cfg.I_offset_cqi, cfg.uci_cfg.I_offset_ri, cfg.uci_cfg.I_offset_ack);

  
  mexPrintf("L_prb: %d, n_prb: %d/%d, TBS=%d\n", grant.L_prb, grant.n_prb[0], grant.n_prb[1], grant.mcs.tbs);

  /** Allocate input buffers */
  int nof_retx=1; 
  if (mexutils_isCell(INPUT)) {
    nof_retx = mexutils_getLength(INPUT);
  } 
  
  cf_t *ce = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));;

  uint8_t *data_bytes = srslte_vec_malloc(sizeof(uint8_t) * grant.mcs.tbs/8);
  if (!data_bytes) {
    return;
  }
  srslte_sch_set_max_noi(&pusch.ul_sch, max_iterations);

  input_fft = NULL; 
  int r=-1;
  for (int rvIdx=0;rvIdx<nof_retx && r != 0;rvIdx++) {
    
    mxArray *tmp = (mxArray*) INPUT; 
    if (mexutils_isCell(INPUT)) {
      tmp = mexutils_getCellArray(INPUT, rvIdx);
      if (nof_retx > 1) {
        cfg.rv = rv_seq[rvIdx%4];
      }
    } 
    
    // Read input signal 
    cf_t *input_signal = NULL; 
    int insignal_len = mexutils_read_cf(tmp, &input_signal);
    if (insignal_len < 0) {
      mexErrMsgTxt("Error reading input signal\n");
      return; 
    }
    if (insignal_len == SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)) {
      input_fft = input_signal; 
      mexPrintf("Input is after fft\n");
    } else {
      input_fft = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));  
      srslte_ofdm_rx_sf(&ofdm_rx, input_signal, input_fft);
      mexPrintf("Input is before fft\n");
      free(input_signal);
    }
    
    if (nrhs > NOF_INPUTS) {
      cf_t *cearray = NULL; 
      mexutils_read_cf(prhs[NOF_INPUTS], &cearray);
      cf_t *cearray_ptr = cearray; 
      for (int j=0;j<SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);j++) {
        ce[j] = *cearray_ptr;
        cearray_ptr++;
      }
      if (cearray)
        free(cearray);
    } else {
      srslte_chest_ul_estimate(&chest, input_fft, ce, grant.L_prb, cfg.sf_idx, 0, grant.n_prb);    
    }
    
    float noise_power;
    if (nrhs > NOF_INPUTS + 1) {
      noise_power = mxGetScalar(prhs[NOF_INPUTS+1]);
    } else if (nrhs > NOF_INPUTS) {
      noise_power = 0; 
    } else {
      noise_power = srslte_chest_ul_get_noise_estimate(&chest);
    }

    r = srslte_pusch_uci_decode(&pusch, &cfg, &softbuffer, input_fft, ce, noise_power, data_bytes, &uci_data);
  }
  
  uint8_t *data = malloc(grant.mcs.tbs);
  srslte_bit_unpack_vector(data_bytes, data, grant.mcs.tbs);
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(r == 0);
  }
  if (nlhs >= 2) {
    mexutils_write_uint8(uci_data.uci_cqi, &plhs[1], uci_data.uci_cqi_len, 1);
  }
  if (nlhs >= 3 && uci_data.uci_ri_len <= 1) {
    mexutils_write_uint8(&uci_data.uci_ri, &plhs[2], uci_data.uci_ri_len, 1);
  }
  if (nlhs >= 4 && uci_data.uci_ack_len <= 1) {
    mexutils_write_uint8(&uci_data.uci_ack, &plhs[3], uci_data.uci_ack_len, 1);
  }
  
  srslte_softbuffer_rx_free(&softbuffer);
  srslte_chest_ul_free(&chest);
  srslte_pusch_free(&pusch);
  srslte_ofdm_rx_free(&ofdm_rx);
  
  free(ce);
  free(data_bytes);
  free(data);
  if (input_fft) {
    free(input_fft); 
  }
  
  return;
}

