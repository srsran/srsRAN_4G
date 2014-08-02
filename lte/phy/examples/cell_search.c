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
#endif

#define MHZ             1000000
#define SAMP_FREQ       1920000
#define FLEN            9600
#define FLEN_PERIOD     0.005

#define MAX_EARFCN 1000


int band = -1;
int earfcn_start=-1, earfcn_end = -1;
int nof_frames_total = 50;
int nof_frames_detected = 10;
float threshold = CS_FIND_THRESHOLD; 


float uhd_gain = 60.0;
char *uhd_args=""; 

void usage(char *prog) {
  printf("Usage: %s [agsendtvb] -b band\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-n nof_frames_total [Default 100]\n");
  printf("\t-d nof_frames_detected [Default 10]\n");
  printf("\t-t threshold [Default %.2f]\n",threshold);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agsendtvb")) != -1) {
    switch(opt) {
    case 'a':
      uhd_args = argv[optind];
      break;
    case 'b':
      band = atoi(argv[optind]);
      break;
    case 's':
      earfcn_start = atoi(argv[optind]);
      break;
    case 'e':
      earfcn_end = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames_total = atoi(argv[optind]);
      break;
    case 'd':
      nof_frames_detected = atoi(argv[optind]);
      break;
    case 't':
      threshold = atof(argv[optind]);
      break;
    case 'g':
      uhd_gain = atof(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (band == -1) {
    usage(argv[0]);
    exit(-1);
  }
}

int decode_pbch(void *uhd, cf_t *buffer, ue_celldetect_result_t *found_cell) 
{
  ue_mib_t uemib;
  pbch_mib_t mib; 
  int n; 
  
  bzero(&mib, sizeof(pbch_mib_t));
  
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
    
    n = ue_mib_decode(&uemib, buffer, flen, &mib);
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
    pbch_mib_fprint(stdout, &mib, found_cell->cell_id);
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
  
  return n; 
}

int main(int argc, char **argv) {
  int n; 
  void *uhd;
  ue_celldetect_t s;
  ue_celldetect_result_t found_cells[3]; 
  cf_t *buffer; 
  int nof_freqs; 
  lte_earfcn_t channels[MAX_EARFCN];
  uint32_t freq;

  parse_args(argc, argv);
    
  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }  
  cuhd_set_rx_gain(uhd, uhd_gain);
  
  nof_freqs = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
  if (nof_freqs < 0) {
    fprintf(stderr, "Error getting EARFCN list\n");
    exit(-1);
  }
    
  buffer = vec_malloc(sizeof(cf_t) * 96000);
  if (!buffer) {
    perror("malloc");
    return LIBLTE_ERROR;
  }
  
  if (ue_celldetect_init(&s)) {
    fprintf(stderr, "Error initiating UE sync module\n");
    exit(-1);
  }
  if (threshold > 0) {
    ue_celldetect_set_threshold(&s, threshold);    
  }
  
  if (nof_frames_total > 0) {
    ue_celldetect_set_nof_frames_total(&s, nof_frames_total);
  }
  if (nof_frames_detected > 0) {
    ue_celldetect_set_nof_frames_detected(&s, nof_frames_detected);
  }

  for (freq=0;freq<nof_freqs;freq+=10) {
  
    /* set uhd_freq */
    cuhd_set_rx_freq(uhd, (double) channels[freq].fd * MHZ);
    cuhd_rx_wait_lo_locked(uhd);
    usleep(10000);
    INFO("Set uhd_freq to %.3f MHz\n", (double) channels[freq].fd * MHZ/1000000);
    
    printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz looking for PSS. \r", freq, nof_freqs,
                      channels[freq].id, channels[freq].fd);fflush(stdout);
    
    if (VERBOSE_ISINFO()) {
      printf("\n");
    }
    
    n = find_cell(uhd, &s, buffer, found_cells);
    if (n < 0) {
      fprintf(stderr, "Error searching cell\n");
      exit(-1);
    }
    if (n == CS_CELL_DETECTED) {
      for (int i=0;i<3;i++) {
        if (found_cells[i].peak > threshold/2) {
          if (decode_pbch(uhd, buffer, &found_cells[i])) {
            fprintf(stderr, "Error decoding PBCH\n");
            exit(-1);
          }          
        }
      }
    }    
  }
    
  ue_celldetect_free(&s);
  cuhd_close(uhd);
  exit(0);
}


