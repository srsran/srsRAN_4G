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
#ifdef UNDEF_BOOL
#undef bool
#endif
#include "mex.h"


/** MEX function to be called from MATLAB to test the channel estimator 
 * 
 * [estChannel] = liblte_chest(cell_id, nof_ports, inputSignal, (optional) sf_idx)
 * 
 * Returns a matrix of size equal to the inputSignal matrix with the channel estimates 
 * for each resource element in inputSignal. The inputSignal matrix is the received Grid
 * of size nof_resource_elements x nof_ofdm_symbols_in_subframe. 
 * 
 * The sf_idx is the subframe index only used if inputSignal is 1 subframe length. 
 * 
 */

#define CELLID  prhs[0]
#define PORTS   prhs[1]
#define INPUT   prhs[2]
#define NOF_INPUTS 3
#define SFIDX   prhs[3]
#define FREQ_FILTER   prhs[3]
#define TIME_FILTER   prhs[4]

void help()
{
  mexErrMsgTxt
    ("[estChannel] = liblte_chest(cell_id, nof_ports, inputSignal,[sf_idx|freq_filter], [time_filter])\n\n"
     " Returns a matrix of size equal to the inputSignal matrix with the channel estimates\n "
     "for each resource element in inputSignal. The inputSignal matrix is the received Grid\n"
     "of size nof_resource_elements x nof_ofdm_symbols.\n"
     "The sf_idx is the subframe index only used if inputSignal is 1 subframe length.\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  int i;
  lte_cell_t cell; 
  chest_dl_t chest;
  cf_t *input_signal; 
  cf_t *ce[MAX_PORTS]; 
  double *outr0, *outi0, *outr1, *outi1;
  float noiseAverage[10];
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }

  if (!mxIsDouble(CELLID) && mxGetN(CELLID) != 1 && 
      !mxIsDouble(PORTS) && mxGetN(PORTS) != 1 && 
      mxGetM(CELLID) != 1) {
    help();
    return;
  }

  cell.id = (uint32_t) *((double*) mxGetPr(CELLID));
  cell.nof_prb = mxGetM(INPUT)/RE_X_RB;
  cell.nof_ports = (uint32_t) *((double*) mxGetPr(PORTS)); 
  if ((mxGetN(INPUT)%14) == 0) {
    cell.cp = CPNORM;    
  } else if ((mxGetN(INPUT)%12)!=0) {
    cell.cp = CPEXT;
  } else {
    mexErrMsgTxt("Invalid number of symbols\n");
    help();
    return;
  }
  
  if (chest_dl_init(&chest, cell)) {
    mexErrMsgTxt("Error initiating channel estimator\n");
    return;
  }
  
  int nsubframes;
  if (cell.cp == CPNORM) {
    nsubframes = mxGetN(INPUT)/14;
  } else {
    nsubframes = mxGetN(INPUT)/12;
  }
  
  uint32_t sf_idx=0; 
  if (nsubframes == 1) {
    if (nrhs != NOF_INPUTS+1) {
      mexErrMsgTxt("Received 1 subframe. Need to provide subframe index.\n");
      help();
      return;
    }
    sf_idx = (uint32_t) *((double*) mxGetPr(SFIDX));
  }
  
  uint32_t filter_len = 0;
  float *filter; 
  double *f; 
  
  if (nrhs > NOF_INPUTS && nsubframes == 10) {
    if (nrhs >= NOF_INPUTS + 1) {
      filter_len = mxGetNumberOfElements(FREQ_FILTER);
      filter = malloc(sizeof(float) * filter_len);
      f = (double*) mxGetPr(FREQ_FILTER);
      for (i=0;i<filter_len;i++) {
        filter[i] = (float) f[i];
      }
      chest_dl_set_filter_freq(&chest, filter, filter_len);
    }
    if (nrhs >= NOF_INPUTS + 2) {
      filter_len = mxGetNumberOfElements(TIME_FILTER);
      filter = malloc(sizeof(float) * filter_len);
      f = (double*) mxGetPr(TIME_FILTER);
      for (i=0;i<filter_len;i++) {
        filter[i] = (float) f[i];
      }
      chest_dl_set_filter_time(&chest, filter, filter_len);
    }
  }

  double *inr=(double *)mxGetPr(INPUT);
  double *ini=(double *)mxGetPi(INPUT);
  
  /** Allocate input buffers */
  int nof_re = 2*CP_NSYMB(cell.cp)*cell.nof_prb*RE_X_RB;
  for (i=0;i<MAX_PORTS;i++) {
    ce[i] = vec_malloc(nof_re * sizeof(cf_t));
  }
  input_signal = vec_malloc(nof_re * sizeof(cf_t));
   
  /* Create output values */
  if (nlhs >= 1) {
    plhs[0] = mxCreateDoubleMatrix(1,nof_re * nsubframes, mxCOMPLEX);
    outr0 = mxGetPr(plhs[0]);
    outi0 = mxGetPi(plhs[0]);
  }  
  if (nlhs >= 2) {
    plhs[1] = mxCreateDoubleMatrix(REFSIGNAL_MAX_NUM_SF(cell.nof_prb)*nsubframes, cell.nof_ports, mxCOMPLEX);
    outr1 = mxGetPr(plhs[1]);
    outi1 = mxGetPi(plhs[1]);
  }
  
  for (int sf=0;sf<nsubframes;sf++) {
    /* Convert input to C complex type */
    for (i=0;i<nof_re;i++) {
      __real__ input_signal[i] = (float) *inr;
      if (ini) {
        __imag__ input_signal[i] = (float) *ini;
      }
      inr++;
      ini++;
    }
    
    if (nsubframes != 1) {
      sf_idx = sf%10;
    }
    
    /* Loop through the 10 subframes */
    if (chest_dl_estimate(&chest, input_signal, ce, sf_idx)) {
      mexErrMsgTxt("Error running channel estimator\n");
      return;
    }
    
    noiseAverage[sf]=chest_dl_get_noise_estimate(&chest);
    
    if (nlhs >= 1) { 
      for (i=0;i<nof_re;i++) {      
        *outr0 = (double) crealf(ce[0][i]);
        if (outi0) {
          *outi0 = (double) cimagf(ce[0][i]);
        }
        outr0++;
        outi0++;
      } 
    }
    if (nlhs >= 2) {    
      for (int j=0;j<cell.nof_ports;j++) {
        for (i=0;i<REFSIGNAL_NUM_SF(cell.nof_prb,j);i++) {
          *outr1 = (double) crealf(chest.pilot_estimates_average[j][i]);
          if (outi1) {
            *outi1 = (double) cimagf(chest.pilot_estimates_average[j][i]);
          }
          outr1++;
          outi1++;
        }
      }    
    }
  }
  
  if (nlhs >= 3) {
    plhs[2] = mxCreateDoubleMatrix(1, 1, mxREAL);
    outr1 = mxGetPr(plhs[2]);
    *outr1 = vec_acc_ff(noiseAverage,10)/10;
  }
    
 
  return;
}

