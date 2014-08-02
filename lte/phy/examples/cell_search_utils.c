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

#ifndef DISABLE_UHD
#include "liblte/cuhd/cuhd.h"

int decode_pbch(void *uhd, cf_t *buffer, ue_celldetect_result_t *found_cell, uint32_t nof_frames_total, pbch_mib_t *mib) 
{
  ue_mib_t uemib;
  int n; 
  
  bzero(mib, sizeof(pbch_mib_t));
  
  uint32_t nof_frames = 0;
  uint32_t flen = MIB_FRAME_SIZE;
  
  if (ue_mib_init(&uemib, found_cell->cell_id, found_cell->cp)) {
    fprintf(stderr, "Error initiating PBCH decoder\n");
    return LIBLTE_ERROR;
  }
  
  INFO("Setting sampling frequency 1.92 MHz for PBCH decoding\n", 0);
  cuhd_set_rx_srate(uhd, 1920000.0);
  INFO("Starting receiver...\n", 0);
  cuhd_start_rx_stream(uhd);
  
  do {
    if (cuhd_recv(uhd, buffer, flen, 1)<0) {
      fprintf(stderr, "Error receiving from USRP\n");
      return LIBLTE_ERROR;
    }      
    
    INFO("Calling ue_mib_decode() %d/%d\n", nof_frames, nof_frames_total);
    
    n = ue_mib_decode(&uemib, buffer, flen, mib);
    if (n == LIBLTE_ERROR || n == LIBLTE_ERROR_INVALID_INPUTS) {
      fprintf(stderr, "Error calling ue_mib_decode()\n");
      return LIBLTE_ERROR;
    }
    if (n == MIB_FRAME_UNALIGNED) {
      printf("Realigning frame\n");
      if (cuhd_recv(uhd, buffer, flen/2, 1)<0) {
        fprintf(stderr, "Error receiving from USRP\n");
        return LIBLTE_ERROR;
      }
    }
    nof_frames++;
  } while (n != MIB_FOUND && nof_frames < 2*nof_frames_total);
  if (n == MIB_FOUND) {
    printf("\n\nMIB decoded in %d ms (%d half frames)\n", nof_frames*5, nof_frames);
    pbch_mib_fprint(stdout, mib, found_cell->cell_id);
  } else {
    printf("\nCould not decode MIB\n");
  }

  cuhd_stop_rx_stream(uhd); 
  cuhd_flush_buffer(uhd);

  ue_mib_free(&uemib);
  
  return LIBLTE_SUCCESS; 
}

int find_cell(void *uhd, ue_celldetect_t *s, cf_t *buffer, ue_celldetect_result_t found_cell[3]) 
{  
  int n; 

  INFO("Setting sampling frequency 960 KHz for PSS search\n", 0);
  cuhd_set_rx_srate(uhd, 960000.0);
  INFO("Starting receiver...\n", 0);
  cuhd_start_rx_stream(uhd);

  uint32_t nof_scanned_cells = 0; 
  uint32_t flen = 4800; 
  int nof_detected_cells = 0; 
  
  do {
    
    if (cuhd_recv(uhd, buffer, flen, 1)<0) {
      fprintf(stderr, "Error receiving from USRP\n");
      return LIBLTE_ERROR;
    }
    
    n = ue_celldetect_scan(s, buffer, flen, &found_cell[nof_scanned_cells]);
    switch(n) {
      case CS_FRAME_UNALIGNED:
        printf("Realigning frame\n");
        if (cuhd_recv(uhd, buffer, flen/2, 1)<0) {
          fprintf(stderr, "Error receiving from USRP\n");
          return LIBLTE_ERROR;
        }
        return LIBLTE_ERROR; 
      case CS_CELL_DETECTED:
        nof_detected_cells++;
        if (found_cell[nof_scanned_cells].peak > 0) {
          printf("\n\tCELL ID: %d, CP: %s, Peak: %.2f, Mode: %d/%d\n", 
                found_cell[nof_scanned_cells].cell_id, 
                lte_cp_string(found_cell[nof_scanned_cells].cp), 
                found_cell[nof_scanned_cells].peak, found_cell[nof_scanned_cells].mode, 
                s->nof_frames_detected);                      
        }
        
        nof_scanned_cells++;
        break;
      case CS_CELL_NOT_DETECTED:
        nof_scanned_cells++;
        break;
      case LIBLTE_ERROR:
      case LIBLTE_ERROR_INVALID_INPUTS: 
        fprintf(stderr, "Error calling cellsearch_scan()\n");
        return LIBLTE_ERROR;         
    }
  } while(nof_scanned_cells < 3);

  INFO("Stopping receiver...\n", 0);
  cuhd_stop_rx_stream(uhd); 
  cuhd_flush_buffer(uhd);
  
  return nof_detected_cells; 
}
#endif
