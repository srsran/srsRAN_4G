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

#include "srslte/ch_estimation/chest_ul.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/convolution.h"

#define NOF_REFS_SYM    (q->cell.nof_prb*SRSLTE_NRE)
#define NOF_REFS_SF     (NOF_REFS_SYM*2) // 2 reference symbols per subframe

/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the srslte_refsignal_t object for creating the LTE CSR signal.  
*/

int srslte_chest_ul_init(srslte_chest_ul_t *q, srslte_cell_t cell) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL &&
      srslte_cell_isvalid(&cell)) 
  {
    bzero(q, sizeof(srslte_chest_ul_t));
    
    ret = srslte_refsignal_ul_init(&q->dmrs_signal, cell); 
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    q->tmp_noise = srslte_vec_malloc(sizeof(cf_t) * NOF_REFS_SF);
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srslte_vec_malloc(sizeof(cf_t) * NOF_REFS_SF);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }      
    q->pilot_estimates_average = srslte_vec_malloc(sizeof(cf_t) * NOF_REFS_SF);
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }      
    q->pilot_recv_signal = srslte_vec_malloc(sizeof(cf_t) * NOF_REFS_SF);
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }
    
    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, NOF_REFS_SYM)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit; 
    }

    q->smooth_filter_len = 3; 
    srslte_chest_ul_set_smooth_filter3_coeff(q, 0.1);
    
    q->cell = cell; 
  }
  
  q->dmrs_signal_configured = false; 
  
  ret = SRSLTE_SUCCESS;
  
clean_exit:
  if (ret != SRSLTE_SUCCESS) {
      srslte_chest_ul_free(q);
  }
  return ret; 
}

void srslte_chest_ul_free(srslte_chest_ul_t *q) 
{
  srslte_refsignal_ul_free(&q->dmrs_signal);

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }      
  if (q->pilot_estimates_average) {
    free(q->pilot_estimates_average);
  }      
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
  bzero(q, sizeof(srslte_chest_ul_t));
}

void srslte_chest_ul_set_cfg(srslte_chest_ul_t *q, 
                             srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg,
                             srslte_pucch_cfg_t *pucch_cfg, 
                             srslte_refsignal_srs_cfg_t *srs_cfg)
{
  srslte_chest_ul_set_cfg(&q->dmrs_signal, pusch_cfg, pucch_cfg, srs_cfg);
  srslte_refsignal_dmrs_pusch_pregen(&q->dmrs_signal, &q->dmrs_pregen);
  q->dmrs_signal_configured = true; 
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srslte_chest_ul_t *q) 
{
  float power = srslte_chest_estimate_noise_pilots(q->pilot_estimates, 
                                                   q->pilot_estimates_average, 
                                                   q->tmp_noise, 
                                                   NOF_REFS_SF);
  
  return (1/q->smooth_filter[0])*power; 
}

#define cesymb(i) ce[SRSLTE_RE_IDX(q->cell.nof_prb,i,0)]

static void interpolate_pilots(srslte_chest_ul_t *q, cf_t *pilot_estimates, cf_t *ce) 
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t L1 = SRSLTE_REFSIGNAL_UL_L(0, q->cell.cp);
  uint32_t L2 = SRSLTE_REFSIGNAL_UL_L(1, q->cell.cp); 
  uint32_t NL = SRSLTE_CP_NSYMB(q->cell.cp);
  
  /* Interpolate in the time domain between symbols */

  srslte_interp_linear_vector3(&q->srslte_interp_linvec, &cesymb(L2), &cesymb(L1), &cesymb(L1), &cesymb(L1-1), (L2-L1)-1);
  srslte_interp_linear_vector( &q->srslte_interp_linvec, &cesymb(L1), &cesymb(L2), &cesymb(L1+1),              (L2-L1)-1);
  srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(L1), &cesymb(L2), &cesymb(L2), &cesymb(L2+1), (NL-L2)-1);

  // TODO: Maybe here need to do some averaging with previous symbols? 
}

void srslte_chest_ul_set_smooth_filter(srslte_chest_ul_t *q, float *filter, uint32_t filter_len) {
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

void srslte_chest_ul_set_smooth_filter3_coeff(srslte_chest_ul_t* q, float w)
{
  srslte_chest_set_smooth_filter3_coeff(q->smooth_filter, w); 
  q->smooth_filter_len = 3; 
}

static void average_pilots(srslte_chest_ul_t *q, cf_t *input, cf_t *output) {
  uint32_t nsymbols = 2; 
  uint32_t nref = NOF_REFS_SYM;
  srslte_chest_average_pilots(input, output, q->smooth_filter, nref, nsymbols, q->smooth_filter_len);
}

int srslte_chest_ul_estimate(srslte_chest_ul_t *q, cf_t *input, cf_t *ce, 
                             uint32_t nof_prb, uint32_t sf_idx, uint32_t cyclic_shift_for_dmrs, uint32_t n_prb[2]) 
{
  if (!q->dmrs_signal_configured) {
    fprintf(stderr, "Error must call srslte_chest_ul_set_cfg() before using the UL estimator\n");
    return SRSLTE_ERROR; 
  }
  
  /* Get references from the input signal */
  srslte_refsignal_dmrs_pusch_get(&q->dmrs_signal, input, nof_prb, n_prb, q->pilot_recv_signal);
  
  /* Use the known DMRS signal to compute Least-squares estimates */
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->dmrs_pregen.r[cyclic_shift_for_dmrs][sf_idx][nof_prb], 
              q->pilot_estimates, NOF_REFS_SF); 

  if (ce != NULL) {
    if (q->smooth_filter_len > 0) {
      average_pilots(q, q->pilot_estimates, q->pilot_estimates_average);
      interpolate_pilots(q, q->pilot_estimates_average, ce);        
      
      /* If averaging, compute noise from difference between received and averaged estimates */
      if (sf_idx == 0 || sf_idx == 5) {
        q->noise_estimate = estimate_noise_pilots(q);
      }      
    } else {
      interpolate_pilots(q, q->pilot_estimates, ce);                  
      q->noise_estimate = 0;              
    }
  }
  
  // Estimate received pilot power 
  q->pilot_power = srslte_vec_avg_power_cf(q->pilot_recv_signal, NOF_REFS_SF); 
  return 0;
}

float srslte_chest_ul_get_noise_estimate(srslte_chest_ul_t *q) {
  return q->noise_estimate;
}

float srslte_chest_ul_get_snr(srslte_chest_ul_t *q) {
  return q->pilot_power/srslte_chest_ul_get_noise_estimate(q);
}

