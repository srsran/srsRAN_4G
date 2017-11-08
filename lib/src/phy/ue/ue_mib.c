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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include "srslte/phy/ue/ue_mib.h"

#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

int srslte_ue_mib_init(srslte_ue_mib_t * q,
                       cf_t *in_buffer[SRSLTE_MAX_PORTS],
                       uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL)
  {

    ret = SRSLTE_ERROR;    
    bzero(q, sizeof(srslte_ue_mib_t));
    
    if (srslte_pbch_init(&q->pbch)) {
      fprintf(stderr, "Error initiating PBCH\n");
      goto clean_exit;
    }

    q->sf_symbols = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }
    
    for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
      q->ce[i] = srslte_vec_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM) * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    if (srslte_ofdm_rx_init(&q->fft, SRSLTE_CP_NORM, in_buffer[0], q->sf_symbols, max_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      goto clean_exit;
    }
    if (srslte_chest_dl_init(&q->chest, max_prb)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    srslte_ue_mib_reset(q);
    
    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_mib_free(q);
  }
  return ret;
}

void srslte_ue_mib_free(srslte_ue_mib_t * q)
{
  if (q->sf_symbols) {
    free(q->sf_symbols);
  }
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  srslte_sync_free(&q->sfind);
  srslte_chest_dl_free(&q->chest);
  srslte_pbch_free(&q->pbch);
  srslte_ofdm_rx_free(&q->fft);
    
  bzero(q, sizeof(srslte_ue_mib_t));
    
}

int srslte_ue_mib_set_cell(srslte_ue_mib_t * q,
                           srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL &&
      cell.nof_ports <= SRSLTE_MAX_PORTS)
  {
    if (srslte_pbch_set_cell(&q->pbch, cell)) {
      fprintf(stderr, "Error initiating PBCH\n");
      return SRSLTE_ERROR;
    }
    if (srslte_ofdm_rx_set_prb(&q->fft, cell.cp, cell.nof_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      return SRSLTE_ERROR;
    }

    if (cell.nof_ports == 0) {
      cell.nof_ports = SRSLTE_MAX_PORTS;
    }

    if (srslte_chest_dl_set_cell(&q->chest, cell)) {
      fprintf(stderr, "Error initializing reference signal\n");
      return SRSLTE_ERROR;
    }
    srslte_ue_mib_reset(q);

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}


void srslte_ue_mib_reset(srslte_ue_mib_t * q)
{
  q->frame_cnt = 0;   
  srslte_pbch_decode_reset(&q->pbch);
}

int srslte_ue_mib_decode(srslte_ue_mib_t * q,
                  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN], uint32_t *nof_tx_ports, int *sfn_offset)
{
  int ret = SRSLTE_SUCCESS;
  cf_t *ce_slot1[SRSLTE_MAX_PORTS]; 

  /* Run FFT for the slot symbols */
  srslte_ofdm_rx_sf(&q->fft);
            
  /* Get channel estimates of sf idx #0 for each port */
  ret = srslte_chest_dl_estimate(&q->chest, q->sf_symbols, q->ce, 0);
  if (ret < 0) {
    return SRSLTE_ERROR;
  }
  /* Reset decoder if we missed a frame */
  if (q->frame_cnt > 8) {
    INFO("Resetting PBCH decoder after %d frames\n", q->frame_cnt);
    srslte_ue_mib_reset(q);
  }
  
  for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
    ce_slot1[i] = &q->ce[i][SRSLTE_SLOT_LEN_RE(q->chest.cell.nof_prb, q->chest.cell.cp)];
  }

  /* Decode PBCH */
  ret = srslte_pbch_decode(&q->pbch, &q->sf_symbols[SRSLTE_SLOT_LEN_RE(q->chest.cell.nof_prb, q->chest.cell.cp)], 
                    ce_slot1, 0,
                    bch_payload, nof_tx_ports, sfn_offset);
  

  if (ret < 0) {
    fprintf(stderr, "Error decoding PBCH (%d)\n", ret);      
  } else if (ret == 1) {
    INFO("MIB decoded: %u\n", q->frame_cnt);
    srslte_ue_mib_reset(q);
    ret = SRSLTE_UE_MIB_FOUND; 
  } else {
    ret = SRSLTE_UE_MIB_NOTFOUND;
    INFO("MIB not decoded: %u\n", q->frame_cnt);
    q->frame_cnt++;
  }    
  
  return ret;
}

int srslte_ue_mib_sync_init_multi(srslte_ue_mib_sync_t *q, 
                                  int (recv_callback)(void*, cf_t*[SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*),
                                  uint32_t nof_rx_antennas,
                                  void *stream_handler) 
{
  for (int i=0;i<nof_rx_antennas;i++) {
    q->sf_buffer[i] = srslte_vec_malloc(3*sizeof(cf_t)*SRSLTE_SF_LEN_PRB(SRSLTE_UE_MIB_NOF_PRB));
  }
  q->nof_rx_antennas = nof_rx_antennas;
  
  if (srslte_ue_mib_init(&q->ue_mib, q->sf_buffer, SRSLTE_UE_MIB_NOF_PRB)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSLTE_ERROR;
  }
  if (srslte_ue_sync_init_multi(&q->ue_sync, SRSLTE_UE_MIB_NOF_PRB, false, recv_callback, nof_rx_antennas, stream_handler)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srslte_ue_mib_free(&q->ue_mib);
    return SRSLTE_ERROR;
  }
  srslte_ue_sync_decode_sss_on_track(&q->ue_sync, true);
  return SRSLTE_SUCCESS;
}

int srslte_ue_mib_sync_set_cell(srslte_ue_mib_sync_t *q,
                                uint32_t cell_id,
                                srslte_cp_t cp)
{
  srslte_cell_t cell;
  // If the ports are set to 0, ue_mib goes through 1, 2 and 4 ports to blindly detect nof_ports
  cell.nof_ports = 0;
  cell.id = cell_id;
  cell.cp = cp;
  cell.nof_prb = SRSLTE_UE_MIB_NOF_PRB;

  if (srslte_ue_mib_set_cell(&q->ue_mib, cell)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSLTE_ERROR;
  }
  if (srslte_ue_sync_set_cell(&q->ue_sync, cell)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srslte_ue_mib_free(&q->ue_mib);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}


void srslte_ue_mib_sync_free(srslte_ue_mib_sync_t *q) {
  for (int i=0;i<q->nof_rx_antennas;i++) {
    if (q->sf_buffer[i]) {
      free(q->sf_buffer[i]);
    }
  }
  srslte_ue_mib_free(&q->ue_mib);
  srslte_ue_sync_free(&q->ue_sync);
}

void srslte_ue_mib_sync_reset(srslte_ue_mib_sync_t * q) {
  srslte_ue_mib_reset(&q->ue_mib);
  srslte_ue_sync_reset(&q->ue_sync);
}

int srslte_ue_mib_sync_decode(srslte_ue_mib_sync_t * q, 
                       uint32_t max_frames_timeout,
                       uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN], 
                       uint32_t *nof_tx_ports, 
                       int *sfn_offset) 
{
  
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  uint32_t nof_frames = 0; 
  int mib_ret = SRSLTE_UE_MIB_NOTFOUND;

  if (q != NULL)
  {
    srslte_ue_mib_sync_reset(q);

    ret = SRSLTE_SUCCESS;
    do {
      mib_ret = SRSLTE_UE_MIB_NOTFOUND; 
      ret = srslte_ue_sync_zerocopy_multi(&q->ue_sync, q->sf_buffer);
      if (ret < 0) {
        fprintf(stderr, "Error calling srslte_ue_sync_work()\n");       
        return -1;
      } else if (srslte_ue_sync_get_sfidx(&q->ue_sync) == 0) {
        if (ret == 1) {
          mib_ret = srslte_ue_mib_decode(&q->ue_mib, bch_payload, nof_tx_ports, sfn_offset);
        } else {
          DEBUG("Resetting PBCH decoder after %d frames\n", q->ue_mib.frame_cnt);
          srslte_ue_mib_reset(&q->ue_mib);
        }
        nof_frames++;
      }
    } while (mib_ret == SRSLTE_UE_MIB_NOTFOUND && ret >= 0 && nof_frames < max_frames_timeout);
    if (mib_ret < 0) {
      ret = mib_ret; 
    }
  }
  return mib_ret;  
}


