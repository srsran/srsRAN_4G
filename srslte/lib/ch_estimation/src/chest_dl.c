/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#define CHEST_RS_AVERAGE_TIME   2
#define CHEST_RS_AVERAGE_FREQ   3

#define NOISE_POWER_METHOD 1 // 0: Difference between noisy received and noiseless; 1: power of empty subcarriers


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
    
    q->tmp_freqavg = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->tmp_freqavg) {
      perror("malloc");
      goto clean_exit;
    }
    q->tmp_noise = srslte_vec_malloc(sizeof(cf_t) * 2 * SRSLTE_REFSIGNAL_MAX_NUM_SF(cell.nof_prb));
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    for (int i=0;i<SRSLTE_CHEST_MAX_FILTER_TIME_LEN;i++) {
      q->tmp_timeavg[i] = srslte_vec_malloc(sizeof(cf_t) * 2*cell.nof_prb);
      if (!q->tmp_timeavg[i]) {
        perror("malloc");
        goto clean_exit;
      }
      bzero(q->tmp_timeavg[i], sizeof(cf_t) * 2*cell.nof_prb);
    }
    q->tmp_timeavg_mult = srslte_vec_malloc(sizeof(cf_t) * 2*cell.nof_prb);
    if (!q->tmp_timeavg_mult) {
      perror("malloc");
      goto clean_exit;
    }
    
    for (int i=0;i<cell.nof_ports;i++) {
      q->pilot_estimates[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_NUM_SF(cell.nof_prb, i));
      if (!q->pilot_estimates[i]) {
        perror("malloc");
        goto clean_exit;
      }      
      // FIXME: There's an invalid read during rsrp estimation for this buffer
      q->pilot_estimates_average[i] = srslte_vec_malloc(2 * sizeof(cf_t) * SRSLTE_REFSIGNAL_NUM_SF(cell.nof_prb, i));
      if (!q->pilot_estimates_average[i]) {
        perror("malloc");
        goto clean_exit;
      }      
      q->pilot_recv_signal[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_REFSIGNAL_NUM_SF(cell.nof_prb, i));
      if (!q->pilot_recv_signal[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }
    
    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, SRSLTE_NRE*cell.nof_prb)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit; 
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin, 2*cell.nof_prb, SRSLTE_NRE/2)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit; 
    }
    
    /* Set default time/freq filters */
    //float f[3]={0.2, 0.6, 0.2};
    //srslte_chest_dl_set_filter_freq(q, f, 3);

    float f[9]={0.025, 0.075, 0.05, 0.15, 0.4, 0.15, 0.05, 0.075, 0.025};
    srslte_chest_dl_set_filter_freq(q, f, 9);

    //srslte_chest_dl_set_filter_time_ema(q, 0.8); 
    
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

  if (q->tmp_freqavg) {
    free(q->tmp_freqavg);
  }
  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  for (int i=0;i<SRSLTE_CHEST_MAX_FILTER_TIME_LEN;i++) {
    if (q->tmp_timeavg[i]) {
      free(q->tmp_timeavg[i]);
    }
  }
  if (q->tmp_timeavg_mult) {
    free(q->tmp_timeavg_mult);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  srslte_interp_linear_free(&q->srslte_interp_lin);
  
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    if (q->pilot_estimates[i]) {
      free(q->pilot_estimates[i]);
    }      
    if (q->pilot_estimates_average[i]) {
      free(q->pilot_estimates_average[i]);
    }      
    if (q->pilot_recv_signal[i]) {
      free(q->pilot_recv_signal[i]);
    }
  }
  bzero(q, sizeof(srslte_chest_dl_t));
}

int srslte_chest_dl_set_filter_freq(srslte_chest_dl_t *q, float *filter, uint32_t filter_len) {
  if (filter_len <= SRSLTE_CHEST_MAX_FILTER_FREQ_LEN) {
    q->filter_freq_len = filter_len; 
    for (int i=0;i<filter_len;i++) {
      q->filter_freq[i] = filter[i];
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR;
  }
}

void srslte_chest_dl_set_filter_time_ema(srslte_chest_dl_t *q, float ema_coefficient) {
  q->filter_time_ema = ema_coefficient;  
}

int srslte_chest_dl_set_filter_time(srslte_chest_dl_t *q, float *filter, uint32_t filter_len) {
  if (filter_len <= SRSLTE_CHEST_MAX_FILTER_TIME_LEN) {
    q->filter_time_len = filter_len; 
    for (int i=0;i<filter_len;i++) {
      q->filter_time[i] = filter[i];
    }    
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR;
  }  
}



#if NOISE_POWER_METHOD==0

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_port(srslte_chest_dl_t *q, uint32_t port_id, cf_t *avg_pilots) {
  /* Use difference between averaged and noisy LS pilot estimates */
  srslte_vec_sub_ccc(avg_pilots, q->pilot_estimates[port_id],
              q->tmp_noise, SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id));

  return srslte_vec_avg_power_cf(q->tmp_noise, SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id));
}
#endif

#if NOISE_POWER_METHOD==1

/* Uses the 5 empty transmitted SC before and after the SSS and PSS sequences for noise estimation */
static float estimate_noise_port(srslte_chest_dl_t *q, cf_t *input) {
  int k_sss = (SRSLTE_CP_NSYMB(q->cell.cp) - 2) * q->cell.nof_prb * SRSLTE_NRE + q->cell.nof_prb * SRSLTE_NRE / 2 - 31;
  float noise_power = 0; 
  noise_power += srslte_vec_avg_power_cf(&input[k_sss-5], 5); // 5 empty SC before SSS
  noise_power += srslte_vec_avg_power_cf(&input[k_sss+62], 5); // 5 empty SC after SSS
  int k_pss = (SRSLTE_CP_NSYMB(q->cell.cp) - 1) * q->cell.nof_prb * SRSLTE_NRE + q->cell.nof_prb * SRSLTE_NRE / 2 - 31;
  noise_power += srslte_vec_avg_power_cf(&input[k_pss-5], 5); // 5 empty SC before PSS
  noise_power += srslte_vec_avg_power_cf(&input[k_pss+62], 5); // 5 empty SC after PSS
  
  return noise_power; 
}
#endif

#define pilot_est(idx) q->pilot_estimates[port_id][SRSLTE_REFSIGNAL_PILOT_IDX(idx,l,q->cell)]
#define pilot_avg(idx) q->pilot_estimates_average[port_id][SRSLTE_REFSIGNAL_PILOT_IDX(idx,l,q->cell)]
#define pilot_tmp(idx) q->tmp_freqavg[SRSLTE_REFSIGNAL_PILOT_IDX(idx,l,q->cell)]

static void average_pilots(srslte_chest_dl_t *q, uint32_t port_id) 
{
  int nref=2*q->cell.nof_prb;
  uint32_t l, i;

  /* For each symbol with pilots in a slot */
  for (l=0;l<srslte_refsignal_cs_nof_symbols(port_id);l++) {
    if (q->filter_freq_len > 0) {
      /* Filter pilot estimates in frequency */
      srslte_conv_same_cf(&pilot_est(0), q->filter_freq, &pilot_tmp(0), nref, q->filter_freq_len);
      
      /* Adjust extremes using linear interpolation */
      
      pilot_tmp(0) += srslte_interp_linear_onesample(pilot_est(1), pilot_est(0)) 
                        * q->filter_freq[q->filter_freq_len/2-1]*1.2;
      pilot_tmp(nref-1) += srslte_interp_linear_onesample(pilot_est(nref-2), pilot_est(nref-1)) 
                        * q->filter_freq[q->filter_freq_len/2+1]*1.2;              
    } else {
      memcpy(&pilot_tmp(0), &pilot_est(0), nref * sizeof(cf_t));
    }
  }

  #if NOISE_POWER_METHOD==0
  q->noise_estimate[port_id] = estimate_noise_port(q, port_id, q->tmp_freqavg);
  #endif
  
  
//#define EMA_VEC
    
  /* Filter with Exponential moving average (IIR) */
  if (q->filter_time_ema > 0) {
#ifdef EMA_VEC
    srslte_vec_ema_filter(&q->tmp_freqavg[0],
                          &q->pilot_estimates_average[port_id][2*q->cell.nof_prb*srslte_refsignal_cs_nof_symbols(port_id)], 
                          &q->pilot_estimates_average[port_id][0],
                          q->filter_time_ema, 
                          nref);
    for (l=1;l<srslte_refsignal_cs_nof_symbols(port_id);l++) {
      srslte_vec_ema_filter(&q->tmp_freqavg[2*q->cell.nof_prb*l],
                            &q->pilot_estimates_average[port_id][2*q->cell.nof_prb*(l-1)], 
                            &q->pilot_estimates_average[port_id][2*q->cell.nof_prb*l],
                            q->filter_time_ema, 
                            nref);
    }
#else    
    for (i=0;i<nref;i++) {
      l=0;
      pilot_avg(i) = SRSLTE_VEC_EMA(pilot_tmp(i), q->pilot_estimates_average[port_id][
          SRSLTE_REFSIGNAL_PILOT_IDX(i,srslte_refsignal_cs_nof_symbols(port_id),q->cell)], q->filter_time_ema);
      for (l=1;l<srslte_refsignal_cs_nof_symbols(port_id);l++) {
        pilot_avg(i) = SRSLTE_VEC_EMA(pilot_tmp(i), q->pilot_estimates_average[port_id][SRSLTE_REFSIGNAL_PILOT_IDX(i,l-1,q->cell)], q->filter_time_ema);
      }    
    }
#endif

  } else {
    /* Filter with FIR or don't filter */ 
    for (l=0;l<srslte_refsignal_cs_nof_symbols(port_id);l++) {
      /* Filter in time domain. */
      if (q->filter_time_len > 0) {
        /* Move last symbols */
        for (i=0;i<q->filter_time_len-1;i++) {
          memcpy(q->tmp_timeavg[i], q->tmp_timeavg[i+1], nref*sizeof(cf_t));                      
        }
        /* Save last symbol to buffer */
        memcpy(q->tmp_timeavg[q->filter_time_len-1], &pilot_tmp(0), nref*sizeof(cf_t));            
        
        /* Multiply all symbols by filter and add them  */
        if (l > 0) {
          bzero(&pilot_avg(0), nref * sizeof(cf_t));
          for (i=0;i<q->filter_time_len;i++) {
            srslte_vec_sc_prod_cfc(q->tmp_timeavg[i], q->filter_time[i], q->tmp_timeavg_mult, nref);
            srslte_vec_sum_ccc(q->tmp_timeavg_mult, &pilot_avg(0), &pilot_avg(0), nref);            
          }        
        } else {
          memcpy(&pilot_avg(0), &pilot_tmp(0), nref * sizeof(cf_t));
        }
      } else {
        memcpy(&pilot_avg(0), &pilot_tmp(0), nref * sizeof(cf_t));        
      }
    } 
  }
}

#define cesymb(i) ce[SRSLTE_RE_IDX(q->cell.nof_prb,i,0)]

static void interpolate_pilots(srslte_chest_dl_t *q, cf_t *ce, uint32_t port_id) 
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t l; 
  uint32_t nsymbols = srslte_refsignal_cs_nof_symbols(port_id); 
  
  /* Interpolate in the frequency domain */
  for (l=0;l<nsymbols;l++) {
    uint32_t fidx_offset = srslte_refsignal_cs_fidx(q->cell, l, port_id, 0);    
    srslte_interp_linear_offset(&q->srslte_interp_lin, &pilot_avg(0),
                         &ce[srslte_refsignal_cs_nsymbol(l,q->cell.cp, port_id) * q->cell.nof_prb * SRSLTE_NRE], 
                         fidx_offset, SRSLTE_NRE/2-fidx_offset); 
  }
  
  /* Now interpolate in the time domain between symbols */
  if (SRSLTE_CP_ISNORM(q->cell.cp)) {
    if (nsymbols == 4) {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(0), &cesymb(4), &cesymb(1), 3);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(4), &cesymb(7), &cesymb(5), 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(8), 3);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(7), &cesymb(11), &cesymb(12), 2);
    } else {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(8), &cesymb(1), &cesymb(0), 1);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(2), 6);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(8), &cesymb(9), 5);
    }    
  } else {
    if (nsymbols == 4) {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(0), &cesymb(3), &cesymb(1), 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(3), &cesymb(6), &cesymb(4), 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(7), 2);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(6), &cesymb(9), &cesymb(9), 2);
    } else {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(7), &cesymb(1), &cesymb(0), 1);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(2), 5);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(1), &cesymb(7), &cesymb(8), 4);
    }    
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

#define RSRP_FROM_ESTIMATES

float srslte_chest_dl_rsrp(srslte_chest_dl_t *q, uint32_t port_id) {
#ifdef RSRP_FROM_ESTIMATES
  return srslte_vec_avg_power_cf(q->pilot_estimates[port_id], 
                          SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id));
#else
  return srslte_vec_avg_power_cf(q->pilot_estimates_average[port_id], 
                          SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id));
#endif
}

int srslte_chest_dl_estimate_port(srslte_chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id) 
{
  /* Get references from the input signal */
  srslte_refsignal_cs_get_sf(q->cell, port_id, input, q->pilot_recv_signal[port_id]);
  
  /* Use the known CSR signal to compute Least-squares estimates */
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal[port_id], q->csr_signal.pilots[port_id/2][sf_idx], 
              q->pilot_estimates[port_id], SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id)); 

  /* Average pilot estimates */
  average_pilots(q, port_id);
  
  #if NOISE_POWER_METHOD==1
  q->noise_estimate[port_id] = estimate_noise_port(q, input);
  #endif
  
  /* Compute RSRP for the channel estimates in this port */
  q->rsrp[port_id] = srslte_chest_dl_rsrp(q, port_id);     
  if (port_id == 0) {
    /* compute rssi only for port 0 */
    q->rssi[port_id] = srslte_chest_dl_rssi(q, input, port_id);     
  }
      
  /* Interpolate to create channel estimates for all resource grid */
  if (ce != NULL) {
    interpolate_pilots(q, ce, port_id);    
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

