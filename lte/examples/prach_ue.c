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

double pss_time_offset = (6/14)*10e-3;
double prach_time_offset = 4*10e-3; //Subframe 4


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
  int net_port; 
  char *net_address; 
  int net_port_signal; 
  char *net_address_signal;   
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
  args->net_port = -1; 
  args->net_address = "127.0.0.1";
  args->net_port_signal = -1; 
  args->net_address_signal = "127.0.0.1";
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
uint8_t data[20000], data_packed[20000];

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

enum receiver_state { DECODE_MIB, SEND_PRACH} state; 

#define NOF_PRACH_SEQUENCES 52

ue_dl_t ue_dl; 
ue_sync_t ue_sync; 
prach_t prach; 
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

int main(int argc, char **argv) {
  int ret; 
  lte_cell_t cell;  
  int64_t sf_cnt;
  ue_mib_t ue_mib; 
  void *uhd; 
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
  cuhd_set_tx_gain(uhd, prog_args.uhd_gain);
  
  cuhd_set_tx_antenna(uhd, "TX/RX");
  
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
    return LIBLTE_ERROR;
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
    return -1;
  }
  prach_buffer_len = prach.N_seq + prach.N_cp;
  for(int i=0;i<NOF_PRACH_SEQUENCES;i++){
    prach_buffers[i] = (cf_t*)malloc(prach_buffer_len*sizeof(cf_t));
    if(!prach_buffers[i]) {
      perror("maloc");
      return -1;
    }
  }
  generate_prach_sequences();

  state = DECODE_MIB; 
  if (ue_sync_init(&ue_sync, cell, cuhd_recv_wrapper_timed, uhd)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1); 
  }

  if (ue_dl_init(&ue_dl, cell)) {  // This is the User RNTI
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }
  
  ue_dl_set_user_rnti(&ue_dl, prog_args.rnti==SIRNTI?1:prog_args.rnti);

  /* Configure downlink receiver for the SI-RNTI since will be the only one we'll use */
  ue_dl_set_rnti(&ue_dl, prog_args.rnti); 

  /* Initialize subframe counter */
  sf_cnt = 0;

  // Register Ctrl+C handler
  signal(SIGINT, sig_int_handler);

  cuhd_start_rx_stream(uhd);    
    
  timestamp_t uhd_time;
  timestamp_t next_frame_time;
  timestamp_t next_prach_time;

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

          ue_sync_get_last_timestamp(&ue_sync, &uhd_time);
          timestamp_init(&next_frame_time, uhd_time.full_secs, uhd_time.frac_secs);
          printf("Last frame time = %.6f\n", timestamp_real(&next_frame_time));

          //Tx PRACH every 10 lte dl frames
          printf("TX PRACH\n");
          timestamp_copy(&next_prach_time, &next_frame_time);
          timestamp_add(&next_prach_time, 0, prach_time_offset);
          cuhd_send_timed(uhd, prach_buffers[7], prach_buffer_len, 0,
                          next_prach_time.full_secs, next_prach_time.frac_secs);

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






  
