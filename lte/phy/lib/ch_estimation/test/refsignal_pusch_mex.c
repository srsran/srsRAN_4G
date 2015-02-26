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
#define NOF_INPUTS 2

void help()
{
  mexErrMsgTxt
    ("[seq] = liblte_refsignal_pusch(ueConfig, puschConfig)\n\n");
}

extern int indices[2048];

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  lte_cell_t cell; 
  refsignal_ul_t refs;
  refsignal_drms_pusch_cfg_t pusch_cfg;
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
  cell.cp = CPNORM;
  cell.nof_ports = 1; 

  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  
  bzero(&pusch_cfg, sizeof(refsignal_drms_pusch_cfg_t));


  char *tmp = mexutils_get_char_struct(UECFG, "Hopping");
  if (tmp) {
    if (!strcmp(tmp, "Group")) {
      pusch_cfg.hopping_method = HOPPING_GROUP;
    } else if (!strcmp(tmp, "Sequence")) {
      pusch_cfg.hopping_method = HOPPING_SEQUENCE;
    } else {
      pusch_cfg.hopping_method = HOPPING_OFF;
    }
    mxFree(tmp);    
  } else {
    pusch_cfg.hopping_method = HOPPING_OFF;
  }
  
  
  if (mexutils_read_uint32_struct(UECFG, "SeqGroup", &pusch_cfg.common.delta_ss)) {
    pusch_cfg.common.delta_ss = 0; 
  }
  if (mexutils_read_uint32_struct(UECFG, "CyclicShift", &pusch_cfg.common.cyclic_shift)) {
    pusch_cfg.common.cyclic_shift = 0; 
  }
  float *prbset; 
  mxArray *p; 
  p = mxGetField(PUSCHCFG, 0, "PRBSet");
  if (!p) {
    mexErrMsgTxt("Error field PRBSet not found in PUSCH config\n");
    return;
  } 
  pusch_cfg.nof_prb = mexutils_read_f(p, &prbset); 
  
  if (mexutils_read_uint32_struct(PUSCHCFG, "DynCyclicShift", &pusch_cfg.common.cyclic_shift_for_drms)) {
    pusch_cfg.common.cyclic_shift_for_drms = 0; 
    pusch_cfg.common.en_drms_2 = false; 
  } else {
    pusch_cfg.common.en_drms_2 = true; 
  }
  
  pusch_cfg.beta_pusch = 1.0; 

  if (refsignal_ul_init(&refs, cell)) {
    mexErrMsgTxt("Error initiating refsignal_ul\n");
    return;
  }

  mexPrintf("nof_prb: %d, ",pusch_cfg.nof_prb);
  mexPrintf("cyclic_shift: %d, ",pusch_cfg.common.cyclic_shift);
  mexPrintf("cyclic_shift_for_drms: %d, ",pusch_cfg.common.cyclic_shift_for_drms);
  mexPrintf("delta_ss: %d, ",pusch_cfg.common.delta_ss);
  mexPrintf("hopping_method: %d\n, ",pusch_cfg.hopping_method);
  
  cf_t *signal = vec_malloc(2*RE_X_RB*pusch_cfg.nof_prb*sizeof(cf_t));
  if (!signal) {
    perror("malloc");
    return;
  }
  cf_t *sf_symbols = vec_malloc(SF_LEN_RE(cell.nof_prb, cell.cp)*sizeof(cf_t));
  if (!sf_symbols) {
    perror("malloc");
    return;
  }
  bzero(sf_symbols, SF_LEN_RE(cell.nof_prb, cell.cp)*sizeof(cf_t));
  for (uint32_t i=0;i<2;i++) {
    //mexPrintf("Generating DRMS for ns=%d, nof_prb=%d\n", 2*sf_idx+i,pusch_cfg.nof_prb);
    refsignal_dmrs_pusch_gen(&refs, &pusch_cfg, 2*sf_idx+i, &signal[i*RE_X_RB*pusch_cfg.nof_prb]);    
  }
  for (uint32_t i=0;i<2;i++) {
    refsignal_drms_pusch_put(&refs, &pusch_cfg, &signal[i*RE_X_RB*pusch_cfg.nof_prb], i, prbset[0], sf_symbols);                
  }
  if (nlhs >= 1) {
    mexutils_write_cf(sf_symbols, &plhs[0], SF_LEN_RE(cell.nof_prb, cell.cp), 1);  
  }

  refsignal_ul_free(&refs);  
  free(signal);
  free(prbset);

  return;
}

