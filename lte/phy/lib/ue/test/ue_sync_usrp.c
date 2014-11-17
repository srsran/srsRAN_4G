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
#include <complex.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "liblte/phy/phy.h"

#include "liblte/cuhd/cuhd.h"
void *uhd;

#ifndef DISABLE_GRAPHICS
#include "liblte/graphics/plot.h"
plot_real_t poutfft;
#endif

int nof_frames = -1;
float threshold = -1.0; 
int N_id_2 = -1;
uint32_t nof_prb = 6; 

float uhd_freq = 0.0, uhd_gain = 20.0;
char *uhd_args = "";
int disable_plots = 0;

void usage(char *prog) {
  printf("Usage: %s [agntdpv] -f uhd_freq -i N_id_2\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-n nof_frames [Default infinite]\n");
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-t threshold [Default %.2f]\n",threshold);
  
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#endif
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "agntdvfip")) != -1) {
    switch (opt) {
    case 'i':
      N_id_2 = atoi(argv[optind]);
      break;
    case 'p':
      nof_prb = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
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
    case 'd':
      disable_plots = 1;
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (uhd_freq == 0.0 || N_id_2 == -1) {
    usage(argv[0]);
    exit(-1);
  }
}

void input_init() {

  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }  
  cuhd_set_rx_gain(uhd, uhd_gain);
  
  /* set uhd_freq */
  cuhd_set_rx_freq(uhd, (double) uhd_freq);
  cuhd_rx_wait_lo_locked(uhd);
  DEBUG("Set uhd_freq to %.3f MHz\n", (double ) uhd_freq/1000000);

  int srate = lte_sampling_freq_hz(nof_prb);
  if (srate > 0) {
    cuhd_set_rx_srate(uhd, (double) srate);    
  } else {
    fprintf(stderr, "Error invalid nof_prb=%d\n",nof_prb);
  }
  DEBUG("Starting receiver...\n", 0);
  cuhd_start_rx_stream(uhd);

}

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

#ifndef DISABLE_GRAPHICS

void init_plots() {
  plot_init();

  plot_real_init(&poutfft);
  plot_real_setTitle(&poutfft, "Output FFT - Magnitude");
  plot_real_setLabels(&poutfft, "Index", "dB");
  plot_real_setYAxisScale(&poutfft, -60, 0);
  
}

#endif

float tmp_plot[100000];

int main(int argc, char **argv) {
  cf_t *input_buffer, *sf_symbols = NULL; 
  int frame_cnt;
  ue_sync_t s; 
  int pos;
  pss_synch_t pss; 
  float peak;
  struct timeval t[3];
  float mean_ce_time=0;
  lte_fft_t fft; 
  lte_cell_t cell; 
  
  bzero(&cell, sizeof(lte_cell_t));
  
  parse_args(argc, argv);
    
  #ifndef DISABLE_GRAPHICS
  if (!disable_plots) {
    init_plots();    
  }
  #endif
  
  input_init();
  
  cell.cp = CPNORM;
  cell.id = N_id_2;
  cell.nof_ports = 1;
  cell.nof_prb = nof_prb;
        
  if (ue_sync_init(&s, cell, cuhd_recv_wrapper, uhd)) {
    fprintf(stderr, "Error initiating UE sync module\n");
    exit(-1);
  }

  pss_synch_init_fft(&pss, 
                    SF_LEN(lte_symbol_sz(cell.nof_prb)), 
                    lte_symbol_sz(cell.nof_prb));
  pss_synch_set_N_id_2(&pss, cell.id%3);
  sf_symbols = vec_malloc(SLOT_LEN_RE(cell.nof_prb, cell.cp) * sizeof(cf_t));
  if (!sf_symbols) {
    perror("malloc");
    exit(-1);
  }
  lte_fft_init(&fft, cell.cp, cell.nof_prb);

  frame_cnt = 0;
  mean_ce_time=0;
  uint32_t valid_frames=0;
  //uint32_t unaligned = 0; 
  while (frame_cnt < nof_frames || nof_frames == -1) {    

    int n = ue_sync_get_buffer(&s, &input_buffer);
    if (n < 0) {
      fprintf(stderr, "Error calling sync work()\n");
      exit(-1);      
    }
    
    if (n == 1 && ue_sync_get_sfidx(&s) == 0) {

      mean_ce_time = (float) (mean_ce_time + (float) t[0].tv_usec * valid_frames) / (valid_frames+1);
      valid_frames++;
      
      #ifndef DISABLE_GRAPHICS
      if (!disable_plots && !(valid_frames % 5) && sf_symbols) {
        
        /* Run FFT for the second slot */
        lte_fft_run_slot(&fft, input_buffer, sf_symbols);
    
        int i;
        int nof_re = SLOT_LEN_RE(cell.nof_prb, cell.cp);
        for (i = 0; i < nof_re; i++) {
          tmp_plot[i] = 10 * log10f(cabsf(sf_symbols[i]));
          if (isinf(tmp_plot[i])) {
            tmp_plot[i] = -80;
          }
        }
        plot_real_setNewData(&poutfft, tmp_plot, nof_re);        
      }
    #endif
      
      pos = pss_synch_find_pss(&pss, input_buffer, &peak);      
      printf("CELL_ID: %3d CFO: %+.4f KHz, SFO: %+.4f Khz, TimeOffset: %4d\r",
              sync_get_cell_id(&s.sfind), ue_sync_get_cfo(&s)/1000, ue_sync_get_sfo(&s)/1000, pos);
          fflush(stdout);
      if (VERBOSE_ISINFO()) {
        printf("\n");
      }
    }
      
    frame_cnt++;
  }

  printf("\nBye\n");
  exit(0);
}


