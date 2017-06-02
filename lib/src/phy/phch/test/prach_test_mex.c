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
#define FORCE_POWER2_FFT
#include "srslte/srslte.h"
#include "srslte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define UECFG     prhs[0]
#define PRACHCFG  prhs[1]
#define NOF_INPUTS 2

void help()
{
  mexErrMsgTxt
    ("waveform = srslte_prach(ueConfig, prachConfig)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
 
  if (nrhs != NOF_INPUTS) {
    help();
    return;
  }
   
  uint32_t n_ul_rb = 0; 
  if (mexutils_read_uint32_struct(UECFG, "NULRB", &n_ul_rb)) {
    mexErrMsgTxt("Field NULRB not found in UE config\n");
    return;
  }  
  int r = srslte_symbol_sz(n_ul_rb);
  if (r < 0) {
    mexErrMsgTxt("Invalid NULRB\n");
    return;
  }
  uint32_t N_ifft_ul = (uint32_t) r; 
  
  uint32_t sf_idx = 0; 
  mexutils_read_uint32_struct(UECFG, "NSubframe", &sf_idx);
  uint32_t nframe = 0; 
  mexutils_read_uint32_struct(UECFG, "NFrame", &nframe);

  uint32_t preamble_format = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "Format", &preamble_format);
  uint32_t root_seq_idx = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "SeqIdx", &root_seq_idx);
  uint32_t seq_idx = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "PreambleIdx", &seq_idx);
  uint32_t zero_corr_zone = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "CyclicShiftIdx", &zero_corr_zone);
  uint32_t high_speed_flag = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "HighSpeed", &high_speed_flag);
  uint32_t frequency_offset = 0; 
  mexutils_read_uint32_struct(PRACHCFG, "FreqOffset", &frequency_offset);

  srslte_prach_t prach; 
  if (srslte_prach_init(&prach, N_ifft_ul, preamble_format*16, root_seq_idx, high_speed_flag, zero_corr_zone)) {
    mexErrMsgTxt("Error initiating PRACH\n");
    return;
  }

  uint32_t nof_samples = srslte_sampling_freq_hz(n_ul_rb) * 0.001;
  
  cf_t *signal = srslte_vec_malloc(sizeof(cf_t) * nof_samples);
  if (!signal) {
    mexErrMsgTxt("malloc");
    return;
  }
  bzero(signal, sizeof(cf_t) * nof_samples);
  if (srslte_prach_gen(&prach, seq_idx, frequency_offset, signal)) {
    mexErrMsgTxt("Error generating PRACH\n");
    return; 
  }

  srslte_vec_sc_prod_cfc(signal, 1.0/sqrtf(N_ifft_ul), signal, nof_samples);              
  
  if (nlhs >= 0) {
    mexutils_write_cf(signal, &plhs[0], nof_samples, 1);  
  }
  
  free(signal);
  
  srslte_prach_free(&prach);
  
  return;
}

