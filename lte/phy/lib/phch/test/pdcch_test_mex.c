/* 
 * Copyright (c) 2012, Ismael Gomez-Miguelez <ismael.gomez@tsc.upc.edu>.
 * This file is part of ALOE++ (http://flexnets.upc.edu/)
 * 
 * ALOE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ALOE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALOE++.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "liblte/phy/phy.h"
#include "liblte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define ENBCFG  prhs[0]
#define RNTI    prhs[1]
#define INPUT   prhs[2]
#define NOF_INPUTS 3

#define MAX_CANDIDATES 64


dci_format_t ue_formats[] = {Format1A,Format1}; // Format1B should go here also
const uint32_t nof_ue_formats = 2; 

dci_format_t common_formats[] = {Format1A,Format1C};
const uint32_t nof_common_formats = 2; 


void help()
{
  mexErrMsgTxt
    ("[decoded_ok, llr, rm, bits] = liblte_pdcch(enbConfig, RNTI, inputSignal)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  int i;
  lte_cell_t cell; 
  pdcch_t pdcch;
  regs_t regs;
  dci_location_t locations[MAX_CANDIDATES];
  uint32_t cfi, sf_idx; 
  uint16_t rnti; 
  cf_t *input_symbols;
  int nof_re; 
  uint32_t nof_formats; 
  dci_format_t *formats = NULL; 
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  if (mexutils_read_uint32_struct(ENBCFG, "CFI", &cfi)) {
    help();
    return;
  }
  if (mexutils_read_uint32_struct(ENBCFG, "NSubframe", &sf_idx)) {
    help();
    return;
  }
  
  rnti = (uint16_t) mxGetScalar(RNTI);
    
  if (regs_init(&regs, cell)) {
    mexErrMsgTxt("Error initiating regs\n");
    return;
  }
  
  if (regs_set_cfi(&regs, cfi)) {
    fprintf(stderr, "Error setting CFI\n");
    exit(-1);
  }
  
  if (pdcch_init(&pdcch, &regs, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
    return;
  }
      
  /** Allocate input buffers */
  nof_re = mexutils_read_cf(INPUT, &input_symbols);
  if (nof_re < 0) {
    mexErrMsgTxt("Error reading input symbols\n");
    return;
  }
  
  // Set Channel estimates to 1.0 (ignore fading) 
  cf_t *ce[MAX_PORTS];
  for (i=0;i<cell.nof_ports;i++) {
    ce[i] = vec_malloc(nof_re * sizeof(cf_t));
    for (int j=0;j<nof_re;j++) {
      ce[i][j] = 1.0; 
    }
  }
  
  pdcch_extract_llr(&pdcch, input_symbols, ce, 0, sf_idx, cfi);
  
  
  uint32_t nof_locations;
  if (rnti == SIRNTI) {
    nof_locations = pdcch_common_locations(&pdcch, locations, MAX_CANDIDATES, cfi);
    formats = common_formats;
    nof_formats = nof_common_formats;
  } else {
    nof_locations = pdcch_ue_locations(&pdcch, locations, MAX_CANDIDATES, sf_idx, cfi, rnti); 
    formats = ue_formats; 
    nof_formats = nof_ue_formats;
  }
  uint16_t crc_rem;   
  dci_msg_t dci_msg; 
  
  for (int f=0;f<nof_formats;f++) {
    for (i=0;i<nof_locations && crc_rem != rnti;i++) {
      if (pdcch_decode_msg(&pdcch, &dci_msg, &locations[i], formats[f], &crc_rem)) {
        fprintf(stderr, "Error decoding DCI msg\n");
        return;
      }
/*      mexPrintf("Trying location (%d,%d), %s, CRC: 0x%x\n",
                locations[i].ncce, locations[i].L, dci_format_string(formats[f]), crc_rem);
*/             
    }
  }    
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(crc_rem == rnti);
  }
  int nof_bits = (regs_pdcch_nregs(&regs, cfi) / 9) * 72;
  if (nlhs >= 2) {
    mexutils_write_f(pdcch.pdcch_llr, &plhs[1], nof_bits, 1);  
  }
  if (nlhs >= 3) {
    mexutils_write_f(pdcch.pdcch_rm_f, &plhs[2], 3*(dci_msg.nof_bits+16), 1);  
  }
  
  pdcch_free(&pdcch);
  regs_free(&regs);
  for (i=0;i<cell.nof_ports;i++) {
    free(ce[i]);
  }
  free(input_symbols);

  return;
}

