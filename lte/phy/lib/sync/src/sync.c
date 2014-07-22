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

int sync_init(sync_t *q, uint32_t find_frame_size, uint32_t track_frame_size, uint32_t fft_size) {

  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL         &&
      find_frame_size   > fft_size &&
      find_frame_size   < 307200        &&
      fft_size_isvalid(fft_size))
  {
    bzero(q, sizeof(sync_t));
    q->pss_mode = PEAK_MEAN;
    q->detect_cp = true;
    q->sss_en = true;
    q->N_id_2 = 1000; 
    q->N_id_1 = 1000;
    q->fft_size = fft_size;
    q->find_frame_size = find_frame_size;
    
    if (pss_synch_init_fft(&q->pss_find, find_frame_size, fft_size)) {
      fprintf(stderr, "Error initializing PSS object\n");
      return LIBLTE_ERROR;
    }
    if (sss_synch_init(&q->sss, fft_size)) {
      fprintf(stderr, "Error initializing SSS object\n");
      return LIBLTE_ERROR;
    }
    if (pss_synch_init_fft(&q->pss_track, track_frame_size, fft_size)) {
      fprintf(stderr, "Error initializing PSS track object\n");
      return LIBLTE_ERROR;
    }      

    DEBUG("SYNC init with find_frame_size=%d and fft_size=%d\n", find_frame_size, fft_size);
    
    ret = LIBLTE_SUCCESS;
  }  
  return ret;
}

int sync_realloc(sync_t *q, uint32_t find_frame_size, uint32_t track_frame_size, 
                 uint32_t fft_size)
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL         &&
      find_frame_size   > fft_size &&
      find_frame_size   < 307200        &&
      fft_size_isvalid(fft_size))
  {
    q->N_id_2 = 1000; 
    q->N_id_1 = 1000;
    q->fft_size = fft_size;
    q->find_frame_size = find_frame_size;
    
    pss_synch_free(&q->pss_find);
    if (pss_synch_init_fft(&q->pss_find, find_frame_size, fft_size)) {
      fprintf(stderr, "Error initializing PSS object\n");
      return LIBLTE_ERROR;
    }
   
    pss_synch_free(&q->pss_track);      
    if (pss_synch_init_fft(&q->pss_track, track_frame_size, fft_size)) {
      fprintf(stderr, "Error initializing PSS track object\n");
      return LIBLTE_ERROR;
    }
    
    if (sss_synch_realloc(&q->sss, fft_size)) {
      fprintf(stderr, "Error realloc'ing SSS object\n");
      return LIBLTE_ERROR;
    }

    DEBUG("SYNC init with find_frame_size=%d and fft_size=%d\n", find_frame_size, fft_size);
    
    ret = LIBLTE_SUCCESS;
  }
  return ret; 
}

void sync_free(sync_t *q) {
  if (q) {
    pss_synch_free(&q->pss_track);      
    pss_synch_free(&q->pss_find);
    sss_synch_free(&q->sss);  
  }
}

void sync_pss_det_absolute(sync_t *q) {
  q->pss_mode = ABSOLUTE;
}
void sync_pss_det_peak_to_avg(sync_t *q) {
  q->pss_mode = PEAK_MEAN;
}

void sync_set_threshold(sync_t *q, float find_threshold, float track_threshold) {
  q->find_threshold = find_threshold;
  q->track_threshold = track_threshold;
}

void sync_sss_en(sync_t *q, bool enabled) {
  q->sss_en = enabled;
}

bool sync_sss_detected(sync_t *q) {
  return lte_N_id_1_isvalid(q->N_id_1);
}

int sync_get_cell_id(sync_t *q) {
  if (q->N_id_2 != 10) {
    if (lte_N_id_2_isvalid(q->N_id_2) && lte_N_id_1_isvalid(q->N_id_1)) {
      return q->N_id_1*3 + q->N_id_2;      
    } else {
      fprintf(stderr, "Error getting cell_id, invalid N_id_1 or N_id_2\n");
      return LIBLTE_ERROR;
    }
  } else {
    fprintf(stderr, "Error getting cell_id, N_id_2 not set\n");
    return LIBLTE_ERROR;
  }
}

uint32_t sync_get_N_id_1(sync_t *q) {
  return q->N_id_1;
}

uint32_t sync_get_N_id_2(sync_t *q) {
  return q->N_id_2;
}

uint32_t sync_get_slot_id(sync_t *q) {
  return q->slot_id;
}

float sync_get_cfo(sync_t *q) {
  return q->cfo;
}

float sync_get_peak_value(sync_t *q) {
  return q->peak_value;
}

void sync_cp_en(sync_t *q, bool enabled) {
  q->detect_cp = enabled;
}

lte_cp_t sync_get_cp(sync_t *q) {
  return q->cp;
}

int sync_sss(sync_t *q, cf_t *input, uint32_t peak_pos, bool en_cp) {
  uint32_t m0, m1;
  int sss_idx_n, sss_idx_e, ret;
  float m0_value_e, m1_value_e,m0_value_n, m1_value_n;
  uint32_t slot_id_e, N_id_1_e, slot_id_n, N_id_1_n;

  sss_synch_set_N_id_2(&q->sss, q->N_id_2);
      
  /* Make sure we have enough room to find SSS sequence */
  sss_idx_n = (int) peak_pos - 2*(q->fft_size + CP(q->fft_size, CPNORM_LEN));
  sss_idx_e = (int) peak_pos - 2*(q->fft_size + CP(q->fft_size, CPEXT_LEN));

  if (en_cp) {
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
      
  slot_id_n = 0;
  slot_id_e = 0;
  N_id_1_n = 0;
  N_id_1_e = 0;
  
  /* try Normal CP length */
  if (en_cp || CP_ISNORM(q->cp)) {
    sss_synch_m0m1(&q->sss, &input[sss_idx_n], &m0, &m0_value_n, &m1, &m1_value_n);

    slot_id_n = 2 * sss_synch_subframe(m0, m1);
    ret = sss_synch_N_id_1(&q->sss, m0, m1);
    if (ret >= 0) {
      N_id_1_n = (uint32_t) ret;
    } else {
      N_id_1_n = 1000;
    }
  }

  if (en_cp || CP_ISEXT(q->cp)) {
    /* Now try Extended CP length */
    sss_synch_m0m1(&q->sss, &input[sss_idx_e], &m0, &m0_value_e, &m1, &m1_value_e);

    slot_id_e = 2 * sss_synch_subframe(m0, m1);
    ret = sss_synch_N_id_1(&q->sss, m0, m1);
    if (ret >= 0) {
      N_id_1_e = (uint32_t) ret;
    } else {
      N_id_1_e = 1000;
    }
  }
  
  /* Correlation with extended CP hypoteshis is greater than with normal? */
  if ((en_cp && m0_value_e * m1_value_e > m0_value_n * m1_value_n)
      || CP_ISEXT(q->cp)) {
    q->cp = CPEXT;
    q->slot_id = slot_id_e;
    q->N_id_1 = N_id_1_e;
  /* otherwise is normal CP */
  } else {
    q->cp = CPNORM;
    q->slot_id = slot_id_n;
    q->N_id_1 = N_id_1_n;
  }
  
  DEBUG("SSS detected N_id_1=%d, slot_idx=%d, position=%d/%d %s CP\n",
    q->N_id_1, q->slot_id, sss_idx_n, sss_idx_e, CP_ISNORM(q->cp)?"Normal":"Extended");

  return 1;
}

int sync_track(sync_t *q, cf_t *input, uint32_t offset, uint32_t *peak_position) {
  
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q                 != NULL   &&
      input             != NULL   && 
      fft_size_isvalid(q->fft_size))
  {
    float peak_value, mean_value, *mean_ptr;
    uint32_t peak_pos;

    pss_synch_set_N_id_2(&q->pss_track, q->N_id_2);
    
    if (q->pss_mode == ABSOLUTE) {
      mean_ptr = NULL;
    } else {
      mean_ptr = &mean_value;
    }
    
    peak_pos = pss_synch_find_pss(&q->pss_track, &input[offset], &peak_value, mean_ptr);

    if (q->pss_mode == ABSOLUTE) {
      q->peak_value = peak_value; 
    } else {
      q->peak_value = peak_value / mean_value;
    }
    
    DEBUG("PSS possible tracking peak pos=%d peak=%.2f threshold=%.2f\n",
          peak_pos, peak_value, q->track_threshold);
    
    if (q->peak_value           > q->track_threshold) {
      if (offset + peak_pos       > q->fft_size) {
        q->cfo = pss_synch_cfo_compute(&q->pss_track, &input[offset+peak_pos-q->fft_size]);
        if (q->sss_en) {
          if (sync_sss(q, input, offset + peak_pos, false) < 0) {
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
  }
  
  return ret; 
}

int sync_find(sync_t *q, cf_t *input, uint32_t *peak_position) {
  uint32_t N_id_2, peak_pos[3];
  float peak_value[3];
  float mean_value[3];
  float max=-999;
  uint32_t i;
  int ret; 
  float *mean_ptr; 
  
  for (N_id_2=0;N_id_2<3;N_id_2++) {
     if (q->pss_mode == ABSOLUTE) {
      mean_ptr = NULL;
    } else {
      mean_ptr = &mean_value[N_id_2];
    }
    pss_synch_set_N_id_2(&q->pss_find, N_id_2);
    ret = pss_synch_find_pss(&q->pss_find, input, &peak_value[N_id_2], mean_ptr);
    if (ret < 0) {
      fprintf(stderr, "Error finding PSS for N_id_2=%d\n", N_id_2);
      return LIBLTE_ERROR;
    } 
    peak_pos[N_id_2] = (uint32_t) ret;
    
  }
  for (i=0;i<3;i++) {
    if (peak_value[i] > max) {
      max = peak_value[i];
      N_id_2 = i;
    }
  }

  if (q->pss_mode == ABSOLUTE) {
    q->peak_value = peak_value[N_id_2];
  } else {
    q->peak_value = peak_value[N_id_2] / mean_value[N_id_2];
  }
 
  if (peak_position) {
    *peak_position = 0;
  }

  DEBUG("PSS possible peak N_id_2=%d, pos=%d peak=%.2f threshold=%.2f\n",
      N_id_2, peak_pos[N_id_2], peak_value[N_id_2], q->find_threshold);

  /* If peak detected */
  if (q->peak_value                   > q->find_threshold) {
    if (peak_pos[N_id_2]                > q->fft_size             && 
        peak_pos[N_id_2] + q->fft_size  < q->find_frame_size) 
    {
      q->N_id_2 = N_id_2;
      pss_synch_set_N_id_2(&q->pss_find, q->N_id_2);
      q->cfo = pss_synch_cfo_compute(&q->pss_find, &input[peak_pos[N_id_2]-q->fft_size]);
      
      DEBUG("PSS peak detected N_id_2=%d, pos=%d peak=%.2f par=%.2f th=%.2f cfo=%.4f\n", N_id_2,
          peak_pos[N_id_2], peak_value[N_id_2], q->peak_value, q->find_threshold, q->cfo);

      if (q->sss_en) {     
        if (sync_sss(q, input, peak_pos[q->N_id_2], q->detect_cp) < 0) {
          fprintf(stderr, "Error synchronizing with SSS\n");
          return LIBLTE_ERROR;
        }
      }
    } 

    if (peak_position) {
      *peak_position = peak_pos[N_id_2];
    }

    return 1;
  } else {
    return LIBLTE_SUCCESS;
  }
}
