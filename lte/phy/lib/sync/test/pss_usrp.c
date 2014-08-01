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
#include <time.h>

#include <stdbool.h>

#include "liblte/phy/phy.h"
#include "liblte/cuhd/cuhd.h"

uint32_t N_id_2 = 100;
char *uhd_args="";
float uhd_gain=40.0, uhd_freq=-1.0;
int nof_frames = -1;
uint32_t fft_size=64;
float threshold = 0.4; 

void usage(char *prog) {
  printf("Usage: %s [agtvnp] -f rx_frequency_hz -i N_id_2\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD Gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-v verbose\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agtvsfi")) != -1) {
    switch (opt) {
    case 'a':
      uhd_args = argv[optind];
      break;
    case 'g':
      uhd_gain = atof(argv[optind]);
      break;
    case 'f':
      uhd_freq = atof(argv[optind]);
      break;
    case 't':
      threshold = atof(argv[optind]);
      break;
    case 'i':
      N_id_2 = atoi(argv[optind]);
      break;
    case 's':
      fft_size = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (N_id_2 > 2 || uhd_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  cf_t *buffer; 
  int frame_cnt, n; 
  void *uhd;
  pss_synch_t pss; 
  int32_t flen; 
  int peak_idx, last_peak;
  float peak_value; 
  float mean_peak; 
  uint32_t nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;
  
  parse_args(argc, argv);

  flen = 4800*(fft_size/64);
  
  buffer = malloc(sizeof(cf_t) * flen);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }
    
  if (pss_synch_init_fft(&pss, flen, fft_size)) {
    fprintf(stderr, "Error initiating PSS\n");
    exit(-1);
  }
  if (pss_synch_set_N_id_2(&pss, N_id_2)) {
    fprintf(stderr, "Error setting N_id_2=%d\n",N_id_2);
    exit(-1);
  }
  
  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
  printf("Set RX rate: %.2f MHz\n", cuhd_set_rx_srate(uhd, flen*2*100) / 1000000);
  printf("Set RX gain: %.1f dB\n", cuhd_set_rx_gain(uhd, uhd_gain));
  printf("Set RX freq: %.2f MHz\n", cuhd_set_rx_freq(uhd, uhd_freq) / 1000000);
  cuhd_rx_wait_lo_locked(uhd);
  cuhd_start_rx_stream(uhd);
  
  printf("Frame length %d samples\n", flen);
  printf("PSS detection threshold: %.2f\n", threshold);
  
  nof_det = nof_nodet = nof_nopeak = nof_nopeakdet = 0;
  frame_cnt = 0;
  last_peak = 0; 
  mean_peak = 0;
  while(frame_cnt < nof_frames || nof_frames == -1) {
    n = cuhd_recv(uhd, buffer, flen, 1);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }

    peak_idx = pss_synch_find_pss(&pss, buffer, &peak_value);
    if (peak_idx < 0) {
      fprintf(stderr, "Error finding PSS peak\n");
      exit(-1);
    }
        
    float y = sqrtf(crealf(vec_dot_prod_conj_ccc(&buffer[peak_idx-fft_size], 
                                                 &buffer[peak_idx-fft_size], 
                                                 fft_size)) /
                                                 fft_size);
    float x = peak_value/y;
    
    mean_peak = EXPAVERAGE(x, mean_peak, frame_cnt);
    
    if (x >= threshold) {
      nof_det++;
    } else {
      nof_nodet++;
    }

    if (frame_cnt > 100) {
      if (abs(last_peak-peak_idx) > 10) {
        if (x >= threshold) {
          nof_nopeakdet++;
        } else {
          if (nof_nodet > 0) {
            nof_nodet--;            
          }
        }
        nof_nopeak++;
      } 
    }

    frame_cnt++;

    printf("[%5d]: Pos: %5d, En: %.4f Val: %.3f MeanVal: %.3f, Det: %.3f, No-Det: %.3f, NoPeak: %.3f, NoPeakDet: %.3f\r",
           frame_cnt, 
           peak_idx, y, x, mean_peak,
           (float) nof_det/frame_cnt, (float) nof_nodet/frame_cnt, 
           (float) nof_nopeak/frame_cnt, (float) nof_nopeakdet/nof_nopeak);
    
    if (VERBOSE_ISINFO()) {
      printf("\n");
    }
    
    last_peak = peak_idx;

  }
  
  pss_synch_free(&pss);
  free(buffer);
  cuhd_close(uhd);

  printf("Ok\n");
  exit(0);
}

