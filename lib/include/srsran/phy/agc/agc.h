/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#ifndef SRSRAN_AGC_H
#define SRSRAN_AGC_H

#include <complex.h>
#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"

#define SRSRAN_AGC_CALLBACK(NAME) void(NAME)(void* h, float gain_db)
#define SRSRAN_AGC_DEFAULT_TARGET (0.3f)  /* Average RMS target or maximum peak target*/
#define SRSRAN_AGC_DEFAULT_BW (0.3f)      /* Moving average coefficient */
#define SRSRAN_AGC_HOLD_COUNT (20)        /* Number of frames to wait after setting the gain before start measuring */
#define SRSRAN_AGC_MIN_MEASUREMENTS (10)  /* Minimum number of measurements  */
#define SRSRAN_AGC_MIN_GAIN_OFFSET (2.0f) /* Mimum of gain offset to set the radio gain */

typedef enum SRSRAN_API { SRSRAN_AGC_MODE_ENERGY = 0, SRSRAN_AGC_MODE_PEAK_AMPLITUDE } srsran_agc_mode_t;

/*
 * The AGC has been implemented using 3 states:
 *  - init: it simply starts the process of measuring
 *  - measure: performs a minimum of SRSRAN_AGC_MIN_MEASUREMENTS and does not set the gain until it needs
 *             SRSRAN_AGC_MIN_GAIN_OFFSET dB more of gain. The gain is set in the enter hold transition.
 *  - hold: waits for SRSRAN_AGC_HOLD_COUNT frames as a Rx gain transition period. After this period, it enters measure
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

typedef enum { SRSRAN_AGC_STATE_INIT = 0, SRSRAN_AGC_STATE_MEASURE, SRSRAN_AGC_STATE_HOLD } srsran_agc_state_t;

typedef struct SRSRAN_API {
  float bandwidth;
  float gain_db;
  float gain_offset_db;
  float min_gain_db;
  float max_gain_db;
  float default_gain_db;
  float y_out;
  bool  isfirst;
  void* uhd_handler;
  SRSRAN_AGC_CALLBACK(*set_gain_callback);
  srsran_agc_mode_t  mode;
  float              target;
  uint32_t           nof_frames;
  uint32_t           frame_cnt;
  uint32_t           hold_cnt;
  float*             y_tmp;
  srsran_agc_state_t state;
} srsran_agc_t;

SRSRAN_API int srsran_agc_init_acc(srsran_agc_t* q, srsran_agc_mode_t mode, uint32_t nof_frames);

SRSRAN_API int srsran_agc_init_uhd(srsran_agc_t*     q,
                                   srsran_agc_mode_t mode,
                                   uint32_t          nof_frames,
                                   SRSRAN_AGC_CALLBACK(set_gain_callback),
                                   void* uhd_handler);

SRSRAN_API void srsran_agc_free(srsran_agc_t* q);

SRSRAN_API void srsran_agc_reset(srsran_agc_t* q);

SRSRAN_API void srsran_agc_set_gain_range(srsran_agc_t* q, float min_gain_db, float max_gain_db);

SRSRAN_API float srsran_agc_get_gain(srsran_agc_t* q);

SRSRAN_API void srsran_agc_set_gain(srsran_agc_t* q, float init_gain_value_db);

SRSRAN_API void srsran_agc_process(srsran_agc_t* q, cf_t* signal, uint32_t len);

#endif // SRSRAN_AGC_H
