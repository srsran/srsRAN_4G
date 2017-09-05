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
#define N_BITS     prhs[2]
#define INPUT      prhs[3]
#define THRESHOLD  prhs[4]
#define NOF_INPUTS 4

void help()
{
  mexErrMsgTxt
    ("[data, symbols, ce]=srslte_pucch(ue, chs, n_bits, input)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  srslte_verbose = SRSLTE_VERBOSE_NONE;
  
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
  
  float *thresholds;
  int th_len = 0; 
  
  if (nrhs > NOF_INPUTS) {
    th_len = mexutils_read_f(THRESHOLD, &thresholds);
    if (th_len == 2) {
      srslte_pucch_set_threshold(&pucch, thresholds[0], thresholds[1]);
    }
  }
  
  uint8_t bits[SRSLTE_PUCCH_MAX_BITS]; 
  int nof_bits = (int) mxGetScalar(N_BITS); 
  
  srslte_pucch_format_t format; 
  switch(nof_bits) {
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
  if (nof_bits > 20) {
    nof_bits = 20; 
  }
  
  cf_t *sf_symbols = NULL; 
  int nof_re = mexutils_read_cf(INPUT, &sf_symbols);
  if (nof_re < 0) {
    mexErrMsgTxt("Error reading input\n");
    return;
  }
  cf_t *ce = srslte_vec_malloc(nof_re*sizeof(cf_t));
  if (!ce) {
    perror("malloc");
    return;
  }
  bzero(ce, nof_re*sizeof(cf_t));
  srslte_chest_ul_t chest_ul; 
  if (srslte_chest_ul_init(&chest_ul, cell)) {
    mexErrMsgTxt("Error initiating PUCCH DMRS\n");
    return; 
  }
  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
  pusch_cfg.group_hopping_en = group_hopping_en; 
  pusch_cfg.sequence_hopping_en = false; 
  srslte_chest_ul_set_cfg(&chest_ul, &pusch_cfg, &pucch_cfg, NULL);
  
  srslte_pucch_set_cfg(&pucch, &pucch_cfg, group_hopping_en);
  
  uint8_t pucch2_ack_bits[2] = {0};

  if (srslte_chest_ul_estimate_pucch(&chest_ul, sf_symbols, ce, format, n_pucch, sf_idx, &pucch2_ack_bits)) {
    mexErrMsgTxt("Error estimating PUCCH DMRS\n");
    return;
  }
  
  if (srslte_pucch_decode(&pucch, format, n_pucch, sf_idx, (uint16_t) rnti, sf_symbols, ce, 0, bits)<0) {
    mexErrMsgTxt("Error decoding PUCCH\n");
    return; 
  }

  if (nlhs >= 1) {
    if (format != SRSLTE_PUCCH_FORMAT_1) {
      mexutils_write_uint8(bits, &plhs[0], nof_bits, 1);  
    } else {
      if (bits[0] == 1) {
        mexutils_write_uint8(bits, &plhs[0], 0, 1);  
      } else {
        mexutils_write_uint8(bits, &plhs[0], 0, 0);  
      }
    }
  }

  if (nlhs >= 2) {
    mexutils_write_cf(pucch.z, &plhs[1], 10, 1);  
  }

  if (nlhs >= 3) {
    mexutils_write_cf(pucch.z_tmp, &plhs[2], 120, 1);
  }

  srslte_pucch_free(&pucch);  
  free(sf_symbols);
  
  return;
}

