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

#ifndef SYNC_FRAME_
#define SYNC_FRAME_

#include <stdbool.h>

#include "liblte/config.h"
#include "liblte/phy/sync/sync.h"
#include "liblte/phy/sync/cfo.h"

/**
 *
 * Uses sync object to automatically manage the FIND and TRACKING states. 
 * It is suposed to work on a subframe basis. The input signal must be sampled at 
 * a frequency integer multiple of 1.92 MHz. The signal is internally downsampled 
 * and fed to the sync object. 
 * 
 * This object also deals with frame alignment and CFO correction, returning an 
 * output signal aligned both in time and frequency. 
 */

enum sync_frame_state { SF_FIND, SF_TRACK };

#define SYNC_SF_LEN      1920  // 1ms at 1.92 MHz

#define TRACK_MAX_LOST   10


typedef struct LIBLTE_API {
  sync_t s;
  enum sync_frame_state state;
  int downsampling; 
  resample_arb_t resample;
  unsigned long frame_cnt;
  bool fb_wp; 
  int frame_size; 
  cf_t *input_buffer; 
  cf_t *input_downsampled;
  cfo_t cfocorr;
  float cur_cfo;
  int peak_idx;
  int cell_id;
  float timeoffset; 
  int last_found;
  int sf_idx;
}sync_frame_t;


/* Initializes the automatic tracker, setting the downsampling ratio for the input signal. 
 * upsampled is the ratio of the provided signal sampling frequency to 1.92 Mhz. E.g. if input is sampled at 3.84 Mhz, 
 * upsampled should be 2.
*/
LIBLTE_API int sync_frame_init(sync_frame_t *q, int upsampled);

LIBLTE_API void sync_frame_free(sync_frame_t *q);

LIBLTE_API void sync_frame_set_threshold(sync_frame_t *q, float threshold);

LIBLTE_API int sync_frame_cell_id(sync_frame_t *q);

LIBLTE_API int sync_frame_sfidx(sync_frame_t *q);

/* Automatically time/freq synchronizes the input signal. Returns 1 if the signal is synched and locked, 
 * and fills the output buffer with the time and frequency aligned version of the signal.
 * If 0 is returned, the PSS was not found. -1 is returned in case of error. 
 * 
 * The provided signal can be sampled at an integer multiple of 1.92 Mhz. 
 * The sampling ratio is provided when calling the sync_auto_reset() function. 
 * 
 * The buffer input must have subframe_size samples (used in sync_init)
 */
LIBLTE_API int sync_frame_push(sync_frame_t *q, cf_t *input, cf_t *output);

/* Resets the automatic tracker */
LIBLTE_API void sync_frame_reset(sync_frame_t *q);




#endif // SYNC_FRAME_

