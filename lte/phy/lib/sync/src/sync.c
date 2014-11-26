/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <strings.h>
#include <complex.h>
#include <math.h>

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/sync/sync.h"
#include "liblte/phy/utils/vector.h"

#define MEANENERGY_EMA_ALPHA    0.5
#define MEANPEAK_EMA_ALPHA      0.2


static bool fft_size_isvalid(uint32_t fft_size) {
  if (fft_size >= FFT_SIZE_MIN && fft_size <= FFT_SIZE_MAX && (fft_size%64) == 0) {
    return true;
  } else {
    return false;
  }
}

int sync_init(sync_t *q, uint32_t frame_size, uint32_t fft_size) {

  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL         &&
      frame_size        >= fft_size     &&
      frame_size        <= 307200       &&
      fft_size_isvalid(fft_size))
  {
    bzero(q, sizeof(sync_t));
    q->detect_cp = true;
    q->normalize_en = true; 
    q->mean_energy = 0.0;
    q->mean_peak_value = 0.0;
    q->sss_en = true;
    q->N_id_2 = 1000; 
    q->N_id_1 = 1000;
    q->fft_size = fft_size;
    q->frame_size = frame_size;
    
    if (pss_synch_init_fft(&q->pss, frame_size, fft_size)) {
      fprintf(stderr, "Error initializing PSS object\n");
      return LIBLTE_ERROR;
    }
    if (sss_synch_init(&q->sss, fft_size)) {
      fprintf(stderr, "Error initializing SSS object\n");
      return LIBLTE_ERROR;
    }

    DEBUG("SYNC init with frame_size=%d and fft_size=%d\n", frame_size, fft_size);
    
    ret = LIBLTE_SUCCESS;
  }  else {
    fprintf(stderr, "Invalid parameters frame_size: %d, fft_size: %d\n", frame_size, fft_size);
  }
  return ret;
}

void sync_free(sync_t *q) {
  if (q) {
    pss_synch_free(&q->pss);     
    sss_synch_free(&q->sss);  
  }
}

void sync_set_threshold(sync_t *q, float threshold) {
  q->threshold = threshold;
}

void sync_sss_en(sync_t *q, bool enabled) {
  q->sss_en = enabled;
}

void sync_normalize_en(sync_t *q, bool enable) {
  q->normalize_en = enable; 
}

bool sync_sss_detected(sync_t *q) {
  return lte_N_id_1_isvalid(q->N_id_1);
}

int sync_get_cell_id(sync_t *q) {
  if (lte_N_id_2_isvalid(q->N_id_2) && lte_N_id_1_isvalid(q->N_id_1)) {
    return q->N_id_1*3 + q->N_id_2;      
  } else {
    fprintf(stderr, "Error getting cell_id, invalid N_id_1 or N_id_2\n");
    return LIBLTE_ERROR;
  }
}

int sync_set_N_id_2(sync_t *q, uint32_t N_id_2) {
  if (lte_N_id_2_isvalid(N_id_2)) {
    q->N_id_2 = N_id_2;    
    return LIBLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid N_id_2=%d\n", N_id_2);
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

uint32_t sync_get_sf_idx(sync_t *q) {
  return q->sf_idx;
}

float sync_get_cfo(sync_t *q) {
  return q->cfo;
}

float sync_get_last_peak_value(sync_t *q) {
  return q->peak_value;
}

float sync_get_peak_value(sync_t *q) {
  return q->mean_peak_value;
}

void sync_cp_en(sync_t *q, bool enabled) {
  q->detect_cp = enabled;
}

lte_cp_t sync_get_cp(sync_t *q) {
  return q->cp;
}
void sync_set_cp(sync_t *q, lte_cp_t cp) {
  q->cp = cp;
}

/* CP detection algorithm taken from: 
 * "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 * by Jung-In Kim et al. 
 */
static lte_cp_t detect_cp(sync_t *q, cf_t *input, uint32_t peak_pos) 
{
  float R_norm, R_ext, C_norm, C_ext; 
  float M_norm, M_ext; 
  
  R_norm = crealf(vec_dot_prod_conj_ccc(&input[peak_pos-q->fft_size-CP_NORM(7, q->fft_size)], 
                                        &input[peak_pos-CP_NORM(7, q->fft_size)], 
                                        CP_NORM(7, q->fft_size)));    
  C_norm = cabsf(vec_dot_prod_conj_ccc(&input[peak_pos-q->fft_size-CP_NORM(7, q->fft_size)], 
                                       &input[peak_pos-q->fft_size-CP_NORM(7, q->fft_size)], 
                                       CP_NORM(7, q->fft_size)));
  R_ext  = crealf(vec_dot_prod_conj_ccc(&input[peak_pos-q->fft_size-CP_EXT(q->fft_size)], 
                                        &input[peak_pos-CP_EXT(q->fft_size)], 
                                        CP_EXT(q->fft_size)));
  C_ext  = cabsf(vec_dot_prod_conj_ccc(&input[peak_pos-q->fft_size-CP_EXT(q->fft_size)], 
                                       &input[peak_pos-q->fft_size-CP_EXT(q->fft_size)], 
                                       CP_EXT(q->fft_size)));
  M_norm = R_norm/C_norm;
  M_ext = R_ext/C_ext;

  if (M_norm > M_ext) {
    return CPNORM;    
  } else if (M_norm < M_ext) {
    return CPEXT;
  } else {
    if (R_norm > R_ext) {
      return CPNORM;
    } else {
      return CPEXT;
    }
  }
}

/* Returns 1 if the SSS is found, 0 if not and -1 if there is not enough space 
 * to correlate
 */
int sync_sss(sync_t *q, cf_t *input, uint32_t peak_pos) {
  int sss_idx, ret;

  sss_synch_set_N_id_2(&q->sss, q->N_id_2);

  if (q->detect_cp) {
    if (peak_pos - q->fft_size - CP_EXT(q->fft_size) >= 0) {
      q->cp = detect_cp(q, input, peak_pos);
    } else {
      INFO("Not enough room to detect CP length. Peak position: %d\n", peak_pos);
      return LIBLTE_ERROR; 
    }
  }
  
  /* Make sure we have enough room to find SSS sequence */
  sss_idx = (int) peak_pos - 2*(q->fft_size + CP(q->fft_size, q->cp));
  if (sss_idx < 0) {
    INFO("Not enough room to decode CP SSS (sss_idx=%d, peak_pos=%d)\n", sss_idx, peak_pos);
    return LIBLTE_ERROR;
  }
      
  sss_synch_m0m1(&q->sss, &input[sss_idx], &q->m0, &q->m0_value, &q->m1, &q->m1_value);

  q->sf_idx = sss_synch_subframe(q->m0, q->m1);
  ret = sss_synch_N_id_1(&q->sss, q->m0, q->m1);
  if (ret >= 0) {
    q->N_id_1 = (uint32_t) ret;
    DEBUG("SSS detected N_id_1=%d, sf_idx=%d, %s CP\n",
      q->N_id_1, q->sf_idx, CP_ISNORM(q->cp)?"Normal":"Extended");
    return 1;
  } else {
    q->N_id_1 = 1000;
    return LIBLTE_SUCCESS;
  }
}


/** Finds the PSS sequence previously defined by a call to sync_set_N_id_2()
 * around the position find_offset in the buffer input. 
 * Returns 1 if the correlation peak exceeds the threshold set by sync_set_threshold() 
 * or 0 otherwise. Returns a negative number on error (if N_id_2 has not been set) 
 * 
 * The maximum of the correlation peak is always stored in *peak_position
 */
int sync_find(sync_t *q, cf_t *input, uint32_t find_offset, uint32_t *peak_position) 
{
  
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  float peak_unnormalized=0, energy=1;
  
  if (q                 != NULL     &&
      input             != NULL     &&
      lte_N_id_2_isvalid(q->N_id_2) && 
      fft_size_isvalid(q->fft_size))
  {
    int peak_pos;
    
    ret = LIBLTE_SUCCESS; 
    
    if (peak_position) {
      *peak_position = 0; 
    }

    pss_synch_set_N_id_2(&q->pss, q->N_id_2);
  
    peak_pos = pss_synch_find_pss(&q->pss, &input[find_offset], &peak_unnormalized);
    if (peak_pos < 0) {
      fprintf(stderr, "Error calling finding PSS sequence\n");
      return LIBLTE_ERROR; 
    }
    if (q->normalize_en                         && 
        peak_pos + find_offset >= q->fft_size ) 
    {
      /* Compute the energy of the received PSS sequence to normalize */
      energy = sqrtf(vec_avg_power_cf(&input[find_offset+peak_pos-q->fft_size], q->fft_size));
      q->mean_energy = VEC_EMA(energy, q->mean_energy, MEANENERGY_EMA_ALPHA);
    } else {     
      if (q->mean_energy == 0.0) {
        energy = 1.0;
      } else {
        energy = q->mean_energy;        
      }
    }

    /* Normalize and compute mean peak value */
    if (q->mean_energy) {
      q->peak_value = peak_unnormalized/q->mean_energy;      
    } else {
      q->peak_value = peak_unnormalized/energy;
    }
    q->mean_peak_value = VEC_EMA(q->peak_value, q->mean_peak_value, MEANPEAK_EMA_ALPHA);
    q->frame_cnt++;

    if (peak_position) {
      *peak_position = (uint32_t) peak_pos;
    }
    
    /* If peak is over threshold, compute CFO and SSS */
    if (q->peak_value >= q->threshold) {
      
      // Set an invalid N_id_1 indicating SSS is yet to be detected
      q->N_id_1 = 1000; 
      
      // Try to detect SSS 
      if (q->sss_en) {
        if (sync_sss(q, input, find_offset + peak_pos) < 0) {
          INFO("No space for SSS processing. Frame starts at %d\n", peak_pos);
        }
      }
      // Make sure we have enough space to estimate CFO
      if (peak_pos + find_offset >= q->fft_size) {
        q->cfo = pss_synch_cfo_compute(&q->pss, &input[find_offset+peak_pos-q->fft_size]);
      } else {
        INFO("No space for CFO computation. Frame starts at \n",peak_pos);
      }
      // Return 1 (peak detected) even if we couldn't estimate CFO and SSS
      ret = 1;
    } else {
      ret = 0;
    }
    
    INFO("SYNC ret=%d N_id_2=%d pos=%d peak=%.2f/%.2f=%.2f mean_energy=%.2f threshold=%.2f sf_idx=%d, CFO=%.3f KHz\n",
          ret, q->N_id_2, peak_pos, peak_unnormalized*1000,energy*1000,q->peak_value, q->mean_energy*1000, 
         q->threshold, q->sf_idx, 15*q->cfo);

  } else if (lte_N_id_2_isvalid(q->N_id_2)) {
    fprintf(stderr, "Must call sync_set_N_id_2() first!\n");
  }
  
  return ret; 
}

void sync_reset(sync_t *q) {
  q->frame_cnt = 0;
}
