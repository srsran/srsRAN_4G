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
#define INPUT   prhs[1]
#define NOF_INPUTS 2


void help()
{
  mexErrMsgTxt
    ("[offset,corr] = liblte_pss(enbConfig, inputSignal)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  lte_cell_t cell; 
  pss_synch_t pss; 
  cf_t *input_symbols;
  int frame_len; 
  
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
    
  if (mexutils_read_cell(ENBCFG, &cell)) {
    help();
    return;
  }
  
  /** Allocate input buffers */
  frame_len = mexutils_read_cf(INPUT, &input_symbols);
  if (frame_len < 0) {
    mexErrMsgTxt("Error reading input symbols\n");
    return;
  }
  
  if (pss_synch_init_fft(&pss, frame_len, lte_symbol_sz(cell.nof_prb))) {
    fprintf(stderr, "Error initiating PSS\n");
    exit(-1);
  }
  if (pss_synch_set_N_id_2(&pss, cell.id%2)) {
    fprintf(stderr, "Error setting N_id_2=%d\n",cell.id%2);
    exit(-1);
  }
      
  int peak_idx = pss_synch_find_pss(&pss, input_symbols, NULL);
  
  if (nlhs >= 1) { 
    plhs[0] = mxCreateLogicalScalar(peak_idx);
  }
  if (nlhs >= 2) {
    mexutils_write_cf(pss.conv_output, &plhs[1], frame_len, 1);  
  }
    
  pss_synch_free(&pss);
  free(input_symbols);

  return;
}

