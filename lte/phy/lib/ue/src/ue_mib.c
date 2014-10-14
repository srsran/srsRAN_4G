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

#define FIND_FFTSIZE   128
#define FIND_SFLEN     5*SF_LEN(FIND_FFTSIZE)

#define MIB_FIND_THRESHOLD          0.0

int ue_mib_init(ue_mib_t * q, 
                uint32_t cell_id, 
                lte_cp_t cp) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {

    ret = LIBLTE_ERROR;

    lte_cell_t cell; 
    cell.nof_ports = MIB_NOF_PORTS;
    cell.nof_prb = 6; 
    cell.id = cell_id; 
    cell.cp = cp; 
    
    q->cell_id = cell_id; 
    
    bzero(q, sizeof(ue_mib_t));
    
    q->slot1_symbols = malloc(SLOT_LEN_RE(6, cp) * sizeof(cf_t));
    if (!q->slot1_symbols) {
      perror("malloc");
      goto clean_exit;
    }
    
    for (int i=0;i<MIB_NOF_PORTS;i++) {
      q->ce[i] = malloc(SLOT_LEN_RE(6, cp) * sizeof(cf_t));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    if (sync_init(&q->sfind, FIND_SFLEN, FIND_FFTSIZE)) {
      goto clean_exit;
    }
    
    sync_set_threshold(&q->sfind, MIB_FIND_THRESHOLD);
    sync_sss_en(&q->sfind, true);
    sync_set_N_id_2(&q->sfind, cell_id % 3);
    sync_cp_en(&q->sfind, false);
    sync_set_cp(&q->sfind, cp);

    if (lte_fft_init(&q->fft, cp, cell.nof_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      goto clean_exit;
    }
    if (chest_init_LTEDL(&q->chest, cell)) {
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
  if (q->slot1_symbols) {
    free(q->slot1_symbols);
  }
  for (int i=0;i<MIB_NOF_PORTS;i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  sync_free(&q->sfind);
  chest_free(&q->chest);
  pbch_free(&q->pbch);
  lte_fft_free(&q->fft);
}


void ue_mib_reset(ue_mib_t * q)
{
  q->frame_cnt = 0; 
  q->last_frame_trial = 0;
  
  pbch_decode_reset(&q->pbch);
}

void ue_mib_set_threshold(ue_mib_t * q, float threshold)
{
  sync_set_threshold(&q->sfind, threshold);
}

static int mib_decoder_run(ue_mib_t * q, cf_t *input, pbch_mib_t *mib)
{
  int ret = LIBLTE_SUCCESS;

  /* Run FFT for the slot symbols */
  lte_fft_run_slot(&q->fft, input, q->slot1_symbols);

  /* Get channel estimates of slot #1 for each port */
  ret = chest_ce_slot(&q->chest, q->slot1_symbols, q->ce, 1);
  if (ret < 0) {
    return LIBLTE_ERROR;
  }

  /* Reset decoder if we missed a frame */
  if ((q->last_frame_trial && (abs(q->frame_cnt - q->last_frame_trial) > 2)) || 
      q->frame_cnt > 16) 
  {
    INFO("Resetting PBCH decoder: last trial %u, now is %u\n",
        q->last_frame_trial, q->frame_cnt);
    ue_mib_reset(q);
  }
  
  /* Decode PBCH */
  ret = pbch_decode(&q->pbch, q->slot1_symbols, q->ce, mib);
  if (ret < 0) {
    fprintf(stderr, "Error decoding PBCH\n");      
  } else if (ret == 1) {
    INFO("MIB decoded: %u\n", q->frame_cnt/2);
    ue_mib_reset(q);
    ret = MIB_FOUND; 
  } else {
    INFO("MIB not decoded: %u\n", q->frame_cnt / 2);
    q->last_frame_trial = q->frame_cnt;
    ret = LIBLTE_SUCCESS;
  }    
  return ret;
}
int counter1=0,counter2=0,counter3=0,counter4=0;

int ue_mib_decode(ue_mib_t * q, 
                  cf_t *signal, 
                  uint32_t nsamples,
                  pbch_mib_t *mib)
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t peak_idx=0;
  uint32_t nof_input_frames; 


  if (q                 != NULL &&
      signal            != NULL) 
  {
    if (nsamples < MIB_FRAME_SIZE) {
      fprintf(stderr, "Error: nsamples must be greater than %d\n", MIB_FRAME_SIZE);
      return LIBLTE_ERROR;
    }
    
    ret = LIBLTE_SUCCESS; 
    
    if (nsamples % MIB_FRAME_SIZE) {
      printf("Warning: nsamples must be a multiple of %d. Some samples will be ignored\n", MIB_FRAME_SIZE);
      nsamples = (nsamples/MIB_FRAME_SIZE) * MIB_FRAME_SIZE;
    }
    nof_input_frames = nsamples/MIB_FRAME_SIZE; 
    
    for (uint32_t nf=0;nf<nof_input_frames;nf++) {

      /* Find peak and cell id */
      ret = sync_find(&q->sfind, signal, nf*MIB_FRAME_SIZE, &peak_idx);
      if (ret < 0) {
        fprintf(stderr, "Error finding correlation peak (%d)\n", ret);
        return -1;
      }
      
      if (ret == 0) {
        counter2++;
      } else if (ret == 1) {
        counter4++;
      }
      
      /* Check if we have space for reading the MIB and we are in Subframe #0 */
      if (ret                                    == 1        && 
          nf*MIB_FRAME_SIZE + peak_idx + 960     <= nsamples &&
          sync_sss_detected(&q->sfind)                       && 
          sync_get_sf_idx(&q->sfind)             == 0) 
      {
        INFO("Trying to decode MIB\n",0);
        ret = mib_decoder_run(q, &signal[nf*MIB_FRAME_SIZE+peak_idx], mib);
        counter3++;
      } else if ((ret == LIBLTE_SUCCESS && peak_idx != 0)   || 
                 (ret == 1              && nf*MIB_FRAME_SIZE + peak_idx + 960 > nsamples)) 
      {
        printf("Not enough space for PBCH\n",0);
        ret = MIB_FRAME_UNALIGNED; 
      } else {
        INFO("SSS not detected\n",0);
        ret = 0; 
      }
      
      counter1++;
      INFO("Total: %3d - Sync0: %3d - Sync1: %3d - Tried: %3d - Peak: %4d - Ret: %d\n",counter1,counter2,counter4, counter3, peak_idx, ret);
      
      q->frame_cnt++;
    } 
  }
  return ret;
}




