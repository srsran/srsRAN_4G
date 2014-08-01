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

int sync_sss(sync_t *q, cf_t *input, uint32_t peak_pos) {
  uint32_t m0, m1;
  int sss_idx_n, sss_idx_e, ret;
  float m0_value_e, m1_value_e,m0_value_n, m1_value_n;
  uint32_t sf_idx_e, N_id_1_e, sf_idx_n, N_id_1_n;

  sss_synch_set_N_id_2(&q->sss, q->N_id_2);
      
  /* Make sure we have enough room to find SSS sequence */
  sss_idx_n = (int) peak_pos - 2*(q->fft_size + CP(q->fft_size, CPNORM_LEN));
  sss_idx_e = (int) peak_pos - 2*(q->fft_size + CP(q->fft_size, CPEXT_LEN));

  if (q->detect_cp) {
    if (sss_idx_n < 0 || sss_idx_e < 0) {
      INFO("Not enough room to decode SSS (%d, %d)\n", sss_idx_n, sss_idx_e);
      return LIBLTE_SUCCESS;
    }
  } else {
    if (CP_ISNORM(q->cp)) {
      if (sss_idx_n < 0) {
        INFO("Not enough room to decode normal CP SSS (sss_idx=%d, peak_pos=%d)\n", sss_idx_n, peak_pos);
        return LIBLTE_SUCCESS;
      }
    } else {
      if (sss_idx_e < 0) {
        INFO("Not enough room to decode extended CP SSS (sss_idx=%d, peak_pos=%d)\n", sss_idx_e, peak_pos);
        return LIBLTE_SUCCESS;
      }
    }
  }
      
  sf_idx_n = 0;
  sf_idx_e = 0;
  N_id_1_n = 0;
  N_id_1_e = 0;
  
  /* try Normal CP length */
  if (q->detect_cp || CP_ISNORM(q->cp)) {
    sss_synch_m0m1(&q->sss, &input[sss_idx_n], &m0, &m0_value_n, &m1, &m1_value_n);

    sf_idx_n = sss_synch_subframe(m0, m1);
    ret = sss_synch_N_id_1(&q->sss, m0, m1);
    if (ret >= 0) {
      N_id_1_n = (uint32_t) ret;
    } else {
      N_id_1_n = 1000;
    }
  }

  if (q->detect_cp || CP_ISEXT(q->cp)) {
    /* Now try Extended CP length */
    sss_synch_m0m1(&q->sss, &input[sss_idx_e], &m0, &m0_value_e, &m1, &m1_value_e);

    sf_idx_e = sss_synch_subframe(m0, m1);
    ret = sss_synch_N_id_1(&q->sss, m0, m1);
    if (ret >= 0) {
      N_id_1_e = (uint32_t) ret;
    } else {
      N_id_1_e = 1000;
    }
  }
  
  /* Correlation with extended CP hypoteshis is greater than with normal? */
  if ((q->detect_cp && m0_value_e * m1_value_e > m0_value_n * m1_value_n)
      || CP_ISEXT(q->cp)) {
    q->cp = CPEXT;
    q->sf_idx = sf_idx_e;
    q->N_id_1 = N_id_1_e;
  /* otherwise is normal CP */
  } else {
    q->cp = CPNORM;
    q->sf_idx = sf_idx_n;
    q->N_id_1 = N_id_1_n;
  }
  
  DEBUG("SSS detected N_id_1=%d, sf_idx=%d, position=%d/%d %s CP\n",
    q->N_id_1, q->sf_idx, sss_idx_n, sss_idx_e, CP_ISNORM(q->cp)?"Normal":"Extended");

  return 1;
}

int sync_find(sync_t *q, cf_t *input, uint32_t find_offset, uint32_t *peak_position) {
  
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  float peak_unnormalized, energy;
  
  if (q                 != NULL     &&
      input             != NULL     &&
      lte_N_id_2_isvalid(q->N_id_2) && 
      fft_size_isvalid(q->fft_size))
  {
    uint32_t peak_pos;
    
    if (peak_position) {
      *peak_position = 0; 
    }

    pss_synch_set_N_id_2(&q->pss, q->N_id_2);
  
    peak_pos = pss_synch_find_pss(&q->pss, &input[find_offset], &peak_unnormalized);
    
    if (q->normalize_en                                        && 
        peak_pos + find_offset               >= q->fft_size    && 
        peak_pos + find_offset + q->fft_size <= q->frame_size) 
    {
      /* Compute the energy of the received PSS sequence to normalize */
      cf_t *pss_ptr = &input[find_offset+peak_pos-q->fft_size];
      energy = sqrtf(crealf(vec_dot_prod_conj_ccc(pss_ptr, pss_ptr, q->fft_size)) / (q->fft_size));
      q->mean_energy = EXPAVERAGE(energy, q->mean_energy, q->frame_cnt);
    } else {     
      if (q->mean_energy == 0.0) {
        q->mean_energy = 1.0;
      }
      energy = q->mean_energy;
    }

    /* Normalize and compute mean peak value */
    q->peak_value = peak_unnormalized/energy;
    q->mean_peak_value = EXPAVERAGE(q->peak_value, q->mean_peak_value, q->frame_cnt);
    q->frame_cnt++;
    
    /* If peak is over threshold, compute CFO and SSS */
    if (q->peak_value                  >= q->threshold) {
      if (find_offset + peak_pos       >= q->fft_size) {
        q->cfo = pss_synch_cfo_compute(&q->pss, &input[find_offset+peak_pos-q->fft_size]);
        if (q->sss_en) {
          if (sync_sss(q, input, find_offset + peak_pos) < 0) {
            fprintf(stderr, "Error synchronizing with SSS\n");
            return LIBLTE_ERROR;
          }
        } 
      } else {
        printf("Warning: no space for CFO computation\n");
      }
      
      if (peak_position) {
        *peak_position = peak_pos;
      }
      ret = 1;
    } else {
      ret = LIBLTE_SUCCESS;
    }

    INFO("SYNC ret=%d pos=%d peak=%.2f energy=%.3f threshold=%.2f sf_idx=%d\n",
          ret, peak_pos, q->peak_value, energy, q->threshold, q->sf_idx);

  } else if (lte_N_id_2_isvalid(q->N_id_2)) {
    fprintf(stderr, "Must call sync_set_N_id_2() first!\n");
  }
  
  return ret; 
}

void sync_reset(sync_t *q) {
  q->frame_cnt = 0;
}
