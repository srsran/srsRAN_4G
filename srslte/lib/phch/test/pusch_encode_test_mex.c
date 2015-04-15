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

#define UECFG      prhs[0]
#define PUCCHCFG   prhs[1]
#define ACK        prhs[2]
#define NOF_INPUTS 3

void help()
{
  mexErrMsgTxt
    ("[subframe, subframe_with_dmrs]=srslte_pucch_encode(ue, chs, ack)\n\n");
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
  cell.cp = SRSLTE_CP_NORM; 
  if (mexutils_read_uint32_struct(UECFG, "NCellID", &cell.id)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  if (mexutils_read_uint32_struct(UECFG, "NULRB", &cell.nof_prb)) {
    mexErrMsgTxt("Field NULRB not found in UE config\n");
    return;
  }
  srslte_pucch_t pucch;
  if (srslte_pucch_init(&pucch, cell)) {
    mexErrMsgTxt("Error initiating PUSCH\n");
    return;
  }

  uint32_t sf_idx = 0; 
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  srslte_pucch_cfg_t pucch_cfg; 
  if (mexutils_read_uint32_struct(PUCCHCFG, "ResourceIdx", &pucch_cfg.n_pucch)) {
    mexErrMsgTxt("Field ResourceIdx not found in PUCCHCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(PUCCHCFG, "DeltaShift", &pucch_cfg.delta_pucch_shift)) {
    mexErrMsgTxt("Field DeltaShift not found in PUCCHCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(PUCCHCFG, "CyclicShifts", &pucch_cfg.N_cs)) {
    mexErrMsgTxt("Field CyclicShifts not found in PUCCHCFG\n");
    return;
  }
  pucch_cfg.group_hopping_en = false; 
  char *hop = mexutils_get_char_struct(PUCCHCFG, "Hopping"); 
  if (hop) {
    if (!strcmp(hop, "Group")) {
      pucch_cfg.group_hopping_en = true; 
    }
    mxFree(hop);
  }
  
  uint8_t bits[SRSLTE_PUCCH_MAX_BITS]; 
  float *bits_ptr; 
  int n = mexutils_read_f(ACK, &bits_ptr); 
  for (int i=0;i<n;i++) {
    bits[i] = bits_ptr>0?1:0; 
  }
  free(bits_ptr); 
  
  switch(n) {
    case 0: 
      pucch_cfg.format = SRSLTE_PUCCH_FORMAT_1; 
      break; 
    case 1: 
      pucch_cfg.format = SRSLTE_PUCCH_FORMAT_1A; 
      break; 
    case 2: 
      pucch_cfg.format = SRSLTE_PUCCH_FORMAT_1B; 
      break; 
    default: 
      mexErrMsgTxt("Invalid number of bits in parameter ack\n");
      return; 
  }
  
  cf_t *sf_symbols = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)); 
  if (!sf_symbols) {
    return; 
  }
  bzero(sf_symbols, SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  if (srslte_pucch_encode(&pucch, &pucch_cfg, sf_idx, bits, sf_symbols)) {
    mexErrMsgTxt("Error encoding PUCCH\n");
    return; 
  }

  if (nlhs >= 1) {
    mexutils_write_cf(sf_symbols, &plhs[0], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), 1);  
  }

  if (nlhs >= 2) {
    srslte_refsignal_ul_t pucch_dmrs; 
    if (srslte_refsignal_ul_init(&pucch_dmrs, cell)) {
      mexErrMsgTxt("Error initiating PUCCH DMRS\n");
      return; 
    }
    cf_t *dmrs_pucch = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_NRE*3);
    if (!dmrs_pucch) {
      return; 
    }
    if (srslte_refsignal_dmrs_pucch_gen(&pucch_dmrs, &pucch_cfg, sf_idx, dmrs_pucch)) {
      mexErrMsgTxt("Error generating PUCCH DMRS\n");
      return; 
    }
    if (srslte_refsignal_dmrs_pucch_put(&pucch_dmrs, &pucch_cfg, dmrs_pucch, sf_symbols)) {
      mexErrMsgTxt("Error generating PUCCH DMRS\n");
      return; 
    }    
    mexutils_write_cf(sf_symbols, &plhs[0], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), 1);  
    srslte_refsignal_ul_free(&pucch_dmrs);
    free(dmrs_pucch);
  }
  
  srslte_pucch_free(&pucch);  
  free(sf_symbols);
  
  return;
}

