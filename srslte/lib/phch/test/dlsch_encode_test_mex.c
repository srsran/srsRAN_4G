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
  srslte_pdsch_cfg_t cfg; 
  srslte_softbuffer_tx_t softbuffer; 

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
  srslte_verbose = SRSLTE_VERBOSE_NONE; 
  
  uint8_t *trblkin_bits = NULL;
  cfg.grant.mcs.tbs = mexutils_read_uint8(TRBLKIN, &trblkin_bits);
  if (cfg.grant.mcs.tbs == 0) {
    mexErrMsgTxt("Error trblklen is zero\n");
    return;
  }
  uint8_t *trblkin = srslte_vec_malloc(cfg.grant.mcs.tbs/8);
  srslte_bit_pack_vector(trblkin_bits, trblkin, cfg.grant.mcs.tbs);
  free(trblkin_bits);

  if (mexutils_read_uint32_struct(PUSCHCFG, "RV", &cfg.rv)) {
    mexErrMsgTxt("Field RV not found in dlsch config\n");
    return;
  }

  char *mod_str = mexutils_get_char_struct(PUSCHCFG, "Modulation");
  
  if (!strcmp(mod_str, "QPSK")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_QPSK;
    cfg.grant.Qm = 2; 
  } else if (!strcmp(mod_str, "16QAM")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_16QAM;
    cfg.grant.Qm = 4; 
  } else if (!strcmp(mod_str, "64QAM")) {
    cfg.grant.mcs.mod = SRSLTE_MOD_64QAM;
    cfg.grant.Qm = 6; 
  } else {
   mexErrMsgTxt("Unknown modulation\n");
   return;
  }

  mxFree(mod_str);

  if (srslte_softbuffer_tx_init(&softbuffer, cell.nof_prb)) {
    mexErrMsgTxt("Error initiating DL-SCH soft buffer\n");
    return; 
  }

  cfg.nbits.nof_bits = mxGetScalar(OUTLEN); 
  uint8_t *e_bits = srslte_vec_malloc(cfg.nbits.nof_bits * sizeof(uint8_t));
  if (!e_bits) {
    return;
  }
  if (srslte_cbsegm(&cfg.cb_segm, cfg.grant.mcs.tbs)) {
    mexErrMsgTxt("Error computing CB segmentation\n");
    return; 
  }
  uint32_t tmp_rv=cfg.rv;
  if (tmp_rv) {
    cfg.rv = 0; 
    if (srslte_dlsch_encode(&dlsch, &cfg, &softbuffer, trblkin, e_bits)) {
      mexErrMsgTxt("Error encoding TB\n");
      return;
    }
    cfg.rv = tmp_rv;
  }
  if (srslte_dlsch_encode(&dlsch, &cfg, &softbuffer, trblkin, e_bits)) {
    mexErrMsgTxt("Error encoding TB\n");
    return;
  }
  uint8_t *e_bits_unpacked = srslte_vec_malloc(cfg.nbits.nof_bits * sizeof(uint8_t));
  if (!e_bits_unpacked) {
    return;
  }
  srslte_bit_unpack_vector(e_bits, e_bits_unpacked, cfg.nbits.nof_bits);
  if (nlhs >= 1) {
    mexutils_write_uint8(e_bits_unpacked, &plhs[0], cfg.nbits.nof_bits, 1);  
  }
  
  srslte_sch_free(&dlsch);
  
  free(trblkin);
  free(e_bits);
  free(e_bits_unpacked);
  
  return;
}

