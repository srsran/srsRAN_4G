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

  srslte_verbose = SRSLTE_VERBOSE_NONE;
  
  if (srslte_softbuffer_tx_init(&softbuffer, cell.nof_prb)) {
    mexErrMsgTxt("Error initiating HARQ\n");
    return;
  }

  uint8_t *trblkin_bits = NULL;
  cfg.grant.mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin_bits);

  uint8_t *trblkin = srslte_vec_malloc(cfg.grant.mcs.tbs/8);
  srslte_bit_pack_vector(trblkin_bits, trblkin, cfg.grant.mcs.tbs);
  free(trblkin_bits);

  
  uint8_t *tmp; 
  uci_data.uci_cqi_len = mexutils_read_uint8(CQI, &tmp);
  memcpy(uci_data.uci_cqi, tmp, uci_data.uci_cqi_len);
  free(tmp);  
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
  cfg.nbits.lstart = 0;
  cfg.nbits.nof_symb = 2*(SRSLTE_CP_NSYMB(cell.cp)-1) - N_srs; 
  cfg.grant.M_sc = cfg.grant.L_prb*SRSLTE_NRE;
  cfg.grant.M_sc_init = cfg.grant.M_sc; // FIXME: What should M_sc_init be? 
  cfg.nbits.nof_re = cfg.nbits.nof_symb*cfg.grant.M_sc;
  cfg.grant.Qm = srslte_mod_bits_x_symbol(cfg.grant.mcs.mod);
  cfg.nbits.nof_bits = cfg.nbits.nof_re * cfg.grant.Qm;

  mexPrintf("Q_m: %d, NPRB: %d, RV: %d, Nsrs=%d\n", srslte_mod_bits_x_symbol(cfg.grant.mcs.mod), cfg.grant.L_prb, cfg.rv, N_srs);

  mexPrintf("I_cqi: %d, I_ri: %d, I_ack=%d\n", cfg.uci_cfg.I_offset_cqi, cfg.uci_cfg.I_offset_ri, cfg.uci_cfg.I_offset_ack);

  if (srslte_cbsegm(&cfg.cb_segm, cfg.grant.mcs.tbs)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
    return;
  }
    
  uint8_t *q_bits = srslte_vec_malloc(cfg.nbits.nof_bits * sizeof(uint8_t)/8);
  if (!q_bits) {
    return;
  }
  uint8_t *q_bits_unpacked = srslte_vec_malloc(cfg.nbits.nof_bits * sizeof(uint8_t));
  if (!q_bits_unpacked) {
    return;
  }
  uint8_t *g_bits = srslte_vec_malloc(cfg.nbits.nof_bits * sizeof(uint8_t)/8);
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

  srslte_bit_unpack_vector(q_bits, q_bits_unpacked, cfg.nbits.nof_bits);

  if (nlhs >= 1) {
    mexutils_write_uint8(q_bits_unpacked, &plhs[0], cfg.nbits.nof_bits, 1);  
  }
  
  srslte_sch_free(&ulsch);  
  srslte_softbuffer_tx_free(&softbuffer);
  
  free(trblkin);
  free(g_bits);    
  free(q_bits_unpacked);
  free(q_bits);    
  
  
  return;
}

