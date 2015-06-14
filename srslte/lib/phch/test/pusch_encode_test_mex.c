/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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
#define TRBLKIN    prhs[2]
#define CQI        prhs[3]
#define RI         prhs[4]
#define ACK        prhs[5]
#define NOF_INPUTS 6

void help()
{
  mexErrMsgTxt
    ("sym=srslte_pusch_encode(ue, chs, trblkin, cqi, ri, ack)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
  
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
  srslte_pusch_t pusch;
  if (srslte_pusch_init(&pusch, cell)) {
    mexErrMsgTxt("Error initiating PUSCH\n");
    return;
  }
  uint32_t rnti32=0;
  if (mexutils_read_uint32_struct(UECFG, "RNTI", &rnti32)) {
    mexErrMsgTxt("Field RNTI not found in pusch config\n");
    return;
  }
  srslte_pusch_set_rnti(&pusch, (uint16_t) (rnti32 & 0xffff));
  
  
  
  srslte_pusch_cfg_t cfg; 
  bzero(&cfg, sizeof(srslte_pusch_cfg_t));
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &cfg.sf_idx)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  char *mod_str = mexutils_get_char_struct(PUSCHCFG, "Modulation");  
  if (!strcmp(mod_str, "QPSK")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_QPSK;
  } else if (!strcmp(mod_str, "16QAM")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_16QAM;
  } else if (!strcmp(mod_str, "64QAM")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_64QAM;
  } else {
   mexErrMsgTxt("Unknown modulation\n");
   return;
  }

  mxFree(mod_str);
  
  float *prbset = NULL; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found\n");
    return;
  } 
  
  uint32_t N_srs = 0; 
  mexutils_read_uint32_struct(PUSCHCFG, "Shortened", &N_srs);
  if (N_srs == 1) {
    pusch.shortened = true; 
  }
  cfg.grant.L_prb = mexutils_read_f(p, &prbset);
  cfg.grant.n_prb[0] = prbset[0];
  cfg.grant.n_prb[1] = prbset[0];
  cfg.grant.M_sc = cfg.grant.L_prb*SRSLTE_NRE;
  cfg.grant.M_sc_init = cfg.grant.M_sc; // FIXME: What should M_sc_init be? 
  cfg.grant.Qm = srslte_mod_bits_x_symbol(cfg.grant.mcs.mod);
  if (srslte_pusch_cfg(&pusch, &cfg, NULL, NULL, NULL, cfg.sf_idx, cfg.rv)) {
    fprintf(stderr, "Error configuring PDSCH\n");
    exit(-1);
  }
  
  free(prbset);
  
  mexPrintf("L_prb: %d, n_prb: %d\n", cfg.grant.L_prb, cfg.grant.n_prb[0]);
  
  uint8_t *trblkin = NULL;
  cfg.grant.mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin);

  srslte_softbuffer_tx_t softbuffer; 
  if (srslte_softbuffer_tx_init(&softbuffer, cell)) {
    mexErrMsgTxt("Error initiating soft buffer\n");
    return;
  }
  if (srslte_cbsegm(&cfg.cb_segm, cfg.grant.mcs.tbs)) {
    mexErrMsgTxt("Error computing CB segmentation\n");
    return; 
  }

  uint32_t nof_re = SRSLTE_NRE*cell.nof_prb*2*SRSLTE_CP_NSYMB(cell.cp);
  cf_t *sf_symbols = srslte_vec_malloc(sizeof(cf_t) * nof_re);
  if (!sf_symbols) {
    mexErrMsgTxt("malloc");
    return;
  }
  bzero(sf_symbols, sizeof(cf_t) * nof_re);
  
  
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  uci_data.uci_cqi_len = mexutils_read_uint8(CQI, &uci_data.uci_cqi);
  uint8_t *tmp;
  uci_data.uci_ri_len = mexutils_read_uint8(RI, &tmp);
  if (uci_data.uci_ri_len > 0) {
    uci_data.uci_ri = *tmp;
  }
  free(tmp);
  uci_data.uci_ack_len = mexutils_read_uint8(ACK, &tmp);
  if (uci_data.uci_ack_len > 0) {
    uci_data.uci_ack = *tmp;
  }
  free(tmp);
  
  
  float beta; 
  if (mexutils_read_float_struct(PUSCHCFG, "BetaCQI", &beta)) {
    uci_data.I_offset_cqi = 7; 
  } else {
    uci_data.I_offset_cqi = srslte_sch_find_Ioffset_cqi(beta);
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaRI", &beta)) {
    uci_data.I_offset_ri = 2; 
  } else {
    uci_data.I_offset_ri = srslte_sch_find_Ioffset_ri(beta);
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaACK", &beta)) {
    uci_data.I_offset_ack = 0; 
  } else {
    uci_data.I_offset_ack = srslte_sch_find_Ioffset_ack(beta);
  }
  mexPrintf("TRBL_len: %d, CQI_len: %d, ACK_len: %d (%d), RI_len: %d (%d)\n", cfg.grant.mcs.tbs, 
            uci_data.uci_cqi_len, uci_data.uci_ack_len, uci_data.uci_ack, uci_data.uci_ri_len, uci_data.uci_ri);

  mexPrintf("I_cqi: %d, I_ri: %d, I_ack=%d\n", uci_data.I_offset_cqi, uci_data.I_offset_ri, uci_data.I_offset_ack);

  mexPrintf("NofRE: %d, NofBits: %d, TBS: %d, N_srs=%d\n", cfg.nbits.nof_re, cfg.nbits.nof_bits, cfg.grant.mcs.tbs, N_srs);
  int r = srslte_pusch_uci_encode(&pusch, &cfg, &softbuffer, trblkin, uci_data, sf_symbols);
  if (r < 0) {
    mexErrMsgTxt("Error encoding PUSCH\n");
    return;
  }
  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &cfg.rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }
  if (cfg.rv > 0) {
    r = srslte_pusch_uci_encode(&pusch, &cfg, &softbuffer, trblkin, uci_data, sf_symbols);
    if (r < 0) {
      mexErrMsgTxt("Error encoding PUSCH\n");
      return;
    }
  }

  cf_t *scfdma = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
  bzero(scfdma, sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
  srslte_ofdm_t fft; 
  srslte_ofdm_tx_init(&fft, SRSLTE_CP_NORM, cell.nof_prb);
  srslte_ofdm_set_normalize(&fft, true);
  srslte_ofdm_set_freq_shift(&fft, 0.5);
  srslte_ofdm_tx_sf(&fft, sf_symbols, scfdma);

  // Matlab toolbox expects further normalization 
  srslte_vec_sc_prod_cfc(scfdma, 1.0/sqrtf(srslte_symbol_sz(cell.nof_prb)), scfdma, SRSLTE_SF_LEN_PRB(cell.nof_prb));
  
  if (nlhs >= 1) {
    mexutils_write_cf(scfdma, &plhs[0], SRSLTE_SF_LEN_PRB(cell.nof_prb), 1);  
  }
  if (nlhs >= 2) {
    mexutils_write_cf(sf_symbols, &plhs[1], nof_re, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_cf(pusch.z, &plhs[2], cfg.nbits.nof_re, 1);  
  }
  srslte_pusch_free(&pusch);  
  free(trblkin);
  free(uci_data.uci_cqi);
  free(sf_symbols);
  free(scfdma);
  
  return;
}

