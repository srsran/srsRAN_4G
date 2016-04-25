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
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <stdbool.h>

#include "srslte/srslte.h"


#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots(float *corr, float energy, uint32_t size, cf_t ce[SRSLTE_PSS_LEN]);
void do_plots_sss(float *corr_m0, float *corr_m1);
void destroy_plots();
#endif


bool disable_plots = false;
char *input_file_name;
int cell_id = -1;
int nof_frames = -1;
uint32_t fft_size=64;
float threshold = 0.4; 
int N_id_2_sync = -1;
srslte_cp_t cp=SRSLTE_CP_NORM;
int file_offset = 0; 

void usage(char *prog) {
  printf("Usage: %s [nlestodv] -i cell_id -f input_file_name\n", prog);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l N_id_2 to sync [Default use cell_id]\n");
  printf("\t-e Extended CP [Default Normal]\n", fft_size);
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-o file read offset [Default %d]\n", file_offset);
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "nlestdvoif")) != -1) {
    switch (opt) {
    case 'f':
      input_file_name = argv[optind];
      break;
    case 't':
      threshold = atof(argv[optind]);
      break;
    case 'e':
      cp = SRSLTE_CP_EXT;
      break;
    case 'i':
      cell_id = atoi(argv[optind]);
      break;
    case 'o':
      file_offset = atoi(argv[optind]);
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
      srslte_verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
  if (cell_id < 0) {
    usage(argv[0]);
    exit(-1);
  }
}
  float m0_value, m1_value; 

int main(int argc, char **argv) {
  srslte_filesource_t fsrc;
  cf_t *buffer; 
  int frame_cnt, n;
  srslte_pss_synch_t pss; 
  srslte_cfo_t cfocorr, cfocorr64; 
  srslte_sss_synch_t sss; 
  int32_t flen; 
  int peak_idx, last_peak;
  float peak_value; 
  float mean_peak; 
  uint32_t nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;
  cf_t ce[SRSLTE_PSS_LEN]; 
  
  parse_args(argc, argv);

  if (N_id_2_sync == -1) {
    N_id_2_sync = cell_id%3;
  }
  uint32_t N_id_2 = cell_id%3;
  uint32_t N_id_1 = cell_id/3;

#ifndef DISABLE_GRAPHICS
  if (!disable_plots)
    init_plots();
#endif
  
  flen = fft_size*15*5;
  
  buffer = malloc(sizeof(cf_t) * flen * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }
    
  if (srslte_pss_synch_init_fft(&pss, flen, fft_size)) {
    fprintf(stderr, "Error initiating PSS\n");
    exit(-1);
  }

  if (srslte_pss_synch_set_N_id_2(&pss, N_id_2_sync)) {
    fprintf(stderr, "Error setting N_id_2=%d\n",N_id_2_sync);
    exit(-1);
  }
  
  srslte_cfo_init(&cfocorr, flen); 
  srslte_cfo_init(&cfocorr64, flen); 
 
  if (srslte_sss_synch_init(&sss, fft_size)) {
    fprintf(stderr, "Error initializing SSS object\n");
    return SRSLTE_ERROR;
  }

  srslte_sss_synch_set_N_id_2(&sss, N_id_2);

  printf("Opening file...\n");
  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }
  printf("N_id_2: %d\n", N_id_2);  
  
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
  
  srslte_sync_t ssync; 
  bzero(&ssync, sizeof(srslte_sync_t));
  ssync.fft_size = fft_size;
  
  n = srslte_filesource_read(&fsrc, buffer, file_offset);
  
  while(frame_cnt < nof_frames || nof_frames == -1) {
    n = srslte_filesource_read(&fsrc, buffer, flen - peak_offset);
    if (n < 0) {
      fprintf(stderr, "Error reading samples\n");
      exit(-1);
    }
    if (n < flen - peak_offset) {
      fprintf(stdout, "End of file (n=%d, flen=%d, peak=%d)\n", n, flen, peak_offset);
      break;
    }
    
    peak_idx = srslte_pss_synch_find_pss(&pss, buffer, &peak_value);
    if (peak_idx < 0) {
      fprintf(stderr, "Error finding PSS peak\n");
      exit(-1);
    }
        
    mean_peak = SRSLTE_VEC_CMA(peak_value, mean_peak, frame_cnt);
    
    if (peak_value >= threshold) {
      nof_det++;
        
      if (peak_idx >= fft_size) {

        // Estimate CFO 
        cfo = srslte_pss_synch_cfo_compute(&pss, &buffer[peak_idx-fft_size]);
        mean_cfo = SRSLTE_VEC_CMA(cfo, mean_cfo, frame_cnt);        

        // Correct CFO
        srslte_cfo_correct(&cfocorr, buffer, buffer, -mean_cfo / fft_size);               

        // Estimate channel
        if (srslte_pss_synch_chest(&pss, &buffer[peak_idx-fft_size], ce)) {
          fprintf(stderr, "Error computing channel estimation\n");
          exit(-1);
        }
        
        // Find SSS 
        int sss_idx = peak_idx-2*fft_size-(SRSLTE_CP_ISNORM(cp)?SRSLTE_CP_LEN(fft_size, SRSLTE_CP_NORM_LEN):SRSLTE_CP_LEN(fft_size, SRSLTE_CP_EXT_LEN));             
        if (sss_idx >= 0 && sss_idx < flen-fft_size) {
          srslte_sss_synch_m0m1_partial(&sss, &buffer[sss_idx], 3, NULL, &m0, &m0_value, &m1, &m1_value);
          if (srslte_sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error2++;            
          }
          INFO("sf_idx = %d\n", srslte_sss_synch_subframe(m0, m1));
          INFO("Partial N_id_1: %d\n", srslte_sss_synch_N_id_1(&sss, m0, m1));
          srslte_sss_synch_m0m1_diff(&sss, &buffer[sss_idx], &m0, &m0_value, &m1, &m1_value);
          if (srslte_sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error3++;            
          }
          INFO("Diff N_id_1: %d\n", srslte_sss_synch_N_id_1(&sss, m0, m1));
          srslte_sss_synch_m0m1_partial(&sss, &buffer[sss_idx], 1, NULL, &m0, &m0_value, &m1, &m1_value);
          if (srslte_sss_synch_N_id_1(&sss, m0, m1) != N_id_1) {
            sss_error1++;     
          }
          INFO("Full N_id_1: %d\n", srslte_sss_synch_N_id_1(&sss, m0, m1));
        }
        
        // Estimate CP 
        if (peak_idx > 2*(fft_size + SRSLTE_CP_LEN_EXT(fft_size))) {
          srslte_cp_t cp = srslte_sync_detect_cp(&ssync, buffer, peak_idx);
          if (SRSLTE_CP_ISNORM(cp)) {
            cp_is_norm++; 
          }          
        }
        
      } else {
        INFO("No space for CFO computation. Frame starts at \n",peak_idx);
      }
      
      if(srslte_sss_synch_subframe(m0,m1) == 0)
      {
#ifndef DISABLE_GRAPHICS
          if (!disable_plots)
            do_plots_sss(sss.corr_output_m0, sss.corr_output_m1);
#endif
      }
      
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
           "FA: %4.2f, CFO: %+4.1f kHz SSSmiss: %4.2f/%4.2f/%4.2f CPNorm: %.0f%%\r", 
           frame_cnt, 
           peak_idx - flen/10, 
           peak_value, mean_peak,
           (float) nof_det/frame_cnt, 
           (float) nof_nopeakdet/frame_cnt, mean_cfo*15, 
           (float) sss_error1/nof_det,(float) sss_error2/nof_det,(float) sss_error3/nof_det,
           (float) cp_is_norm/nof_det * 100);
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }

    usleep(10000);
  
#ifndef DISABLE_GRAPHICS
    if (!disable_plots)
      do_plots(pss.conv_output_avg, pss.conv_output_avg[peak_idx], pss.fft_size+pss.frame_size-1, ce);
#endif

    last_peak = peak_idx;

  }
  
  srslte_pss_synch_free(&pss);
  free(buffer);
  srslte_filesource_free(&fsrc);
#ifndef DISABLE_GRAPHICS
  if (!disable_plots)
    destroy_plots();
#endif

  printf("Ok\n");
  exit(0);
}

extern cf_t *tmp2;


/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS


#include "srsgui/srsgui.h"
plot_real_t pssout;
//plot_complex_t pce; 

plot_real_t psss1;//, psss2;

float tmp[1000000];
cf_t tmpce[SRSLTE_PSS_LEN];


void init_plots() {
  sdrgui_init();
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

void do_plots(float *corr, float energy, uint32_t size, cf_t ce[SRSLTE_PSS_LEN]) {  
  srslte_vec_sc_prod_fff(corr,1./energy,tmp, size);
  plot_real_setNewData(&pssout, tmp, size);        
  
//  float norm = srslte_vec_avg_power_cf(ce, SRSLTE_PSS_LEN);
 // srslte_vec_sc_prod_cfc(ce, 1.0/sqrt(norm), tmpce, SRSLTE_PSS_LEN);
  
  //plot_complex_setNewData(&pce, tmpce, SRSLTE_PSS_LEN);
}

void do_plots_sss(float *corr_m0, float *corr_m1) {  
  if (m0_value > 0) 
    srslte_vec_sc_prod_fff(corr_m0,1./m0_value,corr_m0, SRSLTE_SSS_N);
  plot_real_setNewData(&psss1, corr_m0, SRSLTE_SSS_N);        
  
//  if (m1_value > 0) 
//    srslte_vec_sc_prod_fff(corr_m1,1./m1_value,corr_m1, SRSLTE_SSS_N);
//  plot_real_setNewData(&psss2, corr_m1, SRSLTE_SSS_N);        
}

void destroy_plots() {
  sdrgui_exit();
}


#endif
