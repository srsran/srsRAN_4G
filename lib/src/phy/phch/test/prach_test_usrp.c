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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

#define MAX_LEN  70176


uint32_t nof_prb          = 25;
uint32_t preamble_format  = 0;
uint32_t root_seq_idx     = 0;
uint32_t seq_idx          = 0;
uint32_t frequency_offset = 0;
uint32_t zero_corr_zone   = 11;
float timeadv = 0; 
uint32_t nof_frames = 20; 

float uhd_rx_gain=40, uhd_tx_gain=60, uhd_freq=2.4e9; 
char *uhd_args="";
char *output_filename = "prach_rx";

void usage(char *prog) {
  printf("Usage: %s \n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-f UHD TX/RX frequency [Default %.2f MHz]\n", uhd_freq/1e6);
  printf("\t-g UHD RX gain [Default %.1f dB]\n", uhd_rx_gain);
  printf("\t-G UHD TX gain [Default %.1f dB]\n", uhd_tx_gain);
  printf("\t-p Number of UL RB [Default %d]\n", nof_prb);
  printf("\t-F Preamble format [Default %d]\n", preamble_format);
  printf("\t-O Frequency offset [Default %d]\n", frequency_offset);
  printf("\t-s sequence index [Default %d]\n", seq_idx);
  printf("\t-r Root sequence index [Default %d]\n", root_seq_idx);
  printf("\t-t Time advance (us) [Default %.1f us]\n", timeadv);
  printf("\t-z Zero correlation zone config [Default %d]\n", zero_corr_zone);
  printf("\t-o Save transmitted PRACH in file [Default no]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "apfFgGrstoPOz")) != -1) {
    switch (opt) {
    case 'a':
      uhd_args = argv[optind];
      break;
    case 'o':
      output_filename = argv[optind];
      break;
    case 'f':
      uhd_freq = atof(argv[optind]);
      break;
    case 'g':
      uhd_rx_gain = atof(argv[optind]);
      break;
    case 'G':
      uhd_tx_gain = atof(argv[optind]);
      break;
    case 'P':
      preamble_format = atoi(argv[optind]);
      break;
    case 'O':
      frequency_offset = atoi(argv[optind]);
      break;
    case 't':
      timeadv = atof(argv[optind]);
      break;
    case 'p':
      nof_prb = atoi(argv[optind]);
      if (!srslte_nofprb_isvalid(nof_prb)) {
        fprintf(stderr, "Invalid number of UL RB %d\n", nof_prb);
        exit(-1);
      }
      break;
    case 'F':
      preamble_format = atoi(argv[optind]);
      break;
    case 'r':
      root_seq_idx = atoi(argv[optind]);
      break;
    case 's':
      seq_idx = atoi(argv[optind]);
      break;
    case 'z':
      zero_corr_zone = atoi(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  srslte_prach_t *p = (srslte_prach_t*)malloc(sizeof(srslte_prach_t));

  bool high_speed_flag      = false;

  cf_t preamble[MAX_LEN];
  memset(preamble, 0, sizeof(cf_t)*MAX_LEN);

  srslte_prach_init(p,
                    srslte_symbol_sz(nof_prb));

  srslte_prach_set_cell(p,
                    srslte_symbol_sz(nof_prb),
                    preamble_format,
                    root_seq_idx,
                    high_speed_flag,
                    zero_corr_zone);

  int srate = srslte_sampling_freq_hz(nof_prb);
  uint32_t flen = srate/1000;

  printf("Generating PRACH\n");
  bzero(preamble, flen*sizeof(cf_t));
  srslte_prach_gen(p,
            seq_idx,
            frequency_offset,
            preamble);
  
  
  uint32_t prach_len = p->N_seq+p->N_cp;
  
  srslte_vec_save_file("generated",preamble,prach_len*sizeof(cf_t));
  
  cf_t *buffer = malloc(sizeof(cf_t)*flen*nof_frames);
  
  // Send through UHD 
  srslte_rf_t rf; 
  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, uhd_args)) {
    fprintf(stderr, "Error opening &uhd\n");
    exit(-1);
  }
  printf("Subframe len:   %d samples\n", flen);
  printf("Set RX gain: %.1f dB\n", uhd_rx_gain);
  printf("Set TX gain: %.1f dB\n", uhd_tx_gain);
  printf("Set TX/RX freq: %.2f MHz\n", uhd_freq/ 1000000);
  
  srslte_rf_set_rx_gain(&rf, uhd_rx_gain);
  srslte_rf_set_tx_gain(&rf, uhd_tx_gain);
  srslte_rf_set_rx_freq(&rf, uhd_freq);
  srslte_rf_set_tx_freq(&rf, uhd_freq);
  
  if (srate > 1e6 && (srate/1000) > 0) {
    if (30720%(srate/1000) == 0) {
      srslte_rf_set_master_clock_rate(&rf, 30.72e6);        
    } else {
      srslte_rf_set_master_clock_rate(&rf, 23.04e6);        
    }
  } else {
    printf("Invalid sampling rate %d Hz\n", srate);
    exit(-1);
  }
  printf("Setting sampling rate %.2f MHz\n", (float) srate/1000000);
  float srate_rf = srslte_rf_set_rx_srate(&rf, (double) srate);
  if (srate_rf != srate) {
    fprintf(stderr, "Could not set sampling rate\n");
    exit(-1);
  }
  srslte_rf_set_tx_srate(&rf, (double) srate);
  sleep(1);
  
  cf_t *zeros = calloc(sizeof(cf_t),flen);
  
  srslte_timestamp_t tstamp; 
  
  srslte_rf_start_rx_stream(&rf, false);
  uint32_t nframe=0;
  
  while(nframe<nof_frames) {
    printf("Rx subframe %d\n", nframe);
    srslte_rf_recv_with_time(&rf, &buffer[flen*nframe], flen, true, &tstamp.full_secs, &tstamp.frac_secs);
    nframe++;
    if (nframe==9 || nframe==8) {
      srslte_timestamp_add(&tstamp, 0, 2e-3-timeadv*1e-6);
      if (nframe==8) {
        srslte_rf_send_timed2(&rf, zeros, flen, tstamp.full_secs, tstamp.frac_secs, true, false);      
        printf("Transmitting zeros\n");        
      } else {
        srslte_rf_send_timed2(&rf, preamble, flen, tstamp.full_secs, tstamp.frac_secs, false, true);      
        printf("Transmitting PRACH\n");      
      }
    }
  }
  
  uint32_t indices[1024];
  float offsets[1024];
  uint32_t nof_detected; 
  if (srslte_prach_detect_offset(p, frequency_offset, &buffer[flen*10+p->N_cp], flen, indices, offsets, NULL, &nof_detected)) {
    printf("Error detecting prach\n");
  }
  printf("Nof detected PRACHs: %d\n", nof_detected);
  for (int i=0;i<nof_detected;i++) {
    printf("%d/%d index=%d, offset=%.2f us (%d samples)\n", 
           i, nof_detected, indices[i], offsets[i]*1e6, (int) (offsets[i]*srate));
  }
  
  srslte_vec_save_file(output_filename,buffer,11*flen*sizeof(cf_t));

  srslte_rf_close(&rf);
  srslte_prach_free(p);
  free(p);
  srslte_dft_exit();

  printf("Done\n");
  exit(0);
}
