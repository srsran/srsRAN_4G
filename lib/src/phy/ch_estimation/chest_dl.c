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
#include <srslte/phy/common/phy_common.h>
#include <srslte/srslte.h>

#include "srslte/config.h"

#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/convolution.h"

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
int srslte_chest_dl_init(srslte_chest_dl_t *q, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL)
  {
    bzero(q, sizeof(srslte_chest_dl_t));
    

    ret = srslte_refsignal_cs_init(&q->csr_refs, max_prb);
    if (ret != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
      goto clean_exit;
    }
    
    q->mbsfn_refs = calloc(SRSLTE_MAX_MBSFN_AREA_IDS, sizeof(srslte_refsignal_t));
    if (!q->mbsfn_refs) {
      fprintf(stderr, "Calloc error initializing mbsfn_refs (%d)\n", ret);
      goto clean_exit;
    }
    
    int pilot_vec_size;
    if(SRSLTE_REFSIGNAL_MAX_NUM_SF_MBSFN(max_prb)>SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb)) {
      pilot_vec_size = SRSLTE_REFSIGNAL_MAX_NUM_SF_MBSFN(max_prb);
    }else{
      pilot_vec_size = SRSLTE_REFSIGNAL_MAX_NUM_SF(max_prb);
    }

    q->tmp_noise = srslte_vec_malloc(sizeof(cf_t) * pilot_vec_size);
    if (!q->tmp_noise) {
      perror("malloc");
      goto clean_exit;
    }

    q->tmp_cfo_estimate = srslte_vec_malloc(sizeof(cf_t) * pilot_vec_size);
    if (!q->tmp_cfo_estimate) {
      perror("malloc");
      goto clean_exit;
    }

    q->pilot_estimates = srslte_vec_malloc(sizeof(cf_t) * pilot_vec_size);
    if (!q->pilot_estimates) {
      perror("malloc");
      goto clean_exit;
    }      

    q->pilot_estimates_average = srslte_vec_malloc(sizeof(cf_t) * pilot_vec_size);
    if (!q->pilot_estimates_average) {
      perror("malloc");
      goto clean_exit;
    }
    
    q->pilot_recv_signal = srslte_vec_malloc(sizeof(cf_t) * pilot_vec_size);
    if (!q->pilot_recv_signal) {
      perror("malloc");
      goto clean_exit;
    }
    
    if (srslte_interp_linear_vector_init(&q->srslte_interp_linvec, SRSLTE_NRE*max_prb)) {
      fprintf(stderr, "Error initializing vector interpolator\n");
      goto clean_exit; 
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin, 2*max_prb, SRSLTE_NRE/2)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit; 
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin_3, 4*max_prb, SRSLTE_NRE/4)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit;
    }

    if (srslte_interp_linear_init(&q->srslte_interp_lin_mbsfn, 6*max_prb, SRSLTE_NRE/6)) {
      fprintf(stderr, "Error initializing interpolator\n");
      goto clean_exit;
    }
    
    q->noise_alg = SRSLTE_NOISE_ALG_REFS; 

    q->rsrp_neighbour = false;
    q->average_subframe = false;
    q->smooth_filter_auto = false;
    q->smooth_filter_len = 3; 
    srslte_chest_dl_set_smooth_filter3_coeff(q, 0.1);
    
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
  if(&q->csr_refs)
    srslte_refsignal_free(&q->csr_refs);

  if (q->mbsfn_refs) {
    for (int i=0; i<SRSLTE_MAX_MBSFN_AREA_IDS; i++) {
      if (q->mbsfn_refs[i]) {
        srslte_refsignal_free(q->mbsfn_refs[i]);
        free(q->mbsfn_refs[i]);
      }
    }
    free(q->mbsfn_refs);
  }

  if (q->tmp_noise) {
    free(q->tmp_noise);
  }
  if (q->tmp_cfo_estimate) {
    free(q->tmp_cfo_estimate);
  }
  srslte_interp_linear_vector_free(&q->srslte_interp_linvec);
  srslte_interp_linear_free(&q->srslte_interp_lin);
  srslte_interp_linear_free(&q->srslte_interp_lin_3);
  srslte_interp_linear_free(&q->srslte_interp_lin_mbsfn);
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


int srslte_chest_dl_set_mbsfn_area_id(srslte_chest_dl_t *q, uint16_t mbsfn_area_id){
  if (mbsfn_area_id < SRSLTE_MAX_MBSFN_AREA_IDS) {
    if(!q->mbsfn_refs[mbsfn_area_id]) {
      q->mbsfn_refs[mbsfn_area_id] = calloc(1, sizeof(srslte_refsignal_t));
        if(srslte_refsignal_mbsfn_init(q->mbsfn_refs[mbsfn_area_id], q->cell.nof_prb)) {
          return SRSLTE_ERROR;
        }
    }
    if(q->mbsfn_refs[mbsfn_area_id]) {
      if(srslte_refsignal_mbsfn_set_cell(q->mbsfn_refs[mbsfn_area_id], q->cell, mbsfn_area_id)) {
        return SRSLTE_ERROR;
      }
    }
    return SRSLTE_SUCCESS;
  }
  return SRSLTE_ERROR;
}

int srslte_chest_dl_set_cell(srslte_chest_dl_t *q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q                != NULL &&
      srslte_cell_isvalid(&cell))
  {
    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      memcpy(&q->cell, &cell, sizeof(srslte_cell_t));
      ret = srslte_refsignal_cs_set_cell(&q->csr_refs, cell);
      if (ret != SRSLTE_SUCCESS) {
        fprintf(stderr, "Error initializing CSR signal (%d)\n",ret);
        return SRSLTE_ERROR;
      }
      if (srslte_pss_generate(q->pss_signal, cell.id%3)) {
        fprintf(stderr, "Error initializing PSS signal for noise estimation\n");
        return SRSLTE_ERROR;
      }
      if (srslte_interp_linear_vector_resize(&q->srslte_interp_linvec, SRSLTE_NRE*q->cell.nof_prb)) {
        fprintf(stderr, "Error initializing vector interpolator\n");
        return SRSLTE_ERROR;
      }

      if (srslte_interp_linear_resize(&q->srslte_interp_lin, 2*q->cell.nof_prb, SRSLTE_NRE/2)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSLTE_ERROR;
      }

      if (srslte_interp_linear_resize(&q->srslte_interp_lin_3, 4 * q->cell.nof_prb, SRSLTE_NRE / 4)) {
        fprintf(stderr, "Error initializing interpolator\n");
        return SRSLTE_ERROR;
      }

    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Uses the difference between the averaged and non-averaged pilot estimates */
static float estimate_noise_pilots(srslte_chest_dl_t *q, uint32_t port_id, srslte_sf_t ch_mode)
{
  const float weight = 1.0f;
  float sum_power = 0.0f;
  uint32_t count = 0;
  uint32_t npilots = (ch_mode == SRSLTE_SF_MBSFN)?SRSLTE_REFSIGNAL_NUM_SF_MBSFN(q->cell.nof_prb, port_id):SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id);
  uint32_t nsymbols = (ch_mode == SRSLTE_SF_MBSFN) ? srslte_refsignal_mbsfn_nof_symbols() : srslte_refsignal_cs_nof_symbols(port_id);
  uint32_t nref = npilots / nsymbols;
  uint32_t fidx = (ch_mode == SRSLTE_SF_MBSFN)?srslte_refsignal_mbsfn_fidx(1):srslte_refsignal_cs_fidx(q->cell, 0, port_id, 0);

  cf_t *input2d[nsymbols + 2];
  cf_t *tmp_noise = q->tmp_noise;

  for (int i = 0; i < nsymbols; i++) {
    input2d[i + 1] = &q->pilot_estimates[i * nref];
  }

  input2d[0] = &q->tmp_noise[nref];
  if (nsymbols > 3) {
    srslte_vec_sc_prod_cfc(input2d[2], 2.0f, input2d[0], nref);
    srslte_vec_sub_ccc(input2d[0], input2d[4], input2d[0], nref);
  } else {
    srslte_vec_sc_prod_cfc(input2d[2], 1.0f, input2d[0], nref);
  }

  input2d[nsymbols + 1] = &q->tmp_noise[nref * 2];
  if (nsymbols > 3) {
    srslte_vec_sc_prod_cfc(input2d[nsymbols - 1], 2.0f, input2d[nsymbols + 1], nref);
    srslte_vec_sub_ccc(input2d[nsymbols + 1], input2d[nsymbols - 3], input2d[nsymbols + 1], nref);
  } else {
    srslte_vec_sc_prod_cfc(input2d[nsymbols - 1], 1.0f, input2d[nsymbols + 1], nref);
  }

  for (int i = 1; i < nsymbols + 1; i++) {
    uint32_t offset = ((fidx < 3) ^ (i & 1)) ? 0 : 1;
    srslte_vec_sc_prod_cfc(input2d[i], weight, tmp_noise, nref);

    srslte_vec_sum_ccc(&input2d[i - 1][0], &tmp_noise[offset], &tmp_noise[offset], nref - offset);
    srslte_vec_sum_ccc(&input2d[i - 1][1 - offset], &tmp_noise[0], &tmp_noise[0], nref + offset - 1);
    if (offset) {
      tmp_noise[0] += 2.0f * input2d[i - 1][0] - input2d[i - 1][1];
    } else {
      tmp_noise[nref - 1] += 2.0f * input2d[i - 1][nref - 2] - input2d[i - 1][nref - 1];
    }

    srslte_vec_sum_ccc(&input2d[i + 1][0], &tmp_noise[offset], &tmp_noise[offset], nref - offset);
    srslte_vec_sum_ccc(&input2d[i + 1][1 - offset], &tmp_noise[0], &tmp_noise[0], nref + offset - 1);
    if (offset) {
      tmp_noise[0] += 2.0f * input2d[i + 1][0] - input2d[i + 1][1];
    } else {
      tmp_noise[nref - 1] += 2.0f * input2d[i + 1][nref - 2] - input2d[i + 1][nref - 1];
    }

    srslte_vec_sc_prod_cfc(tmp_noise, 1.0f / (weight + 4.0f), tmp_noise, nref);

    srslte_vec_sub_ccc(input2d[i], tmp_noise, tmp_noise, nref);
    sum_power = srslte_vec_avg_power_cf(tmp_noise, nref);
    count++;
  }

  return sum_power / (float) count * sqrtf(weight + 4.0f);
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

static void interpolate_pilots(srslte_chest_dl_t *q, cf_t *pilot_estimates, cf_t *ce, uint32_t port_id, srslte_sf_t ch_mode) 
{
  /* interpolate the symbols with references in the freq domain */
  uint32_t l; 
  uint32_t nsymbols = (ch_mode == SRSLTE_SF_MBSFN ) ? srslte_refsignal_mbsfn_nof_symbols() + 1 : srslte_refsignal_cs_nof_symbols(port_id);
  uint32_t fidx_offset = 0;
  /* Interpolate in the frequency domain */

  if (q->average_subframe) {
    nsymbols = 1;
  }

   // we add one to nsymbols to allow for inclusion of the non-mbms references in the channel estimation       
  for (l=0;l<nsymbols;l++) {
    if (ch_mode == SRSLTE_SF_MBSFN) {
      if (l == 0) {
        fidx_offset = srslte_refsignal_cs_fidx(q->cell, l, port_id, 0);
        srslte_interp_linear_offset(&q->srslte_interp_lin, &pilot_estimates[2*q->cell.nof_prb*l],
                                    &ce[srslte_refsignal_cs_nsymbol(l,q->cell.cp, port_id) * q->cell.nof_prb * SRSLTE_NRE], 
                                    fidx_offset, SRSLTE_NRE/2-fidx_offset); 
      } else {
        fidx_offset = srslte_refsignal_mbsfn_fidx(l - 1);
        srslte_interp_linear_offset(&q->srslte_interp_lin_mbsfn, &pilot_estimates[(2*q->cell.nof_prb) + 6*q->cell.nof_prb*(l - 1)],
                                    &ce[srslte_refsignal_mbsfn_nsymbol(l - 1) * q->cell.nof_prb * SRSLTE_NRE],
                                    fidx_offset, SRSLTE_NRE/6-fidx_offset);
      }
    } else {
      if (q->average_subframe) {
        fidx_offset = q->cell.id % 3;
        srslte_interp_linear_offset(&q->srslte_interp_lin_3,
                                    pilot_estimates,
                                    ce,
                                    fidx_offset,
                                    SRSLTE_NRE / 4 - fidx_offset);
      } else {
        fidx_offset = srslte_refsignal_cs_fidx(q->cell, l, port_id, 0);
        srslte_interp_linear_offset(&q->srslte_interp_lin, &pilot_estimates[2 * q->cell.nof_prb * l],
                                    &ce[srslte_refsignal_cs_nsymbol(l, q->cell.cp, port_id) * q->cell.nof_prb
                                        * SRSLTE_NRE], fidx_offset, SRSLTE_NRE / 2 - fidx_offset);
      }
    }  
  }
 
  /* Now interpolate in the time domain between symbols */
  if (q->average_subframe) {
    // If we average per subframe, just copy the estimates in the time domain
    for (l=1;l<2*SRSLTE_CP_NSYMB(q->cell.cp);l++) {
      memcpy(&ce[l*SRSLTE_NRE*q->cell.nof_prb], ce, sizeof(cf_t)*SRSLTE_NRE*q->cell.nof_prb);
    }
  } else {
    if (ch_mode == SRSLTE_SF_MBSFN) {
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(0), &cesymb(2), &cesymb(1), 2, 1);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(2), &cesymb(6), &cesymb(3), 4, 3);
      srslte_interp_linear_vector(&q->srslte_interp_linvec, &cesymb(6), &cesymb(10), &cesymb(7), 4, 3);
      srslte_interp_linear_vector2(&q->srslte_interp_linvec, &cesymb(6), &cesymb(10), &cesymb(10), &cesymb(11), 4, 1);
    } else {
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

void srslte_chest_dl_set_smooth_filter_gauss(srslte_chest_dl_t* q, uint32_t order, float std_dev)
{
  const uint32_t filterlen = order + 1;
  const int center = (filterlen - 1) / 2;
  float *filter = q->smooth_filter;
  float norm_p = 0.0f;

  if (filterlen) {

    for (int i = 0; i < filterlen; i++) {
      filter[i] = expf(-powf(i - center, 2) / (2.0f * powf(std_dev, 2)));
      norm_p += powf(filter[i], 2);
    }

    const float norm = srslte_vec_acc_ff(filter, filterlen);

    srslte_vec_sc_prod_fff(filter, 1.0f / norm, filter, filterlen);
    q->smooth_filter_len = filterlen;
  }
}

void srslte_chest_dl_set_smooth_filter_auto(srslte_chest_dl_t *q, bool enable) {
  q->smooth_filter_auto = enable;
}

uint32_t srslte_chest_dl_interleave_pilots(srslte_chest_dl_t *q, cf_t *input, cf_t *tmp, cf_t *output, uint32_t port_id, srslte_sf_t ch_mode) {
  uint32_t nsymbols = (ch_mode == SRSLTE_SF_MBSFN)?srslte_refsignal_mbsfn_nof_symbols(port_id):srslte_refsignal_cs_nof_symbols(port_id);
  uint32_t nref = (ch_mode == SRSLTE_SF_MBSFN)?6*q->cell.nof_prb:2*q->cell.nof_prb;
  uint32_t fidx = (ch_mode == SRSLTE_SF_MBSFN)?srslte_refsignal_mbsfn_fidx(1):srslte_refsignal_cs_fidx(q->cell, 0, port_id, 0);
  
  if (fidx < 3) {
    srslte_vec_interleave(input, &input[nref], tmp, nref);
    for (int l = 2; l < nsymbols - 1; l += 2) {
      srslte_vec_interleave_add(&input[l * nref], &input[(l + 1) * nref], tmp, nref);
    }
  } else {
    srslte_vec_interleave(&input[nref], input, tmp, nref);
    for (int l = 2; l < nsymbols - 1; l += 2) {
      srslte_vec_interleave_add(&input[(l + 1) * nref], &input[l * nref], tmp, nref);
    }
  }

  nref *= 2;
  srslte_vec_sc_prod_cfc(tmp, 2.0f / nsymbols, output, nref);

  return nref;
}

static void average_pilots(srslte_chest_dl_t *q, cf_t *input, cf_t *output, uint32_t port_id, srslte_sf_t ch_mode)  {
  uint32_t nsymbols = (ch_mode == SRSLTE_SF_MBSFN)?srslte_refsignal_mbsfn_nof_symbols(port_id):srslte_refsignal_cs_nof_symbols(port_id);
  uint32_t nref = (ch_mode == SRSLTE_SF_MBSFN)?6*q->cell.nof_prb:2*q->cell.nof_prb;

  // Average in the time domain if enabled
  if (q->average_subframe) {
    if (ch_mode == SRSLTE_SF_MBSFN) {
      for (int l = 1; l < nsymbols; l++) {
        srslte_vec_sum_ccc(&input[l * nref], input, input, nref);
      }
      srslte_vec_sc_prod_cfc(input, 1.0f / ((float) nsymbols), input, nref);
      nsymbols = 1;
    } else {
      cf_t *temp = output; // Use ouput as temporal buffer

      if (srslte_refsignal_cs_fidx(q->cell, 0, port_id, 0) < 3) {
        srslte_vec_interleave(input, &input[nref], temp, nref);
        for (int l = 2; l < nsymbols - 1; l += 2) {
          srslte_vec_interleave_add(&input[l * nref], &input[(l + 1) * nref], temp, nref);
        }
      } else {
        srslte_vec_interleave(&input[nref], input, temp, nref);
        for (int l = 2; l < nsymbols - 1; l += 2) {
          srslte_vec_interleave_add(&input[(l + 1) * nref], &input[l * nref], temp, nref);
        }
      }
      nref *= 2;
      srslte_vec_sc_prod_cfc(temp, 2.0f / (float) nsymbols, input, nref);

      nsymbols = 1;
    }
  }


  uint32_t skip = (ch_mode == SRSLTE_SF_MBSFN)?2*q->cell.nof_prb:0;
  
  if(ch_mode == SRSLTE_SF_MBSFN){
    memcpy(&output[0],&input[0],skip*sizeof(cf_t));
  }

  // Average in the frequency domain
  for (int l=0;l<nsymbols;l++) {
    srslte_conv_same_cf(&input[l*nref + skip], q->smooth_filter, &output[l*nref + skip], nref, q->smooth_filter_len);    
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

// CFO estimation algorithm taken from "Carrier Frequency Synchronization in the
// Downlink of 3GPP LTE", Qi Wang, C. Mehlfuhrer, M. Rupp
float chest_estimate_cfo(srslte_chest_dl_t *q)
{
  float n  = (float) srslte_symbol_sz(q->cell.nof_prb);
  float ns = (float) SRSLTE_CP_NSYMB(q->cell.cp);
  float ng = (float) SRSLTE_CP_LEN_NORM(1, n);

  uint32_t npilots = SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, 0);

  // Compute angles between slots
  for (int i=0;i<2;i++) {
    srslte_vec_prod_conj_ccc(&q->pilot_estimates[i*npilots/4],
                             &q->pilot_estimates[(i+2)*npilots/4],
                             &q->tmp_cfo_estimate[i*npilots/4],
                             npilots/4);
  }
  // Average all angles
  cf_t sum = srslte_vec_acc_cc(q->tmp_cfo_estimate, npilots/2);

  // Compute CFO
  return -cargf(sum)*n/(ns*(n+ng))/2/M_PI;
}

void chest_interpolate_noise_est(srslte_chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id, uint32_t rxant_id, srslte_sf_t ch_mode){
  if (q->cfo_estimate_enable && ((1<<sf_idx) & q->cfo_estimate_sf_mask) && ch_mode != SRSLTE_SF_MBSFN ) {
    q->cfo = chest_estimate_cfo(q);
  }

  /* Estimate noise */
  if (q->noise_alg == SRSLTE_NOISE_ALG_REFS && ch_mode != SRSLTE_SF_MBSFN ) {
    q->noise_estimate[rxant_id][port_id] = estimate_noise_pilots(q, port_id, ch_mode);
  }

  if (ce != NULL) {
    if (q->smooth_filter_auto) {
      srslte_chest_dl_set_smooth_filter_gauss(q, 4, q->noise_estimate[rxant_id][port_id] * 200.0f);
    }

    /* Smooth estimates (if applicable) and interpolate */
    if (q->smooth_filter_len == 0 || (q->smooth_filter_len == 3 && q->smooth_filter[0] == 0)) {
      interpolate_pilots(q, q->pilot_estimates, ce, port_id, ch_mode);
    } else {
      average_pilots(q, q->pilot_estimates, q->pilot_estimates_average, port_id, ch_mode);
      interpolate_pilots(q, q->pilot_estimates_average, ce, port_id, ch_mode);
    }
  
    /* Estimate noise power */
    if (q->noise_alg == SRSLTE_NOISE_ALG_PSS) {
      if (sf_idx == 0 || sf_idx == 5) {
        q->noise_estimate[rxant_id][port_id] = estimate_noise_pss(q, input, ce);
      }
    } else if (q->noise_alg != SRSLTE_NOISE_ALG_REFS) {
      if (sf_idx == 0 || sf_idx == 5) {
        q->noise_estimate[rxant_id][port_id] = estimate_noise_empty_sc(q, input);        
      }
    } 
  }
}

int srslte_chest_dl_estimate_port(srslte_chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id, uint32_t rxant_id)
{
  uint32_t npilots = SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, port_id);

  /* Get references from the input signal */
  srslte_refsignal_cs_get_sf(q->cell, port_id, input, q->pilot_recv_signal);
  
  /* Use the known CSR signal to compute Least-squares estimates */
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->csr_refs.pilots[port_id/2][sf_idx], 
              q->pilot_estimates, npilots);

  /* Compute RSRP for the channel estimates in this port */
  if (q->rsrp_neighbour) {
    double energy = cabs(srslte_vec_acc_cc(q->pilot_estimates, npilots)/npilots);
    q->rsrp_corr[rxant_id][port_id] = energy*energy;
  }
  q->rsrp[rxant_id][port_id] = srslte_vec_avg_power_cf(q->pilot_recv_signal, npilots);
  q->rssi[rxant_id][port_id] = srslte_chest_dl_rssi(q, input, port_id);

  chest_interpolate_noise_est(q, input, ce, sf_idx, port_id, rxant_id, SRSLTE_SF_NORM);

  return 0;
}

int srslte_chest_dl_estimate_port_mbsfn(srslte_chest_dl_t *q, cf_t *input, cf_t *ce, uint32_t sf_idx, uint32_t port_id, uint32_t rxant_id, uint16_t mbsfn_area_id)
{

  /* Use the known CSR signal to compute Least-squares estimates */
  srslte_refsignal_mbsfn_get_sf(q->cell, port_id, input, q->pilot_recv_signal);
  // estimate for non-mbsfn section of subframe
  srslte_vec_prod_conj_ccc(q->pilot_recv_signal, q->csr_refs.pilots[port_id/2][sf_idx],
                           q->pilot_estimates, (2*q->cell.nof_prb));
  
  srslte_vec_prod_conj_ccc(&q->pilot_recv_signal[(2*q->cell.nof_prb)], q->mbsfn_refs[mbsfn_area_id]->pilots[port_id/2][sf_idx],
                           &q->pilot_estimates[(2*q->cell.nof_prb)], SRSLTE_REFSIGNAL_NUM_SF_MBSFN(q->cell.nof_prb, port_id)-(2*q->cell.nof_prb));
  

  chest_interpolate_noise_est(q, input, ce, sf_idx, port_id, rxant_id, SRSLTE_SF_MBSFN);
      
  return 0;
}

int srslte_chest_dl_estimate_multi(srslte_chest_dl_t *q, cf_t *input[SRSLTE_MAX_PORTS], cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], uint32_t sf_idx, uint32_t nof_rx_antennas) 
{
  for (uint32_t rxant_id=0;rxant_id<nof_rx_antennas;rxant_id++) {
    for (uint32_t port_id=0;port_id<q->cell.nof_ports;port_id++) {
      if (srslte_chest_dl_estimate_port(q, input[rxant_id], ce[port_id][rxant_id], sf_idx, port_id, rxant_id)) {
        return SRSLTE_ERROR; 
      }
    }
  }
  q->last_nof_antennas = nof_rx_antennas; 
  return SRSLTE_SUCCESS;
}

int srslte_chest_dl_estimate(srslte_chest_dl_t *q, cf_t *input, cf_t *ce[SRSLTE_MAX_PORTS], uint32_t sf_idx) 
{
  uint32_t port_id; 

  for (port_id=0;port_id<q->cell.nof_ports;port_id++) {
    if (srslte_chest_dl_estimate_port(q, input, ce[port_id], sf_idx, port_id, 0)) {
      return SRSLTE_ERROR;
    }
  }
  q->last_nof_antennas = 1; 
  return SRSLTE_SUCCESS;
}

int srslte_chest_dl_estimate_multi_mbsfn(srslte_chest_dl_t *q, cf_t *input[SRSLTE_MAX_PORTS], cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], uint32_t sf_idx, uint32_t nof_rx_antennas, uint16_t mbsfn_area_id)
{
  for (uint32_t rxant_id=0;rxant_id<nof_rx_antennas;rxant_id++) {
    for (uint32_t port_id=0;port_id<q->cell.nof_ports;port_id++) {
      if (srslte_chest_dl_estimate_port_mbsfn(q, input[rxant_id], ce[port_id][rxant_id], sf_idx, port_id, rxant_id, mbsfn_area_id)) {
        return SRSLTE_ERROR;
      }
    }
  }
  q->last_nof_antennas = nof_rx_antennas;
  return SRSLTE_SUCCESS;
}

void srslte_chest_dl_set_rsrp_neighbour(srslte_chest_dl_t *q, bool rsrp_for_neighbour) {
  q->rsrp_neighbour = rsrp_for_neighbour;
}

void srslte_chest_dl_average_subframe(srslte_chest_dl_t *q, bool enable)
{
  q->average_subframe = enable;
}

void srslte_chest_dl_cfo_estimate_enable(srslte_chest_dl_t *q, bool enable, uint32_t mask)
{
  q->cfo_estimate_enable  = enable;
  q->cfo_estimate_sf_mask = mask;
}

float srslte_chest_dl_get_cfo(srslte_chest_dl_t *q) {
  return q->cfo;
}

float srslte_chest_dl_get_noise_estimate(srslte_chest_dl_t *q) {
  float n = 0; 
  for (int i=0;i<q->last_nof_antennas;i++) {
    n += srslte_vec_acc_ff(q->noise_estimate[i], q->cell.nof_ports)/q->cell.nof_ports;
  }
  if (q->last_nof_antennas) {
    n /= q->last_nof_antennas;
  }
  return n;
}

float srslte_chest_dl_get_snr(srslte_chest_dl_t *q) {
#ifdef FREQ_SEL_SNR
  int nref=SRSLTE_REFSIGNAL_NUM_SF(q->cell.nof_prb, 0);
  return srslte_vec_acc_ff(q->snr_vector, nref)/nref; 
#else
  float rsrp  = 0;
  for (int i=0;i<q->last_nof_antennas;i++) {
    for (int j=0;j<q->cell.nof_ports;j++) {
      rsrp += q->rsrp[i][j]/q->cell.nof_ports;
    }
  }
  return rsrp/srslte_chest_dl_get_noise_estimate(q);
#endif
}


float srslte_chest_dl_get_snr_ant_port(srslte_chest_dl_t *q, uint32_t ant_idx, uint32_t port_idx) {
  return srslte_chest_dl_get_rsrp_ant_port(q, ant_idx, port_idx)/srslte_chest_dl_get_noise_estimate(q);
}

float srslte_chest_dl_get_rssi(srslte_chest_dl_t *q) {
  float n = 0; 
  for (int i=0;i<q->last_nof_antennas;i++) {
   n += 4*q->rssi[i][0]/q->cell.nof_prb/SRSLTE_NRE; 
  } 
  return n/q->last_nof_antennas;
}

/* q->rssi[0] is the average power in all RE in all symbol containing references for port 0 . q->rssi[0]/q->cell.nof_prb is the average power per PRB 
 * q->rsrp[0] is the average power of RE containing references only (for port 0). 
*/ 
float srslte_chest_dl_get_rsrq(srslte_chest_dl_t *q) {
  float n = 0; 
  for (int i=0;i<q->last_nof_antennas;i++) {
    n += q->cell.nof_prb*q->rsrp[i][0] / q->rssi[i][0];
  }
  return n/q->last_nof_antennas;
  
}

float srslte_chest_dl_get_rsrq_ant_port(srslte_chest_dl_t *q, uint32_t ant_idx, uint32_t port_idx) {
  return q->cell.nof_prb*q->rsrp[ant_idx][port_idx] / q->rssi[ant_idx][port_idx];
}

float srslte_chest_dl_get_rsrp_ant_port(srslte_chest_dl_t *q, uint32_t ant_idx, uint32_t port) {
  return q->rsrp[ant_idx][port];
}

float srslte_chest_dl_get_rsrp_port(srslte_chest_dl_t *q, uint32_t port) {
  float sum = 0.0f;
  for (int j = 0; j < q->cell.nof_ports; ++j) {
    sum +=q->rsrp[port][j];
  }

  if (q->cell.nof_ports) {
    sum /= q->cell.nof_ports;
  }

  return sum;
}

float srslte_chest_dl_get_rsrp_neighbour_port(srslte_chest_dl_t *q, uint32_t port) {
  float sum = 0.0f;
  for (int j = 0; j < q->cell.nof_ports; ++j) {
    sum +=q->rsrp_corr[port][j];
  }

  if (q->cell.nof_ports) {
    sum /= q->cell.nof_ports;
  }

  return sum;
}

float srslte_chest_dl_get_rsrp(srslte_chest_dl_t *q) {
  float max = -1e9;
  for (int i = 0; i < q->last_nof_antennas; ++i) {
    float v = srslte_chest_dl_get_rsrp_port(q, i);
    if (v > max) {
      max = v;
    }
  }
  return max;
}

float srslte_chest_dl_get_rsrp_neighbour(srslte_chest_dl_t *q) {
  float max = -1e9;
  for (int i = 0; i < q->last_nof_antennas; ++i) {
    float v = srslte_chest_dl_get_rsrp_neighbour_port(q, i);
    if (v > max) {
      max = v;
    }
  }
  return max;
}
