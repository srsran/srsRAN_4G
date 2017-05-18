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
#define PUCCHCFG   prhs[1]
#define ACK        prhs[2]
#define NOF_INPUTS 3

void help()
{
  mexErrMsgTxt
    ("[sym, sym_with_dmrs, subframe_all]=srslte_pucch_encode(ue, chs, ack)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
  srslte_verbose = SRSLTE_VERBOSE_DEBUG;
  
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
  uint32_t rnti; 
  if (mexutils_read_uint32_struct(UECFG, "RNTI", &rnti)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  if (srslte_pucch_set_crnti(&pucch, (uint16_t) rnti&0xffff)) {
    mexErrMsgTxt("Error setting C-RNTI\n");
    return;
  }
  uint32_t n_pucch; 
  if (mexutils_read_uint32_struct(PUCCHCFG, "ResourceIdx", &n_pucch)) {
    mexErrMsgTxt("Field ResourceIdx not found in PUCCHCFG\n");
    return;
  }
  srslte_pucch_cfg_t pucch_cfg; 
  bzero(&pucch_cfg, sizeof(srslte_pucch_cfg_t));
  
  if (mexutils_read_uint32_struct(PUCCHCFG, "DeltaShift", &pucch_cfg.delta_pucch_shift)) {
    mexErrMsgTxt("Field DeltaShift not found in PUCCHCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(PUCCHCFG, "ResourceSize", &pucch_cfg.n_rb_2)) {
    mexErrMsgTxt("Field DeltaShift not found in PUCCHCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(PUCCHCFG, "CyclicShifts", &pucch_cfg.N_cs)) {
    mexErrMsgTxt("Field CyclicShifts not found in PUCCHCFG\n");
    return;
  }
  bool group_hopping_en = false; 
  char *hop = mexutils_get_char_struct(UECFG, "Hopping"); 
  if (hop) {
    if (!strcmp(hop, "Group")) {
      group_hopping_en = true; 
    }
    mxFree(hop);
  }
  
  pucch.shortened = false; 
  uint32_t sh = 0; 
  mexutils_read_uint32_struct(PUCCHCFG, "Shortened", &sh);
  if (sh == 1) {
    pucch.shortened = true;     
  }
  
  uint8_t bits[SRSLTE_PUCCH_MAX_BITS]; 
  uint8_t pucch2_bits[2]; 
  float *bits_ptr; 
  int n = mexutils_read_f(ACK, &bits_ptr); 
  
  srslte_pucch_format_t format; 
  switch(n) {
    case 0: 
      format = SRSLTE_PUCCH_FORMAT_1; 
      break; 
    case 1: 
      format = SRSLTE_PUCCH_FORMAT_1A; 
      break; 
    case 2: 
      format = SRSLTE_PUCCH_FORMAT_1B; 
      break; 
    case 20: 
      format = SRSLTE_PUCCH_FORMAT_2;
      break;
    case 21: 
      format = SRSLTE_PUCCH_FORMAT_2A;
      break;
    case 22: 
      format = SRSLTE_PUCCH_FORMAT_2B;
      break;
    default: 
      mexErrMsgTxt("Invalid number of bits in parameter ack\n");
      return; 
  }
  if (n > 20) {
    n = 20; 
  }
  for (int i=0;i<n;i++) {
    bits[i] = bits_ptr[i]>0?1:0; 
  }
  if (format == SRSLTE_PUCCH_FORMAT_2A) {
    pucch2_bits[0] = bits_ptr[20];
  }
  if (format == SRSLTE_PUCCH_FORMAT_2B) {
    pucch2_bits[0] = bits_ptr[20];
    pucch2_bits[1] = bits_ptr[21];
  }
  free(bits_ptr); 
  
  cf_t *sf_symbols = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)); 
  if (!sf_symbols) {
    return; 
  }
  bzero(sf_symbols, SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  
  srslte_pucch_set_cfg(&pucch, &pucch_cfg, group_hopping_en);
  
  if (srslte_pucch_encode(&pucch, format, n_pucch, sf_idx, bits, sf_symbols)) {
    mexErrMsgTxt("Error encoding PUCCH\n");
    return; 
  }

  if (nlhs >= 1) {
    uint32_t n_bits = srslte_pucch_nof_symbols(&pucch_cfg, format, pucch.shortened);
    mexutils_write_cf(pucch.z, &plhs[0], n_bits, 1);  
  }

  if (nlhs >= 2) {
    srslte_refsignal_ul_t pucch_dmrs; 
    if (srslte_refsignal_ul_init(&pucch_dmrs, cell)) {
      mexErrMsgTxt("Error initiating PUCCH DMRS\n");
      return; 
    }
    cf_t *dmrs_pucch = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_NRE*3*2);
    if (!dmrs_pucch) {
      return; 
    }
    bzero(dmrs_pucch, sizeof(cf_t)*SRSLTE_NRE*3*2);
    
    srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
    pusch_cfg.group_hopping_en = group_hopping_en; 
    pusch_cfg.sequence_hopping_en = false; 
    srslte_refsignal_ul_set_cfg(&pucch_dmrs, &pusch_cfg, &pucch_cfg, NULL);
    
    if (srslte_refsignal_dmrs_pucch_gen(&pucch_dmrs, format, n_pucch, sf_idx, pucch2_bits, dmrs_pucch)) {
      mexErrMsgTxt("Error generating PUCCH DMRS\n");
      return; 
    }
    uint32_t n_rs = 3;
    if (format >= SRSLTE_PUCCH_FORMAT_2) {
      n_rs = 2;
    }
    mexutils_write_cf(dmrs_pucch, &plhs[1], 2*n_rs*SRSLTE_NRE, 1);  
    
    if (nlhs >= 3) {
      if (srslte_refsignal_dmrs_pucch_put(&pucch_dmrs, format, n_pucch, dmrs_pucch, sf_symbols)) {
        mexErrMsgTxt("Error generating PUCCH DMRS\n");
        return; 
      }   
      mexutils_write_cf(sf_symbols, &plhs[2], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), 1);  
    }
    
    srslte_refsignal_ul_free(&pucch_dmrs);
    free(dmrs_pucch);
  }

  srslte_pucch_free(&pucch);  
  free(sf_symbols);
  
  return;
}

