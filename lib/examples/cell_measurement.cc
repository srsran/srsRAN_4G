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
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#define ENABLE_AGC_DEFAULT

extern  "C" {
#include "srslte/common/crash_handler.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/rf/rf_utils.h"
#include "srslte/srslte.h"
}
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/bcd_helpers.h"

#define MHZ             1000000
#define SAMP_FREQ       1920000
#define FLEN            9600
#define FLEN_PERIOD     0.005

#define MAX_EARFCN 1000


cell_search_cfg_t cell_detect_config = {
  SRSLTE_DEFAULT_MAX_FRAMES_PBCH,
  SRSLTE_DEFAULT_MAX_FRAMES_PSS, 
  SRSLTE_DEFAULT_NOF_VALID_PSS_FRAMES,
  0
};

struct cells {
  srslte_cell_t cell;
  float freq; 
  int dl_earfcn;
  float power;
};
struct cells results[1024]; 

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  bool disable_plots;
  int force_N_id_2;
  char *rf_args; 
  float rf_freq; 
  float rf_gain;
  int band;
  int earfcn_start;
  int earfcn_end;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1; 
  args->force_N_id_2 = -1; // Pick the best

  args->rf_args = "";
  args->rf_freq = -1.0;
  args->band = -1;
  args->earfcn_start = -1;
  args->earfcn_end = -1;
  
#ifdef ENABLE_AGC_DEFAULT
  args->rf_gain = -1; 
#else
  args->rf_gain = 50; 
#endif
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agselnv] -b band\n", prog);
  printf("\t-a RF args [Default %s]\n", args->rf_args);
  printf("\t-g RF RX gain [Default %.2f dB]\n", args->rf_gain);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-l Force N_id_2 [Default best]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

int  parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "aglnvfseb")) != -1) {
    switch (opt) {
    case 'a':
      args->rf_args = argv[optind];
      break;
    case 'g':
      args->rf_gain = atof(argv[optind]);
      break;
    case 'f':
      args->rf_freq = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'l':
      args->force_N_id_2 = atoi(argv[optind]);
      break;
    case 'b':
      args->band = atoi(argv[optind]);
      break;
    case 's':
      args->earfcn_start = atoi(argv[optind]);
      break;
    case 'e':
      args->earfcn_end = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    default:
      usage(args, argv[0]);
      return -1;
    }
  }
  if (args->band < 0) {
    usage(args, argv[0]);
    return -1;
  }
  return 0;
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t *data[SRSLTE_MAX_CODEWORDS];

bool go_exit = false; 
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

double srslte_rf_set_rx_gain_wrapper(void *h, double f) {
  return srslte_rf_set_rx_gain((srslte_rf_t*) h, f);
}

int srslte_rf_recv_wrapper(void *h, cf_t *data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t *q) {  
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  
  return srslte_rf_recv((srslte_rf_t*)h, data[0], nsamples, 1);
}

enum receiver_state { DECODE_MIB, DECODE_SIB, MEASURE} state; 

#define MAX_SINFO 10
#define MAX_NEIGHBOUR_CELLS     128

int main(int argc, char **argv) {
  int ret; 
  cf_t *sf_buffer[SRSLTE_MAX_PORTS] = {NULL, NULL}; 
  prog_args_t prog_args; 
  srslte_cell_t cell;  
  int64_t sf_cnt;
  srslte_ue_sync_t ue_sync; 
  srslte_ue_mib_t ue_mib; 
  srslte_rf_t rf; 
  srslte_ue_dl_t ue_dl; 
  srslte_ofdm_t fft; 
  srslte_chest_dl_t chest; 
  uint32_t nframes=0;
  uint32_t nof_trials = 0; 
  uint32_t max_trials = 16; 
  uint32_t sfn = 0; // system frame number
  int n; 
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  int sfn_offset; 
  float rssi_utra=0,rssi=0, rsrp=0, rsrq=0, snr=0;
  cf_t *ce[SRSLTE_MAX_PORTS];
  float cfo = 0;
  bool acks[SRSLTE_MAX_CODEWORDS] = {false};

  srslte_ue_cellsearch_t        cs;
  srslte_ue_cellsearch_result_t found_cells[3];
  int32_t                           nof_freqs;
  srslte_earfcn_t               channels[MAX_EARFCN];
  int32_t                      freq;
  uint32_t                      n_found_cells = 0;

  srslte_debug_handle_crash(argc, argv);

  if (parse_args(&prog_args, argc, argv)) {
    exit(-1);
  }

  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, prog_args.rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }
  if (prog_args.rf_gain > 0) {
    srslte_rf_set_rx_gain(&rf, prog_args.rf_gain);      
  } else {
    printf("Starting AGC thread...\n");
    if (srslte_rf_start_gain_thread(&rf, false)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
    srslte_rf_set_rx_gain(&rf, 50);
  }
  
  sf_buffer[0] = (cf_t*) srslte_vec_malloc(3*sizeof(cf_t)*SRSLTE_SF_LEN_PRB(100));
  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    data[i] = (uint8_t*)srslte_vec_malloc(sizeof(uint8_t) * 1500*8);
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  srslte_rf_set_master_clock_rate(&rf, 30.72e6);        

  // Supress RF messages
  srslte_rf_suppress_stdout(&rf);
  
  nof_freqs = srslte_band_get_fd_band(prog_args.band, channels, prog_args.earfcn_start, prog_args.earfcn_end, MAX_EARFCN);
  if (nof_freqs < 0) {
    fprintf(stderr, "Error getting EARFCN list\n");
    exit(-1);
  }

  if (srslte_ue_cellsearch_init_multi(&cs, cell_detect_config.max_frames_pss, srslte_rf_recv_wrapper, 1, (void*) &rf)) {
    fprintf(stderr, "Error initiating UE cell detect\n");
    exit(-1);
  }

  if (cell_detect_config.max_frames_pss) {
    srslte_ue_cellsearch_set_nof_valid_frames(&cs, cell_detect_config.nof_valid_pss_frames);
  }
  if (cell_detect_config.init_agc) {
    srslte_rf_info_t *rf_info = srslte_rf_get_info(&rf);
    srslte_ue_sync_start_agc(&cs.ue_sync,
                             srslte_rf_set_rx_gain_wrapper,
                             rf_info->min_rx_gain,
                             rf_info->max_rx_gain,
                             cell_detect_config.init_agc);
  }


  
  /* begin cell search loop */
  freq = -1;
  while (! go_exit) {
    /* set rf_freq */
    freq++;
    float rx_freq = channels[freq].fd * MHZ;
    srslte_rf_set_rx_freq(&rf, (double) rx_freq);
    srslte_rf_rx_wait_lo_locked(&rf);
    INFO("Set rf_freq to %.3f MHz\n", (double) rx_freq/1000000);
    
    printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz looking for PSS.\n", freq, nof_freqs,
                      channels[freq].id, channels[freq].fd);fflush(stdout);
    
    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }
      
    bzero(found_cells, 3*sizeof(srslte_ue_cellsearch_result_t));

    INFO("Setting sampling frequency %.2f MHz for PSS search\n", SRSLTE_CS_SAMP_FREQ/1000000);
    srslte_rf_set_rx_srate(&rf, SRSLTE_CS_SAMP_FREQ);
    INFO("Starting receiver...\n");
    srslte_rf_start_rx_stream(&rf, false);
    
    n = srslte_ue_cellsearch_scan(&cs, found_cells, NULL); 
    int ret = SRSLTE_UE_MIB_NOTFOUND;
    srslte_cell_t cell;
    if (n < 0) {
      fprintf(stderr, "Error searching cell\n");
      exit(-1);
    } else if (n > 0) {
      for (int i=0;i<3;i++) {
        if (found_cells[i].psr > 10.0) {
          cell.id = found_cells[i].cell_id; 
          cell.cp = found_cells[i].cp; 
          ret = rf_mib_decoder(&rf, 1, &cell_detect_config, &cell, NULL);
          if (ret < 0) {
            fprintf(stderr, "Error decoding MIB\n");
            continue;
          }
        }
      }
      if (ret == SRSLTE_UE_MIB_NOTFOUND) {
        continue;
      }

      if (ret == SRSLTE_UE_MIB_FOUND) {
      printf("Found CELL ID %d. %d PRB, %d ports\n", 
                 cell.id, 
                 cell.nof_prb, 
                 cell.nof_ports);
      }

      /* set receiver frequency */
      printf("Tunning receiver to %.3f MHz\n", (double ) rx_freq/1000000);
      
      cell_detect_config.init_agc = (prog_args.rf_gain<0);
      
      uint32_t ntrial=0; 
      const int MAX_ATTEMPTS = 1;
      do {
        ret = rf_search_and_decode_mib(&rf, 1, &cell_detect_config, prog_args.force_N_id_2, &cell, &cfo);
        if (ret < 0) {
          fprintf(stderr, "Error searching for cell\n");
          exit(-1); 
        } else if (ret == 0 && !go_exit) {
          printf("Cell not found after %d trials. Trying again (Press Ctrl+C to exit)\n", ntrial++);
        }      
      } while (ret == 0 && !go_exit && ntrial < MAX_ATTEMPTS); 
      
      if (go_exit) {
        exit(0);
      }
      
      /* set sampling frequency */
        int srate = srslte_sampling_freq_hz(cell.nof_prb);    
        if (srate != -1) {  
          if (srate < 10e6) {          
            srslte_rf_set_master_clock_rate(&rf, 4*srate);        
          } else {
            srslte_rf_set_master_clock_rate(&rf, srate);        
          }
          printf("Setting sampling rate %.2f MHz\n", (float) srate/1000000);
          float srate_rf = srslte_rf_set_rx_srate(&rf, (double) srate);
          if (srate_rf != srate) {
            fprintf(stderr, "Could not set sampling rate\n");
            exit(-1);
          }
        } else {
          fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
          exit(-1);
        }

      INFO("Stopping RF and flushing buffer...\n");
      srslte_rf_stop_rx_stream(&rf);
      srslte_rf_flush_buffer(&rf);
      
      if (srslte_ue_sync_init_multi(&ue_sync, cell.nof_prb, cell.id==1000, srslte_rf_recv_wrapper, 1, (void*) &rf)) {
        fprintf(stderr, "Error initiating ue_sync\n");
        return -1; 
      }
      if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
        fprintf(stderr, "Error initiating ue_sync\n");
        return -1;
      }
      if (srslte_ue_dl_init(&ue_dl, sf_buffer, cell.nof_prb, 1)) {
        fprintf(stderr, "Error initiating UE downlink processing module\n");
        return -1;
      }
      if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
        fprintf(stderr, "Error initiating UE downlink processing module\n");
        return -1;
      }
      if (srslte_ue_mib_init(&ue_mib, sf_buffer, cell.nof_prb)) {
        fprintf(stderr, "Error initaiting UE MIB decoder\n");
        return -1;
      }
      if (srslte_ue_mib_set_cell(&ue_mib, cell)) {
        fprintf(stderr, "Error initaiting UE MIB decoder\n");
        return -1;
      }

      /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
      srslte_ue_dl_set_rnti(&ue_dl, SRSLTE_SIRNTI); 

      /* Initialize subframe counter */
      sf_cnt = 0;

      int sf_re = SRSLTE_SF_LEN_RE(cell.nof_prb, cell.cp);

      cf_t *sf_symbols = (cf_t*) srslte_vec_malloc(sf_re * sizeof(cf_t));

      for (int i=0;i<SRSLTE_MAX_PORTS;i++) {
        ce[i] = (cf_t*) srslte_vec_malloc(sizeof(cf_t) * sf_re);
      }

      if (srslte_ofdm_rx_init(&fft, cell.cp, sf_buffer[0], sf_symbols, cell.nof_prb)) {
        fprintf(stderr, "Error initiating FFT\n");
        return -1;
      }
      if (srslte_chest_dl_init(&chest, cell.nof_prb)) {
        fprintf(stderr, "Error initiating channel estimator\n");
        return -1;
      }
      if (srslte_chest_dl_set_cell(&chest, cell)) {
        fprintf(stderr, "Error initiating channel estimator\n");
        return -1;
      }
      
      srslte_rf_start_rx_stream(&rf, false);
      
      float rx_gain_offset = 0;

      typedef struct {
        uint16_t    mcc;
        uint16_t    mnc;
        uint16_t    tac;
        uint32_t    cid;
        uint16_t    phyid;
        uint16_t    earfcn;
        uint16_t    rssi;
        uint16_t    frequency;
        uint16_t    enodeb_id;
        uint16_t    sector_id;
      } tower_info_t;

      printf("Begin SIB Decoding Loop");

      /* Main loop */
      bool exit_decode_loop = false;
      //state = DECODE_MIB;
      while ((sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1) && !go_exit && !exit_decode_loop) {
        tower_info_t tower;
        
        printf("calling zerocopy");

        ret = srslte_ue_sync_zerocopy_multi(&ue_sync, sf_buffer);
        if (ret < 0) {
          fprintf(stderr, "Error calling srslte_ue_sync_work()\n");
        }
            
        /* srslte_ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
        if (ret == 1) {
          switch (state) {
            case DECODE_MIB:
              printf("0");
              if (srslte_ue_sync_get_sfidx(&ue_sync) == 0) {
                srslte_pbch_decode_reset(&ue_mib.pbch);
                n = srslte_ue_mib_decode(&ue_mib, bch_payload, NULL, &sfn_offset);
                if (n < 0) {
                  fprintf(stderr, "Error decoding UE MIB\n");
                  return -1;
                } else if (n == SRSLTE_UE_MIB_FOUND) {   
                  srslte_pbch_mib_unpack(bch_payload, &cell, &sfn);
                  printf("Decoded MIB. SFN: %d, offset: %d\n", sfn, sfn_offset);
                  tower.phyid = cell.id;
                  sfn = (sfn + sfn_offset)%1024; 
                  state = DECODE_SIB; 
                }
              }
              break;
            case DECODE_SIB:
              /* We are looking for SI Blocks, search only in appropiate places */
              if ((srslte_ue_sync_get_sfidx(&ue_sync) == 5 && (sfn%2)==0)) {
                n = srslte_ue_dl_decode(&ue_dl, data, 0, sfn*10+srslte_ue_sync_get_sfidx(&ue_sync), acks);
                if (n < 0) {
                  fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
                  exit_decode_loop = true; 
                  break;
                } else if (n == 0) {
                  printf("CFO: %+6.4f kHz, SFO: %+6.4f kHz, PDCCH-Det: %.3f\r",
                          srslte_ue_sync_get_cfo(&ue_sync)/1000, srslte_ue_sync_get_sfo(&ue_sync)/1000, 
                          (float) ue_dl.nof_detected/nof_trials);
                  nof_trials++; 
                  if(nof_trials > max_trials ){
                    fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
                    exit_decode_loop = true; 
                    break;
                  }
                } else {
                  printf("Decoded SIB1. Payload: ");
                  srslte_vec_fprint_byte(stdout, data[0], n/8);;
                  asn1::rrc::bcch_dl_sch_msg_s dlsch_msg;

                  asn1::bit_ref     bref(data[0], n / 8);
                  asn1::SRSASN_CODE unpackResult = dlsch_msg.unpack(bref);
                  //printf("unpackResult = %d\n", unpackResult);

                  if (unpackResult == asn1::SRSASN_SUCCESS) {
                    int msgTypeValue = dlsch_msg.msg.type().value;
                    //printf("dlsch_msg.msg.type().value=%d\n", msgTypeValue);
                    if (msgTypeValue == 0) {
                      if (dlsch_msg.msg.c1().type().value == asn1::rrc::bcch_dl_sch_msg_type_c::c1_c_::types::sib_type1) {
                        //                                        printf("Accessing dlsch_msg.msg.c1().sib_type1()\n");
                        asn1::rrc::sib_type1_s* sib1 = &dlsch_msg.msg.c1().sib_type1();
                        //                                      printf("Accessing sib1->cell_access_related_info.plmn_id_list[0].plmn_id\n");
                        asn1::rrc::plmn_id_s plmn = sib1->cell_access_related_info.plmn_id_list[0].plmn_id;

                        std::string plmn_string = srslte::plmn_id_to_string(plmn);
                        // If we were using C++11 we could just use stoi
                        tower.mcc = atoi(plmn_string.substr(0, 3).c_str());
                        tower.mnc = atoi(plmn_string.substr(3, plmn_string.length() - 3).c_str());
                        // srslte::bytes_to_mcc(&plmn.mcc[0], &mcc);
                        // srslte::bytes_to_mnc(&plmn.mnc[0], &mnc, plmn.mnc.size());
                        tower.tac = (uint16_t) sib1->cell_access_related_info.tac.to_number();
                        tower.cid = (uint32_t) sib1->cell_access_related_info.cell_id.to_number();

                        printf("MCC=%d, MNC=%d, PID=%d, TAC=%d, CID=%d\n", tower.mcc, tower.mnc, tower.phyid, tower.tac, tower.cid);
                        if ((tower.mnc != 0) && (tower.mcc != 0)) {
                          //state = MEASURE;
                          exit_decode_loop = true; 
                        }
                      }
                    }
                  }
                  //state = MEASURE;
                  exit_decode_loop = true; 
                }
              }
            break;
            
          case MEASURE:
            
            if (srslte_ue_sync_get_sfidx(&ue_sync) == 5) {
              /* Run FFT for all subframe data */
              srslte_ofdm_rx_sf(&fft);
              
              srslte_chest_dl_estimate(&chest, sf_symbols, ce, srslte_ue_sync_get_sfidx(&ue_sync));
                      
              rssi = SRSLTE_VEC_EMA(srslte_vec_avg_power_cf(sf_buffer[0],SRSLTE_SF_LEN(srslte_symbol_sz(cell.nof_prb))),rssi,0.05);
              rssi_utra = SRSLTE_VEC_EMA(srslte_chest_dl_get_rssi(&chest),rssi_utra,0.05);
              rsrq = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrq(&chest),rsrq,0.05);
              rsrp = SRSLTE_VEC_EMA(srslte_chest_dl_get_rsrp(&chest),rsrp,0.05);      
              snr = SRSLTE_VEC_EMA(srslte_chest_dl_get_snr(&chest),snr,0.05);      
              
              nframes++;          
            } 
            
            
            if ((nframes%100) == 0 || rx_gain_offset == 0) {
              if (srslte_rf_has_rssi(&rf)) {
                rx_gain_offset = 30+10*log10(rssi*1000)-srslte_rf_get_rssi(&rf);
              } else {
                rx_gain_offset = srslte_rf_get_rx_gain(&rf);            
              }
            }
            
            // Plot and Printf
            if ((nframes%10) == 0) {

              printf("CFO: %+8.4f kHz, SFO: %+8.4f Hz, RSSI: %5.1f dBm, RSSI/ref-symbol: %+5.1f dBm, "
                    "RSRP: %+5.1f dBm, RSRQ: %5.1f dB, SNR: %5.1f dB\r",
                    srslte_ue_sync_get_cfo(&ue_sync)/1000, srslte_ue_sync_get_sfo(&ue_sync), 
                    10*log10(rssi*1000) - rx_gain_offset,                        
                    10*log10(rssi_utra*1000)- rx_gain_offset, 
                    10*log10(rsrp*1000) - rx_gain_offset, 
                    10*log10(rsrq), 10*log10(snr));                
              if (srslte_verbose != SRSLTE_VERBOSE_NONE) {
                printf("\n");
              }
              if (srslte_rf_has_rssi(&rf)) {
                exit_decode_loop = true; 
              }
            }
          }
          if (srslte_ue_sync_get_sfidx(&ue_sync) == 9) {
            sfn++; 
            if (sfn == 1024) {
              sfn = 0; 
            }
          }
        } else if (ret == 0) {
          printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
            srslte_sync_get_peak_value(&ue_sync.sfind), 
            ue_sync.frame_total_cnt, ue_sync.state);      
        }
        
        sf_cnt++;                  
        if (exit_decode_loop){
          sf_cnt = 0;
          break;
        }
      } // Decoding Loop
    } // if found cell 
    if (freq == nof_freqs) {
      freq = -1; //continue loop at the beginning
    }
  } // Search loop

  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (data[i]) {
      free(data[i]);
    }
  }

  srslte_ue_sync_free(&ue_sync);
  srslte_rf_close(&rf);
  printf("\nBye\n");
  exit(0);
}



