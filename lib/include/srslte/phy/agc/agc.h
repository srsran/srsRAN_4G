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

/**********************************************************************************************
 *  File:         agc.h
 *
 *  Description:  Automatic gain control
 *                This module is not currently used
 *
 *  Reference:
 *********************************************************************************************/

#ifndef SRSLTE_AGC_H
#define SRSLTE_AGC_H

#include <stdbool.h>
#include <stdint.h>
#include <complex.h>

#include "srslte/config.h"

#define SRSLTE_AGC_DEFAULT_TARGET 0.3
#define SRSLTE_AGC_DEFAULT_BW     0.7

typedef enum SRSLTE_API {
  SRSLTE_AGC_MODE_ENERGY = 0, 
  SRSLTE_AGC_MODE_PEAK_AMPLITUDE  
} srslte_agc_mode_t; 

typedef struct SRSLTE_API{
  float bandwidth;
  double gain; 
  double min_gain;
  double max_gain;
  float y_out;
  bool lock;
  bool isfirst; 
  void *uhd_handler; 
  double (*set_gain_callback) (void*,double);
  srslte_agc_mode_t mode; 
  float target;
  uint32_t nof_frames; 
  uint32_t frame_cnt; 
  float *y_tmp;
} srslte_agc_t;

SRSLTE_API int srslte_agc_init(srslte_agc_t *q, srslte_agc_mode_t mode);

SRSLTE_API int srslte_agc_init_acc(srslte_agc_t *q, srslte_agc_mode_t mode, uint32_t nof_frames);

SRSLTE_API int srslte_agc_init_uhd(srslte_agc_t *q, 
                                   srslte_agc_mode_t mode, 
                                   uint32_t nof_frames,
                                   double (set_gain_callback)(void*, double), 
                                   void *uhd_handler); 

SRSLTE_API void srslte_agc_free(srslte_agc_t *q);

SRSLTE_API void srslte_agc_reset(srslte_agc_t *q);

SRSLTE_API void srslte_agc_set_gain_range(srslte_agc_t *q, double min_gain, double max_gain);

SRSLTE_API void srslte_agc_set_bandwidth(srslte_agc_t *q, 
                                         float bandwidth);

SRSLTE_API void srslte_agc_set_target(srslte_agc_t *q, 
                                      float target);

SRSLTE_API float srslte_agc_get_rssi(srslte_agc_t *q);

SRSLTE_API float srslte_agc_get_output_level(srslte_agc_t *q); 

SRSLTE_API float srslte_agc_get_gain(srslte_agc_t *q);

SRSLTE_API void srslte_agc_set_gain(srslte_agc_t *q, 
                                    float init_gain_value); 

SRSLTE_API void srslte_agc_lock(srslte_agc_t *q, 
                                bool enable);

SRSLTE_API void srslte_agc_process(srslte_agc_t *q, 
                                   cf_t *signal,
                                   uint32_t len);

#endif // SRSLTE_AGC_H
