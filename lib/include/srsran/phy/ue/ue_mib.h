/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         ue_mib.h
 *
 *  Description:  This object decodes the MIB from the PBCH of an LTE signal.
 *
 *                The function srsran_ue_mib_decode() shall be called multiple
 *                times, each passing a number of samples multiple of 19200,
 *                sampled at 1.92 MHz (that is, 10 ms of samples).
 *
 *                The function uses the sync_t object to find the PSS sequence and
 *                decode the PBCH to obtain the MIB.
 *
 *                The function returns 0 until the MIB is decoded.
 *
 *                See ue_cell_detect.c for an example.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_UE_MIB_H
#define SRSRAN_UE_MIB_H

#include <stdbool.h>

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/pbch.h"
#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/ue/ue_sync.h"

#define SRSRAN_UE_MIB_NOF_PRB              6

#define SRSRAN_UE_MIB_FOUND                1
#define SRSRAN_UE_MIB_NOTFOUND             0

typedef struct SRSRAN_API {
  srsran_sync_t sfind;

  cf_t* sf_symbols;

  srsran_ofdm_t fft;
  srsran_pbch_t pbch;

  srsran_chest_dl_t     chest;
  srsran_chest_dl_res_t chest_res;

  uint8_t  bch_payload[SRSRAN_BCH_PAYLOAD_LEN];
  uint32_t nof_tx_ports; 
  uint32_t sfn_offset;

  uint32_t frame_cnt;
} srsran_ue_mib_t;

SRSRAN_API int srsran_ue_mib_init(srsran_ue_mib_t* q, cf_t* in_buffer, uint32_t max_prb);

SRSRAN_API void srsran_ue_mib_free(srsran_ue_mib_t* q);

SRSRAN_API int srsran_ue_mib_set_cell(srsran_ue_mib_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_ue_mib_reset(srsran_ue_mib_t* q);

SRSRAN_API int srsran_ue_mib_decode(srsran_ue_mib_t* q,
                                    uint8_t          bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                                    uint32_t*        nof_tx_ports,
                                    int*             sfn_offset);

/* This interface uses ue_mib and ue_sync to first get synchronized subframes 
 * and then decode MIB
 * 
 * This object calls the pbch object with nof_ports=0 for blind nof_ports determination 
*/
typedef struct {
  srsran_ue_mib_t  ue_mib;
  srsran_ue_sync_t ue_sync;
  cf_t*            sf_buffer[SRSRAN_MAX_CHANNELS];
  uint32_t         nof_rx_channels;
} srsran_ue_mib_sync_t;

SRSRAN_API int
srsran_ue_mib_sync_init_multi(srsran_ue_mib_sync_t* q,
                              int(recv_callback)(void*, cf_t* [SRSRAN_MAX_CHANNELS], uint32_t, srsran_timestamp_t*),
                              uint32_t nof_rx_channels,
                              void*    stream_handler);

SRSRAN_API void srsran_ue_mib_sync_free(srsran_ue_mib_sync_t* q);

SRSRAN_API int srsran_ue_mib_sync_set_cell(srsran_ue_mib_sync_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_ue_mib_sync_reset(srsran_ue_mib_sync_t* q);

SRSRAN_API int srsran_ue_mib_sync_decode(srsran_ue_mib_sync_t* q,
                                         uint32_t              max_frames_timeout,
                                         uint8_t               bch_payload[SRSRAN_BCH_PAYLOAD_LEN],
                                         uint32_t*             nof_tx_ports,
                                         int*                  sfn_offset);

#endif // SRSRAN_UE_MIB_H

