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


#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots(float *corr, float energy, uint32_t size, cf_t ce[PSS_LEN]);
void do_plots_sss(float *corr_m0, float *corr_m1);
#endif


bool disable_plots = false; 
int cell_id = -1;
char *uhd_args="";
float uhd_gain=40.0, uhd_freq=-1.0;
int nof_frames = -1;
uint32_t fft_size=64;
float threshold = 0.4; 
int N_id_2_sync = -1;
lte_cp_t cp=CPNORM;

void usage(char *prog) {
  printf("Usage: %s [aedgtvnp] -f rx_frequency_hz -i cell_id\n", prog);
  printf("\t-a UHD args [Default %s]\n", uhd_args);
  printf("\t-g UHD Gain [Default %.2f dB]\n", uhd_gain);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l N_id_2 to sync [Default use cell_id]\n");
  printf("\t-e Extended CP [Default Normal]\n", fft_size);
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v verbose\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "adgetvsfil")) != -1) {
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
    case 'e':
      cp = CPEXT;
      break;
    case 'i':
      cell_id = atoi(argv[optind]);
      break;
    case 'l':
      N_id_2_sync = atoi(argv[optind]);
      break;
    case 's':
      fft_size = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'd':
      disable_plots = true;
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (cell_id < 0 || uhd_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}
  float m0_value, m1_value; 

int main(int argc, char **argv) {
  cf_t *buffer; 
  int frame_cnt, n; 
  void *uhd;
  pss_synch_t pss; 
  cfo_t cfocorr, cfocorr64; 
  sss_synch_t sss; 
  int32_t flen; 
  int peak_idx, last_peak;
  float peak_value; 
  float mean_peak; 
  uint32_t nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;
  cf_t ce[PSS_LEN]; 
  
  parse_args(argc, argv);

  if (N_id_2_sync == -1) {
    N_id_2_sync = cell_id%3;
  }
  uint32_t N_id_2 = cell_id%3;
  uint32_t N_id_1 = cell_id/3;
  
  if (!disable_plots)
    init_plots();
  
  flen = 4800*(fft_size/64);
  
  buffer = malloc(sizeof(cf_t) * flen * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }
    
  if (pss_synch_init_fft(&pss, flen, fft_size)) {
    fprintf(stderr, "Error initiating PSS\n");
    exit(-1);
  }

  if (pss_synch_set_N_id_2(&pss, N_id_2_sync)) {
    fprintf(stderr, "Error setting N_id_2=%d\n",N_id_2_sync);
    exit(-1);
  }
  
  cfo_init(&cfocorr, flen); 
  cfo_init(&cfocorr64, flen); 
 
  if (sss_synch_init(&sss, fft_size)) {
    fprintf(stderr, "Error initializing SSS object\n");
    return LIBLTE_ERROR;
  }

  sss_synch_set_N_id_2(&sss, N_id_2);

  printf("Opening UHD device...\n");
  if (cuhd_open(uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
  printf("N_id_2: %d\n", N_id_2);  
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
  int peak_offset = 0;
  float cfo; 
  float mean_cfo = 0; 
  uint32_t m0, m1; 
  uint32_t sss_error1 = 0, sss_error2 = 0, sss_error3 = 0; 
  uint32_t cp_is_norm = 0; 
  
  sync_t ssync; 
  bzero(&ssync, sizeof(sync_t));
  ssync.fft_size = fft_size;
  
  while(frame_cnt < nof_frames || nof_frames == -1) {
    n = cuhd_recv(uhd, buffer, flen - peak_offset, 1);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }
    
    peak_idx = pss_synch_find_pss(&pss, buffer, &peak_value);
    if (peak_idx < 0) {
      fprintf(stderr, "Error finding PSS peak\n");
      exit(-1);
    }
        
    mean_peak = VEC_CMA(peak_value, mean_peak, frame_cnt);
    
    if (peak_value >= threshold) {
      nof_det++;
        
      if (peak_idx >= fft_size) {

        // Estimate CFO 
        cfo = pss_synch_cfo_compute(&pss, &buffer[peak_idx-fft_size]);
        mean_cfo = VEC_CMA(cfo, mean_cfo, frame_cnt);        

        // Correct CFO
        cfo_correct(&cfocorr, buffer, buffer, -mean_cfo / fft_size);               

        // Estimate channel
        if (pss_synch_chest(&pss, &buffer[peak_idx-fft_size], ce)) {
          fprintf(stderr, "Error computing channel estimation\n");
          exit(-1);
        }
        
        // Find SSS 
        int sss_idx = peak_idx-2*fft_size-(CP_ISNORM(cp)?CP(fft_size, CPNORM_LEN):CP(fft_size, CPEXT_LEN));             
        if (sss_idx >= 0 && sss_idx < flen-fft_size) {
          sss_synch_m0m1_partial(&sss, &buffer[sss_idx], 3, NULL, &m0, &m0_value, &m1, &m1_value);
          if (sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error2++;            
          }
          INFO("Partial N_id_1: %d\n", sss_synch_N_id_1(&sss, m0, m1));
          sss_synch_m0m1_diff(&sss, &buffer[sss_idx], &m0, &m0_value, &m1, &m1_value);
          if (sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error3++;            
          }
          INFO("Diff N_id_1: %d\n", sss_synch_N_id_1(&sss, m0, m1));
          sss_synch_m0m1_partial(&sss, &buffer[sss_idx], 1, NULL, &m0, &m0_value, &m1, &m1_value);
          if (sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error1++;            
          }
          INFO("Full N_id_1: %d\n", sss_synch_N_id_1(&sss, m0, m1));
        }
        
        // Estimate CP 
        if (peak_idx > 2*(fft_size + CP_EXT(fft_size))) {
          lte_cp_t cp = sync_detect_cp(&ssync, buffer, peak_idx);
          if (CP_ISNORM(cp)) {
            cp_is_norm++; 
          }          
        }
        
      } else {
        INFO("No space for CFO computation. Frame starts at \n",peak_idx);
      }
      
      if (!disable_plots && sss_synch_subframe(m0,m1) == 0)
        do_plots_sss(sss.corr_output_m0, sss.corr_output_m1);
      
    } else {
      nof_nodet++;
    }

    if (frame_cnt > 100) {
      if (abs(last_peak-peak_idx) > 4) {
        if (peak_value >= threshold) {
          nof_nopeakdet++;
        } 
        nof_nopeak++;                  
      } 
    }
    
    frame_cnt++;
   
    printf("[%5d]: Pos: %5d, PSR: %4.1f (~%4.1f) Pdet: %4.2f, "
           "FA: %4.2f, CFO: %+4.1f KHz SSSmiss: %4.2f/%4.2f/%4.2f CPNorm: %.0f\%\r", 
           frame_cnt, 
           peak_idx, 
           peak_value, mean_peak,
           (float) nof_det/frame_cnt, 
           (float) nof_nopeakdet/frame_cnt, mean_cfo*15, 
           (float) sss_error1/nof_det,(float) sss_error2/nof_det,(float) sss_error3/nof_det,
           (float) cp_is_norm/nof_det * 100);
    
    if (VERBOSE_ISINFO()) {
      printf("\n");
    }
  
    if (!disable_plots)
      do_plots(pss.conv_output_avg, pss.conv_output_avg[peak_idx], pss.fft_size+pss.frame_size-1, ce);
    
    last_peak = peak_idx;

  }
  
  pss_synch_free(&pss);
  free(buffer);
  cuhd_close(uhd);

  printf("Ok\n");
  exit(0);
}

extern cf_t *tmp2;


/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS


#include "liblte/graphics/plot.h"
plot_real_t pssout;
//plot_complex_t pce; 

plot_real_t psss1;//, psss2;

float tmp[100000];
cf_t tmpce[PSS_LEN];


void init_plots() {
  plot_init();
  plot_real_init(&pssout);
  plot_real_setTitle(&pssout, "PSS xCorr");
  plot_real_setLabels(&pssout, "Index", "Absolute value");
  plot_real_setYAxisScale(&pssout, 0, 1);
  
  /*
  plot_complex_init(&pce);
  plot_complex_setTitle(&pce, "Channel Estimates");
  plot_complex_setYAxisScale(&pce, Ip, -2, 2);
  plot_complex_setYAxisScale(&pce, Q, -2, 2);
  plot_complex_setYAxisScale(&pce, Magnitude, 0, 2);
  plot_complex_setYAxisScale(&pce, Phase, -M_PI, M_PI);
  */
  
  plot_real_init(&psss1);
  plot_real_setTitle(&psss1, "SSS xCorr m0");
  plot_real_setLabels(&psss1, "Index", "Absolute value");
  plot_real_setYAxisScale(&psss1, 0, 1);
  
  /*
  plot_real_init(&psss2);
  plot_real_setTitle(&psss2, "SSS xCorr m1");
  plot_real_setLabels(&psss2, "Index", "Absolute value");
  plot_real_setYAxisScale(&psss2, 0, 1);
  */
  

}

void do_plots(float *corr, float energy, uint32_t size, cf_t ce[PSS_LEN]) {  
  vec_sc_prod_fff(corr,1./energy,tmp, size);
  plot_real_setNewData(&pssout, tmp, size);        
  
//  float norm = vec_avg_power_cf(ce, PSS_LEN);
 // vec_sc_prod_cfc(ce, 1.0/sqrt(norm), tmpce, PSS_LEN);
  
  //plot_complex_setNewData(&pce, tmpce, PSS_LEN);
}

void do_plots_sss(float *corr_m0, float *corr_m1) {  
  if (m0_value > 0) 
    vec_sc_prod_fff(corr_m0,1./m0_value,corr_m0, N_SSS);
  plot_real_setNewData(&psss1, corr_m0, N_SSS);        
  
//  if (m1_value > 0) 
//    vec_sc_prod_fff(corr_m1,1./m1_value,corr_m1, N_SSS);
//  plot_real_setNewData(&psss2, corr_m1, N_SSS);        
}

#endif

