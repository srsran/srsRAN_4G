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
    ("[cwout] = srslte_srslte_pusch_encode(ue, chs, trblkin, cqi, ri, ack)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  srslte_sch_t ulsch;
  uint8_t *trblkin;
  srslte_ra_mcs_t mcs;
  srslte_srslte_ra_ul_alloc_t prb_alloc;
  srslte_harq_t harq_process;
  uint32_t rv;
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  if (srslte_sch_init(&ulsch)) {
    mexErrMsgTxt("Error initiating ULSCH\n");
    return;
  }
  srslte_cell_t cell;
  cell.nof_prb = 100;
  cell.id=1;
  cell.cp=SRSLTE_SRSLTE_CP_NORM;
  if (srslte_harq_init(&harq_process, cell)) {
    mexErrMsgTxt("Error initiating HARQ\n");
    return;
  }

  mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin);

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
  
  mexPrintf("TRBL_len: %d, CQI_len: %d, ACK_len: %d, RI_len: %d\n", mcs.tbs, 
            uci_data.uci_cqi_len, uci_data.uci_ack_len, uci_data.uci_ri_len);
  
  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &rv)) {
    mexErrMsgTxt("Field RV not found in pdsch config\n");
    return;
  }

  if (mexutils_read_float_struct(PUSCHCFG, "BetaCQI", &uci_data.beta_cqi)) {
    uci_data.beta_cqi = 2.0; 
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaRI", &uci_data.beta_ri)) {
    uci_data.beta_ri = 2.0; 
  }
  if (mexutils_read_float_struct(PUSCHCFG, "BetaACK", &uci_data.beta_ack)) {
    uci_data.beta_ack = 2.0; 
  }
  mexPrintf("Beta_CQI: %.1f, Beta_ACK: %.1f, Beta_RI: %.1f\n", 
            uci_data.beta_cqi, uci_data.beta_ack, uci_data.beta_ri);
  
  char *mod_str = mexutils_get_char_struct(PUSCHCFG, "Modulation");
  
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
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found\n");
    return;
  } 
  
  prb_alloc.L_prb = mexutils_read_f(p, &prbset);
  prb_alloc.n_prb[0] = prbset[0];
  prb_alloc.n_prb[1] = prbset[0];
  free(prbset);
  
  mexPrintf("Q_m: %d, NPRB: %d, RV: %d\n", srslte_mod_bits_x_symbol(mcs.mod), prb_alloc.L_prb, rv);

  if (srslte_harq_setup_ul(&harq_process, mcs, 0, 0, &prb_alloc)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
    return;
  }
    
  uint8_t *q_bits = srslte_vec_malloc(harq_process.nof_bits * sizeof(uint8_t));
  if (!q_bits) {
    return;
  }
  uint8_t *g_bits = srslte_vec_malloc(harq_process.nof_bits * sizeof(uint8_t));
  if (!g_bits) {
    return;
  }

  if (srslte_ulsch_uci_encode(&ulsch, &harq_process, trblkin, uci_data, g_bits, q_bits)) 
  {
    mexErrMsgTxt("Error encoding TB\n");
    return;
  }    
  if (rv > 0) {
    if (srslte_harq_setup_ul(&harq_process, mcs, rv, 0, &prb_alloc)) {
      mexErrMsgTxt("Error configuring HARQ process\n");
      return;
    }
    if (srslte_ulsch_uci_encode(&ulsch, &harq_process, trblkin, uci_data, g_bits, q_bits)) {
      mexErrMsgTxt("Error encoding TB\n");
      return;
    }    
  }
  
  if (nlhs >= 1) {
    mexutils_write_uint8(q_bits, &plhs[0], harq_process.nof_bits, 1);  
  }
  
  srslte_sch_free(&ulsch);  
  srslte_harq_free(&harq_process);
  
  free(trblkin);
  free(g_bits);    
  free(q_bits);    
  free(uci_data.uci_cqi);
  
  return;
}

