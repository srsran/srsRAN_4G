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
#include <strings.h>
#include <complex.h>
#include <math.h>

#include "srslte/utils/debug.h"
#include "srslte/common/phy_common.h"
#include "srslte/sync/sync.h"
#include "srslte/utils/vector.h"
#include "srslte/sync/cfo.h"

#define MEANPEAK_EMA_ALPHA      0.2
#define CFO_EMA_ALPHA           0.9
#define CP_EMA_ALPHA            0.2

static bool fft_size_isvalid(uint32_t fft_size) {
  if (fft_size >= SRSLTE_SYNC_FFT_SZ_MIN && fft_size <= SRSLTE_SYNC_FFT_SZ_MAX && (fft_size%64) == 0) {
    return true;
  } else {
    return false;
  }
}

int srslte_sync_init(srslte_sync_t *q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size) {

  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL         &&
      frame_size        <= 307200       &&
      fft_size_isvalid(fft_size))
  {
    ret = SRSLTE_ERROR; 
    
    bzero(q, sizeof(srslte_sync_t));
    q->detect_cp = true;
    q->mean_peak_value = 0.0;
    q->sss_en = true;
    q->mean_cfo = 0; 
    q->N_id_2 = 1000; 
    q->N_id_1 = 1000;
    q->cfo_i = 0; 
    q->find_cfo_i = false; 
    q->cfo_ema_alpha = CFO_EMA_ALPHA;
    q->fft_size = fft_size;
    q->frame_size = frame_size;
    q->max_offset = max_offset;
    q->sss_alg = SSS_PARTIAL_3;
    q->enable_cfo_corr = true; 

    for (int i=0;i<2;i++) {
      q->cfo_i_corr[i] = srslte_vec_malloc(sizeof(cf_t)*q->frame_size);
      if (!q->cfo_i_corr[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }
    
    srslte_sync_set_cp(q, SRSLTE_CP_NORM);
    
    if (srslte_pss_synch_init_fft(&q->pss, max_offset, fft_size)) {
      fprintf(stderr, "Error initializing PSS object\n");
      goto clean_exit;
    }
    if (srslte_sss_synch_init(&q->sss, fft_size)) {
      fprintf(stderr, "Error initializing SSS object\n");
      goto clean_exit;
    }

    if (srslte_cfo_init(&q->cfocorr, frame_size)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }

    if (srslte_cp_synch_init(&q->cp_synch, fft_size)) {
      fprintf(stderr, "Error initiating CFO\n");
      goto clean_exit;
    }

    DEBUG("SYNC init with frame_size=%d, max_offset=%d and fft_size=%d\n", frame_size, max_offset, fft_size);
    
    ret = SRSLTE_SUCCESS;
  }  else {
    fprintf(stderr, "Invalid parameters frame_size: %d, fft_size: %d\n", frame_size, fft_size);
  }
  
clean_exit: 
  if (ret == SRSLTE_ERROR) {
    srslte_sync_free(q);
  }
  return ret;
}

void srslte_sync_free(srslte_sync_t *q) {
  if (q) {
    srslte_pss_synch_free(&q->pss);     
    srslte_sss_synch_free(&q->sss);  
    srslte_cfo_free(&q->cfocorr);
    srslte_cp_synch_free(&q->cp_synch);
    for (int i=0;i<2;i++) {
      if (q->cfo_i_corr[i]) {
        free(q->cfo_i_corr[i]);
      }
      srslte_pss_synch_free(&q->pss_i[i]);
    }
  }
}

void srslte_sync_set_threshold(srslte_sync_t *q, float threshold) {
  q->threshold = threshold;
}

void srslte_sync_cfo_i_detec_en(srslte_sync_t *q, bool enabled) {
  q->find_cfo_i = enabled;
  for (int i=0;i<2;i++) {
    int offset=(i==0)?-1:1;
    if (srslte_pss_synch_init_fft_offset(&q->pss_i[i], q->max_offset, q->fft_size, offset)) {
      fprintf(stderr, "Error initializing PSS object\n");      
    }
    for (int t=0;t<q->frame_size;t++) {
      q->cfo_i_corr[i][t] = cexpf(-2*_Complex_I*M_PI*offset*(float) t/q->fft_size);
    }
  }
}

void srslte_sync_sss_en(srslte_sync_t *q, bool enabled) {
  q->sss_en = enabled;
}

bool srslte_sync_sss_detected(srslte_sync_t *q) {
  return srslte_N_id_1_isvalid(q->N_id_1);
}

int srslte_sync_get_cell_id(srslte_sync_t *q) {
  if (srslte_N_id_2_isvalid(q->N_id_2) && srslte_N_id_1_isvalid(q->N_id_1)) {
    return q->N_id_1*3 + q->N_id_2;      
  } else {
    return -1; 
  }
}

int srslte_sync_set_N_id_2(srslte_sync_t *q, uint32_t N_id_2) {
  if (srslte_N_id_2_isvalid(N_id_2)) {
    q->N_id_2 = N_id_2;    
    return SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid N_id_2=%d\n", N_id_2);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

uint32_t srslte_sync_get_sf_idx(srslte_sync_t *q) {
  return q->sf_idx;
}

float srslte_sync_get_cfo(srslte_sync_t *q) {
  return q->mean_cfo + q->cfo_i;
}

void srslte_sync_set_cfo(srslte_sync_t *q, float cfo) {
  q->mean_cfo = cfo;
}

void srslte_sync_set_cfo_i(srslte_sync_t *q, int cfo_i) {
  q->cfo_i = cfo_i;
}

void srslte_sync_set_cfo_enable(srslte_sync_t *q, bool enable) {
  q->enable_cfo_corr = enable; 
}

void srslte_sync_set_cfo_ema_alpha(srslte_sync_t *q, float alpha) {
  q->cfo_ema_alpha = alpha;
}

float srslte_sync_get_last_peak_value(srslte_sync_t *q) {
  return q->peak_value;
}

float srslte_sync_get_peak_value(srslte_sync_t *q) {
  return q->mean_peak_value;
}

void srslte_sync_cp_en(srslte_sync_t *q, bool enabled) {
  q->detect_cp = enabled;
}

bool srslte_sync_sss_is_en(srslte_sync_t *q) {
  return q->sss_en;
}

void srslte_sync_set_em_alpha(srslte_sync_t *q, float alpha) {
  srslte_pss_synch_set_ema_alpha(&q->pss, alpha);
}

srslte_cp_t srslte_sync_get_cp(srslte_sync_t *q) {
  return q->cp;
}
void srslte_sync_set_cp(srslte_sync_t *q, srslte_cp_t cp) {
  q->cp = cp;
  q->cp_len = SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_LEN_NORM(1,q->fft_size):SRSLTE_CP_LEN_EXT(q->fft_size);
  q->nof_symbols = q->frame_size/(q->fft_size+q->cp_len)-1;
}

void srslte_sync_set_sss_algorithm(srslte_sync_t *q, sss_alg_t alg) {
  q->sss_alg = alg; 
}

/* CP detection algorithm taken from: 
 * "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 * by Jung-In Kim et al. 
 */
srslte_cp_t srslte_sync_detect_cp(srslte_sync_t *q, cf_t *input, uint32_t peak_pos) 
{
  float R_norm, R_ext, C_norm, C_ext; 
  float M_norm=0, M_ext=0; 
  
  uint32_t cp_norm_len = SRSLTE_CP_LEN_NORM(7, q->fft_size);
  uint32_t cp_ext_len = SRSLTE_CP_LEN_EXT(q->fft_size);
 
  cf_t *input_cp_norm = &input[peak_pos-2*(q->fft_size+cp_norm_len)]; 
  cf_t *input_cp_ext = &input[peak_pos-2*(q->fft_size+cp_ext_len)]; 

  for (int i=0;i<2;i++) {
    R_norm  = crealf(srslte_vec_dot_prod_conj_ccc(&input_cp_norm[q->fft_size], input_cp_norm, cp_norm_len));    
    C_norm  = cp_norm_len * srslte_vec_avg_power_cf(input_cp_norm, cp_norm_len);    
    input_cp_norm += q->fft_size+cp_norm_len;
    M_norm += R_norm/C_norm;
  }
  
  q->M_norm_avg = SRSLTE_VEC_EMA(M_norm/2, q->M_norm_avg, CP_EMA_ALPHA);
  
  for (int i=0;i<2;i++) {
    R_ext  = crealf(srslte_vec_dot_prod_conj_ccc(&input_cp_ext[q->fft_size], input_cp_ext, cp_ext_len));
    C_ext  = cp_ext_len * srslte_vec_avg_power_cf(input_cp_ext, cp_ext_len);
    input_cp_ext += q->fft_size+cp_ext_len;
    if (C_ext > 0) {
      M_ext += R_ext/C_ext;      
    }
  }

  q->M_ext_avg = SRSLTE_VEC_EMA(M_ext/2, q->M_ext_avg, CP_EMA_ALPHA);

  if (q->M_norm_avg > q->M_ext_avg) {
    return SRSLTE_CP_NORM;    
  } else if (q->M_norm_avg < q->M_ext_avg) {
    return SRSLTE_CP_EXT;
  } else {
    if (R_norm > R_ext) {
      return SRSLTE_CP_NORM;
    } else {
      return SRSLTE_CP_EXT;
    }
  }
}

/* Returns 1 if the SSS is found, 0 if not and -1 if there is not enough space 
 * to correlate
 */
int sync_sss(srslte_sync_t *q, cf_t *input, uint32_t peak_pos, srslte_cp_t cp) {
  int sss_idx, ret;

  srslte_sss_synch_set_N_id_2(&q->sss, q->N_id_2);

  /* Make sure we have enough room to find SSS sequence */
  sss_idx = (int) peak_pos-2*q->fft_size-SRSLTE_CP_LEN(q->fft_size, (SRSLTE_CP_ISNORM(q->cp)?SRSLTE_CP_NORM_LEN:SRSLTE_CP_EXT_LEN));
  if (sss_idx < 0) {
    DEBUG("Not enough room to decode CP SSS (sss_idx=%d, peak_pos=%d)\n", sss_idx, peak_pos);
    return SRSLTE_ERROR;
  }
  DEBUG("Searching SSS around sss_idx: %d, peak_pos: %d\n", sss_idx, peak_pos);
      
  switch(q->sss_alg) {
    case SSS_DIFF:
      srslte_sss_synch_m0m1_diff(&q->sss, &input[sss_idx], &q->m0, &q->m0_value, &q->m1, &q->m1_value);
      break;
    case SSS_PARTIAL_3:
      srslte_sss_synch_m0m1_partial(&q->sss, &input[sss_idx], 3, NULL, &q->m0, &q->m0_value, &q->m1, &q->m1_value);
      break;
    case SSS_FULL:
      srslte_sss_synch_m0m1_partial(&q->sss, &input[sss_idx], 1, NULL, &q->m0, &q->m0_value, &q->m1, &q->m1_value);
      break;
  }

  q->sf_idx = srslte_sss_synch_subframe(q->m0, q->m1);
  ret = srslte_sss_synch_N_id_1(&q->sss, q->m0, q->m1);
  if (ret >= 0) {
    q->N_id_1 = (uint32_t) ret;
    DEBUG("SSS detected N_id_1=%d, sf_idx=%d, %s CP\n",
      q->N_id_1, q->sf_idx, SRSLTE_CP_ISNORM(q->cp)?"Normal":"Extended");
    return 1;
  } else {
    q->N_id_1 = 1000;
    return SRSLTE_SUCCESS;
  }
}

srslte_pss_synch_t* srslte_sync_get_cur_pss_obj(srslte_sync_t *q) {
 srslte_pss_synch_t *pss_obj[3] = {&q->pss_i[0], &q->pss, &q->pss_i[1]};
 return pss_obj[q->cfo_i+1];
}

/** Finds the PSS sequence previously defined by a call to srslte_sync_set_N_id_2()
 * around the position find_offset in the buffer input. 
 * Returns 1 if the correlation peak exceeds the threshold set by srslte_sync_set_threshold() 
 * or 0 otherwise. Returns a negative number on error (if N_id_2 has not been set) 
 * 
 * The maximum of the correlation peak is always stored in *peak_position
 */
int srslte_sync_find(srslte_sync_t *q, cf_t *input, uint32_t find_offset, uint32_t *peak_position) 
{
  
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL     &&
      input             != NULL     &&
      srslte_N_id_2_isvalid(q->N_id_2) && 
      fft_size_isvalid(q->fft_size))
  {
    int peak_pos;
    float correct_cfo = 0; 
    
    ret = SRSLTE_SUCCESS; 
    
    if (peak_position) {
      *peak_position = 0; 
    }

    /* Estimate CFO using CP */
    if (q->enable_cfo_corr) {
      uint32_t cp_offset = srslte_cp_synch(&q->cp_synch, input, q->nof_symbols, q->nof_symbols, SRSLTE_CP_LEN_NORM(1,q->fft_size));
      cf_t cp_corr_max = srslte_cp_synch_corr_output(&q->cp_synch, cp_offset);
      float cfo = -carg(cp_corr_max) / M_PI / 2; 
      
      /* compute cumulative moving average CFO */
      DEBUG("cp_offset_pos=%d, abs=%f, cfo=%f, mean_cfo=%f, nof_symb=%d\n", 
            cp_offset, cabs(cp_corr_max), cfo, q->mean_cfo, q->nof_symbols);
      if (q->mean_cfo) {
        q->mean_cfo = SRSLTE_VEC_EMA(cfo, q->mean_cfo, q->cfo_ema_alpha);
      } else {
        q->mean_cfo = cfo;
      }
      
      /* Correct CFO with the averaged CFO estimation */
      correct_cfo = q->mean_cfo;
      if (!q->find_cfo_i) {
        correct_cfo = q->cfo_i + q->mean_cfo;
        DEBUG("cfo_i disabled, correct_cfo=%d+%f=%f\n",q->cfo_i, q->mean_cfo, correct_cfo);
      }
      srslte_cfo_correct(&q->cfocorr, input, input, -correct_cfo / q->fft_size);                 
    }
    
    srslte_pss_synch_set_N_id_2(&q->pss, q->N_id_2);
  
    if (q->find_cfo_i && q->enable_cfo_corr) {
      float peak_value; 
      float max_peak_value = -99;
      peak_pos = 0; 
      srslte_pss_synch_t *pss_obj[3] = {&q->pss_i[0], &q->pss, &q->pss_i[1]};
      for (int cfo_i=0;cfo_i<3;cfo_i++) {
        srslte_pss_synch_set_N_id_2(pss_obj[cfo_i], q->N_id_2);
        int p = srslte_pss_synch_find_pss(pss_obj[cfo_i], &input[find_offset], &peak_value);
        if (peak_value > max_peak_value) {
          max_peak_value = peak_value;
          peak_pos = p; 
          q->peak_value = peak_value;
          q->cfo_i = cfo_i-1;
        }
      }      
      if (q->cfo_i != 0) {
        correct_cfo = q->cfo_i+q->mean_cfo;
        srslte_vec_prod_ccc(input, q->cfo_i_corr[q->cfo_i<0?0:1], input, q->frame_size);
        DEBUG("Compensating cfo_i=%d, total_cfo=%f\n", q->cfo_i, correct_cfo);
      }
    } else {
      peak_pos = srslte_pss_synch_find_pss(&q->pss, &input[find_offset], &q->peak_value);
      if (peak_pos < 0) {
        fprintf(stderr, "Error calling finding PSS sequence\n");
        return SRSLTE_ERROR; 
      }      
      if (q->max_offset<128) {
        float temp[128];
        srslte_vec_sc_prod_fff(q->pss.conv_output_avg, 1e8, temp, q->max_offset);
        srslte_vec_fprint_f(stdout, temp, q->max_offset);
      }
    }
    q->mean_peak_value = SRSLTE_VEC_EMA(q->peak_value, q->mean_peak_value, MEANPEAK_EMA_ALPHA);

    if (peak_position) {
      *peak_position = (uint32_t) peak_pos;
    }
    
    /* If peak is over threshold, compute CFO and SSS */
    if (q->peak_value >= q->threshold) {
      
      // Try to detect SSS 
      if (q->sss_en) {
        // Set an invalid N_id_1 indicating SSS is yet to be detected
        q->N_id_1 = 1000; 
        
        if (sync_sss(q, input, find_offset + peak_pos, q->cp) < 0) {
          DEBUG("No space for SSS processing. Frame starts at %d\n", peak_pos);
        }
      }
      
      if (q->detect_cp) {
        if (peak_pos + find_offset >= 2*(q->fft_size + SRSLTE_CP_LEN_EXT(q->fft_size))) {
          srslte_sync_set_cp(q, srslte_sync_detect_cp(q, input, peak_pos + find_offset));
        } else {
          DEBUG("Not enough room to detect CP length. Peak position: %d\n", peak_pos);
        }
      }
  
      // Return 1 (peak detected) even if we couldn't estimate CFO and SSS
      ret = 1;
    } else {
      ret = 0;
    }
    
    DEBUG("SYNC ret=%d N_id_2=%d find_offset=%d pos=%d peak=%.2f threshold=%.2f sf_idx=%d, CFO=%.3f KHz\n",
         ret, q->N_id_2, find_offset, peak_pos, q->peak_value, q->threshold, q->sf_idx, 15*correct_cfo);

  } else if (srslte_N_id_2_isvalid(q->N_id_2)) {
    fprintf(stderr, "Must call srslte_sync_set_N_id_2() first!\n");
  }
  
  return ret; 
}

void srslte_sync_reset(srslte_sync_t *q) {
  q->M_ext_avg = 0; 
  q->M_norm_avg = 0; 
  srslte_pss_synch_reset(&q->pss);
}
