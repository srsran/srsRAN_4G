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



#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include "srslte/ch_estimation/chest_common.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/convolution.h"

void srslte_chest_set_triangle_filter(float *fil, int filter_len) 
{
  for (int i=0;i<filter_len/2;i++) {
    fil[i] = i+1;
    fil[i+filter_len/2+1]=filter_len/2-i;
  }
  fil[filter_len/2]=filter_len/2+1;
  
  float s=0;
  for (int i=0;i<filter_len;i++) {
    s+=fil[i];
  }
  for (int i=0;i<filter_len;i++) {
    fil[i]/=s;
  }
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
float srslte_chest_estimate_noise_pilots(cf_t *noisy, cf_t *noiseless, cf_t *noise_vec, uint32_t nof_pilots) 
{
  /* Substract noisy pilot estimates */
  srslte_vec_sub_ccc(noiseless, noisy, noise_vec, nof_pilots);  
    
  /* Compute average power */
  float power = srslte_vec_avg_power_cf(noise_vec, nof_pilots);
  return power; 
}

void srslte_chest_set_smooth_filter3_coeff(float *smooth_filter, float w)
{
  smooth_filter[0] = w; 
  smooth_filter[2] = w; 
  smooth_filter[1] = 1-2*w; 
}

void srslte_chest_average_pilots(cf_t *input, cf_t *output, float *filter, 
                                 uint32_t nof_ref, uint32_t nof_symbols, uint32_t filter_len) 
{
  for (int l=0;l<nof_symbols;l++) {
    srslte_conv_same_cf(&input[l*nof_ref], filter, &output[l*nof_ref], nof_ref, filter_len);    
  }
}

