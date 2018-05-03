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
#include <srslte/srslte.h>
#include <srslte/phy/common/phy_common.h>

#include "srslte/config.h"

#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/ch_estimation/chest_ul.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/convolution.h"

#define NOF_REFS_SYM    (q->cell.nof_prb*SRSLTE_NRE)
#define NOF_REFS_SF     (NOF_REFS_SYM*2) // 2 reference symbols per subframe

#define MAX_REFS_SYM    (max_prb*SRSLTE_NRE)
#define MAX_REFS_SF     (max_prb*SRSLTE_NRE*2) // 2 reference symbols per subframe

/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the srslte_refsignal_t object for creating the LTE CSR signal.  
*/

int srslte_chest_ul_init(srslte_chest_ul_t *q, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL)
  {
    bzero(q, sizeof(srslte_chest_ul_t));

    ret = srslte_refsignal_ul_init(&q->dmrs_signal, max_prb);
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    q->tmp_noise = srslte_vec_malloc(sizeof(cf_t) * MAX_REFS_SF);
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }
    q->pilot_estimates = srslte_vec_malloc(sizeof(cf_t) * MAX_REFS_SF);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }      
    for (int i=0;i<4;i++) {
      q->pilot_estimates_tmp[i] = srslte_vec_malloc(sizeof(cf_t) * MAX_REFS_SF);
      if (!q->pilot_estimates_tmp[i]) {
        perror("malloc");
        goto clean_exit;
      }      
    }
    q->pilot_recv_signal = srslte_vec_malloc(sizeof(cf_t) * (MAX_REFS_SF+1));
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }
    
    q->pilot_known_signal = srslte_vec_malloc(sizeof(cf_t) * (MAX_REFS_SF+1));
    if (!q->pilot_known_signal) {
      perror("malloc");
      goto clean_exit;
    }
    
    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, MAX_REFS_SYM)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit; 
    }

    q->smooth_filter_len = 3; 
    srslte_chest_ul_set_smooth_filter3_coeff(q, 0.3333);
  
    q->dmrs_signal_configured = false;

    if (srslte_refsignal_dmrs_pusch_pregen_init(&q->dmrs_signal, &q->dmrs_pregen, max_prb)) {
      fprintf(stderr, "Error allocating memory for pregenerated signals\n");
      goto clean_exit;
    }
  
  }
    
  ret = SRSLTE_SUCCESS;

clean_exit:
  if (ret != SRSLTE_SUCCESS) {
      srslte_chest_ul_free(q);
  }
  return ret; 
}

void srslte_chest_ul_free(srslte_chest_ul_t *q) 
{
  srslte_refsignal_dmrs_pusch_pregen_free(&q->dmrs_signal, &q->dmrs_pregen);

  srslte_refsignal_ul_free(&q->dmrs_signal);
  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  
  if (q->pilot_estimates) {
    free(q->pilot_estimates);
  }      
  for (int i=0;i<4;i++) {
    if (q->pilot_estimates_tmp[i]) {
      free(q->pilot_estimates_tmp[i]);
    }
  }
  if (q->pilot_recv_signal) {
    free(q->pilot_recv_signal);
  }
  if (q->pilot_known_signal) {
    free(q->pilot_known_signal);
  }
  bzero(q, sizeof(srslte_chest_ul_t));
}

int srslte_chest_ul_set_cell(srslte_chest_ul_t *q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL &&
      srslte_cell_isvalid(&cell))
  {
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      memcpy(&q->cell, &cell, sizeof(srslte_cell_t));
      ret = srslte_refsignal_ul_set_cell(&q->dmrs_signal, cell);
      if (ret != SRSLTE_SUCCESS) {
        fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
        return SRSLTE_ERROR;
      }

      if (srslte_interp_linear_vector_resize(&q->srslte_interp_linvec, NOF_REFS_SYM)) {
        fprintf(stderr, "Error initializing vector interpolator\n");
        return SRSLTE_ERROR;
      }
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

void srslte_chest_ul_set_cfg(srslte_chest_ul_t *q, 
                             srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg,
                             srslte_pucch_cfg_t *pucch_cfg, 
                             srslte_refsignal_srs_cfg_t *srs_cfg)
{
  srslte_refsignal_ul_set_cfg(&q->dmrs_signal, pusch_cfg, pucch_cfg, srs_cfg);
  srslte_refsignal_dmrs_pusch_pregen(&q->dmrs_signal, &q->dmrs_pregen);
  q->dmrs_signal_configured = true; 
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srslte_chest_ul_t *q, cf_t *ce, uint32_t nrefs, uint32_t n_prb[2]) 
{
  
  float power = 0; 
  for (int i=0;i<2;i++) {
    power += srslte_chest_estimate_noise_pilots(&q->pilot_estimates[i*nrefs], 
                                                &ce[SRSLTE_REFSIGNAL_UL_L(i, q->cell.cp)*q->cell.nof_prb*SRSLTE_NRE+n_prb[i]*SRSLTE_NRE], 
                                                q->tmp_noise, 
                                                nrefs);
  }

  power/=2; 
  
  if (q->smooth_filter_len == 3) {
    // Calibrated for filter length 3
    float w=q->smooth_filter[0];
    float a=7.419*w*w+0.1117*w-0.005387;
    return (power/(a*0.8)); 
  } else {
    return power;     
  }
}

// The interpolator currently only supports same frequency allocation for each subframe
#define cesymb(i) ce[SRSLTE_RE_IDX(q->cell.nof_prb,i,n_prb[0]*SRSLTE_NRE)]
static void interpolate_pilots(srslte_chest_ul_t *q, cf_t *ce, uint32_t nrefs, uint32_t n_prb[2]) 
{
  uint32_t L1 = SRSLTE_REFSIGNAL_UL_L(0, q->cell.cp);
  uint32_t L2 = SRSLTE_REFSIGNAL_UL_L(1, q->cell.cp); 
  uint32_t NL = 2*SRSLTE_CP_NSYMB(q->cell.cp);
    
  /* Interpolate in the time domain between symbols */
  srslte_interp_linear_vector3(&q->srslte_interp_linvec, 
                               &cesymb(L2), &cesymb(L1), &cesymb(L1), &cesymb(L1-1), (L2-L1), L1,        false, nrefs);
  srslte_interp_linear_vector3(&q->srslte_interp_linvec, 
                               &cesymb(L1), &cesymb(L2), NULL,        &cesymb(L1+1), (L2-L1), (L2-L1)-1, true,  nrefs);
  srslte_interp_linear_vector3(&q->srslte_interp_linvec, 
                               &cesymb(L1), &cesymb(L2), &cesymb(L2), &cesymb(L2+1), (L2-L1), (NL-L2)-1, true,  nrefs);
  
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

static void average_pilots(srslte_chest_ul_t *q, cf_t *input, cf_t *ce, uint32_t nrefs, uint32_t n_prb[2]) {
  for (int i=0;i<2;i++) {
    srslte_chest_average_pilots(&input[i*nrefs], 
                                &ce[SRSLTE_REFSIGNAL_UL_L(i, q->cell.cp)*q->cell.nof_prb*SRSLTE_NRE+n_prb[i]*SRSLTE_NRE], 
                                q->smooth_filter, nrefs, 1, q->smooth_filter_len);
  }
}

int srslte_chest_ul_estimate(srslte_chest_ul_t *q, cf_t *input, cf_t *ce, 
                             uint32_t nof_prb, uint32_t sf_idx, uint32_t cyclic_shift_for_dmrs, uint32_t n_prb[2]) 
{
  if (!q->dmrs_signal_configured) {
    fprintf(stderr, "Error must call srslte_chest_ul_set_cfg() before using the UL estimator\n");
    return SRSLTE_ERROR; 
  }
  
  if (!srslte_dft_precoding_valid_prb(nof_prb)) {
    fprintf(stderr, "Error invalid nof_prb=%d\n", nof_prb);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
  
  int nrefs_sym = nof_prb*SRSLTE_NRE; 
  int nrefs_sf  = nrefs_sym*2; 
  
  /* Get references from the input signal */
  srslte_refsignal_dmrs_pusch_get(&q->dmrs_signal, input, nof_prb, n_prb, q->pilot_recv_signal);
  
  /* Use the known DMRS signal to compute Least-squares estimates */
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->dmrs_pregen.r[cyclic_shift_for_dmrs][sf_idx][nof_prb], 
                           q->pilot_estimates, nrefs_sf);
  
  if (n_prb[0] != n_prb[1]) {
    printf("ERROR: intra-subframe frequency hopping not supported in the estimator!!\n");
  }
  
  if (ce != NULL) {
    if (q->smooth_filter_len > 0) {
      average_pilots(q, q->pilot_estimates, ce, nrefs_sym, n_prb);
      interpolate_pilots(q, ce, nrefs_sym, n_prb);

      /* If averaging, compute noise from difference between received and averaged estimates */
      q->noise_estimate = estimate_noise_pilots(q, ce, nrefs_sym, n_prb);
    } else {
      // Copy estimates to CE vector without averaging
      for (int i=0;i<2;i++) {
        memcpy(&ce[SRSLTE_REFSIGNAL_UL_L(i, q->cell.cp)*q->cell.nof_prb*SRSLTE_NRE+n_prb[i]*SRSLTE_NRE], 
               &q->pilot_estimates[i*nrefs_sym], 
               nrefs_sym*sizeof(cf_t));        
      }
      interpolate_pilots(q, ce, nrefs_sym, n_prb);                  
      q->noise_estimate = 0;              
    }
  }
  // Estimate received pilot power
  q->pilot_power = srslte_vec_avg_power_cf(q->pilot_recv_signal, nrefs_sf); 
  return 0;
}

int srslte_chest_ul_estimate_pucch(srslte_chest_ul_t *q, cf_t *input, cf_t *ce, 
                                   srslte_pucch_format_t format, uint32_t n_pucch, uint32_t sf_idx, 
                                   uint8_t *pucch2_ack_bits) 
{
  if (!q->dmrs_signal_configured) {
    fprintf(stderr, "Error must call srslte_chest_ul_set_cfg() before using the UL estimator\n");
    return SRSLTE_ERROR; 
  }
    
  int n_rs = srslte_refsignal_dmrs_N_rs(format, q->cell.cp);
  if (!n_rs) {
    fprintf(stderr, "Error computing N_rs\n");
    return SRSLTE_ERROR; 
  }
  int nrefs_sf = SRSLTE_NRE*n_rs*2; 
  
  /* Get references from the input signal */
  srslte_refsignal_dmrs_pucch_get(&q->dmrs_signal, format, n_pucch, input, q->pilot_recv_signal);
  
  /* Generate known pilots */
  uint8_t pucch2_bits[2] = {0, 0};
  if (format == SRSLTE_PUCCH_FORMAT_2A || format == SRSLTE_PUCCH_FORMAT_2B) {
    float max = -1e9;
    int i_max = 0; 
    
    int m = 0; 
    if (format == SRSLTE_PUCCH_FORMAT_2A) {
      m = 2; 
    } else {
      m = 4; 
    }
    
    for (int i=0;i<m;i++) {
      pucch2_bits[0] = i%2;
      pucch2_bits[1] = i/2;
      srslte_refsignal_dmrs_pucch_gen(&q->dmrs_signal, format, n_pucch, sf_idx, pucch2_bits, q->pilot_known_signal);
      srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->pilot_known_signal, q->pilot_estimates_tmp[i], nrefs_sf);
      float x = cabsf(srslte_vec_acc_cc(q->pilot_estimates_tmp[i], nrefs_sf));
      if (x >= max) {
        max = x; 
        i_max = i; 
      }      
    }
    memcpy(q->pilot_estimates, q->pilot_estimates_tmp[i_max], nrefs_sf*sizeof(cf_t));
    pucch2_ack_bits[0] = i_max%2;
    pucch2_ack_bits[1] = i_max/2;
  } else {
    srslte_refsignal_dmrs_pucch_gen(&q->dmrs_signal, format, n_pucch, sf_idx, pucch2_bits, q->pilot_known_signal);
    /* Use the known DMRS signal to compute Least-squares estimates */
    srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->pilot_known_signal, q->pilot_estimates, nrefs_sf);
  }
  
  if (ce != NULL) {
    /* FIXME: Currently averaging entire slot, performance good enough? */
    for (int ns=0;ns<2;ns++) {
      // Average all slot 
      for (int i=1;i<n_rs;i++) {
        srslte_vec_sum_ccc(&q->pilot_estimates[ns*n_rs*SRSLTE_NRE], &q->pilot_estimates[(i+ns*n_rs)*SRSLTE_NRE], 
                           &q->pilot_estimates[ns*n_rs*SRSLTE_NRE], 
                           SRSLTE_NRE);
      }
      srslte_vec_sc_prod_ccc(&q->pilot_estimates[ns*n_rs*SRSLTE_NRE], (float) 1.0/n_rs, 
                             &q->pilot_estimates[ns*n_rs*SRSLTE_NRE], 
                             SRSLTE_NRE);
      
      // Average in freq domain
      srslte_chest_average_pilots(&q->pilot_estimates[ns*n_rs*SRSLTE_NRE], &q->pilot_recv_signal[ns*n_rs*SRSLTE_NRE], 
                                  q->smooth_filter, SRSLTE_NRE, 1, q->smooth_filter_len);
      
      // Determine n_prb
      uint32_t n_prb = srslte_pucch_n_prb(&q->dmrs_signal.pucch_cfg, format, n_pucch, q->cell.nof_prb, q->cell.cp, ns); 

      // copy estimates to slot 
      for (int i=0;i<SRSLTE_CP_NSYMB(q->cell.cp);i++) {
        memcpy(&ce[SRSLTE_RE_IDX(q->cell.nof_prb, i+ns*SRSLTE_CP_NSYMB(q->cell.cp), n_prb*SRSLTE_NRE)], 
               &q->pilot_recv_signal[ns*n_rs*SRSLTE_NRE], sizeof(cf_t)*SRSLTE_NRE);
      }
    }
  }
  
  return 0;
}


float srslte_chest_ul_get_noise_estimate(srslte_chest_ul_t *q) {
  return q->noise_estimate;
}

float srslte_chest_ul_get_snr(srslte_chest_ul_t *q) {
  return q->pilot_power/srslte_chest_ul_get_noise_estimate(q);
}

