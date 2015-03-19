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
#define OUTLEN     prhs[2]
#define TRBLKIN    prhs[3]
#define NOF_INPUTS 4

void help()
{
  mexErrMsgTxt
    ("[cwout] = srslte_dlsch_encode(ue, chs, outlen, trblkin)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  srslte_sch_t dlsch;
  uint8_t *trblkin;
  srslte_ra_mcs_t mcs;
  srslte_ra_dl_alloc_t prb_alloc;
  srslte_harq_t harq_process;
  uint32_t rv;

  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
      
  if (srslte_sch_init(&dlsch)) {
    mexErrMsgTxt("Error initiating DL-SCH\n");
    return;
  }
  srslte_cell_t cell;
  cell.nof_prb = 100;
  cell.id=1;
  if (srslte_harq_init(&harq_process, cell)) {
    mexErrMsgTxt("Error initiating HARQ\n");
    return;
  }
 
  mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin);
  if (mcs.tbs == 0) {
    mexErrMsgTxt("Error trblklen is zero\n");
    return;
  }
  
  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &rv)) {
    mexErrMsgTxt("Field RV not found in dlsch config\n");
    return;
  }

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
      
  if (srslte_harq_setup_dl(&harq_process, mcs, rv, 0, &prb_alloc)) {
    mexErrMsgTxt("Error configuring HARQ process\n");
    return;
  }
  harq_process.nof_bits = mxGetScalar(OUTLEN); 
    
  uint8_t *e_bits = srslte_vec_malloc(harq_process.nof_bits* sizeof(uint8_t));
  if (!e_bits) {
    return;
  }

  if (srslte_dlsch_encode(&dlsch, &harq_process, trblkin, e_bits)) {
    mexErrMsgTxt("Error encoding TB\n");
    return;
  }
  
  if (nlhs >= 1) {
    mexutils_write_uint8(e_bits, &plhs[0], harq_process.nof_bits, 1);  
  }
  
  srslte_sch_free(&dlsch);
  
  free(trblkin);
  free(e_bits);
  
  return;
}

