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


int band = -1;
int earfcn_start=-1, earfcn_end = -1;
int nof_frames_find=200;


float uhd_gain = 60.0;
char *uhd_args=""; 

#define MAX_EARFCN 1000
lte_earfcn_t channels[MAX_EARFCN];


void usage(char *prog) {
  printf("Usage: %s [asefgv] -b band\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-f nof_frames_find [Default %d]\n", nof_frames_find);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "asefgvb")) != -1) {
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
    case 'f':
      nof_frames_find = atoi(argv[optind]);
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

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}


int main(int argc, char **argv) {
  int ret;
  int frame_cnt;
  int nof_freqs; 
  uint32_t freq;
  ue_sync_t uesync; 
  void *uhd;
  cf_t *buffer; 
  lte_cell_t cell; 
  
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
 
  parse_args(argc,argv);
 
  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
 
  /* set uhd_gain */
  cuhd_set_rx_gain(uhd, uhd_gain);

  nof_freqs = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
  if (nof_freqs < 0) {
    fprintf(stderr, "Error getting EARFCN list\n");
    exit(-1);
  }
  

  for (freq=0;freq<nof_freqs;freq++) {
    /* set freq */
    cuhd_stop_rx_stream(uhd);
    cuhd_set_rx_freq(uhd, (double) channels[freq].fd * MHZ);
    cuhd_rx_wait_lo_locked(uhd);
        
    if (ue_sync_init(&uesync, cell, cuhd_recv_wrapper, uhd)) {
      fprintf(stderr, "Error initiating UE sync\n");
      exit(-1);
    }
    
    ue_sync_decode_sss_on_track(&uesync, true);

    DEBUG("Starting receiver...\n",0);
    cuhd_start_rx_stream(uhd);
    usleep(10000);
    
    
    /* Receive up to a maximum of nof_frames_find */
    frame_cnt = 0; 
    ret = 0;
    ue_sync_reset(&uesync);

    while(frame_cnt < nof_frames_find && ret == 0) {
      ret = ue_sync_get_buffer(&uesync, &buffer);
      if (ret < 0) {
        fprintf(stderr, "Error calling ue_sync_work()\n");
        exit(-1);
      }
      frame_cnt++;
      printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz looking for PSS. \r", freq, nof_freqs,
                      channels[freq].id, channels[freq].fd);fflush(stdout);
      if (VERBOSE_ISINFO()) {
        printf("\n");
      }
    }
    if (ret == 1) {
      if (VERBOSE_ISINFO()) {
        printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz FOUND MIB ", freq, nof_freqs,
                    channels[freq].id, channels[freq].fd);
      }
      printf("CFO: %+.4f KHz\n", ue_sync_get_cfo(&uesync));
      printf("\n");fflush(stdout); 
    }
    
    ue_sync_free(&uesync);      
  }
  
  cuhd_close(uhd);
  
  
  printf("\n\nDone\n");
  exit(0);
}

