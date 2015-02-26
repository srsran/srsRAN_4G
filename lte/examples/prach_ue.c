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
#include <pthread.h>
#include <semaphore.h>

#include "liblte/rrc/rrc.h"
#include "liblte/phy/phy.h"


#include "liblte/cuhd/cuhd.h"
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
  uint16_t rnti;
  uint32_t file_nof_prb;
  char *uhd_args; 
  float uhd_rx_freq; 
  float uhd_tx_freq; 
  float uhd_tx_freq_offset; 
  float uhd_gain;
}prog_args_t;

void args_default(prog_args_t *args) {
  args->nof_subframes = -1;
  args->rnti = SIRNTI;
  args->force_N_id_2 = -1; // Pick the best
  args->file_nof_prb = 6; 
  args->uhd_args = "";
  args->uhd_rx_freq = 2112500000.0;
  args->uhd_tx_freq = 1922500000.0;
  args->uhd_tx_freq_offset = 8000000.0;
  args->uhd_gain = 60.0; 
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [agfFrlnv]\n", prog);
  printf("\t-a UHD args [Default %s]\n", args->uhd_args);
  printf("\t-g UHD TX/RX gain [Default %.2f dB]\n", args->uhd_gain);
  printf("\t-f UHD RX freq [Default %.1f MHz]\n", args->uhd_rx_freq/1000000);
  printf("\t-F UHD TX freq [Default %.1f MHz]\n", args->uhd_tx_freq/1000000);
  printf("\t-r RNTI [Default 0x%x]\n",args->rnti);
  printf("\t-l Force N_id_2 [Default best]\n");
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "agfFrlnv")) != -1) {
    switch (opt) {
    case 'a':
      args->uhd_args = argv[optind];
      break;
    case 'g':
      args->uhd_gain = atof(argv[optind]);
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
    case 'r':
      args->rnti = atoi(argv[optind]);
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
  return cuhd_recv_timed(h, data, nsamples, 1, &uhd_time->full_secs, &uhd_time->frac_secs);
}

extern float mean_exec_time;

enum receiver_state { DECODE_MIB, SEND_PRACH, RECV_RAR} state; 

#define NOF_PRACH_SEQUENCES 52

ue_dl_t ue_dl; 
ue_sync_t ue_sync; 
prach_t prach; 
pusch_t pusch; 
lte_fft_t fft; 
harq_t pusch_harq; 
cf_t *prach_buffers[NOF_PRACH_SEQUENCES];
int prach_buffer_len;

prog_args_t prog_args; 

uint32_t sfn = 0; // system frame number
cf_t *sf_buffer = NULL; 


int generate_prach_sequences(){
  for(int i=0;i<NOF_PRACH_SEQUENCES;i++){
    if(prach_gen(&prach, i, 2, prach_buffers[i])){
      fprintf(stderr, "Error generating prach sequence\n");
      return -1;
    }
  }
  return 0;
}

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
    int ret = LIBLTE_ERROR;
    uint8_t *ptr = buffer; 
    
    if(buffer != NULL &&
          msg != NULL)
    {
      ptr++;
      msg->hdr_type = *ptr++;
      if(msg->hdr_type == rar_header_type_bi) {
        ptr += 2; 
        msg->BI = bit_unpack(&ptr, 4);
        ret = LIBLTE_SUCCESS; 
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
        ret = LIBLTE_SUCCESS;
      } 
    }

    return(ret);
}

int rar_to_ra_pusch(rar_msg_t *rar, ra_pusch_t *ra, uint32_t nof_prb) {
  bzero(ra, sizeof(ra_pusch_t));
  if (!rar->hopping_flag) {
    ra->freq_hop_fl = hop_disabled;
  } else {
    fprintf(stderr, "FIXME: Frequency hopping in RAR not implemented\n");
    ra->freq_hop_fl = 1;
  }
  uint32_t riv = rar->rba; 
  // Truncate resource block assignment 
  uint32_t b = 0;
  if (nof_prb <= 44) {
    b = (uint32_t) (ceilf(log2((float) nof_prb*(nof_prb+1)/2)));
    riv = riv & ((1<<(b+1))-1); 
  }
  ra->type2_alloc.riv = riv; 
  ra->mcs_idx = rar->mcs;
  printf("b: %d, RIV: %d\n", b, riv);
  ra_type2_from_riv(riv, &ra->type2_alloc.L_crb, &ra->type2_alloc.RB_start,
      nof_prb, nof_prb);
  
  ra_mcs_from_idx_ul(ra->mcs_idx, ra_nprb_ul(ra, nof_prb), &ra->mcs);
  return LIBLTE_SUCCESS;
}

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
  ra_ul_alloc_t prb_alloc;
  uint32_t rar_window_start = 0, rar_trials = 0, rar_window_stop = 0; 
  timestamp_t uhd_time;
  timestamp_t next_tx_time;  
  const uint8_t conn_request_msg[] = {0x20, 0x06, 0x1F, 0x5C, 0x2C, 0x04, 0xB2, 0xAC, 0xF6};

  parse_args(&prog_args, argc, argv);

  printf("Opening UHD device...\n");
  if (cuhd_open(prog_args.uhd_args, &uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }

  /* Set receiver gain */
  cuhd_set_rx_gain(uhd, prog_args.uhd_gain);
  cuhd_set_tx_gain(uhd, prog_args.uhd_gain);
  
  //cuhd_set_tx_antenna(uhd, "TX/RX");
  
  /* set receiver frequency */
  cuhd_set_rx_freq(uhd, (double) prog_args.uhd_rx_freq);
  cuhd_rx_wait_lo_locked(uhd);
  printf("Tunning RX receiver to %.3f MHz\n", (double ) prog_args.uhd_rx_freq/1000000);

  cuhd_set_tx_freq(uhd, prog_args.uhd_tx_freq);
  cuhd_set_tx_freq_offset(uhd, prog_args.uhd_tx_freq, prog_args.uhd_tx_freq_offset);
  printf("Tunning TX receiver to %.3f MHz\n", (double ) prog_args.uhd_tx_freq/1000000);

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
    cuhd_set_tx_srate(uhd, (double) srate);      
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
    exit(-1);
  }

  INFO("Stopping UHD and flushing buffer...\r",0);
  cuhd_stop_rx_stream(uhd);
  cuhd_flush_buffer(uhd);
  
  if (ue_mib_init(&ue_mib, cell)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }
  
  if (prach_init(&prach, lte_symbol_sz(cell.nof_prb), 0, 0, false, 1)) {
    fprintf(stderr, "Error initializing PRACH\n");
    exit(-1);
  }
  prach_buffer_len = prach.N_seq + prach.N_cp;
  for(int i=0;i<NOF_PRACH_SEQUENCES;i++){
    prach_buffers[i] = (cf_t*)malloc(prach_buffer_len*sizeof(cf_t));
    if(!prach_buffers[i]) {
      perror("maloc");
      exit(-1);
    }
  }
  generate_prach_sequences();
  
  refsignal_ul_t drms; 
  refsignal_drms_pusch_cfg_t pusch_cfg;
  pusch_cfg.nof_prb = 3; 
  bzero(&pusch_cfg, sizeof(refsignal_drms_pusch_cfg_t));
  if (refsignal_ul_init(&drms, cell)) {
    fprintf(stderr, "Error initiating refsignal_ul\n");
    exit(-1);
  }
  cf_t *drms_signal = vec_malloc(2*RE_X_RB*pusch_cfg.nof_prb*sizeof(cf_t));
  if (!drms_signal) {
    perror("malloc");
    exit(-1);
  }
  for (uint32_t i=0;i<2;i++) {
    refsignal_dmrs_pusch_gen(&drms, &pusch_cfg, 2*4+i, &drms_signal[i*RE_X_RB*pusch_cfg.nof_prb]);
  }
  
  if (pusch_init(&pusch, cell)) {
    fprintf(stderr, "Error initiating PUSCH\n");
    exit(-1);
  }

  if (harq_init(&pusch_harq, cell)) {
    fprintf(stderr, "Error initiating HARQ process\n");
    exit(-1);
  }
  
  if (lte_ifft_init(&fft, cell.cp, cell.nof_prb)) {
    fprintf(stderr, "Error initiating SC-FDMA modulator\n");
    exit(-1);
  }
  lte_fft_set_freq_shift(&fft, 0.5);

  cf_t *ul_signal = vec_malloc(sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));
  if (!ul_signal) {
    perror("malloc");
    exit(-1);
  }
  bzero(ul_signal, sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));

  cf_t *sf_symbols = vec_malloc(sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));
  if (!sf_symbols) {
    perror("malloc");
    exit(-1);
  }
  bzero(sf_symbols, sizeof(cf_t) * SF_LEN_PRB(cell.nof_prb));

  state = DECODE_MIB; 
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper_timed, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }

  if (ue_dl_init(&ue_dl, cell)) {  // This is the User RNTI
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  
  /* Initialize subframe counter */
  sf_cnt = 0;

  // Register Ctrl+C handler
  signal(SIGINT, sig_int_handler);

  cuhd_start_rx_stream(uhd);    
    
  struct timeval tdata[3];
  
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
              state = SEND_PRACH; 
            }
          }
          break;
        case SEND_PRACH:

          if (((sfn%2) == 1) && (ue_sync_get_sfidx(&ue_sync) == 1)) {
            ue_sync_get_last_timestamp(&ue_sync, &uhd_time);

            timestamp_copy(&next_tx_time, &uhd_time);
            timestamp_add(&next_tx_time, 0, 0.01); // send next frame (10 ms)
            printf("Send prach sfn: %d. Last frame time = %.6f, send prach time = %.6f\n", 
                   sfn, timestamp_real(&uhd_time), timestamp_real(&next_tx_time));
            cuhd_send_timed(uhd, prach_buffers[7], prach_buffer_len, 1,
                            next_tx_time.full_secs, next_tx_time.frac_secs);
            
            uint16_t ra_rnti = 2; 
            ue_dl_set_rnti(&ue_dl, ra_rnti); 
            rar_window_start = sfn+1;
            rar_window_stop = sfn+3;
            state = RECV_RAR;
          }
          break;
        case RECV_RAR:
          if ((sfn == rar_window_start && ue_sync_get_sfidx(&ue_sync) > 3) || sfn > rar_window_start) {
            gettimeofday(&tdata[1], NULL);
            printf("Looking for RAR in sfn: %d sf_idx: %d\n", sfn, ue_sync_get_sfidx(&ue_sync));
            n = ue_dl_decode(&ue_dl, sf_buffer, data_rx, ue_sync_get_sfidx(&ue_sync));
            if (n < 0) {
              fprintf(stderr, "Error decoding UE DL\n");fflush(stdout);
            } else if (n > 0) {
              printf("RAR received %d bits: ", n);
              vec_fprint_hex(stdout, data_rx, n);
              rar_unpack(data_rx, &rar_msg);
              rar_msg_fprint(stdout, &rar_msg);              
              
              pusch_set_rnti(&pusch, rar_msg.temp_c_rnti);
              
              rar_to_ra_pusch(&rar_msg, &ra_pusch, cell.nof_prb);
              ra_pusch_fprint(stdout, &ra_pusch, cell.nof_prb);

              ra_ul_alloc(&prb_alloc, &ra_pusch, 0, cell.nof_prb);
              
              printf("Sending ConnectionRequest in sfn: %d sf_idx: %d\n", sfn, ue_sync_get_sfidx(&ue_sync));
              verbose=VERBOSE_INFO;
              if (harq_setup_ul(&pusch_harq, ra_pusch.mcs, 0, (ue_sync_get_sfidx(&ue_sync)+6)%10, &prb_alloc)) {
                fprintf(stderr, "Error configuring HARQ process\n");
                exit(-1);;
              }
              if (pusch_encode(&pusch, &pusch_harq, (uint8_t*) conn_request_msg, sf_symbols)) {
                fprintf(stderr, "Error encoding TB\n");
                exit(-1);
              }
              for (uint32_t i=0;i<2;i++) {
                refsignal_drms_pusch_put(&drms, &pusch_cfg, &drms_signal[i*RE_X_RB*pusch_cfg.nof_prb], i, prb_alloc.n_prb[i], sf_symbols);                
              }
              
              lte_ifft_run_sf(&fft, sf_symbols, ul_signal);
              
              ue_sync_get_last_timestamp(&ue_sync, &uhd_time);
              timestamp_copy(&next_tx_time, &uhd_time);
              timestamp_add(&next_tx_time, 0, 0.006); // send after 6 sub-frames (6 ms)
              printf("Send PUSCH sfn: %d. Last frame time = %.6f, send PUSCH time = %.6f\n", 
                    sfn, timestamp_real(&uhd_time), timestamp_real(&next_tx_time));
              gettimeofday(&tdata[2], NULL);
              get_time_interval(tdata);
              printf("time exec: %d\n",tdata[0].tv_usec);
              cuhd_send_timed(uhd, ul_signal, SF_LEN_PRB(cell.nof_prb), 1,
                              next_tx_time.full_secs, next_tx_time.frac_secs);
              go_exit = 1; 
            }
            if (sfn >= rar_window_stop) {              
              state = SEND_PRACH;
              rar_trials++;
              if (rar_trials >= 10) {
                go_exit = 1; 
              }
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






  
