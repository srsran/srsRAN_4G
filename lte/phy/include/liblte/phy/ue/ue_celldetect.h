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

#ifndef UE_CELLSEARCH_
#define UE_CELLSEARCH_

#include <stdbool.h>

#include "liblte/config.h"
#include "liblte/phy/sync/sync.h"
#include "liblte/phy/sync/cfo.h"
#include "liblte/phy/ch_estimation/chest_dl.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/common/fft.h"

/************************************************************
 * 
 * This object scans a signal for LTE cells using the known PSS 
 * and SSS sequences. 
 * 
 * The function ue_celldetect_scan() shall be called multiple times, 
 * each passing a number of samples multiple of 4800, sampled at 960 KHz
 * (that is, 5 ms of samples). 
 * 
 * The function returns 0 until a signal is found nof_frames_detected times or 
 * after nof_frames_total with no signal detected. 
 * 
 * See ue_cell_detect.c for an example. 
 * 
 ************************************************************/

/** 
 * TODO: Check also peak offset 
 */

#define CS_DEFAULT_MAXFRAMES_TOTAL      500
#define CS_DEFAULT_MAXFRAMES_DETECTED   50

#define CS_DEFAULT_NOFFRAMES_TOTAL      100
#define CS_DEFAULT_NOFFRAMES_DETECTED   10

#define CS_FRAME_UNALIGNED     -3
#define CS_CELL_DETECTED        2
#define CS_CELL_NOT_DETECTED    3

#define CS_FFTSIZE   128
#define CS_SAMP_FREQ    (960000*(CS_FFTSIZE/64))
#define CS_FLEN      (4800*(CS_FFTSIZE/64))

typedef struct LIBLTE_API {
  uint32_t cell_id; 
  lte_cp_t cp; 
  float peak; 
  float mode; 
} ue_celldetect_result_t;


typedef struct LIBLTE_API {
  sync_t sfind;
  
  uint32_t max_frames_total;
  uint32_t nof_frames_total;  // number of 5 ms frames to scan 
  float detect_threshold; // early-stops scan if mean PSR above this threshold
  
  uint32_t current_nof_detected; 
  uint32_t current_nof_total; 
  
  uint32_t *mode_ntimes;
  uint8_t *mode_counted; 
  
  ue_celldetect_result_t *candidates; 
} ue_celldetect_t;


LIBLTE_API int ue_celldetect_init(ue_celldetect_t *q);

LIBLTE_API int ue_celldetect_init_max(ue_celldetect_t *q, 
                                      uint32_t max_frames_total);

LIBLTE_API void ue_celldetect_free(ue_celldetect_t *q);

LIBLTE_API void ue_celldetect_reset(ue_celldetect_t *q);

LIBLTE_API int ue_celldetect_scan(ue_celldetect_t *q,
                                  cf_t *signal, 
                                  uint32_t nsamples);

LIBLTE_API int ue_celldetect_set_N_id_2(ue_celldetect_t *q, 
                                        uint32_t N_id_2);

LIBLTE_API void ue_celldetect_get_cell(ue_celldetect_t * q, 
                                       ue_celldetect_result_t *found_cell); 

LIBLTE_API int ue_celldetect_set_nof_frames_total(ue_celldetect_t *q, 
                                                   uint32_t nof_frames);

LIBLTE_API void ue_celldetect_set_threshold(ue_celldetect_t *q, 
                                            float threshold); 

LIBLTE_API void ue_celldetect_reset(ue_celldetect_t *q);






#endif // SYNC_FRAME_

