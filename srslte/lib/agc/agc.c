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


#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "srslte/utils/debug.h"

#include "srslte/agc/agc.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"

int srslte_agc_init (srslte_agc_t *q, srslte_agc_mode_t mode) {
  return srslte_agc_init_acc(q, mode, 0);
}

int srslte_agc_init_acc(srslte_agc_t *q, srslte_agc_mode_t mode, uint32_t nof_frames) {
  bzero(q, sizeof(srslte_agc_t));
  q->mode = mode; 
  q->nof_frames = nof_frames;  
  if (nof_frames > 0) {
    q->y_tmp = srslte_vec_malloc(sizeof(float) * nof_frames);
    if (!q->y_tmp) {
      return SRSLTE_ERROR; 
    }
  } else {
    q->y_tmp = NULL; 
  }
  q->target = SRSLTE_AGC_DEFAULT_TARGET; 
  srslte_agc_reset(q);
  return SRSLTE_SUCCESS;
}

int srslte_agc_init_uhd(srslte_agc_t *q, srslte_agc_mode_t mode, uint32_t nof_frames, double (set_gain_callback)(void*, double), void *uhd_handler) {
  if (!srslte_agc_init_acc(q, mode, nof_frames)) {
    q->set_gain_callback = set_gain_callback;
    q->uhd_handler = uhd_handler;    
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR; 
  }
}

void srslte_agc_free(srslte_agc_t *q) {
  if (q->y_tmp) {
    free(q->y_tmp);
  }
  bzero(q, sizeof(srslte_agc_t));
}

void srslte_agc_reset(srslte_agc_t *q) {
  q->bandwidth = SRSLTE_AGC_DEFAULT_BW;
  q->lock = false; 
  q->gain = pow(10,50/10);
  q->y_out = 1.0; 
  q->isfirst = true; 
  if (q->set_gain_callback && q->uhd_handler) {
    q->set_gain_callback(q->uhd_handler, 10*log10(q->gain));
  }
}

void srslte_agc_set_bandwidth(srslte_agc_t *q, float bandwidth) {
  q->bandwidth = bandwidth;
}

void srslte_agc_set_target(srslte_agc_t *q, float target) {
  q->target = target;
}

float srslte_agc_get_rssi(srslte_agc_t *q) {
  return q->target/q->gain;
}

float srslte_agc_get_output_level(srslte_agc_t *q) {
  return q->y_out;
}

float srslte_agc_get_gain(srslte_agc_t *q) {
  return q->gain;
}

void srslte_agc_set_gain(srslte_agc_t *q, float init_gain_value) {
  q->gain = init_gain_value;
}

void srslte_agc_lock(srslte_agc_t *q, bool enable) {
  q->lock = enable;
}

void srslte_agc_process(srslte_agc_t *q, cf_t *signal, uint32_t len) {
  if (!q->lock) {
    float gain_db = 10*log10(q->gain); 
    float gain_uhd_db = 1.0;
    //float gain_uhd = 1.0;  
    float y = 0; 
    // Apply current gain to input signal 
    if (!q->uhd_handler) {
      srslte_vec_sc_prod_cfc(signal, q->gain, signal, len);
    } else {
      if (gain_db < 0) {
        gain_db = 0.0; 
      }
      if (isinf(gain_db) || isnan(gain_db)) {
        gain_db = 10.0; 
      } else {
        gain_uhd_db = q->set_gain_callback(q->uhd_handler, gain_db);        
        q->gain = pow(10, gain_uhd_db/10);
      }
    }
    float *t; 
    switch(q->mode) {
      case SRSLTE_AGC_MODE_ENERGY:
        y = sqrtf(crealf(srslte_vec_dot_prod_conj_ccc(signal, signal, len))/len);
        break;
      case SRSLTE_AGC_MODE_PEAK_AMPLITUDE:
        t = (float*) signal; 
        y = t[srslte_vec_max_fi(t, 2*len)];// take only positive max to avoid abs() (should be similar) 
        break;
      default: 
        fprintf(stderr, "Unsupported AGC mode\n");
        return; 
    }
    
    if (q->nof_frames > 0) {
      q->y_tmp[q->frame_cnt++] = y; 
      if (q->frame_cnt == q->nof_frames) {
        q->frame_cnt = 0; 
        switch(q->mode) {
          case SRSLTE_AGC_MODE_ENERGY:
            y = srslte_vec_acc_ff(q->y_tmp, q->nof_frames)/q->nof_frames;
            break;
          case SRSLTE_AGC_MODE_PEAK_AMPLITUDE:
            y = q->y_tmp[srslte_vec_max_fi(q->y_tmp, q->nof_frames)];
            break;
          default: 
            fprintf(stderr, "Unsupported AGC mode\n");
            return; 
        }
      }
    }
    
    double gg = 1.0; 
    if (q->isfirst) {
      q->y_out = y; 
      q->isfirst = false; 
    } else {
      if (q->frame_cnt == 0) {
        q->y_out = (1-q->bandwidth) * q->y_out + q->bandwidth * y;
        if (!q->lock) {
          gg = expf(-0.5*q->bandwidth*logf(q->y_out/q->target));
          q->gain *= gg; 
        }          
        DEBUG("AGC gain: %.2f (%.2f) y_out=%.3f, y=%.3f target=%.1f gg=%.2f\n", gain_db, gain_uhd_db, q->y_out, y, q->target, gg);      
      }
    }
  }
}
