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
#include <assert.h>
#include <signal.h>

#include "liblte/rrc/rrc.h"
#include "liblte/phy/phy.h"
#include "liblte/cuhd/cuhd.h"
#include "cuhd_utils.h"

#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots(ue_dl_t *q, uint32_t sf_idx, ue_sync_t *qs);
#endif


#define B210_DEFAULT_GAIN         40.0
#define B210_DEFAULT_GAIN_CORREC  110.0 // Gain of the Rx chain when the gain is set to 40

float gain_offset = B210_DEFAULT_GAIN_CORREC;


cell_search_cfg_t cell_detect_config = {
  5000,
  100, // nof_frames_total 
  4.0 // threshold
};

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  bool disable_plots;
  int force_N_id_2;
  uint16_t rnti;
  char *uhd_args; 
  float uhd_freq; 
  float uhd_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1;
  args->rnti = SIRNTI;
  args->force_N_id_2 = -1; // Pick the best
  args->uhd_args = "";
  args->uhd_freq = -1.0;
  args->uhd_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agldnrv] -f rx_frequency (in Hz)\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->uhd_gain);
  printf("\t-r RNTI [Default 0x%x]\n",args->rnti);
  printf("\t-l Force N_id_2 [Default best]\n");
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "agldnvrf")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'g':
      args->uhd_gain = atof(argv[optind]);
      break;
    case 'f':
      args->uhd_freq = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'r':
      args->rnti = atoi(argv[optind]);
      break;
    case 'l':
      args->force_N_id_2 = atoi(argv[optind]);
      break;
    case 'd':
      args->disable_plots = true;
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->uhd_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t data[10000], data_unpacked[1000];

bool go_exit = false; 

void sig_int_handler(int signo)
{
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}

extern float mean_exec_time;

enum receiver_state { DECODE_MIB, DECODE_SIB} state; 


int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer; 
  prog_args_t prog_args; 
  lte_cell_t cell;  
  int64_t sf_cnt;
  ue_sync_t ue_sync; 
  ue_mib_t ue_mib; 
  void *uhd; 
  ue_dl_t ue_dl; 
  uint32_t nof_trials = 0; 
  uint32_t sfn = 0; // system frame number
  int n; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN], bch_payload_unpacked[BCH_PAYLOAD_LEN];
  uint32_t sfn_offset; 
  parse_args(&prog_args, argc, argv);

  printf("Opening UHD device...\n");
  if (cuhd_open(prog_args.uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }
  /* Set receiver gain */
  cuhd_set_rx_gain(uhd, prog_args.uhd_gain);

  /* set receiver frequency */
  cuhd_set_rx_freq(uhd, (double) prog_args.uhd_freq);
  cuhd_rx_wait_lo_locked(uhd);
  printf("Tunning receiver to %.3f MHz\n", (double ) prog_args.uhd_freq/1000000);

  ret = cuhd_search_and_decode_mib(uhd, &cell_detect_config, prog_args.force_N_id_2, &cell);
  if (ret < 0) {
    fprintf(stderr, "Error searching for cell\n");
    exit(-1); 
  } else if (ret == 0) {
    printf("Cell not found\n");
    exit(0);
  }
  
  /* set sampling frequency */
  int srate = lte_sampling_freq_hz(cell.nof_prb);
  if (srate != -1) {  
    cuhd_set_rx_srate(uhd, (double) srate);      
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
    return LIBLTE_ERROR;
  }


  INFO("Stopping UHD and flushing buffer...\r",0);
  cuhd_stop_rx_stream(uhd);
  cuhd_flush_buffer(uhd);
  
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }
  if (ue_dl_init(&ue_dl, cell, 1234)) {  // This is the User RNTI
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  if (ue_mib_init(&ue_mib, cell)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }

  /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
  ue_dl_set_rnti(&ue_dl, prog_args.rnti); 

  /* Initialize subframe counter */
  sf_cnt = 0;

  // Register Ctrl+C handler
  signal(SIGINT, sig_int_handler);

  #ifndef DISABLE_GRAPHICS
  if (!prog_args.disable_plots) {
    init_plots();    
  }
  #endif
  

  
  cuhd_start_rx_stream(uhd);
  
  // Variables for measurements 
  uint32_t nframes=0;
  float rsrp=0.0, rsrq=0.0, snr=0.0;

  /* Main loop */
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {
    
    ret = ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling ue_sync_work()\n");
    }
        
    /* ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
      switch (state) {
        case DECODE_MIB:
          if (ue_sync_get_sfidx(&ue_sync) == 0) {
            pbch_decode_reset(&ue_mib.pbch);
            n = ue_mib_decode(&ue_mib, sf_buffer, bch_payload_unpacked, NULL, &sfn_offset);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE MIB\n");
              exit(-1);
            } else if (n == MIB_FOUND) {             
              bit_unpack_vector(bch_payload_unpacked, bch_payload, BCH_PAYLOAD_LEN);
              bcch_bch_unpack(bch_payload, BCH_PAYLOAD_LEN, &cell, &sfn);
              printf("Decoded MIB. SFN: %d, offset: %d\n", sfn, sfn_offset);
              sfn = (sfn + sfn_offset)%1024; 
              state = DECODE_SIB; 
            }
          }
          break;
        case DECODE_SIB:
          /* We are looking for SI Blocks, search only in appropiate places */
          if ((ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) {
            n = ue_dl_decode_sib(&ue_dl, sf_buffer, data, ue_sync_get_sfidx(&ue_sync), 
                                 ((int) ceilf((float)3*(((sfn)/2)%4)/2))%4);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
              exit(-1);
            } 
            nof_trials++; 
            
            rsrq = VEC_EMA(chest_dl_get_rsrq(&ue_dl.chest), rsrq, 0.05);
            rsrp = VEC_EMA(chest_dl_get_rsrp(&ue_dl.chest), rsrp, 0.05);      
            snr = VEC_EMA(chest_dl_get_snr(&ue_dl.chest), snr, 0.05);      
            nframes++;
            if (isnan(rsrq)) {
              rsrq = 0; 
            }
          }
          
          // Plot and Printf
          if (ue_sync_get_sfidx(&ue_sync) == 5) {
            printf("CFO: %+8.4f KHz, SFO: %+8.4f Khz, "
                  "RSRP: %+5.1f dBm, RSRQ: %5.1f dB, SNR: %4.1f dB, "
                  "PDCCH-Miss: %5.2f%%, PDSCH-BLER: %5.2f%% (%d blocks)\r",
                  ue_sync_get_cfo(&ue_sync)/1000, ue_sync_get_sfo(&ue_sync)/1000, 
                  10*log10(rsrp*1000)-gain_offset, 
                  10*log10(rsrq), 10*log10(snr), 
                  100*(1-(float) ue_dl.nof_pdcch_detected/nof_trials),
                  (float) 100*ue_dl.pkt_errors/ue_dl.pkts_total,nof_trials, ue_dl.pkts_total);                
          }
          break;
      }
      if (ue_sync_get_sfidx(&ue_sync) == 9) {
        sfn++; 
        if (sfn == 1024) {
          sfn = 0; 
        }
      }
      #ifndef DISABLE_GRAPHICS
      if (!prog_args.disable_plots && ue_sync_get_sfidx(&ue_sync) == 5) {
        do_plots(&ue_dl, 5, &ue_sync);          
      }
      #endif
    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        sync_get_peak_value(&ue_sync.sfind), 
        ue_sync.frame_total_cnt, ue_sync.state);      
    }
        
    sf_cnt++;                  
  } // Main loop
  
  ue_dl_free(&ue_dl);
  ue_mib_free(&ue_mib);
  ue_sync_free(&ue_sync);
  cuhd_close(uhd);
  printf("\nBye\n");
  exit(0);
}






  

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS


#include "liblte/graphics/plot.h"
plot_real_t poutfft, p_sync;
plot_real_t pce;
plot_scatter_t  pscatequal, pscatequal_pdcch;

float tmp_plot[SLOT_LEN_RE(MAX_PRB, CPNORM)];
float tmp_plot2[SLOT_LEN_RE(MAX_PRB, CPNORM)];
float tmp_plot3[SLOT_LEN_RE(MAX_PRB, CPNORM)];

void init_plots() {
  plot_init();
  plot_real_init(&poutfft);
  plot_real_setTitle(&poutfft, "Output FFT - Magnitude");
  plot_real_setLabels(&poutfft, "Index", "dB");
  plot_real_setYAxisScale(&poutfft, -60, 0);

  plot_real_init(&pce);
  plot_real_setTitle(&pce, "Channel Response - Magnitude");
  plot_real_setLabels(&pce, "Index", "dB");
  plot_real_setYAxisScale(&pce, -60, 0);

  plot_real_init(&p_sync);
  plot_real_setTitle(&p_sync, "PSS Cross-Corr abs value");
  plot_real_setYAxisScale(&p_sync, 0, 1);

  plot_scatter_init(&pscatequal);
  plot_scatter_setTitle(&pscatequal, "PDSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal, -2, 2);
  plot_scatter_setYAxisScale(&pscatequal, -2, 2);

  plot_scatter_init(&pscatequal_pdcch);
  plot_scatter_setTitle(&pscatequal_pdcch, "PDCCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal_pdcch, -2, 2);
  plot_scatter_setYAxisScale(&pscatequal_pdcch, -2, 2);

}

void do_plots(ue_dl_t *q, uint32_t sf_idx, ue_sync_t *qs) {
  int i;
  uint32_t nof_re = SLOT_LEN_RE(q->cell.nof_prb, q->cell.cp);
  uint32_t nof_symbols = q->harq_process[0].prb_alloc.re_sf[sf_idx];
  for (i = 0; i < nof_re; i++) {
    tmp_plot[i] = 20 * log10f(cabsf(q->sf_symbols[i]));
    if (isinf(tmp_plot[i])) {
      tmp_plot[i] = -80;
    }
  }
  for (i = 0; i < REFSIGNAL_NUM_SF(q->cell.nof_prb,0); i++) {
    tmp_plot2[i] = 20 * log10f(cabsf(q->chest.pilot_estimates_average[0][i]));
    if (isinf(tmp_plot2[i])) {
      tmp_plot2[i] = -80;
    }
  }
  plot_real_setNewData(&poutfft, tmp_plot, nof_re);        
  plot_real_setNewData(&pce, tmp_plot2, REFSIGNAL_NUM_SF(q->cell.nof_prb,0));        
  int max = vec_max_fi(qs->strack.pss.conv_output_avg, qs->strack.pss.frame_size+qs->strack.pss.fft_size-1);
  vec_sc_prod_fff(qs->strack.pss.conv_output_avg, 
                  1/qs->strack.pss.conv_output_avg[max], 
                  tmp_plot2, 
                  qs->strack.pss.frame_size+qs->strack.pss.fft_size-1);        
  plot_real_setNewData(&p_sync, tmp_plot2, qs->strack.pss.frame_size);        
  
  plot_scatter_setNewData(&pscatequal, q->pdsch.pdsch_d, nof_symbols);
  plot_scatter_setNewData(&pscatequal_pdcch, q->pdcch.pdcch_d, 36*q->pdcch.nof_cce);
}

#endif
