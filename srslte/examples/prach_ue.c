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

#include "srslte/rrc/rrc.h"
#include "srslte/srslte.h"


#include "srslte/cuhd/cuhd.h"
#include "cuhd_utils.h"

cell_search_cfg_t cell_detect_config = {
  5000,
  100, // nof_frames_total 
  16.0 // threshold
};

#define B210_DEFAULT_GAIN         40.0
#define B210_DEFAULT_GAIN_CORREC  110.0 // Gain of the Rx chain when the gain is set to 40

float gain_offset = B210_DEFAULT_GAIN_CORREC;


/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int nof_subframes;
  int force_N_id_2;
  uint32_t file_nof_prb;
  uint32_t preamble_idx;
  float beta_prach;
  float ta_usec; 
  float beta_pusch; 
  char *uhd_args; 
  float uhd_rx_freq; 
  float uhd_tx_freq; 
  float uhd_tx_freq_offset; 
  float uhd_tx_gain;
  float uhd_rx_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1;
  args->force_N_id_2 = -1; // Pick the best
  args->file_nof_prb = 6; 
  args->beta_prach = 0.005;
  args->beta_pusch = 2.0;
  args->ta_usec = -1.0;
  args->preamble_idx = 7; 
  args->uhd_args = "";
  args->uhd_rx_freq = 2112500000.0;
  args->uhd_tx_freq = 1922500000.0;
  args->uhd_tx_freq_offset = 8000000.0;
  args->uhd_tx_gain = 60.0; 
  args->uhd_rx_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agfFbrlpnv]\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD TX/RX gain [Default %.2f dB]\n", args->uhd_rx_gain);
  printf("\t-G UHD TX/RX gain [Default %.2f dB]\n", args->uhd_tx_gain);
  printf("\t-f UHD RX freq [Default %.1f MHz]\n", args->uhd_rx_freq/1000000);
  printf("\t-F UHD TX freq [Default %.1f MHz]\n", args->uhd_tx_freq/1000000);
  printf("\t-b beta PRACH (transmission amplitude) [Default %f]\n",args->beta_prach);
  printf("\t-B beta PUSCH (transmission amplitude) [Default %f]\n",args->beta_pusch);
  printf("\t-t TA usec (time advance, -1 from RAR) [Default %f]\n",args->ta_usec);
  printf("\t-p PRACH preamble idx [Default %d]\n",args->preamble_idx);
  printf("\t-l Force N_id_2 [Default best]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "agGfFplnvbBt")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'b':
      args->beta_prach = atof(argv[optind]);
      break;
    case 'B':
      args->beta_pusch = atof(argv[optind]);
      break;
    case 't':
      args->ta_usec = atof(argv[optind]);
      break;
    case 'g':
      args->uhd_rx_gain = atof(argv[optind]);
      break;
    case 'G':
      args->uhd_tx_gain = atof(argv[optind]);
      break;
    case 'f':
      args->uhd_rx_freq = atof(argv[optind]);
      break;
    case 'F':
      args->uhd_tx_freq = atof(argv[optind]);
      break;
    case 'n':
      args->nof_subframes = atoi(argv[optind]);
      break;
    case 'p':
      args->preamble_idx = atoi(argv[optind]);
      break;
    case 'l':
      args->force_N_id_2 = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->uhd_tx_freq < 0 && args->uhd_rx_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}
/**********************************************************************/

/* TODO: Do something with the output data */
uint8_t data_rx[20000];

bool go_exit = false; 

void sig_int_handler(int signo)
{
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int cuhd_recv_wrapper_timed(void *h, void *data, uint32_t nsamples, timestamp_t *uhd_time) {
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return cuhd_recv_with_time(h, data, nsamples, true, &uhd_time->full_secs, &uhd_time->frac_secs);
}

extern float mean_exec_time;

enum receiver_state { DECODE_MIB, SEND_PRACH, RECV_RAR, RECV_CONNSETUP} state; 

#define NOF_PRACH_SEQUENCES 52

ue_dl_t ue_dl; 
ue_ul_t ue_ul; 
ue_sync_t ue_sync; 
prach_t prach; 
int prach_buffer_len;

prog_args_t prog_args; 

uint32_t sfn = 0; // system frame number
cf_t *sf_buffer = NULL; 



typedef enum{
    rar_tpc_n6dB = 0,
    rar_tpc_n4dB,
    rar_tpc_n2dB,
    rar_tpc_0dB,
    rar_tpc_2dB,
    rar_tpc_4dB,
    rar_tpc_6dB,
    rar_tpc_8dB,
    rar_tpc_n_items,
}rar_tpc_command_t;
static const char tpc_command_text[rar_tpc_n_items][8] = {"-6dB", "-4dB", "-2dB",  "0dB", "2dB",  "4dB",  "6dB",  "8dB"};
typedef enum{
    rar_header_type_bi = 0,
    rar_header_type_rapid,
    rar_header_type_n_items,
}rar_header_t;
static const char rar_header_text[rar_header_type_n_items][8] = {"BI", "RAPID"};

typedef struct {
  rar_header_t      hdr_type;
  bool              hopping_flag;
  rar_tpc_command_t tpc_command;
  bool              ul_delay;
  bool              csi_req;
  uint16_t          rba; 
  uint16_t          timing_adv_cmd;
  uint16_t          temp_c_rnti;
  uint8_t           mcs; 
  uint8_t           RAPID;
  uint8_t           BI;
}rar_msg_t; 

char *bool_to_string(bool x) {
  if (x) {
    return "Enabled";
  } else {
    return "Disabled";
  }
}

void rar_msg_fprint(FILE *stream, rar_msg_t *msg) 
{
  fprintf(stream, "Header type:  %s\n", rar_header_text[msg->hdr_type]);
  fprintf(stream, "Hopping flag: %s\n", bool_to_string(msg->hopping_flag));
  fprintf(stream, "TPC command:  %s\n", tpc_command_text[msg->tpc_command]);
  fprintf(stream, "UL delay:     %s\n", bool_to_string(msg->ul_delay));
  fprintf(stream, "CSI required: %s\n", bool_to_string(msg->csi_req));
  fprintf(stream, "RBA:          %d\n", msg->rba);
  fprintf(stream, "TA:           %d\n", msg->timing_adv_cmd);
  fprintf(stream, "T-CRNTI:      %d\n", msg->temp_c_rnti);
  fprintf(stream, "MCS:          %d\n", msg->mcs);
  fprintf(stream, "RAPID:        %d\n", msg->RAPID);
  fprintf(stream, "BI:           %d\n", msg->BI);
}

int rar_unpack(uint8_t *buffer, rar_msg_t *msg)
{
    int ret = SRSLTE_ERROR;
    uint8_t *ptr = buffer; 
    
    if(buffer != NULL &&
          msg != NULL)
    {
      ptr++;
      msg->hdr_type = *ptr++;
      if(msg->hdr_type == rar_header_type_bi) {
        ptr += 2; 
        msg->BI = bit_unpack(&ptr, 4);
        ret = SRSLTE_SUCCESS; 
      } else if (msg->hdr_type == rar_header_type_rapid) {
        msg->RAPID = bit_unpack(&ptr, 6);
        ptr++;
        
        msg->timing_adv_cmd = bit_unpack(&ptr, 11);
        msg->hopping_flag   = *ptr++;
        msg->rba            = bit_unpack(&ptr, 10); 
        msg->mcs            = bit_unpack(&ptr, 4);
        msg->tpc_command    = (rar_tpc_command_t) bit_unpack(&ptr, 3);
        msg->ul_delay       = *ptr++;
        msg->csi_req        = *ptr++;
        msg->temp_c_rnti    = bit_unpack(&ptr, 16);
        ret = SRSLTE_SUCCESS;
      } 
    }

    return(ret);
}

#define kk
#define use_usrp 

int main(int argc, char **argv) {
  int ret; 
  lte_cell_t cell;  
  int64_t sf_cnt;
  ue_mib_t ue_mib; 
  void *uhd; 
  int n; 
  uint8_t bch_payload[BCH_PAYLOAD_LEN], bch_payload_unpacked[BCH_PAYLOAD_LEN];
  uint32_t sfn_offset;
  rar_msg_t rar_msg; 
  ra_pusch_t ra_pusch; 
  uint32_t rar_window_start = 0, rar_trials = 0, rar_window_stop = 0; 
  timestamp_t uhd_time;
  timestamp_t next_tx_time;  
  const uint8_t conn_request_msg[] = {0x20, 0x06, 0x1F, 0x5C, 0x2C, 0x04, 0xB2, 0xAC, 0xF6, 0x00, 0x00, 0x00};
  uint8_t data[1000];
  cf_t *prach_buffer;

  parse_args(&prog_args, argc, argv);

#ifdef use_usrp
  printf("Opening UHD device...\n");
  if (cuhd_open(prog_args.uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }

  /* Set receiver gain */
  float x = cuhd_set_rx_gain(uhd, prog_args.uhd_rx_gain);
  printf("Set RX gain to %.1f dB\n", x);
  x = cuhd_set_tx_gain(uhd, prog_args.uhd_tx_gain);
  printf("Set TX gain to %.1f dB\n", x);
  
  /* set receiver frequency */
  cuhd_set_rx_freq(uhd, (double) prog_args.uhd_rx_freq);
  cuhd_rx_wait_lo_locked(uhd);
  printf("Tunning RX receiver to %.3f MHz\n", (double ) prog_args.uhd_rx_freq/1000000);

  cuhd_set_tx_freq_offset(uhd, prog_args.uhd_tx_freq, prog_args.uhd_tx_freq_offset);
  printf("Tunning TX receiver to %.3f MHz\n", (double ) prog_args.uhd_tx_freq/1000000);

#endif
  
#ifdef kk
  ret = cuhd_search_and_decode_mib(uhd, &cell_detect_config, prog_args.force_N_id_2, &cell);
  if (ret < 0) {
    fprintf(stderr, "Error searching for cell\n");
    exit(-1); 
  } else if (ret == 0) {
    printf("Cell not found\n");
    exit(0);
  }
#else
  cell.id = 1; 
  cell.nof_ports = 1; 
  cell.nof_prb = 25; 
  cell.cp = CPNORM; 
  cell.phich_length = PHICH_NORM; 
  cell.phich_resources = R_1; 
#endif

#ifdef use_usrp

  /* set sampling frequency */
  int srate = lte_sampling_freq_hz(cell.nof_prb);
  if (srate != -1) {  
    cuhd_set_rx_srate(uhd, (double) srate);      
    cuhd_set_tx_srate(uhd, (double) srate);      
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
    exit(-1);
  }

  INFO("Stopping UHD and flushing buffer...\r",0);
  cuhd_stop_rx_stream(uhd);
  cuhd_flush_buffer(uhd);

#endif
  
  if (ue_mib_init(&ue_mib, cell)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }
  
  if (prach_init(&prach, lte_symbol_sz(cell.nof_prb), 0, 0, false, 1)) {
    fprintf(stderr, "Error initializing PRACH\n");
    exit(-1);
  }
  prach_buffer_len = prach.N_seq + prach.N_cp;
  prach_buffer = vec_malloc(prach_buffer_len*sizeof(cf_t));
  if(!prach_buffer) {
    perror("maloc");
    exit(-1);
  }
  if(prach_gen(&prach, prog_args.preamble_idx, 0, prog_args.beta_prach, prach_buffer)){
    fprintf(stderr, "Error generating prach sequence\n");
    return -1;
  }

  if (ue_ul_init(&ue_ul, cell)) {
    fprintf(stderr, "Error initiating UE UL\n");
    exit(-1);
  }
  
  pusch_hopping_cfg_t hop_cfg; 
  bzero(&hop_cfg, sizeof(pusch_hopping_cfg_t));
  refsignal_drms_pusch_cfg_t drms_cfg; 
  bzero(&drms_cfg, sizeof(refsignal_drms_pusch_cfg_t));  
  drms_cfg.beta_pusch = 1.0; 
  drms_cfg.group_hopping_en = false; 
  drms_cfg.sequence_hopping_en = false; 
  drms_cfg.delta_ss = 0;
  drms_cfg.cyclic_shift = 0; 
  drms_cfg.cyclic_shift_for_drms = 0; 
  drms_cfg.en_drms_2 = false; 
  ue_ul_set_pusch_cfg(&ue_ul, &drms_cfg, &hop_cfg);

  cf_t *ul_signal = vec_malloc(sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));
  if (!ul_signal) {
    perror("malloc");
    exit(-1);
  }
  bzero(ul_signal, sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));
    
  if (ue_dl_init(&ue_dl, cell)) { 
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }

  /* Initialize subframe counter */
  sf_cnt = 0;


#ifdef use_usrp
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper_timed, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }

  cuhd_start_rx_stream(uhd);    
#endif           
    
  uint16_t ra_rnti; 
  uint32_t conn_setup_trial = 0; 
  uint32_t ul_sf_idx = 0; 
#ifdef kk
  // Register Ctrl+C handler
  signal(SIGINT, sig_int_handler);
  state = DECODE_MIB; 
#else
  state = RECV_RAR; 
#endif  
  /* Main loop */
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {
    
#ifdef kk
    ret = ue_sync_get_buffer(&ue_sync, &sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling ue_sync_work()\n");
    }
#else 
  ret = 1;
            timestamp_t rx_time, tx_time; 
            cf_t dummy[4];
#endif  
            
    /* ue_sync_get_buffer returns 1 if successfully read 1 aligned subframe */
    if (ret == 1) {
    
      if (state != RECV_RAR) {
        /* Run FFT for all subframe data */
        lte_fft_run_sf(&ue_dl.fft, sf_buffer, ue_dl.sf_symbols);

        /* Get channel estimates for each port */
        chest_dl_estimate(&ue_dl.chest, ue_dl.sf_symbols, ue_dl.ce, ue_sync_get_sfidx(&ue_sync));        
      }
        
      if (sf_cnt > 1000) {
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
                state = SEND_PRACH; 
              }
            }
            break;
          case SEND_PRACH:

  #ifdef kk
            if (((sfn%2) == 1) && (ue_sync_get_sfidx(&ue_sync) == 1)) {
              ue_sync_get_last_timestamp(&ue_sync, &uhd_time);
      
              timestamp_copy(&next_tx_time, &uhd_time);
              timestamp_add(&next_tx_time, 0, 0.01); // send next frame (10 ms)
              printf("Send prach sfn: %d. Last frame time = %.6f, send prach time = %.6f\n", 
                    sfn, timestamp_real(&uhd_time), timestamp_real(&next_tx_time));

              cuhd_send_timed(uhd, prach_buffer, prach_buffer_len, 
                              next_tx_time.full_secs, next_tx_time.frac_secs);
              
              ra_rnti = 2; 
              rar_window_start = sfn+1;
              rar_window_stop = sfn+3;
              state = RECV_RAR;
            }
  #else
              cuhd_recv_with_time(uhd, dummy, 4, 1, &rx_time.full_secs, &rx_time.frac_secs);

              timestamp_copy(&tx_time, &rx_time);
              printf("Transmitting PRACH...\n");
              vec_save_file("prach_tx", prach_buffers[7], prach_buffer_len*sizeof(cf_t));
              while(1) {
                timestamp_add(&tx_time, 0, 0.001); // send every (10 ms)            
                cuhd_send_timed(uhd, prach_buffers[7], prach_buffer_len, 
                              tx_time.full_secs, tx_time.frac_secs);
                
              }
  #endif
            break;
          case RECV_RAR:
            #ifdef kk

            if ((sfn == rar_window_start && ue_sync_get_sfidx(&ue_sync) > 3) || sfn > rar_window_start) {
              printf("Looking for RAR in sfn: %d sf_idx: %d\n", sfn, ue_sync_get_sfidx(&ue_sync));
              n = ue_dl_decode_rnti(&ue_dl, sf_buffer, data_rx, ue_sync_get_sfidx(&ue_sync), ra_rnti);
              if (n < 0) {
                fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
              } else if (n > 0) {

                rar_unpack(data_rx, &rar_msg);
                //if (rar_msg.RAPID != prog_args.preamble_idx) {
                //  printf("Found RAR for sequence %d\n", rar_msg.RAPID);
                //} else {
                  //cuhd_stop_rx_stream(uhd);
                  //cuhd_flush_buffer(uhd);
                  rar_msg_fprint(stdout, &rar_msg);              
                  
                  dci_rar_to_ra_ul(rar_msg.rba, rar_msg.mcs, rar_msg.hopping_flag, cell.nof_prb, &ra_pusch);
                  ra_pusch_fprint(stdout, &ra_pusch, cell.nof_prb);

                  ra_ul_alloc(&ra_pusch.prb_alloc, &ra_pusch, 0, cell.nof_prb);

                  ue_sync_get_last_timestamp(&ue_sync, &uhd_time);
                  
                  bit_pack_vector((uint8_t*) conn_request_msg, data, ra_pusch.mcs.tbs);

                  uint32_t n_ta = lte_N_ta_new_rar(rar_msg.timing_adv_cmd);
                  printf("ta: %d, n_ta: %d\n", rar_msg.timing_adv_cmd, n_ta);
                  float time_adv_sec = TA_OFFSET+((float) n_ta)*LTE_TS;
                  if (prog_args.ta_usec >= 0) {
                    time_adv_sec = prog_args.ta_usec*1e-6;
                  }
  #define N_TX  1
                  const uint32_t rv[N_TX]={0,2,3,1,0};
                  for (int i=0; i<N_TX;i++) {
                    ra_pusch.rv_idx = rv[i];
                    ul_sf_idx = (ue_sync_get_sfidx(&ue_sync)+6+i*8)%10;

                    n = ue_ul_pusch_encode_rnti(&ue_ul, &ra_pusch, data, ul_sf_idx, rar_msg.temp_c_rnti, ul_signal);
                    if (n < 0) {
                      fprintf(stderr, "Error encoding PUSCH\n");
                      exit(-1);
                    }
                    
                    vec_sc_prod_cfc(ul_signal, prog_args.beta_pusch, ul_signal, SF_LEN_PRB(cell.nof_prb));
                    
                    timestamp_copy(&next_tx_time, &uhd_time);
                    timestamp_add(&next_tx_time, 0, 0.006 + i*0.008 - time_adv_sec); // send after 6 sub-frames (6 ms)
                    printf("Send %d samples PUSCH sfn: %d. RV_idx=%d, Last frame time = %.6f "
                          "send PUSCH time = %.6f TA: %.1f us\n", 
                          SF_LEN_PRB(cell.nof_prb), sfn, ra_pusch.rv_idx, 
                          timestamp_real(&uhd_time), 
                          timestamp_real(&next_tx_time), time_adv_sec*1000000);
                    
                    cuhd_send_timed(uhd, ul_signal, SF_LEN_PRB(cell.nof_prb),
                                  next_tx_time.full_secs, next_tx_time.frac_secs);                

                    //cuhd_start_rx_stream(uhd);
                    state = RECV_CONNSETUP;                   
                    conn_setup_trial = 0; 

                 // }
                }

              }
              if (sfn >= rar_window_stop) {              
                state = SEND_PRACH;
                rar_trials++;
                if (rar_trials >= 1) {
                  go_exit = 1; 
                }
              }              
            }
  #else

            ra_pusch.mcs.mod = LTE_QPSK;
            ra_pusch.mcs.tbs = 94;
            ra_pusch.rv_idx = 0; 
            ra_pusch.prb_alloc.freq_hopping = 0; 
            ra_pusch.prb_alloc.L_prb = 4; 
            ra_pusch.prb_alloc.n_prb[0] = 19; 
            ra_pusch.prb_alloc.n_prb[1] = 19; 
            
            uint32_t ul_sf_idx = 4;
            printf("L: %d\n", ra_pusch.prb_alloc.L_prb);
            // ue_ul_set_cfo(&ue_ul, sync_get_cfo(&ue_sync.strack));
            bit_pack_vector((uint8_t*) conn_request_msg, data, ra_pusch.mcs.tbs);
            n = ue_ul_pusch_encode_rnti(&ue_ul, &ra_pusch, data, ul_sf_idx, 111, ul_signal);
            if (n < 0) {
              fprintf(stderr, "Error encoding PUSCH\n");
              exit(-1);
            }
            
            vec_save_file("pusch_tx", ul_signal, SF_LEN_PRB(cell.nof_prb)*sizeof(cf_t));

  #ifdef use_usrp
            cuhd_recv_with_time(uhd, dummy, 4, 1, &uhd_time.full_secs, &uhd_time.frac_secs);
            timestamp_copy(&next_tx_time, &uhd_time);
            while(1) {
              timestamp_add(&next_tx_time, 0, 0.002); // send every 2 ms 
              cuhd_send_timed(uhd, ul_signal, SF_LEN_PRB(cell.nof_prb),
                            next_tx_time.full_secs, next_tx_time.frac_secs);               
            }
  #else
            exit(-1);
  #endif
  #endif

            break;
            
          case RECV_CONNSETUP: 
            if (ue_sync_get_sfidx(&ue_sync) == (ul_sf_idx+4)%10) {
              //verbose=VERBOSE_DEBUG;
              vec_save_file("connsetup",sf_buffer,SF_LEN_PRB(cell.nof_prb)*sizeof(cf_t));
            } else {
              //verbose=VERBOSE_NONE;
            }
            printf("Looking for ConnectionSetup in sfn: %d sf_idx: %d, RNTI: %d\n", sfn, ue_sync_get_sfidx(&ue_sync),rar_msg.temp_c_rnti);
            n = ue_dl_decode_rnti(&ue_dl, sf_buffer, data_rx, ue_sync_get_sfidx(&ue_sync), rar_msg.temp_c_rnti);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
            } else if (n > 0) {
              printf("Received ConnectionSetup len: %d.\n", n);
              vec_fprint_hex(stdout, data_rx, n);
            } else {
              conn_setup_trial++;
              if (conn_setup_trial == 20) {
                go_exit = 1; 
              }
            }
            
            break;
        }
        if (ue_sync_get_sfidx(&ue_sync) == 9) {
          sfn++; 
          if (sfn == 1024) {
            sfn = 0; 
          } 
        }
      
      }

    } else if (ret == 0) {
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r", 
        sync_get_peak_value(&ue_sync.sfind), 
        ue_sync.frame_total_cnt, ue_sync.state);      
    }
        
    sf_cnt++;                  
  } // Main loop
  
  ue_dl_free(&ue_dl);
  ue_sync_free(&ue_sync);
  
  ue_mib_free(&ue_mib);
  cuhd_close(uhd);    
  printf("\nBye\n");
  exit(0);
}






  
