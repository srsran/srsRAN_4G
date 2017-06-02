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
#define NOF_INPUTS 2

void help()
{
  mexErrMsgTxt
    ("[seq] = srslte_refsignal_pusch(ueConfig, puschConfig)\n\n");
}

extern int indices[2048];

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  srslte_cell_t cell; 
  srslte_refsignal_ul_t refs;
  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg;
  uint32_t sf_idx; 

  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_uint32_struct(UECFG, "NCellID", &cell.id)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  if (mexutils_read_uint32_struct(UECFG, "NULRB", &cell.nof_prb)) {
    mexErrMsgTxt("Field NCellID not found in UE config\n");
    return;
  }
  cell.cp = SRSLTE_CP_NORM;
  cell.nof_ports = 1; 

  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  
  bzero(&pusch_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));


  pusch_cfg.group_hopping_en = false;
  pusch_cfg.sequence_hopping_en = false;
  char *tmp = mexutils_get_char_struct(UECFG, "Hopping");
  if (tmp) {
    if (!strcmp(tmp, "Group")) {
      pusch_cfg.group_hopping_en = true;
    } else if (!strcmp(tmp, "Sequence")) {
      pusch_cfg.sequence_hopping_en = true;
    }
    mxFree(tmp);    
  }
  
  
  if (mexutils_read_uint32_struct(UECFG, "SeqGroup", &pusch_cfg.delta_ss)) {
    pusch_cfg.delta_ss = 0; 
  }
  if (mexutils_read_uint32_struct(UECFG, "CyclicShift", &pusch_cfg.cyclic_shift)) {
    pusch_cfg.cyclic_shift = 0; 
  }
  float *prbset; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found in PUSCH config\n");
    return;
  } 
  uint32_t nof_prb = mexutils_read_f(p, &prbset); 
  
  uint32_t cyclic_shift_for_dmrs = 0; 
  if (mexutils_read_uint32_struct(PUSCHCFG, "DynCyclicShift", &cyclic_shift_for_dmrs)) {
    cyclic_shift_for_dmrs = 0; 
  } 
  
  if (srslte_refsignal_ul_init(&refs, cell)) {
    mexErrMsgTxt("Error initiating srslte_refsignal_ul\n");
    return;
  }

  mexPrintf("nof_prb: %d, ",nof_prb);
  mexPrintf("cyclic_shift: %d, ",pusch_cfg.cyclic_shift);
  mexPrintf("cyclic_shift_for_dmrs: %d, ", cyclic_shift_for_dmrs);
  mexPrintf("delta_ss: %d, ",pusch_cfg.delta_ss);
  
  cf_t *signal = srslte_vec_malloc(2*SRSLTE_NRE*nof_prb*sizeof(cf_t));
  if (!signal) {
    perror("malloc");
    return;
  }
  cf_t *sf_symbols = srslte_vec_malloc(SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)*sizeof(cf_t));
  if (!sf_symbols) {
    perror("malloc");
    return;
  }
  bzero(sf_symbols, SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)*sizeof(cf_t));
  
  srslte_refsignal_ul_set_cfg(&refs, &pusch_cfg, NULL, NULL);
  
  //mexPrintf("Generating DRMS for ns=%d, nof_prb=%d\n", 2*sf_idx+i,pusch_cfg.nof_prb);
  srslte_refsignal_dmrs_pusch_gen(&refs, nof_prb, sf_idx, cyclic_shift_for_dmrs, signal);    
  uint32_t n_prb[2]; 
  n_prb[0] = prbset[0];
  n_prb[1] = prbset[0];
  srslte_refsignal_dmrs_pusch_put(&refs, signal, nof_prb, n_prb, sf_symbols);                
  if (nlhs >= 1) {
    mexutils_write_cf(sf_symbols, &plhs[0], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), 1);  
  }

  srslte_refsignal_ul_free(&refs);  
  free(signal);
  free(prbset);

  return;
}

