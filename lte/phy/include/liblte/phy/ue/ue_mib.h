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

#include "liblte/config.h"
#include "liblte/phy/sync/sync.h"
#include "liblte/phy/sync/cfo.h"
#include "liblte/phy/ch_estimation/chest_dl.h"
#include "liblte/phy/phch/pbch.h"
#include "liblte/phy/common/fft.h"


#define MIB_MAX_PORTS            4
#define MIB_FRAME_SIZE_SEARCH   9600
#define MIB_FFT_SIZE            128

#define MIB_FRAME_UNALIGNED     -3
#define MIB_FOUND                1
#define MIB_NOTFOUND             0

typedef struct LIBLTE_API {
  sync_t sfind;
 
  cf_t *sf_symbols;
  cf_t *ce[MIB_MAX_PORTS];
  
  cfo_t cfocorr; 
  lte_fft_t fft;
  chest_dl_t chest; 
  pbch_t pbch;
  
  uint8_t bch_payload[BCH_PAYLOAD_LEN];
  uint32_t nof_tx_ports; 
  uint32_t sfn_offset; 
  
  uint32_t frame_cnt; 
  uint32_t last_frame_trial; 
} ue_mib_t;


LIBLTE_API int ue_mib_init_1_92(ue_mib_t *q, 
                           uint32_t cell_id, 
                           lte_cp_t cp);

LIBLTE_API int ue_mib_init(ue_mib_t *q, 
                           lte_cell_t cell, 
                           bool do_sync);

LIBLTE_API void ue_mib_free(ue_mib_t *q);

LIBLTE_API void ue_mib_reset(ue_mib_t *q);

LIBLTE_API int ue_mib_sync_and_decode_1_92(ue_mib_t *q,
                                           cf_t *signal, 
                                           uint32_t nsamples);

LIBLTE_API int ue_mib_decode_aligned_frame(ue_mib_t * q, 
                                           cf_t *input, 
                                           uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                                           uint32_t *nof_tx_ports, 
                                           uint32_t *sfn_offset); 

LIBLTE_API void ue_mib_get_payload(ue_mib_t *q,
                                   uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                                   uint32_t *nof_tx_ports,
                                   uint32_t *sfn_offset);

LIBLTE_API void ue_mib_set_threshold(ue_mib_t *q, 
                                            float threshold); 

LIBLTE_API void ue_mib_reset(ue_mib_t *q);






#endif // SYNC_FRAME_

