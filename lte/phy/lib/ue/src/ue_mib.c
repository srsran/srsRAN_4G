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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include "liblte/phy/ue/ue_mib.h"

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

int ue_mib_init(ue_mib_t * q, 
                lte_cell_t cell) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && 
      cell.nof_ports <= MIB_MAX_PORTS) 
  {

    ret = LIBLTE_ERROR;    
    bzero(q, sizeof(ue_mib_t));
    
    q->sf_symbols = vec_malloc(SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }
    
    for (int i=0;i<cell.nof_ports;i++) {
      q->ce[i] = vec_malloc(SF_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    if (lte_fft_init(&q->fft, cell.cp, cell.nof_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      goto clean_exit;
    }
    if (chest_dl_init(&q->chest, cell)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    if (pbch_init(&q->pbch, cell)) {
      fprintf(stderr, "Error initiating PBCH\n");
      goto clean_exit;
    }
    ue_mib_reset(q);
    
    ret = LIBLTE_SUCCESS;
  }

clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_mib_free(q);
  }
  return ret;
}

void ue_mib_free(ue_mib_t * q)
{
  if (q->sf_symbols) {
    free(q->sf_symbols);
  }
  for (int i=0;i<MIB_MAX_PORTS;i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  sync_free(&q->sfind);
  chest_dl_free(&q->chest);
  pbch_free(&q->pbch);
  lte_fft_free(&q->fft);
    
  bzero(q, sizeof(ue_mib_t));
    
}


void ue_mib_reset(ue_mib_t * q)
{
  q->frame_cnt = 0;   
  pbch_decode_reset(&q->pbch);
}

int ue_mib_decode(ue_mib_t * q, cf_t *input, 
                  uint8_t bch_payload[BCH_PAYLOAD_LEN], uint32_t *nof_tx_ports, uint32_t *sfn_offset)
{
  int ret = LIBLTE_SUCCESS;
  cf_t *ce_slot1[MAX_PORTS]; 

  /* Run FFT for the slot symbols */
  lte_fft_run_sf(&q->fft, input, q->sf_symbols);
            
  /* Get channel estimates of slot #1 for each port */
  ret = chest_dl_estimate(&q->chest, q->sf_symbols, q->ce, 0);
  if (ret < 0) {
    return LIBLTE_ERROR;
  }
  /* Reset decoder if we missed a frame */
  if (q->frame_cnt > 8) {
    INFO("Resetting PBCH decoder after %d frames\n", q->frame_cnt);
    ue_mib_reset(q);
  }
  
  for (int i=0;i<MAX_PORTS;i++) {
    ce_slot1[i] = &q->ce[i][SLOT_LEN_RE(q->chest.cell.nof_prb, q->chest.cell.cp)];
  }
  
  /* Decode PBCH */
  ret = pbch_decode(&q->pbch, &q->sf_symbols[SLOT_LEN_RE(q->chest.cell.nof_prb, q->chest.cell.cp)], 
                    ce_slot1, chest_dl_get_noise_estimate(&q->chest),
                    bch_payload, nof_tx_ports, sfn_offset);
  if (ret < 0) {
    fprintf(stderr, "Error decoding PBCH (%d)\n", ret);      
  } else if (ret == 1) {
    INFO("MIB decoded: %u\n", q->frame_cnt);
    ue_mib_reset(q);
    ret = MIB_FOUND; 
  } else {
    INFO("MIB not decoded: %u\n", q->frame_cnt);
    q->frame_cnt++;
    ret = MIB_NOTFOUND;
  }    
  return ret;
}



int ue_mib_sync_init(ue_mib_sync_t *q, 
                     uint32_t cell_id, 
                     lte_cp_t cp, 
                     int (recv_callback)(void*, void*, uint32_t),                             
                     void *stream_handler) 
{
  lte_cell_t cell; 
  cell.nof_ports = MIB_MAX_PORTS; 
  cell.id = cell_id; 
  cell.cp = cp; 
  cell.nof_prb = MIB_NOF_PRB; 
  
  if (ue_mib_init(&q->ue_mib, cell)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return LIBLTE_ERROR;
  }
  if (ue_sync_init(&q->ue_sync, cell, recv_callback, stream_handler)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    ue_mib_free(&q->ue_mib);
    return LIBLTE_ERROR;
  }
  ue_sync_decode_sss_on_track(&q->ue_sync, true);
  return LIBLTE_SUCCESS;
}

void ue_mib_sync_free(ue_mib_sync_t *q) {
  ue_mib_free(&q->ue_mib);
  ue_sync_free(&q->ue_sync);
}

void ue_mib_sync_reset(ue_mib_sync_t * q) {
  ue_mib_reset(&q->ue_mib);
  ue_sync_reset(&q->ue_sync);
}

int ue_mib_sync_decode(ue_mib_sync_t * q, 
                       uint32_t max_frames_timeout,
                       uint8_t bch_payload[BCH_PAYLOAD_LEN], 
                       uint32_t *nof_tx_ports, 
                       uint32_t *sfn_offset) 
{
  
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  cf_t *sf_buffer = NULL; 
  uint32_t nof_frames = 0; 
  int mib_ret = MIB_NOTFOUND; 

  if (q != NULL) 
  {
    ret = LIBLTE_SUCCESS;     
    do {
      mib_ret = MIB_NOTFOUND; 
      ret = ue_sync_get_buffer(&q->ue_sync, &sf_buffer);
      if (ret < 0) {
        fprintf(stderr, "Error calling ue_sync_work()\n");       
        break; 
      } else if (ue_sync_get_sfidx(&q->ue_sync) == 0) {
        if (ret == 1) {
          ue_mib_reset(&q->ue_mib);
          mib_ret = ue_mib_decode(&q->ue_mib, sf_buffer, bch_payload, nof_tx_ports, sfn_offset);          
          
        } else {
          INFO("Resetting PBCH decoder after %d frames\n", q->ue_mib.frame_cnt);
          ue_mib_reset(&q->ue_mib);
        }
        nof_frames++;
      }
    } while (mib_ret == MIB_NOTFOUND && ret >= 0 && nof_frames < max_frames_timeout);
    if (mib_ret < 0) {
      ret = mib_ret; 
    }
  }
  return mib_ret;  
}


