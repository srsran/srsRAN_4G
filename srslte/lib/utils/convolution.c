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


#include <stdlib.h>
#include <string.h>

#include "srslte/dft/dft.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/convolution.h"


int srslte_conv_fft_cc_init(srslte_conv_fft_cc_t *q, uint32_t input_len, uint32_t filter_len) {
  q->input_len = input_len;
  q->filter_len = filter_len;
  q->output_len = input_len+filter_len;
  q->input_fft = srslte_vec_malloc(sizeof(cf_t)*q->output_len);
  q->filter_fft = srslte_vec_malloc(sizeof(cf_t)*q->output_len);
  q->output_fft = srslte_vec_malloc(sizeof(cf_t)*q->output_len);
  if (!q->input_fft || !q->filter_fft || !q->output_fft) {
    return SRSLTE_ERROR;
  }
  if (srslte_dft_plan(&q->input_plan,q->output_len,SRSLTE_DFT_FORWARD,SRSLTE_DFT_COMPLEX)) {
    fprintf(stderr, "Error initiating input plan\n");
    return SRSLTE_ERROR;
  }
  if (srslte_dft_plan(&q->filter_plan,q->output_len,SRSLTE_DFT_FORWARD,SRSLTE_DFT_COMPLEX)) {
    fprintf(stderr, "Error initiating filter plan\n");
    return SRSLTE_ERROR;
  }
  if (srslte_dft_plan(&q->output_plan,q->output_len,SRSLTE_DFT_BACKWARD,SRSLTE_DFT_COMPLEX)) {
    fprintf(stderr, "Error initiating output plan\n");
    return SRSLTE_ERROR;
  }
  srslte_dft_plan_set_norm(&q->input_plan, true);
  srslte_dft_plan_set_norm(&q->filter_plan, true);
  srslte_dft_plan_set_norm(&q->output_plan, false);
  return SRSLTE_SUCCESS;
}

void srslte_conv_fft_cc_free(srslte_conv_fft_cc_t *q) {
  if (q->input_fft) {
    free(q->input_fft);
  }
  if (q->filter_fft) {
    free(q->filter_fft);
  }
  if (q->output_fft) {
    free(q->output_fft);
  }
  srslte_dft_plan_free(&q->input_plan);
  srslte_dft_plan_free(&q->filter_plan);
  srslte_dft_plan_free(&q->output_plan);
  
  bzero(q, sizeof(srslte_conv_fft_cc_t));

}

uint32_t srslte_conv_fft_cc_run(srslte_conv_fft_cc_t *q, cf_t *input, cf_t *filter, cf_t *output) {

  srslte_dft_run_c(&q->input_plan, input, q->input_fft);
  srslte_dft_run_c(&q->filter_plan, filter, q->filter_fft);

  srslte_vec_prod_ccc(q->input_fft,q->filter_fft,q->output_fft,q->output_len);

  srslte_dft_run_c(&q->output_plan, q->output_fft, output);

  return q->output_len-1;

}

uint32_t srslte_conv_cc(cf_t *input, cf_t *filter, cf_t *output, uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 

  for (i=0;i<M;i++) {
    output[i]=srslte_vec_dot_prod_ccc(&input[i],&filter[i],i);
  }
  for (;i<M+N-1;i++) {
    output[i] = srslte_vec_dot_prod_ccc(&input[i-M], filter, M);
  }
  return M+N-1;
}

/* Centered convolution. Returns the same number of input elements. Equivalent to conv(x,h,'same') in matlab. 
 * y(n)=sum_i x(n+i-M/2)*h(i) for n=1..N with N input samples and M filter len 
 */
uint32_t srslte_conv_same_cc(cf_t *input, cf_t *filter, cf_t *output, uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 
  
  for (i=0;i<M/2;i++) {
    output[i]=srslte_vec_dot_prod_ccc(&input[i],&filter[M/2-i],M-M/2+i);
  }
  for (;i<N-M/2;i++) {
    output[i]=srslte_vec_dot_prod_ccc(&input[i-M/2],filter,M);
  }
  for (;i<N;i++) {
    output[i]=srslte_vec_dot_prod_ccc(&input[i-M/2],filter,N-i+M/2);    
  }
  return N;
}


#define conv_same_extrapolates_extremes

#ifdef conv_same_extrapolates_extremes
uint32_t srslte_conv_same_cf(cf_t *input, float *filter, cf_t *output, 
                      uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 
  cf_t first[filter_len+filter_len/2]; 
  cf_t last[filter_len+filter_len/2]; 
  
  for (i=0;i<M+M/2;i++) {
    if (i<M/2) {
      first[i] = (2+M/2-i)*input[1]-(1+M/2-i)*input[0]; 
    } else {
      first[i] = input[i-M/2]; 
    }
  }

  for (i=0;i<M+M/2;i++) {
    if (i>=M-1) {
      last[i] = (2+i-M/2)*input[N-1]-(1+i-M/2)*input[N-2];
    } else {
      last[i] = input[N-M+i+1]; 
    }
  }

  for (i=0;i<M/2;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&first[i],filter,M);
  }
  
  for (;i<N-M/2;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&input[i-M/2],filter,M);
  }
  int j=0;
  for (;i<N;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&last[j++],filter,M);
  }
  return N;
}
#else

uint32_t srslte_conv_same_cf(cf_t *input, float *filter, cf_t *output, 
                      uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 
  
  for (i=0;i<M/2;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&input[i],&filter[M/2-i],M-M/2+i);
  }
  for (;i<N-M/2;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&input[i-M/2],filter,M);
  }
  for (;i<N;i++) {
    output[i]=srslte_vec_dot_prod_cfc(&input[i-M/2],filter,N-i+M/2);    
  }
  return N;
}

#endif
