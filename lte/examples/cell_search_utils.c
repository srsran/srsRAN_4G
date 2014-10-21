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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

#include <unistd.h>

#include "liblte/phy/phy.h"
#include "liblte/rrc/rrc.h"

#ifndef DISABLE_UHD
#include "liblte/cuhd/cuhd.h"

int decode_pbch(void *uhd, ue_celldetect_result_t *found_cell, uint32_t nof_frames_total, 
                uint8_t bch_payload[BCH_PAYLOAD_LEN], uint32_t *nof_tx_ports, uint32_t *sfn_offset) 
{
  ue_mib_t uemib;
  int n; 
  int ret = LIBLTE_ERROR; 

  uint32_t nof_frames = 0;
  uint32_t flen = MIB_FRAME_SIZE;

  cf_t *buffer = vec_malloc(sizeof(cf_t) * flen);
  if (!buffer) {
    perror("malloc");
    goto free_and_exit;
  }
  
  if (ue_mib_init(&uemib, found_cell->cell_id, found_cell->cp)) {
    fprintf(stderr, "Error initiating PBCH decoder\n");
    goto free_and_exit;
  }
  
  INFO("Setting sampling frequency 1.92 MHz for PBCH decoding\n", 0);
  cuhd_set_rx_srate(uhd, 1920000.0);
  INFO("Starting receiver...\n", 0);
  cuhd_start_rx_stream(uhd);
  
  do {
    if (cuhd_recv(uhd, buffer, flen, 1)<0) {
      fprintf(stderr, "Error receiving from USRP\n");
      goto free_and_exit;
    }      
    
    DEBUG("Calling ue_mib_decode() %d/%d\n", nof_frames, nof_frames_total);
    
    n = ue_mib_decode(&uemib, buffer, flen, bch_payload, nof_tx_ports, sfn_offset);
    if (n == LIBLTE_ERROR || n == LIBLTE_ERROR_INVALID_INPUTS) {
      fprintf(stderr, "Error calling ue_mib_decode()\n");
      goto free_and_exit;
    }
    if (n == MIB_FRAME_UNALIGNED) {
      printf("Realigning frame\n");
      if (cuhd_recv(uhd, buffer, 1500, 1)<0) {
        fprintf(stderr, "Error receiving from USRP\n");
        goto free_and_exit;
      }
      bzero(buffer, flen * sizeof(cf_t));
    }
    nof_frames++;
  } while (n != MIB_FOUND && nof_frames < 2*nof_frames_total);
  
  if (n == MIB_FOUND) {
    printf("\n\nMIB decoded in %d ms (%d half frames)\n", nof_frames*5, nof_frames);
    ret = LIBLTE_SUCCESS;
  } else {
    ret = LIBLTE_ERROR;
  }

free_and_exit:
  free(buffer);

  cuhd_stop_rx_stream(uhd); 
  cuhd_flush_buffer(uhd);

  ue_mib_free(&uemib);
  
  return ret; 
}

int find_cell(void *uhd, ue_celldetect_result_t *found_cell, uint32_t N_id_2) 
{  
  int ret = LIBLTE_ERROR; 
  ue_celldetect_t cd;
  
  cf_t *buffer = vec_malloc(sizeof(cf_t) * 96000);
  if (!buffer) {
    perror("malloc");
    goto free_and_exit;
  }
  
  if (ue_celldetect_init(&cd)) {
    fprintf(stderr, "Error initiating UE cell detect\n");
    goto free_and_exit;
  }
  
  ue_celldetect_set_nof_frames_detected(&cd, 50);

  ue_celldetect_set_nof_frames_total(&cd, 500);
  
  INFO("Setting sampling frequency 960 KHz for PSS search\n", 0);
  cuhd_set_rx_srate(uhd, 960000.0);
  INFO("Starting receiver...\n", 0);
  cuhd_start_rx_stream(uhd);

  uint32_t flen = 4800; 
  int n; 
  
  ue_celldetect_set_N_id_2(&cd, N_id_2);
  
  do {
    if (cuhd_recv(uhd, buffer, flen, 1)<0) {
      fprintf(stderr, "Error receiving from USRP\n");
      goto free_and_exit;
    }

    DEBUG("Scanning cell at N_id_2=%d\n",N_id_2);
    
    n = ue_celldetect_scan(&cd, buffer, flen);
    switch(n) {
      case CS_FRAME_UNALIGNED:
        printf("Realigning frame\n");
        if (cuhd_recv(uhd, buffer, flen/2, 1)<0) {
          fprintf(stderr, "Error receiving from USRP\n");
          goto free_and_exit;
        }
        /* FIXME: What should we do here?? */
        ret = -1;
        goto free_and_exit;
      case CS_CELL_DETECTED:
        ue_celldetect_get_cell(&cd, found_cell);
        if (found_cell->peak > 0) {
          printf("\n\tCELL ID: %d, CP: %s, Peak: %.2f, Mode: %d/%d\n", 
                found_cell->cell_id, 
                lte_cp_string(found_cell->cp), 
                found_cell->peak, found_cell->mode, 
                cd.nof_frames_detected);                      
        }
        
        ret = 1;
        INFO("Cell found at N_id_2=%d\n",N_id_2);
        break;
      case CS_CELL_NOT_DETECTED:
        ret = 0; 
        DEBUG("No cell found at N_id_2=%d\n",N_id_2);
        break;
      case LIBLTE_ERROR:
      case LIBLTE_ERROR_INVALID_INPUTS: 
        ret = LIBLTE_ERROR;
        fprintf(stderr, "Error calling cellsearch_scan()\n");
        goto free_and_exit;       
    }
    
  } while(n == 0);
  
free_and_exit:
  free(buffer);
  ue_celldetect_free(&cd);
  INFO("Stopping receiver...\n", 0);
  cuhd_stop_rx_stream(uhd); 
  cuhd_flush_buffer(uhd);

  return ret; 
}


int find_all_cells(void *uhd, ue_celldetect_result_t found_cell[3]) 
{

  uint32_t N_id_2; 
  int ret;
  int nof_detected_cells = 0;
  
  for (N_id_2=0;N_id_2<3;N_id_2++) {
    ret = find_cell(uhd, &found_cell[N_id_2], N_id_2);    
    if (ret == 1) {
      nof_detected_cells++;
    } else if (ret == LIBLTE_ERROR) {
      return LIBLTE_ERROR; 
    }
  }
  return nof_detected_cells;
}

int cell_search(void *uhd, int force_N_id_2, lte_cell_t *cell) 
{
  int ret; 
  uint32_t nof_tx_ports; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN], bch_payload_unpacked[BCH_PAYLOAD_LEN];
  
  ue_celldetect_result_t found_cells[3];
  bzero(found_cells, 3*sizeof(ue_celldetect_result_t));

  if (force_N_id_2 >= 0) {
    ret = find_cell(uhd, &found_cells[force_N_id_2], force_N_id_2);
  } else {
    ret = find_all_cells(uhd, found_cells);      
  }
  if (ret < 0) {
    fprintf(stderr, "Error searching cell\n");
    exit(-1);
  }

  int max_peak_cell = 0;
  float max_peak_value = -1.0;
  if (ret > 0) {
    if (force_N_id_2 >= 0) {
      max_peak_cell = force_N_id_2;
    } else {
      for (int i=0;i<3;i++) {
        if (found_cells[i].peak > max_peak_value) {
          max_peak_value = found_cells[i].peak;
          max_peak_cell = i;
        }
      }        
    }
    
    printf("Decoding PBCH for cell %d (N_id_2=%d)\n", found_cells[max_peak_cell].cell_id, max_peak_cell);
    if (decode_pbch(uhd, &found_cells[max_peak_cell], 400, bch_payload, &nof_tx_ports, NULL)) {
      fprintf(stderr, "Could not decode PBCH from CELL ID %d\n", found_cells[max_peak_cell].cell_id);
      return LIBLTE_ERROR;
    }
  } else {
    fprintf(stderr, "Could not find any cell in this frequency\n");
    return LIBLTE_ERROR;
  }
  
  cell->cp = found_cells[max_peak_cell].cp;
  cell->id = found_cells[max_peak_cell].cell_id;
  cell->nof_ports = nof_tx_ports; 

  bit_unpack_vector(bch_payload, bch_payload_unpacked, BCH_PAYLOAD_LEN);
  bcch_bch_unpack(bch_payload_unpacked, BCH_PAYLOAD_LEN, cell, NULL);  
  
  /* set sampling frequency */
  int srate = lte_sampling_freq_hz(cell->nof_prb);
  if (srate != -1) {  
    cuhd_set_rx_srate(uhd, (double) srate);      
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", cell->nof_prb);
    return LIBLTE_ERROR;
  }
  return LIBLTE_SUCCESS;
}


#endif
