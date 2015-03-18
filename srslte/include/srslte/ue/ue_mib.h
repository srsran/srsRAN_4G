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

#ifndef UE_MIB_
#define UE_MIB_


/************************************************************
 * 
 * This object decodes the MIB from the PBCH of an LTE signal. 
 * 
 * The function ue_mib_decode() shall be called multiple times, 
 * each passing a number of samples multiple of 19200, sampled at 1.92 MHz
 * (that is, 10 ms of samples). 
 * 
 * The function uses the sync_t object to find the PSS sequence and 
 * decode the PBCH to obtain the MIB. 
 * 
 * The function returns 0 until the MIB is decoded. 
 * 
 * See ue_cell_detect.c for an example. 
 * 
 ************************************************************/

#include <stdbool.h>

#include "srslte/config.h"
#include "srslte/ue/ue_sync.h"
#include "srslte/sync/cfo.h"
#include "srslte/ch_estimation/chest_dl.h"
#include "srslte/phch/pbch.h"
#include "srslte/common/fft.h"


#define MIB_SRSLTE_MAX_PORTS            4
#define MIB_NOF_PRB              6

#define MIB_FOUND                1
#define MIB_NOTFOUND             0

typedef struct SRSLTE_API {
  sync_t sfind;
 
  cf_t *sf_symbols;
  cf_t *ce[MIB_SRSLTE_MAX_PORTS];
  
  srslte_fft_t fft;
  srslte_chest_dl_t chest; 
  pbch_t pbch;
  
  uint8_t bch_payload[BCH_PAYLOAD_LEN];
  uint32_t nof_tx_ports; 
  uint32_t sfn_offset; 
  
  uint32_t frame_cnt; 
} ue_mib_t;

SRSLTE_API int ue_mib_init(ue_mib_t *q, 
                           srslte_cell_t cell);

SRSLTE_API void ue_mib_free(ue_mib_t *q);

SRSLTE_API void ue_mib_reset(ue_mib_t * q); 

SRSLTE_API int ue_mib_decode(ue_mib_t * q, 
                             cf_t *input, 
                             uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                             uint32_t *nof_tx_ports, 
                             uint32_t *sfn_offset); 


/* This interface uses ue_mib and ue_sync to first get synchronized subframes 
 * and then decode MIB
*/
typedef struct {
  ue_mib_t ue_mib; 
  ue_sync_t ue_sync; 
} ue_mib_sync_t;

SRSLTE_API int ue_mib_sync_init(ue_mib_sync_t *q, 
                                uint32_t cell_id, 
                                srslte_cp_t cp,
                                int (recv_callback)(void*, void*, uint32_t, timestamp_t *),                             
                                void *stream_handler);

SRSLTE_API void ue_mib_sync_free(ue_mib_sync_t *q);

SRSLTE_API void ue_mib_sync_reset(ue_mib_sync_t * q); 

SRSLTE_API int ue_mib_sync_decode(ue_mib_sync_t * q, 
                                  uint32_t max_frames_timeout,
                                  uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                                  uint32_t *nof_tx_ports, 
                                  uint32_t *sfn_offset); 




#endif // SYNC_FRAME_

