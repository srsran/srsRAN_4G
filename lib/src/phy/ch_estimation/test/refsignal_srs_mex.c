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
#define SRSCFG   prhs[1]
#define NOF_INPUTS 2

void help()
{
  mexErrMsgTxt
    ("[sym, subframe]=srslte_refsignal_srs(ue, chs)\n\n");
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

  uint32_t sf_idx = 0; 
  if (mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx)) {
    mexErrMsgTxt("Field NSubframe not found in UE config\n");
    return;
  }
  uint32_t nf = 0;
  if (mexutils_read_uint32_struct(UECFG, "NFrame", &nf)) {
    mexErrMsgTxt("Field NFrame not found in UE config\n");
    return;
  }
  uint32_t tti = nf*10+sf_idx;
  
  srslte_refsignal_srs_cfg_t srs_cfg; 
  bzero(&srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  
  if (mexutils_read_uint32_struct(SRSCFG, "BWConfig", &srs_cfg.bw_cfg)) {
    mexErrMsgTxt("Field BWConfig not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "BW", &srs_cfg.B)) {
    mexErrMsgTxt("Field BW not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "ConfigIdx", &srs_cfg.I_srs)) {
    mexErrMsgTxt("Field ConfigIdx not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "FreqPosition", &srs_cfg.n_rrc)) {
    mexErrMsgTxt("Field FreqPosition not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "HoppingBW", &srs_cfg.b_hop)) {
    mexErrMsgTxt("Field HoppingBW not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "TxComb", &srs_cfg.k_tc)) {
    mexErrMsgTxt("Field TxComb not found in SRSCFG\n");
    return;
  }
  if (mexutils_read_uint32_struct(SRSCFG, "CyclicShift", &srs_cfg.n_srs)) {
    mexErrMsgTxt("Field CyclicShift not found in SRSCFG\n");
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
  
  cf_t *r_srs = srslte_vec_malloc(sizeof(cf_t) * cell.nof_prb * 12); 
  if (!r_srs) {
    return; 
  }
  bzero(r_srs, cell.nof_prb * 12 * sizeof(cf_t));
  
  cf_t *sf_symbols = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp)); 
  if (!sf_symbols) {
    return; 
  }
  bzero(sf_symbols, SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  
  srslte_refsignal_ul_t refsignal;
  if (srslte_refsignal_ul_init(&refsignal, cell)) {
    mexErrMsgTxt("Error initiating UL refsignal\n");
    return;
  }
  srslte_refsignal_dmrs_pusch_cfg_t pusch_cfg; 
  pusch_cfg.group_hopping_en = group_hopping_en;
  pusch_cfg.sequence_hopping_en = false; 
  srslte_refsignal_ul_set_cfg(&refsignal, &pusch_cfg, NULL, &srs_cfg);

  if (srslte_refsignal_srs_gen(&refsignal, sf_idx, r_srs)) {
    mexErrMsgTxt("Error generating SRS\n");    
    return;
  }
  
  if (srslte_refsignal_srs_put(&refsignal, tti, r_srs, sf_symbols)) {
    mexErrMsgTxt("Error allocating SRS\n");    
    return;
  }
  
  if (nlhs >= 1) {
    uint32_t M_sc = srslte_refsignal_srs_M_sc(&refsignal); ;
    mexutils_write_cf(r_srs, &plhs[0], M_sc, 1);  
  }

  if (nlhs >= 2) {    
    mexutils_write_cf(sf_symbols, &plhs[1], SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp), 1);
  }

  srslte_refsignal_ul_free(&refsignal);  
  free(sf_symbols);
  free(r_srs);
  
  return;
}

