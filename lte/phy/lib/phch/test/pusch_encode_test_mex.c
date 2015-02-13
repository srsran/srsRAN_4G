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

#define UECFG      prhs[0]
#define PUSCHCFG   prhs[1]
#define TRBLKIN    prhs[2]
#define CQI        prhs[3]
#define RI         prhs[4]
#define ACK        prhs[5]
#define NOF_INPUTS 3

void help()
{
  mexErrMsgTxt
    ("sym=liblte_pusch_encode(ue, chs, trblkin, cqi, ri, ack)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  int i; 
  lte_cell_t cell; 
  pusch_t pusch;
  uint32_t sf_idx; 
  uint8_t *trblkin = NULL;
  cf_t *sf_symbols = NULL; 
  ra_mcs_t mcs;
  ra_prb_t prb_alloc;
  harq_t harq_process;
  uint32_t rv;
  uint32_t rnti32;
  uci_data_t uci_data; 
  bzero(&uci_data, sizeof(uci_data_t));

  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_uint32_struct(UECFG, "NCellID", &cell.id)) {
    mexErrMsgTxt("Field NCellID not found in pusch config\n");
    return;
  }
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    mexErrMsgTxt("Field NCellID not found in pusch config\n");
    return;
  }
  
  if (mexutils_read_uint32_struct(UECFG, "RNTI", &rnti32)) {
    mexErrMsgTxt("Field RNTI not found in pusch config\n");
    return;
  }
  
  char *mod_str = mexutils_get_char_struct(PUSCHCFG, "Modulation");
  
  if (!strcmp(mod_str, "QPSK")) {
    mcs.mod = LTE_QPSK;
  } else if (!strcmp(mod_str, "16QAM")) {
    mcs.mod = LTE_QAM16;
  } else if (!strcmp(mod_str, "64QAM")) {
    mcs.mod = LTE_QAM64;
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
  
  // Only localized PRB supported 
  prb_alloc.slot[0].nof_prb = mexutils_read_f(p, &prbset);

  for (i=0;i<cell.nof_prb;i++) {
    prb_alloc.slot[0].prb_idx[i] = false; 
    for (int j=0;j<prb_alloc.slot[0].nof_prb && !prb_alloc.slot[0].prb_idx[i];j++) {
      if ((int) prbset[j] == i) {
        prb_alloc.slot[0].prb_idx[i] = true;
      }
    }
  }
  memcpy(&prb_alloc.slot[1], &prb_alloc.slot[0], sizeof(ra_prb_slot_t));

  free(prbset);
  
  
  if (pusch_init(&pusch, cell)) {
    mexErrMsgTxt("Error initiating PDSCH\n");
    return;
  }
  pusch_set_rnti(&pusch, (uint16_t) (rnti32 & 0xffff));

  if (harq_init(&harq_process, cell)) {
    mexErrMsgTxt("Error initiating HARQ process\n");
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
     
  sf_symbols = vec_malloc(sizeof(cf_t) * harq_process.nof_re);
  if (!sf_symbols) {
    mexErrMsgTxt("malloc");
    return;
  }
  
  if (harq_setup_ul(&harq_process, mcs, rv, sf_idx, &prb_alloc)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
    return;
  }

  int r = pusch_uci_encode(&pusch, &harq_process, trblkin, uci_data, sf_symbols);
  if (r < 0) {
    mexErrMsgTxt("Error encoding PUSCH\n");
    return;
  }
  
  if (nlhs >= 1) {
    mexutils_write_cf(sf_symbols, &plhs[0], harq_process.nof_re, 1);  
  }
  pusch_free(&pusch);  
  free(trblkin);
  free(uci_data.uci_cqi);
  free(sf_symbols);
  
  return;
}

