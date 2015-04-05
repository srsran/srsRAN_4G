/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
#include <pthread.h>
#include <semaphore.h>

#include "srslte/srslte.h"


#ifndef DISABLE_UHD
#include "srslte/cuhd/cuhd.h"
#include "cuhd_utils.h"

cell_search_cfg_t cell_detect_config = {
  5000,
  100, // nof_frames_total 
  16.0 // threshold
};

#endif

//#define STDOUT_COMPACT

#ifndef DISABLE_GRAPHICS
#include "srsgui/srsgui.h"
void init_plots();
pthread_t plot_thread; 
sem_t plot_sem; 
uint32_t plot_sf_idx=0;
#endif


#define B210_DEFAULT_GAIN         40.0
#define B210_DEFAULT_GAIN_CORREC  110.0 // Gain of the Rx chain when the gain is set to 40

float gain_offset = B210_DEFAULT_GAIN_CORREC;


/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  bool disable_plots;
  int force_N_id_2;
  uint16_t rnti;
  char *input_file_name; 
  uint32_t file_nof_prb;
  char *uhd_args; 
  float uhd_freq; 
  float uhd_freq_offset; 
  float uhd_gain;
  int net_port; 
  char *net_address; 
  int net_port_signal; 
  char *net_address_signal;   
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1;
  args->rnti = SRSLTE_SIRNTI;
  args->force_N_id_2 = -1; // Pick the best
  args->input_file_name = NULL;
  args->file_nof_prb = 6; 
  args->uhd_args = "";
  args->uhd_freq = -1.0;
  args->uhd_freq = 8000000.0;
  args->uhd_gain = 60.0; 
  args->net_port = -1; 
  args->net_address = "127.0.0.1";
  args->net_port_signal = -1; 
  args->net_address_signal = "127.0.0.1";
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agildnruv] -f rx_frequency (in Hz) | -i input_file\n", prog);
#ifndef DISABLE_UHD
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD RX gain [Default %.2f dB]\n", args->uhd_gain);
  printf("\t-o UHD RX freq offset [Default %.1f MHz]\n", args->uhd_freq_offset/1000000);
#else
  printf("\t   UHD is disabled. CUHD library not available\n");
#endif
  printf("\t-i input_file [Default USRP]\n");
  printf("\t-p nof_prb for input file [Default %d]\n", args->file_nof_prb);
  printf("\t-r RNTI [Default 0x%x]\n",args->rnti);
  printf("\t-l Force N_id_2 [Default best]\n");
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-s remote UDP port to send input signal (-1 does nothing with it) [Default %d]\n", args->net_port_signal);
  printf("\t-S remote UDP address to send input signal [Default %s]\n", args->net_address_signal);
  printf("\t-u remote TCP port to send data (-1 does nothing with it) [Default %d]\n", args->net_port);
  printf("\t-U remote TCP address to send data [Default %s]\n", args->net_address);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "aoglipdnvrfuUsS")) != -1) {
    switch (opt) {
    case 'i':
      args->input_file_name = argv[optind];
      break;
    case 'p':
      args->file_nof_prb = atoi(argv[optind]);
      break;
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'g':
      args->uhd_gain = atof(argv[optind]);
      break;
    case 'o':
      args->uhd_freq_offset = atof(argv[optind]);
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
    case 'u':
      args->net_port = atoi(argv[optind]);
      break;
    case 'U':
      args->net_address = argv[optind];
      break;
    case 's':
      args->net_port_signal = atoi(argv[optind]);
      break;
    case 'S':
      args->net_address_signal = argv[optind];
      break;
    case 'd':
      args->disable_plots = true;
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->uhd_freq < 0 && args->input_file_name == NULL) {
    usage(args, argv[0]);
    exit(-1);
  }
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t data[20000], data_packed[20000];

bool go_exit = false; 

void sig_int_handler(int signo)
{
  if (signo == SIGINT) {
    go_exit = true;
  }
}

#ifndef DISABLE_UHD
int cuhd_recv_wrapper(void *h, void *data, uint32_t nsamples, srslte_timestamp_t *t) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv(h, data, nsamples, 1);
}
#endif

extern float mean_exec_time;

enum receiver_state { DECODE_MIB, DECODE_PDSCH} state; 

srslte_ue_dl_t ue_dl; 
srslte_ue_sync_t ue_sync; 
prog_args_t prog_args; 

uint32_t sfn = 0; // system frame number
cf_t *sf_buffer = NULL; 
srslte_netsink_t net_sink, net_sink_signal; 

int main(int argc, char **argv) {
  int ret; 
  srslte_cell_t cell;  
  int64_t sf_cnt;
  srslte_ue_mib_t ue_mib; 
#ifndef DISABLE_UHD
  void *uhd; 
#endif
  uint32_t nof_trials = 0; 
  int n; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN];
  uint32_t sfn_offset;
  
  parse_args(&prog_args, argc, argv);

  if (prog_args.net_port > 0) {
    if (srslte_netsink_init(&net_sink, prog_args.net_address, prog_args.net_port, SRSLTE_NETSINK_TCP)) {
      fprintf(stderr, "Error initiating UDP socket to %s:%d\n", prog_args.net_address, prog_args.net_port);
      exit(-1);
    }
    srslte_netsink_set_nonblocking(&net_sink);
  }
  if (prog_args.net_port_signal > 0) {
    if (srslte_netsink_init(&net_sink_signal, prog_args.net_address_signal, prog_args.net_port_signal, SRSLTE_NETSINK_UDP)) {
      fprintf(stderr, "Error initiating UDP socket to %s:%d\n", prog_args.net_address_signal, prog_args.net_port_signal);
      exit(-1);
    }
    srslte_netsink_set_nonblocking(&net_sink_signal);
  }
  
#ifndef DISABLE_UHD
  if (!prog_args.input_file_name) {
    printf("Opening UHD device...\n");
    if (cuhd_open(prog_args.uhd_args, &uhd)) {
      fprintf(stderr, "Error opening uhd\n");
      exit(-1);
    }
    /* Set receiver gain */
    cuhd_set_rx_gain(uhd, prog_args.uhd_gain);

    /* set receiver frequency */
    cuhd_set_rx_freq_offset(uhd, (double) prog_args.uhd_freq, prog_args.uhd_freq_offset);
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
    int srate = srslte_sampling_freq_hz(cell.nof_prb);
    if (srate != -1) {  
      cuhd_set_rx_srate(uhd, (double) srate);      
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
      return SRSLTE_ERROR;
    }

    INFO("Stopping UHD and flushing buffer...\r",0);
    cuhd_stop_rx_stream(uhd);
    cuhd_flush_buffer(uhd);
    
    if (srslte_ue_mib_init(&ue_mib, cell)) {
      fprintf(stderr, "Error initaiting UE MIB decoder\n");
      exit(-1);
    }    
  }
#endif

  /* If reading from file, go straight to PDSCH decoding. Otherwise, decode MIB first */
  if (prog_args.input_file_name) {
    state = DECODE_PDSCH; 
    /* preset cell configuration */
    cell.id = 1; 
    cell.cp = SRSLTE_SRSLTE_CP_NORM; 
    cell.phich_length = SRSLTE_PHICH_NORM;
    cell.phich_resources = SRSLTE_PHICH_R_1;
    cell.nof_ports = 1; 
    cell.nof_prb = prog_args.file_nof_prb; 
    
    if (srslte_ue_sync_init_file(&ue_sync, prog_args.file_nof_prb, prog_args.input_file_name)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      exit(-1); 
    }

  } else {
#ifndef DISABLE_UHD
    state = DECODE_MIB; 
    if (srslte_ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper, uhd)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      exit(-1); 
    }
#endif
  }

  if (srslte_ue_dl_init(&ue_dl, cell)) {  // This is the User RNTI
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  
  /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
  srslte_ue_dl_set_rnti(&ue_dl, prog_args.rnti); 

  /* Initialize subframe counter */
  sf_cnt = 0;

  // Register Ctrl+C handler
  signal(SIGINT, sig_int_handler);

#ifndef DISABLE_GRAPHICS
  if (!prog_args.disable_plots) {
    init_plots(cell);    
  }
#endif

#ifndef DISABLE_UHD
  if (!prog_args.input_file_name) {
    cuhd_start_rx_stream(uhd);    
  }
#endif
    
  // Variables for measurements 
  uint32_t nframes=0;
  float rsrp=0.0, rsrq=0.0, snr=0.0;
  bool decode_pdsch; 
  int pdcch_tx=0; 
          
  /* Main loop */
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {
    
    ret = srslte_ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling srslte_ue_sync_work()\n");
    }
            
    /* srslte_ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
      switch (state) {
        case DECODE_MIB:
          if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
            srslte_pbch_decode_reset(&ue_mib.pbch);
            n = srslte_ue_mib_decode(&ue_mib, sf_buffer, bch_payload, NULL, &sfn_offset);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE MIB\n");
              exit(-1);
            } else if (n == SRSLTE_UE_MIB_FOUND) {             
              srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);
              srslte_pbch_mib_fprint(stdout, &cell, sfn, cell.id);
              printf("Decoded MIB. SFN: %d, offset: %d\n", sfn, sfn_offset);
              sfn = (sfn + sfn_offset)%1024; 
              state = DECODE_PDSCH; 
            }
          }
          break;
        case DECODE_PDSCH:
          if (prog_args.rnti != SRSLTE_SIRNTI) {
            decode_pdsch = true; 
          } else {
            /* We are looking for SIB1 Blocks, search only in appropiate places */
            if ((srslte_ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) {
              decode_pdsch = true; 
            } else {
              decode_pdsch = false; 
            }
          }
          if (decode_pdsch) {
            if (prog_args.rnti != SRSLTE_SIRNTI) {
              n = srslte_ue_dl_decode(&ue_dl, sf_buffer, data_packed, srslte_ue_sync_get_sfidx(&ue_sync));
            } else {
              n = srslte_ue_dl_decode_rnti_rv(&ue_dl, sf_buffer, data_packed, srslte_ue_sync_get_sfidx(&ue_sync), SRSLTE_SIRNTI,
                                 ((int) ceilf((float)3*(((sfn)/2)%4)/2))%4);             
            }
            if (n < 0) {
             // fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
            } else if (n > 0) {
              /* Send data if socket active */
              if (prog_args.net_port > 0) {
                srslte_bit_unpack_vector(data_packed, data, n);
                srslte_netsink_write(&net_sink, data, 1+(n-1)/8);
              }
            }
            nof_trials++; 
            
            rsrq = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrq(&ue_dl.chest), rsrq, 0.05);
            rsrp = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrp(&ue_dl.chest), rsrp, 0.05);      
            snr = SRSLTE_VEC_EMA(srslte_chest_dl_get_snr(&ue_dl.chest), snr, 0.01);      
            nframes++;
            if (isnan(rsrq)) {
              rsrq = 0; 
            }
            if (isnan(snr)) {
              snr = 0; 
            }
            if (isnan(rsrp)) {
              rsrp = 0; 
            }
            
#ifdef adjust_estimator
            /* Adjust channel estimator based on SNR */
            if (10*log10(snr) < 5.0) {
              float f_low_snr[5]={0.05, 0.15, 0.6, 0.15, 0.05};
              srslte_chest_dl_set_filter_freq(&ue_dl.chest, f_low_snr, 5);
            } else if (10*log10(snr) < 10.0) {
              float f_mid_snr[3]={0.1, 0.8, 0.1};
              srslte_chest_dl_set_filter_freq(&ue_dl.chest, f_mid_snr, 3);
            } else {
              float f_high_snr[3]={0.05, 0.9, 0.05};
              srslte_chest_dl_set_filter_freq(&ue_dl.chest, f_high_snr, 3);
            }
#endif
            
          }
          if (srslte_ue_sync_get_sfidx(&ue_sync) != 5 && srslte_ue_sync_get_sfidx(&ue_sync) != 0) {
            pdcch_tx++;
          }
          
          
          // Plot and Printf
          if (srslte_ue_sync_get_sfidx(&ue_sync) == 5) {
#ifdef STDOUT_COMPACT
            printf("SFN: %4d, PDCCH-Miss: %5.2f%% (%d missed), PDSCH-BLER: %5.2f%% (%d errors)\r",
                  sfn, 100*(1-(float) ue_dl.nof_detected/nof_trials),pdcch_tx-ue_dl.nof_detected,
                  (float) 100*ue_dl.pkt_errors/ue_dl.pkts_total,ue_dl.pkt_errors);                
#else
            printf("CFO: %+6.2f KHz, SFO: %+6.2f Khz, "
                  "RSRP: %+5.1f dBm, RSRQ: %5.1f dB, SNR: %4.1f dB, "
                  "PDCCH-Miss: %5.2f%% (%d), PDSCH-BLER: %5.2f%% (%d)\r",
                  srslte_ue_sync_get_cfo(&ue_sync)/1000, srslte_ue_sync_get_sfo(&ue_sync)/1000, 
                  10*log10(rsrp*1000)-gain_offset, 
                  10*log10(rsrq), 10*log10(snr), 
                  100*(1-(float) ue_dl.nof_detected/nof_trials), pdcch_tx-ue_dl.nof_detected, 
                  (float) 100*ue_dl.pkt_errors/ue_dl.pkts_total, ue_dl.pkt_errors);                
            
#endif            
          }
          break;
      }
      if (srslte_ue_sync_get_sfidx(&ue_sync) == 9) {
        sfn++; 
        if (sfn == 1024) {
          sfn = 0; 
        } 
      }
      
      #ifndef DISABLE_GRAPHICS
      if (!prog_args.disable_plots) {
        plot_sf_idx = srslte_ue_sync_get_sfidx(&ue_sync);
        sem_post(&plot_sem);
      }
      #endif
    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        srslte_sync_get_peak_value(&ue_sync.sfind), 
        ue_sync.frame_total_cnt, ue_sync.state);      
    }
        
    sf_cnt++;                  
  } // Main loop
  
  srslte_ue_dl_free(&ue_dl);
  srslte_ue_sync_free(&ue_sync);
  
#ifndef DISABLE_UHD
  if (!prog_args.input_file_name) {
    srslte_ue_mib_free(&ue_mib);
    cuhd_close(uhd);    
  }
#endif
  printf("\nBye\n");
  exit(0);
}






  

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS


//plot_waterfall_t poutfft;
plot_real_t p_sync, pce;
plot_scatter_t  pscatequal, pscatequal_pdcch;

float tmp_plot[SRSLTE_SLOT_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_SRSLTE_CP_NORM)];
float tmp_plot2[SRSLTE_SLOT_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_SRSLTE_CP_NORM)];
float tmp_plot3[SRSLTE_SLOT_LEN_RE(SRSLTE_MAX_PRB, SRSLTE_SRSLTE_CP_NORM)];

void *plot_thread_run(void *arg) {
  int i;
  uint32_t nof_re = SRSLTE_SF_LEN_RE(ue_dl.cell.nof_prb, ue_dl.cell.cp);
    
  while(1) {
    sem_wait(&plot_sem);
    
    uint32_t nof_symbols = ue_dl.harq_process[0].dl_alloc.re_sf[plot_sf_idx];
    for (i = 0; i < nof_re; i++) {
      tmp_plot[i] = 20 * log10f(cabsf(ue_dl.sf_symbols[i]));
      if (isinf(tmp_plot[i])) {
        tmp_plot[i] = -80;
      }
    }
    for (i = 0; i < SRSLTE_REFSIGNAL_NUM_SF(ue_dl.cell.nof_prb,0); i++) {
      tmp_plot2[i] = 20 * log10f(cabsf(ue_dl.chest.pilot_estimates_average[0][i]));
      if (isinf(tmp_plot2[i])) {
        tmp_plot2[i] = -80;
      }
    }
    //for (i=0;i<SRSLTE_CP_NSYMB(ue_dl.cell.cp);i++) {
    //  plot_waterfall_appendNewData(&poutfft, &tmp_plot[i*SRSLTE_NRE*ue_dl.cell.nof_prb], SRSLTE_NRE*ue_dl.cell.nof_prb);            
    //}
    plot_real_setNewData(&pce, tmp_plot2, SRSLTE_REFSIGNAL_NUM_SF(ue_dl.cell.nof_prb,0));        
    if (!prog_args.input_file_name) {
      int max = srslte_vec_max_fi(ue_sync.strack.pss.conv_output_avg, ue_sync.strack.pss.frame_size+ue_sync.strack.pss.fft_size-1);
      srslte_vec_sc_prod_fff(ue_sync.strack.pss.conv_output_avg, 
                      1/ue_sync.strack.pss.conv_output_avg[max], 
                      tmp_plot2, 
                      ue_sync.strack.pss.frame_size+ue_sync.strack.pss.fft_size-1);        
      plot_real_setNewData(&p_sync, tmp_plot2, ue_sync.strack.pss.frame_size);        
      
    }
    
    plot_scatter_setNewData(&pscatequal, ue_dl.pdsch.d, nof_symbols);
    plot_scatter_setNewData(&pscatequal_pdcch, ue_dl.pdcch.d, 36*ue_dl.pdcch.nof_cce);
    
    if (plot_sf_idx == 1) {
      if (prog_args.net_port_signal > 0) {
        srslte_netsink_write(&net_sink_signal, &sf_buffer[srslte_ue_sync_sf_len(&ue_sync)/7], 
                            srslte_ue_sync_sf_len(&ue_sync)); 
      }
    }

  }
  
  return NULL;
}

void init_plots() {

  sdrgui_init();
  
  //plot_waterfall_init(&poutfft, SRSLTE_NRE * ue_dl.cell.nof_prb, 1000);
  //plot_waterfall_setTitle(&poutfft, "Output FFT - Magnitude");
  //plot_waterfall_setPlotYAxisScale(&poutfft, -40, 40);

  plot_real_init(&pce);
  plot_real_setTitle(&pce, "Channel Response - Magnitude");
  plot_real_setLabels(&pce, "Index", "dB");
  plot_real_setYAxisScale(&pce, -40, 40);

  plot_real_init(&p_sync);
  plot_real_setTitle(&p_sync, "PSS Cross-Corr abs value");
  plot_real_setYAxisScale(&p_sync, 0, 1);

  plot_scatter_init(&pscatequal);
  plot_scatter_setTitle(&pscatequal, "PDSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal, -4, 4);
  plot_scatter_setYAxisScale(&pscatequal, -4, 4);

  plot_scatter_init(&pscatequal_pdcch);
  plot_scatter_setTitle(&pscatequal_pdcch, "PDCCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&pscatequal_pdcch, -4, 4);
  plot_scatter_setYAxisScale(&pscatequal_pdcch, -4, 4);

  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }
  
  if (pthread_create(&plot_thread, NULL, plot_thread_run, NULL)) {
    perror("pthread_create");
    exit(-1);
  }  
}

#endif
