/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"

#define SRSLTE_AGC_CALLBACK(NAME) void(NAME)(void* h, float gain_db)
#define SRSLTE_AGC_DEFAULT_TARGET (0.3f)  /* Average RMS target or maximum peak target*/
#define SRSLTE_AGC_DEFAULT_BW (0.3f)      /* Moving average coefficient */
#define SRSLTE_AGC_HOLD_COUNT (20)        /* Number of frames to wait after setting the gain before start measuring */
#define SRSLTE_AGC_MIN_MEASUREMENTS (10)  /* Minimum number of measurements  */
#define SRSLTE_AGC_MIN_GAIN_OFFSET (2.0f) /* Mimum of gain offset to set the radio gain */

typedef enum SRSLTE_API { SRSLTE_AGC_MODE_ENERGY = 0, SRSLTE_AGC_MODE_PEAK_AMPLITUDE } srslte_agc_mode_t;

/*
 * The AGC has been implemented using 3 states:
 *  - init: it simply starts the process of measuring
 *  - measure: performs a minimum of SRSLTE_AGC_MIN_MEASUREMENTS and does not set the gain until it needs
 *             SRSLTE_AGC_MIN_GAIN_OFFSET dB more of gain. The gain is set in the enter hold transition.
 *  - hold: waits for SRSLTE_AGC_HOLD_COUNT frames as a Rx gain transition period. After this period, it enters measure
 *          state.
 *
 * FSM abstraction:
 *
 * +------+   Enter measure  +---------+   Enter hold   +------+
 * | init | ---------------->| Measure |--------------->| Hold |
 * +------+                  +---------+                +------+
 *                                ^      Enter measure      |
 *                                +-------------------------+
 */

typedef enum { SRSLTE_AGC_STATE_INIT = 0, SRSLTE_AGC_STATE_MEASURE, SRSLTE_AGC_STATE_HOLD } srslte_agc_state_t;

typedef struct SRSLTE_API {
  float bandwidth;
  float gain_db;
  float gain_offset_db;
  float min_gain_db;
  float max_gain_db;
  float default_gain_db;
  float y_out;
  bool  isfirst;
  void* uhd_handler;
  SRSLTE_AGC_CALLBACK(*set_gain_callback);
  srslte_agc_mode_t  mode;
  float              target;
  uint32_t           nof_frames;
  uint32_t           frame_cnt;
  uint32_t           hold_cnt;
  float*             y_tmp;
  srslte_agc_state_t state;
} srslte_agc_t;

SRSLTE_API int srslte_agc_init_acc(srslte_agc_t* q, srslte_agc_mode_t mode, uint32_t nof_frames);

SRSLTE_API int srslte_agc_init_uhd(srslte_agc_t*     q,
                                   srslte_agc_mode_t mode,
                                   uint32_t          nof_frames,
                                   SRSLTE_AGC_CALLBACK(set_gain_callback),
                                   void* uhd_handler);

SRSLTE_API void srslte_agc_free(srslte_agc_t* q);

SRSLTE_API void srslte_agc_reset(srslte_agc_t* q);

SRSLTE_API void srslte_agc_set_gain_range(srslte_agc_t* q, float min_gain_db, float max_gain_db);

SRSLTE_API float srslte_agc_get_gain(srslte_agc_t* q);

SRSLTE_API void srslte_agc_set_gain(srslte_agc_t* q, float init_gain_value_db);

SRSLTE_API void srslte_agc_process(srslte_agc_t* q, cf_t* signal, uint32_t len);

#endif // SRSLTE_AGC_H
