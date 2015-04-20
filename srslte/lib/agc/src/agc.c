/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
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
  bzero(q, sizeof(srslte_agc_t));
  q->mode = mode; 
  q->target = SRSLTE_AGC_DEFAULT_TARGET; 
  srslte_agc_reset(q);
  return SRSLTE_SUCCESS;
}

int srslte_agc_init_uhd(srslte_agc_t *q, srslte_agc_mode_t mode, double (set_gain_callback)(void*, double), void *uhd_handler) {
  if (!srslte_agc_init(q, mode)) {
    q->set_gain_callback = set_gain_callback;
    q->uhd_handler = uhd_handler;    
    set_gain_callback(uhd_handler, 30.0);
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR; 
  }
}

void srslte_agc_free(srslte_agc_t *q) {
  bzero(q, sizeof(srslte_agc_t));
}

void srslte_agc_reset(srslte_agc_t *q) {
  q->bandwidth = SRSLTE_AGC_DEFAULT_BW;
  q->lock = false; 
  q->gain = 1.0;
  q->y_out = 1.0; 
  q->isfirst = true; 
}

void srslte_agc_set_bandwidth(srslte_agc_t *q, float bandwidth) {
  q->bandwidth = bandwidth;
}

void srslte_agc_set_target(srslte_agc_t *q, float target) {
  q->target = target;
}

float srslte_agc_get_rssi(srslte_agc_t *q) {
  return 1.0/q->gain;
}

float srslte_agc_get_output_level(srslte_agc_t *q) {
  return q->y_out;
}

float srslte_agc_get_gain(srslte_agc_t *q) {
  return q->gain;
}

void srslte_agc_lock(srslte_agc_t *q, bool enable) {
  q->lock = enable;
}

void srslte_agc_process(srslte_agc_t *q, cf_t *input, cf_t *output, uint32_t len) {
  float gain_res = 1.0;
  // Apply current gain to input signal 
  if (!q->uhd_handler) {
    srslte_vec_sc_prod_cfc(input, q->gain, output, len);
  } else {
    if (q->gain < 1) {
      q->gain = 1.0; 
    }
    if (isinf(10*log10(q->gain)) || isnan(10*log10(q->gain))) {
      q->gain = 1.0; 
    }
    gain_res = q->set_gain_callback(q->uhd_handler, 10*log10(q->gain));
    gain_res = pow(10, gain_res/10);
    if (gain_res > q->gain) {
      q->gain = gain_res; 
    }
  }
  float y = 0; 
  switch(q->mode) {
    case SRSLTE_AGC_MODE_ENERGY:
      y = sqrtf(crealf(srslte_vec_dot_prod_conj_ccc(output, output, len))/len);
      break;
    case SRSLTE_AGC_MODE_PEAK_AMPLITUDE:
      y = -99; 
      for (int i=0;i<len;i++) {
        if (fabs(crealf(input[i])) > y) {
          y = fabs(crealf(input[i]));
        }
        if (fabs(cimagf(input[i])) > y) {
          y = fabs(cimagf(input[i]));
        }
      }
      break;
    default: 
      fprintf(stderr, "Unsupported AGC mode\n");
      return; 
  }
  
  float gg = 1.0; 
  if (q->isfirst) {
    q->y_out = y; 
    q->gain = q->target/y;
    q->isfirst = false; 
  } else {
    q->y_out = (1-q->bandwidth) * q->y_out + q->bandwidth * y;
    if (!q->lock) {
      gg = expf(-0.5*q->bandwidth*logf(q->y_out/q->target));
      q->gain *= gg; 
    }    
  }
  INFO("AGC gain: %.3f (%.2f - %.2f) y_out=%.3f, y=%.3f target=%.1f gg=%.2f\n", q->gain, 10*log10(q->gain), gain_res, q->y_out, y, q->target, gg);
}
