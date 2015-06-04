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
    ("[cwout] = srslte_pusch_encode(ue, chs, trblkin, cqi, ri, ack)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  srslte_sch_t ulsch;
  uint8_t *trblkin;
  srslte_pusch_cfg_t cfg;
  srslte_softbuffer_tx_t softbuffer; 
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  uint32_t rv; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  bzero(&cfg, sizeof(srslte_pusch_cfg_t));
  
  if (srslte_sch_init(&ulsch)) {
    mexErrMsgTxt("Error initiating ULSCH\n");
    return;
  }
  srslte_cell_t cell;
  cell.nof_prb = 100;
  cell.id=1;
  cell.cp=SRSLTE_CP_NORM;

  if (srslte_softbuffer_tx_init(&softbuffer, cell)) {
    mexErrMsgTxt("Error initiating HARQ\n");
    return;
  }

  cfg.grant.mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin);

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
  
  mexPrintf("TRBL_len: %d, CQI_len: %d, ACK_len: %d, RI_len: %d\n", cfg.grant.mcs.tbs, 
            uci_data.uci_cqi_len, uci_data.uci_ack_len, uci_data.uci_ri_len);
  
  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }

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
  
  float *prbset; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found\n");
    return;
  } 
  
  uint32_t N_srs = 0; 
  mexutils_read_uint32_struct(PUSCHCFG, "Shortened", &N_srs);
  
  
  cfg.grant.L_prb = mexutils_read_f(p, &prbset);
  cfg.grant.n_prb[0] = prbset[0];
  cfg.grant.n_prb[1] = prbset[0];
  free(prbset);
  cfg.grant.L_prb = mexutils_read_f(p, &prbset);
  cfg.grant.n_prb[0] = prbset[0];
  cfg.grant.n_prb[1] = prbset[0];
  cfg.grant.lstart = 0;
  cfg.grant.nof_symb = 2*(SRSLTE_CP_NSYMB(cell.cp)-1) - N_srs; 
  cfg.grant.M_sc = cfg.grant.L_prb*SRSLTE_NRE;
  cfg.grant.M_sc_init = cfg.grant.M_sc; // FIXME: What should M_sc_init be? 
  cfg.grant.nof_re = cfg.grant.nof_symb*cfg.grant.M_sc;
  cfg.grant.Qm = srslte_mod_bits_x_symbol(cfg.grant.mcs.mod);
  cfg.grant.nof_bits = cfg.grant.nof_re * cfg.grant.Qm;

  mexPrintf("Q_m: %d, NPRB: %d, RV: %d, Nsrs=%d\n", srslte_mod_bits_x_symbol(cfg.grant.mcs.mod), cfg.grant.L_prb, cfg.rv, N_srs);

  if (srslte_cbsegm(&cfg.cb_segm, cfg.grant.mcs.tbs)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
    return;
  }
    
  uint8_t *q_bits = srslte_vec_malloc(cfg.grant.nof_bits * sizeof(uint8_t));
  if (!q_bits) {
    return;
  }
  uint8_t *g_bits = srslte_vec_malloc(cfg.grant.nof_bits * sizeof(uint8_t));
  if (!g_bits) {
    return;
  }

  if (srslte_ulsch_uci_encode(&ulsch, &cfg, &softbuffer, trblkin, uci_data, g_bits, q_bits)) 
  {
    mexErrMsgTxt("Error encoding TB\n");
    return;
  }    
  if (rv > 0) {
    cfg.rv = rv; 
    if (srslte_ulsch_uci_encode(&ulsch, &cfg, &softbuffer, trblkin, uci_data, g_bits, q_bits)) {
      mexErrMsgTxt("Error encoding TB\n");
      return;
    }    
  }
  
  if (nlhs >= 1) {
    mexutils_write_uint8(q_bits, &plhs[0], cfg.grant.nof_bits, 1);  
  }
  
  srslte_sch_free(&ulsch);  
  srslte_softbuffer_tx_free(&softbuffer);
  
  free(trblkin);
  free(g_bits);    
  free(q_bits);    
  free(uci_data.uci_cqi);
  
  return;
}

