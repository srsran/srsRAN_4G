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

#include "srslte/config.h"

#include "srslte/ch_estimation/chest_dl.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/convolution.h"

//#define DEFAULT_FILTER_LEN 3

#ifdef DEFAULT_FILTER_LEN 
static void set_default_filter(srslte_chest_dl_t *q, int filter_len) {
  
  float fil[SRSLTE_CHEST_DL_MAX_SMOOTH_FIL_LEN]; 

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

  srslte_chest_dl_set_smooth_filter(q, fil, filter_len);
}
#endif

/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the srslte_refsignal_t object for creating the LTE CSR signal.  
*/

int srslte_chest_dl_init(srslte_chest_dl_t *q, srslte_cell_t cell) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL &&
      srslte_cell_isvalid(&cell)) 
  {
    bzero(q, sizeof(srslte_chest_dl_t));
    
    ret = srslte_refsignal_cs_init(&q->csr_signal, cell); 
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    q->tmp_noise = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }      
    q->pilot_estimates_average = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }      
    q->pilot_recv_signal = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }
    
    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, SRSLTE_NRE*cell.nof_prb)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit; 
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin, 2*cell.nof_prb, SRSLTE_NRE/2)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit; 
    }
    
    if (srslte_pss_generate(q->pss_signal, cell.id%3)) {
      fprintf(stderr, "Error initializing PSS signal for noise estimation\n");
      goto clean_exit;
    }
    
    q->noise_alg = SRSLTE_NOISE_ALG_REFS; 
    
    q->smooth_filter_len = 3; 
    srslte_chest_dl_set_smooth_filter3_coeff(q, 0.1);
    
    q->cell = cell; 
  }
  
  ret = SRSLTE_SUCCESS;
  
clean_exit:
  if (ret != SRSLTE_SUCCESS) {
      srslte_chest_dl_free(q);
  }
  return ret; 
}

void srslte_chest_dl_free(srslte_chest_dl_t *q) 
{
  srslte_refsignal_cs_free(&q->csr_signal);

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  srslte_interp_linear_free(&q->srslte_interp_lin);
  
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }      
  if (q->pilot_estimates_average) {
    free(q->pilot_estimates_average);
  }      
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
  bzero(q, sizeof(srslte_chest_dl_t));
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srslte_chest_dl_t *q, uint32_t port_id) 
{
  int nref=SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id);
  /* Substract noisy pilot estimates */
  srslte_vec_sub_ccc(q->pilot_estimates_average, q->pilot_estimates, q->tmp_noise, nref);  
  
#ifdef FREQ_SEL_SNR
  /* Compute frequency-selective SNR */
  srslte_vec_abs_square_cf(q->tmp_noise, q->snr_vector, nref);
  srslte_vec_abs_square_cf(q->pilot_estimates, q->pilot_power, nref);
  srslte_vec_div_fff(q->pilot_power, q->snr_vector, q->snr_vector, nref);
  
  srslte_vec_fprint_f(stdout, q->snr_vector, nref);
#endif
  
  /* Compute average power. Normalized for filter len 3 using matlab */
  float norm  = 1;
  if (q->smooth_filter_len == 3) {
    float a = q->smooth_filter[0];
    float norm3 = 6.143*a*a+0.04859*a-0.002774;
    norm /= norm3; 
  }
  float power = norm*q->cell.nof_ports*srslte_vec_avg_power_cf(q->tmp_noise, nref);
  return power; 
}

static float estimate_noise_pss(srslte_chest_dl_t *q, cf_t *input, cf_t *ce) 
{
  /* Get PSS from received signal */
  srslte_pss_get_slot(input, q->tmp_pss, q->cell.nof_prb, q->cell.cp);
    
  /* Get channel estimates for PSS position */
  srslte_pss_get_slot(ce, q->tmp_pss_noisy, q->cell.nof_prb, q->cell.cp);

  /* Multiply known PSS by channel estimates */
  srslte_vec_prod_ccc(q->tmp_pss_noisy, q->pss_signal, q->tmp_pss_noisy, SRSLTE_PSS_LEN);

  /* Substract received signal */
  srslte_vec_sub_ccc(q->tmp_pss_noisy, q->tmp_pss, q->tmp_pss_noisy, SRSLTE_PSS_LEN);
  
  /* Compute average power */
  float power = q->cell.nof_ports*srslte_vec_avg_power_cf(q->tmp_pss_noisy, SRSLTE_PSS_LEN)/sqrt(2);
  return power; 
}

/* Uses the 5 empty transmitted SC before and after the SSS and PSS sequences for noise estimation */
static float estimate_noise_empty_sc(srslte_chest_dl_t *q, cf_t *input) {
  int k_sss = (SRSLTE_CP_NSYMB(q->cell.cp) - 2) * q->cell.nof_prb * SRSLTE_NRE + q->cell.nof_prb * SRSLTE_NRE / 2 - 31;
  float noise_power = 0; 
  noise_power += srslte_vec_avg_power_cf(&input[k_sss-5], 5); // 5 empty SC before SSS
  noise_power += srslte_vec_avg_power_cf(&input[k_sss+62], 5); // 5 empty SC after SSS
  int k_pss = (SRSLTE_CP_NSYMB(q->cell.cp) - 1) * q->cell.nof_prb * SRSLTE_NRE + q->cell.nof_prb * SRSLTE_NRE / 2 - 31;
  noise_power += srslte_vec_avg_power_cf(&input[k_pss-5], 5); // 5 empty SC before PSS
  noise_power += srslte_vec_avg_power_cf(&input[k_pss+62], 5); // 5 empty SC after PSS
  
  return noise_power; 
}

#define cesymb(i) ce[SRSLTE_RE_IDX(q->cell.nof_prb,i,0)]

static void interpolate_pilots(srslte_chest_dl_t *q, cf_t *pilot_estimates, cf_t *ce, uint32_t port_id) 
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t l; 
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(port_id); 
  
  /* Interpolate in the frequency domain */
  for (l=0;l<nsymbols;l++) {
    uint32_t fidx_offset = srslte_refsignal_cs_fidx(q->cell, l, port_id, 0);    
    srslte_interp_linear_offset(&q->srslte_interp_lin, &pilot_estimates[2*q->cell.nof_prb*l],
                         &ce[srslte_refsignal_cs_nsymbol(l,q->cell.cp, port_id) * q->cell.nof_prb * SRSLTE_NRE], 
                         fidx_offset, SRSLTE_NRE/2-fidx_offset); 
  }
  
  /* Now interpolate in the time domain between symbols */
  if (SRSLTE_CP_ISNORM(q->cell.cp)) {
    if (nsymbols == 4) {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(0), &cesymb(4),  &cesymb(1), 4, 3);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(4), &cesymb(7),  &cesymb(5), 3, 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(8), 4, 3);
      srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(11), &cesymb(12), 4, 2);
    } else {
      srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(8), &cesymb(1), &cesymb(1), &cesymb(0), 7, 1);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(2), 7, 6);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(9), 7, 5);
    }    
  } else {
    if (nsymbols == 4) {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(0), &cesymb(3), &cesymb(1), 3, 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(3), &cesymb(6), &cesymb(4), 3, 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(7), 3, 2);
      srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(9), &cesymb(10), 3, 2);
    } else {
      srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(7), &cesymb(1), &cesymb(1), &cesymb(0), 6, 1);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(2), 6, 5);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(8), 6, 4);
    }    
  }
}

void srslte_chest_dl_set_smooth_filter(srslte_chest_dl_t *q, float *filter, uint32_t filter_len) {
  if (filter_len < SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN) {
    if (filter) {
      memcpy(q->smooth_filter, filter, filter_len*sizeof(float));    
      q->smooth_filter_len = filter_len; 
    } else {
      q->smooth_filter_len = 0; 
    }
  } else {
    fprintf(stderr, "Error setting smoothing filter: filter len exceeds maximum (%d>%d)\n", 
      filter_len, SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN);
  }
}

void srslte_chest_dl_set_noise_alg(srslte_chest_dl_t *q, srslte_chest_dl_noise_alg_t noise_estimation_alg) {
  q->noise_alg = noise_estimation_alg; 
}

void srslte_chest_dl_set_smooth_filter3_coeff(srslte_chest_dl_t* q, float w)
{
  q->smooth_filter_len = 3;
  q->smooth_filter[0] = w; 
  q->smooth_filter[2] = w; 
  q->smooth_filter[1] = 1-2*w; 
}

static void average_pilots(srslte_chest_dl_t *q, cf_t *input, cf_t *output, uint32_t port_id) {
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(port_id); 
  uint32_t nref = 2*q->cell.nof_prb;

  // Average in the frequency domain
  for (int l=0;l<nsymbols;l++) {
    srslte_conv_same_cf(&input[l*nref], q->smooth_filter, &output[l*nref], nref, q->smooth_filter_len);    
  }
}

float srslte_chest_dl_rssi(srslte_chest_dl_t *q, cf_t *input, uint32_t port_id) {
  uint32_t l;
  
  float rssi = 0;
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(port_id);   
  for (l=0;l<nsymbols;l++) {
    cf_t *tmp = &input[srslte_refsignal_cs_nsymbol(l, q->cell.cp, port_id) * q->cell.nof_prb * SRSLTE_NRE];
    rssi += srslte_vec_dot_prod_conj_ccc(tmp, tmp, q->cell.nof_prb * SRSLTE_NRE);    
  }    
  return rssi/nsymbols; 
}

int srslte_chest_dl_estimate_port(srslte_chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id) 
{
  /* Get references from the input signal */
  srslte_refsignal_cs_get_sf(q->cell, port_id, input, q->pilot_recv_signal);
  
  /* Use the known CSR signal to compute Least-squares estimates */
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->csr_signal.pilots[port_id/2][sf_idx], 
              q->pilot_estimates, SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id)); 
  if (ce != NULL) {
    
    /* Smooth estimates (if applicable) and interpolate */
    if (q->smooth_filter_len == 0 || (q->smooth_filter_len == 3 && q->smooth_filter[0] == 0)) {
      interpolate_pilots(q, q->pilot_estimates, ce, port_id);            
    } else {
      average_pilots(q, q->pilot_estimates, q->pilot_estimates_average, port_id);
      interpolate_pilots(q, q->pilot_estimates_average, ce, port_id);              
    }
    
    /* Estimate noise power */
    if (q->noise_alg == SRSLTE_NOISE_ALG_REFS && q->smooth_filter_len > 0) {
      q->noise_estimate[port_id] = estimate_noise_pilots(q, port_id);                  
    } else if (q->noise_alg == SRSLTE_NOISE_ALG_PSS) {
      if (sf_idx == 0 || sf_idx == 5) {
        q->noise_estimate[port_id] = estimate_noise_pss(q, input, ce);
      }
    } else {
      if (sf_idx == 0 || sf_idx == 5) {
        q->noise_estimate[port_id] = estimate_noise_empty_sc(q, input);        
      }
    }
    
  }
    
  /* Compute RSRP for the channel estimates in this port */
  q->rsrp[port_id] = srslte_vec_avg_power_cf(q->pilot_recv_signal, SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id));     
  if (port_id == 0) {
    /* compute rssi only for port 0 */
    q->rssi[port_id] = srslte_chest_dl_rssi(q, input, port_id);     
  }
      
  return 0;
}

int srslte_chest_dl_estimate(srslte_chest_dl_t *q, cf_t *input, cf_t *ce[SRSLTE_MAX_PORTS], uint32_t sf_idx) 
{
  uint32_t port_id; 
  
  for (port_id=0;port_id<q->cell.nof_ports;port_id++) {
    srslte_chest_dl_estimate_port(q, input, ce[port_id], sf_idx, port_id);
  }
  return SRSLTE_SUCCESS;
}

float srslte_chest_dl_get_noise_estimate(srslte_chest_dl_t *q) {
  return srslte_vec_acc_ff(q->noise_estimate, q->cell.nof_ports)/q->cell.nof_ports;
}

float srslte_chest_dl_get_snr(srslte_chest_dl_t *q) {
#ifdef FREQ_SEL_SNR
  int nref=SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, 0);
  return srslte_vec_acc_ff(q->snr_vector, nref)/nref; 
#else
  return srslte_chest_dl_get_rsrp(q)/srslte_chest_dl_get_noise_estimate(q);
#endif
}

float srslte_chest_dl_get_rssi(srslte_chest_dl_t *q) {
  return 4*q->rssi[0]/q->cell.nof_prb/SRSLTE_NRE; 
}

/* q->rssi[0] is the average power in all RE in all symbol containing references for port 0 . q->rssi[0]/q->cell.nof_prb is the average power per PRB 
 * q->rsrp[0] is the average power of RE containing references only (for port 0). 
*/ 
float srslte_chest_dl_get_rsrq(srslte_chest_dl_t *q) {
  return q->cell.nof_prb*q->rsrp[0] / q->rssi[0];
  
}

float srslte_chest_dl_get_rsrp(srslte_chest_dl_t *q) {
  
  // return sum of power received from all tx ports
  return srslte_vec_acc_ff(q->rsrp, q->cell.nof_ports); 
}

