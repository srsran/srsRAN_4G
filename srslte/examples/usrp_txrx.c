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

#include "srslte/rf/rf.h"
#include "srslte/srslte.h"

uint32_t nof_prb    = 25;
uint32_t nof_frames = 20; 

float tone_offset_hz = 0;
float rf_rx_gain=40, rf_tx_gain=40, rf_freq=2.4e9; 
char *rf_args="";
char *output_filename = NULL;
char *input_filename = NULL;

void usage(char *prog) {
  printf("Usage: %s -o [rx_signal_file]\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-f RF TX/RX frequency [Default %.2f MHz]\n", rf_freq/1e6);
  printf("\t-g RF RX gain [Default %.1f dB]\n", rf_rx_gain);
  printf("\t-G RF TX gain [Default %.1f dB]\n", rf_tx_gain);
  printf("\t-t Single tone offset (Hz) [Default %f]\n", tone_offset_hz);    
  printf("\t-i File name to read signal from [Default single tone]\n");
  printf("\t-p Number of UL RB [Default %d]\n", nof_prb);  
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "ioafgpt")) != -1) {
    switch (opt) {
    case 'a':
      rf_args = argv[optind];
      break;
    case 'o':
      output_filename = argv[optind];
      break;
    case 'i':
      input_filename = argv[optind];
      break;
    case 't':
      tone_offset_hz = atof(argv[optind]);
      break;
    case 'f':
      rf_freq = atof(argv[optind]);
      break;
    case 'g':
      rf_rx_gain = atof(argv[optind]);
      break;
    case 'G':
      rf_tx_gain = atof(argv[optind]);
      break;
    case 'p':
      nof_prb = atoi(argv[optind]);
      if (!srslte_nofprb_isvalid(nof_prb)) {
        fprintf(stderr, "Invalid number of UL RB %d\n", nof_prb);
        exit(-1);
      }
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (!output_filename) {
    usage(argv[0]);
    exit(-1);
  }
}

int main(int argc, char **argv) {
  parse_args(argc, argv);
  
  uint32_t flen = srslte_sampling_freq_hz(nof_prb)/1000;
  uint32_t nsamples_adv = 3000; 

  cf_t *rx_buffer = malloc(sizeof(cf_t)*flen*nof_frames);
  if (!rx_buffer) {
    perror("malloc");
    exit(-1);
  }

  cf_t *tx_buffer = malloc(sizeof(cf_t)*(flen+nsamples_adv));
  if (!tx_buffer) {
    perror("malloc");
    exit(-1);
  }
  bzero(tx_buffer, sizeof(cf_t)*(flen+nsamples_adv));
  
  cf_t *zeros = calloc(sizeof(cf_t),flen);
  if (!zeros) {
    perror("calloc");
    exit(-1);
  }
 
  // Send through RF 
  rf_t rf; 
  printf("Opening RF device...\n");
  if (rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }
  rf_set_master_clock_rate(&rf, 30.72e6);        
  
  int srate = srslte_sampling_freq_hz(nof_prb);    
  if (srate < 10e6) {          
    rf_set_master_clock_rate(&rf, 4*srate);        
  } else {
    rf_set_master_clock_rate(&rf, srate);        
  }
  rf_set_rx_srate(&rf, (double) srate);
  rf_set_tx_srate(&rf, (double) srate);

  printf("Subframe len:   %d samples\n", flen);
  printf("Set TX/RX rate: %.2f MHz\n", (float) srate / 1000000);
  printf("Set RX gain: %.1f dB\n", rf_set_rx_gain(&rf, rf_rx_gain));
  printf("Set TX gain: %.1f dB\n", rf_set_tx_gain(&rf, rf_tx_gain));
  printf("Set TX/RX freq: %.2f MHz\n", rf_set_rx_freq(&rf, rf_freq) / 1000000);
  
  sleep(1);
  
  if (input_filename) {
    srslte_vec_load_file(input_filename, &tx_buffer[nsamples_adv], flen*sizeof(cf_t));
  } else {
    for (int i=0;i<flen-nsamples_adv;i++) {
      tx_buffer[i+nsamples_adv] = cexpf(_Complex_I*2*M_PI*tone_offset_hz*(float) i/(float) srate);       
    }
    srslte_vec_save_file("rf_txrx_tone", tx_buffer, flen*sizeof(cf_t));
  }

  srslte_timestamp_t tstamp; 
  
  rf_start_rx_stream(&rf);
  uint32_t nframe=0;
  
  float burst_settle_time = (float) nsamples_adv/srslte_sampling_freq_hz(nof_prb);
  
  printf("timeadv=%f\n",burst_settle_time);

  while(nframe<nof_frames) {
    printf("Rx subframe %d\n", nframe);
    rf_recv_with_time(&rf, &rx_buffer[flen*nframe], flen, true, &tstamp.full_secs, &tstamp.frac_secs);
    nframe++;
    if (nframe==9 || nframe==8) {
      srslte_timestamp_add(&tstamp, 0, 2e-3-burst_settle_time);
      if (nframe==8) {
        //rf_send_timed2(&rf, zeros, flen, tstamp.full_secs, tstamp.frac_secs, true, false);      
        printf("Transmitting zeros\n");        
      } else {
        rf_send_timed2(&rf, tx_buffer, flen+nsamples_adv, tstamp.full_secs, tstamp.frac_secs, true, true);      
        printf("Transmitting Signal\n");  
      }
    }

  }

  srslte_vec_save_file(output_filename, &rx_buffer[10*flen], flen*sizeof(cf_t));

  free(tx_buffer);
  free(rx_buffer);

  printf("Done\n");
  exit(0);
}
