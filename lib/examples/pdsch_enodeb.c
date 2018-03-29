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
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/phch/pdsch_cfg.h>

#include "srslte/srslte.h"


#define UE_CRNTI 0x1234
#define M_CRNTI 0xFFFD

#ifndef DISABLE_RF
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/common/phy_common.h"
srslte_rf_t rf;
#else
#warning Compiling pdsch_ue with no RF support
#endif

char *output_file_name = NULL;

#define LEFT_KEY  68
#define RIGHT_KEY 67
#define UP_KEY    65
#define DOWN_KEY  66

srslte_cell_t cell = {
  25,               // nof_prb
  1,                // nof_ports
  0,                // cell_id
  SRSLTE_CP_NORM,   // cyclic prefix
  SRSLTE_PHICH_NORM, // PHICH length
  SRSLTE_PHICH_R_1 // PHICH resources
};

uint16_t c = -1;
  
int net_port = -1; // -1 generates random dataThat means there is some problem sending samples to the device

uint32_t cfi = 2;
uint32_t mcs_idx = 1, last_mcs_idx = 1;
int nof_frames = -1;


char mimo_type_str[32] = "single";
uint32_t nof_tb = 1;
uint32_t multiplex_pmi = 0;
uint32_t multiplex_nof_layers = 1;

int mbsfn_area_id = -1;
char *rf_args = "";
float rf_amp = 0.8, rf_gain = 70.0, rf_freq = 2400000000;

float output_file_snr = +INFINITY;

bool null_file_sink=false; 
srslte_filesink_t fsink;
srslte_ofdm_t ifft[SRSLTE_MAX_PORTS];
srslte_ofdm_t ifft_mbsfn;
srslte_pbch_t pbch;
srslte_pcfich_t pcfich;
srslte_pdcch_t pdcch;
srslte_pdsch_t pdsch;
srslte_pdsch_cfg_t pdsch_cfg;
srslte_pmch_t pmch;
srslte_pdsch_cfg_t  pmch_cfg;
srslte_softbuffer_tx_t *softbuffers[SRSLTE_MAX_CODEWORDS];
srslte_regs_t regs;
srslte_ra_dl_dci_t ra_dl;  
int rvidx[SRSLTE_MAX_CODEWORDS] = {0, 0};

cf_t *sf_buffer[SRSLTE_MAX_PORTS] = {NULL}, *output_buffer [SRSLTE_MAX_PORTS] = {NULL};


int sf_n_re, sf_n_samples;

pthread_t net_thread; 
void *net_thread_fnc(void *arg);
sem_t net_sem;
bool net_packet_ready = false; 
srslte_netsource_t net_source; 
srslte_netsink_t net_sink; 


int prbset_num = 1, last_prbset_num = 1; 
int prbset_orig = 0; 
//#define DATA_BUFF_SZ    1024*128
//uint8_t data[8*DATA_BUFF_SZ], data2[DATA_BUFF_SZ];
//uint8_t data_tmp[DATA_BUFF_SZ];


#define DATA_BUFF_SZ    1024*1024
uint8_t *data[2], data2[DATA_BUFF_SZ];
uint8_t data_tmp[DATA_BUFF_SZ];

void usage(char *prog) {
  printf("Usage: %s [agmfoncvpuxb]\n", prog);
#ifndef DISABLE_RF
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-l RF amplitude [Default %.2f]\n", rf_amp);
  printf("\t-g RF TX gain [Default %.2f dB]\n", rf_gain);
  printf("\t-f RF TX frequency [Default %.1f MHz]\n", rf_freq / 1000000);
#else
  printf("\t   RF is disabled.\n");
#endif
  printf("\t-o output_file [Default use RF board]\n");
  printf("\t-m MCS index [Default %d]\n", mcs_idx);
  printf("\t-n number of frames [Default %d]\n", nof_frames);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-p nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-M MBSFN area id [Default %d]\n", mbsfn_area_id);
  printf("\t-x Transmission mode[single|diversity|cdd|multiplex] [Default %s]\n", mimo_type_str);
  printf("\t-b Precoding Matrix Index (multiplex mode only)* [Default %d]\n", multiplex_pmi);
  printf("\t-w Number of codewords/layers (multiplex mode only)* [Default %d]\n", multiplex_nof_layers);
  printf("\t-u listen TCP port for input data (-1 is random) [Default %d]\n", net_port);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
  printf("\t-s output file SNR [Default %f]\n", output_file_snr);
  printf("\n");
  printf("\t*: See 3GPP 36.212 Table  5.3.3.1.5-4 for more information\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "aglfmoncpvutxbwMs")) != -1) {

    switch (opt) {
    case 'a':
      rf_args = argv[optind];
      break;
    case 'g':
      rf_gain = atof(argv[optind]);
      break;
    case 'l':
      rf_amp = atof(argv[optind]);
      break;
    case 'f':
      rf_freq = atof(argv[optind]);
      break;
    case 'o':
      output_file_name = argv[optind];
      break;
    case 'm':
      mcs_idx = atoi(argv[optind]);
      break;
    case 'u':
      net_port = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'p':
      cell.nof_prb = atoi(argv[optind]);
      break;
    case 'c':
      cell.id = atoi(argv[optind]);
      break;
    case 'x':
      strncpy(mimo_type_str, argv[optind], 31);
      mimo_type_str[31] = 0;
      break;
    case 'b':
      multiplex_pmi = (uint32_t) atoi(argv[optind]);
      break;
    case 'w':
      multiplex_nof_layers = (uint32_t) atoi(argv[optind]);
      break;
    case 'M':
      mbsfn_area_id = atoi(argv[optind]);
      break;
    case 'v':
      srslte_verbose++;
      break;
    case 's':
      output_file_snr = atof(argv[optind]);
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
#ifdef DISABLE_RF
  if (!output_file_name) {
    usage(argv[0]);
    exit(-1);
  }
#endif
}
  
void base_init() {
  int i;

  /* Select transmission mode */
  if (srslte_str2mimotype(mimo_type_str, &pdsch_cfg.mimo_type)) {
    ERROR("Wrong transmission mode! Allowed modes: single, diversity, cdd and multiplex");
    exit(-1);
  }

  /* Configure cell and PDSCH in function of the transmission mode */
  switch(pdsch_cfg.mimo_type) {
    case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
      cell.nof_ports = 1;
      break;
    case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
      cell.nof_ports = 2;
      break;
    case SRSLTE_MIMO_TYPE_CDD:
      cell.nof_ports = 2;
      break;
    case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
      cell.nof_ports = 2;
      break;
    default:
      ERROR("Transmission mode not implemented.");
      exit(-1);
  }

  /* Allocate memory */
  for(i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    data[i] = srslte_vec_malloc(sizeof(uint8_t) * SOFTBUFFER_SIZE);
    if (!data[i]) {
      perror("malloc");
      exit(-1);
    }
    bzero(data[i], sizeof(uint8_t) * SOFTBUFFER_SIZE);
  }

  /* init memory */
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    sf_buffer[i] = srslte_vec_malloc(sizeof(cf_t) * sf_n_re);
    if (!sf_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    output_buffer[i] = srslte_vec_malloc(sizeof(cf_t) * sf_n_samples);
    if (!output_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
    bzero(output_buffer[i], sizeof(cf_t) * sf_n_samples);
  }


  /* open file or USRP */
  if (output_file_name) {
    if (strcmp(output_file_name, "NULL")) {
      if (srslte_filesink_init(&fsink, output_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
        fprintf(stderr, "Error opening file %s\n", output_file_name);
        exit(-1);
      }      
      null_file_sink = false; 
    } else {
      null_file_sink = true; 
    }
  } else {
#ifndef DISABLE_RF
    printf("Opening RF device...\n");
    if (srslte_rf_open_multi(&rf, rf_args, cell.nof_ports)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
#else
    printf("Error RF not available. Select an output file\n");
    exit(-1);
#endif
  }
  
  if (net_port > 0) {
    if (srslte_netsource_init(&net_source, "0.0.0.0", net_port, SRSLTE_NETSOURCE_TCP)) {
      fprintf(stderr, "Error creating input UDP socket at port %d\n", net_port);
      exit(-1);
    }
    if (null_file_sink) {
      if (srslte_netsink_init(&net_sink, "127.0.0.1", net_port+1, SRSLTE_NETSINK_TCP)) {
        fprintf(stderr, "Error sink\n");
        exit(-1);
      }      
    }
    if (sem_init(&net_sem, 0, 1)) {
      perror("sem_init");
      exit(-1);
    }
  }

  /* create ifft object */
  for (i = 0; i < cell.nof_ports; i++) {
    if (srslte_ofdm_tx_init(&ifft[i], SRSLTE_CP_NORM, sf_buffer[i], output_buffer[i], cell.nof_prb)) {
      fprintf(stderr, "Error creating iFFT object\n");
      exit(-1);
    }

    srslte_ofdm_set_normalize(&ifft[i], true);
  }

  if (srslte_ofdm_tx_init_mbsfn(&ifft_mbsfn, SRSLTE_CP_EXT, sf_buffer[0], output_buffer[0], cell.nof_prb)) {
    fprintf(stderr, "Error creating iFFT object\n");
    exit(-1);
  }
  srslte_ofdm_set_non_mbsfn_region(&ifft_mbsfn, 2);
  srslte_ofdm_set_normalize(&ifft_mbsfn, true);
  
  if (srslte_pbch_init(&pbch)) {
    fprintf(stderr, "Error creating PBCH object\n");
    exit(-1);
  }
  if (srslte_pbch_set_cell(&pbch, cell)) {
    fprintf(stderr, "Error creating PBCH object\n");
    exit(-1);
  }
  
  
  

  if (srslte_regs_init(&regs, cell)) {
    fprintf(stderr, "Error initiating regs\n");
    exit(-1);
  }
  if (srslte_pcfich_init(&pcfich, 1)) {
    fprintf(stderr, "Error creating PBCH object\n");
    exit(-1);
  }
  if (srslte_pcfich_set_cell(&pcfich, &regs, cell)) {
    fprintf(stderr, "Error creating PBCH object\n");
    exit(-1);
  }

  if (srslte_pdcch_init_enb(&pdcch, cell.nof_prb)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }
  if (srslte_pdcch_set_cell(&pdcch, &regs, cell)) {
    fprintf(stderr, "Error creating PDCCH object\n");
    exit(-1);
  }

  if (srslte_pdsch_init_enb(&pdsch, cell.nof_prb)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    exit(-1);
  }
  if (srslte_pdsch_set_cell(&pdsch, cell)) {
    fprintf(stderr, "Error creating PDSCH object\n");
    exit(-1);
  }

  srslte_pdsch_set_rnti(&pdsch, UE_CRNTI);


  if(mbsfn_area_id > -1){
    if (srslte_pmch_init(&pmch, cell.nof_prb)) {
      fprintf(stderr, "Error creating PMCH object\n");
    }
    srslte_pmch_set_area_id(&pmch, mbsfn_area_id);
  }
  
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    softbuffers[i] = calloc(sizeof(srslte_softbuffer_tx_t), 1);
    if (!softbuffers[i]) {
      fprintf(stderr, "Error allocating soft buffer\n");
      exit(-1);
    }

    if (srslte_softbuffer_tx_init(softbuffers[i], cell.nof_prb)) {
      fprintf(stderr, "Error initiating soft buffer\n");
      exit(-1);
    }
  }
}


void base_free() {
  int i;
  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    srslte_softbuffer_tx_free(softbuffers[i]);
    if (softbuffers[i]) {
      free(softbuffers[i]);
    }
  }
  srslte_pdsch_free(&pdsch);
  srslte_pdcch_free(&pdcch);
  srslte_regs_free(&regs);
  srslte_pbch_free(&pbch);
  if(mbsfn_area_id > -1){
    srslte_pmch_free(&pmch); 
  }
  srslte_ofdm_tx_free(&ifft_mbsfn);
  for (i = 0; i < cell.nof_ports; i++) {
    srslte_ofdm_tx_free(&ifft[i]);
  }

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (data[i]) {
      free(data[i]);
    }
  }

  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (sf_buffer[i]) {
      free(sf_buffer[i]);
    }

    if (output_buffer[i]) {
      free(output_buffer[i]);
    }
  }
  if (output_file_name) {
    if (!null_file_sink) {
      srslte_filesink_free(&fsink);      
    }
  } else {
#ifndef DISABLE_RF
    srslte_rf_close(&rf);
#endif
  }
  
  if (net_port > 0) {
    srslte_netsource_free(&net_source);
    sem_close(&net_sem);
  }  
}


bool go_exit = false; 
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}



unsigned int
reverse(register unsigned int x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));

}

uint32_t prbset_to_bitmask() {
  uint32_t mask=0;
  int nb = (int) ceilf((float) cell.nof_prb / srslte_ra_type0_P(cell.nof_prb));
  for (int i=0;i<nb;i++) {
    if (i >= prbset_orig && i < prbset_orig + prbset_num) {
      mask = mask | (0x1<<i);     
    }
  }
  return reverse(mask)>>(32-nb); 
}

int update_radl() {

  /* Configure cell and PDSCH in function of the transmission mode */
  switch(pdsch_cfg.mimo_type) {
    case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
      pdsch_cfg.nof_layers = 1;
      nof_tb = 1;
      break;
    case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
      pdsch_cfg.nof_layers = 2;
      nof_tb = 1;
      break;
    case SRSLTE_MIMO_TYPE_CDD:
      pdsch_cfg.nof_layers = 2;
      nof_tb = 2;
      break;
    case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
      pdsch_cfg.nof_layers = multiplex_nof_layers;
      nof_tb = multiplex_nof_layers;
      break;
    default:
      ERROR("Transmission mode not implemented.");
      exit(-1);
  }

  bzero(&ra_dl, sizeof(srslte_ra_dl_dci_t));
  ra_dl.harq_process = 0;
  ra_dl.mcs_idx = mcs_idx;
  ra_dl.ndi = 0;
  ra_dl.rv_idx = rvidx[0];
  ra_dl.alloc_type = SRSLTE_RA_ALLOC_TYPE0;
  ra_dl.type0_alloc.rbg_bitmask = prbset_to_bitmask();
  ra_dl.tb_en[0] = 1;

  if (nof_tb > 1) {
    ra_dl.mcs_idx_1 = mcs_idx;
    ra_dl.ndi_1 = 0;
    ra_dl.rv_idx_1 = rvidx[1];
    ra_dl.tb_en[1] = 1;
  }

  srslte_ra_pdsch_fprint(stdout, &ra_dl, cell.nof_prb);
  srslte_ra_dl_grant_t dummy_grant; 
  srslte_ra_nbits_t dummy_nbits[SRSLTE_MAX_CODEWORDS];
  srslte_ra_dl_dci_to_grant(&ra_dl, cell.nof_prb, UE_CRNTI, &dummy_grant);
  srslte_ra_dl_grant_to_nbits(&dummy_grant, cfi, cell, 0, dummy_nbits);
  srslte_ra_dl_grant_fprint(stdout, &dummy_grant);
  dummy_grant.sf_type = SRSLTE_SF_NORM;
  if (pdsch_cfg.mimo_type != SRSLTE_MIMO_TYPE_SINGLE_ANTENNA) {
    printf("\nTransmission mode key table:\n");
    printf("   Mode   |   1TB   | 2TB |\n");
    printf("----------+---------+-----+\n");
    printf("Diversity |    x    |     |\n");
    printf("      CDD |         |  z  |\n");
    printf("Multiplex | q,w,e,r | a,s |\n");
    printf("\n");
    printf("Type new MCS index (0-28) or mode key and press Enter: ");
  } else {
    printf("Type new MCS index (0-28) and press Enter: ");
  }
  fflush(stdout);

  return 0; 
}

/* Read new MCS from stdin */
int update_control() {
  char input[128];
  
  fd_set set; 
  FD_ZERO(&set);
  FD_SET(0, &set);
  
  struct timeval to; 
  to.tv_sec = 0; 
  to.tv_usec = 0; 

  int n = select(1, &set, NULL, NULL, &to);
  if (n == 1) {
    // stdin ready
    if (fgets(input, sizeof(input), stdin)) {
      if(input[0] == 27) {
        switch(input[2]) {
          case RIGHT_KEY:
            if (prbset_orig  + prbset_num < (int) ceilf((float) cell.nof_prb / srslte_ra_type0_P(cell.nof_prb)))
              prbset_orig++;
            break;
          case LEFT_KEY:
            if (prbset_orig > 0)
              prbset_orig--;
            break;
          case UP_KEY:
            if (prbset_num < (int) ceilf((float) cell.nof_prb / srslte_ra_type0_P(cell.nof_prb)))
              prbset_num++;
            break;
          case DOWN_KEY:
            last_prbset_num = prbset_num;
            if (prbset_num > 0)
              prbset_num--;          
            break;          
        }
      } else {
        switch (input[0]) {
          case 'q':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 0;
            multiplex_nof_layers = 1;
            break;
          case 'w':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 1;
            multiplex_nof_layers = 1;
            break;
          case 'e':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 2;
            multiplex_nof_layers = 1;
            break;
          case 'r':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 3;
            multiplex_nof_layers = 1;
            break;
          case 'a':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 0;
            multiplex_nof_layers = 2;
            break;
          case 's':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
            multiplex_pmi = 1;
            multiplex_nof_layers = 2;
            break;
          case 'z':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_CDD;
            break;
          case 'x':
            pdsch_cfg.mimo_type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
            break;
          default:
            last_mcs_idx = mcs_idx;
            mcs_idx = atoi(input);
        }
      }
      bzero(input,sizeof(input));
      if (update_radl()) {
        printf("Trying with last known MCS index\n");
        mcs_idx = last_mcs_idx; 
        prbset_num = last_prbset_num; 
        return update_radl();
      }
    }
    return 0; 
  } else if (n < 0) {
    // error
    perror("select");
    return -1; 
  } else {
    return 0; 
  }
}


/** Function run in a separate thread to receive UDP data */
void *net_thread_fnc(void *arg) {
  int n; 
  int rpm = 0, wpm=0; 
  
  do {
    n = srslte_netsource_read(&net_source, &data2[rpm], DATA_BUFF_SZ-rpm);
    if (n > 0) {
      // FIXME: I assume that both transport blocks have same size in case of 2 tb are active
      int nbytes = 1 + (pdsch_cfg.grant.mcs[0].tbs + pdsch_cfg.grant.mcs[1].tbs - 1) / 8;
      rpm += n; 
      INFO("received %d bytes. rpm=%d/%d\n",n,rpm,nbytes);
      wpm = 0; 
      while (rpm >= nbytes) {
        // wait for packet to be transmitted
        sem_wait(&net_sem);
        memcpy(data[0], &data2[wpm], nbytes / (size_t) 2);
        memcpy(data[1], &data2[wpm], nbytes / (size_t) 2);
        INFO("Sent %d/%d bytes ready\n", nbytes, rpm);
        rpm -= nbytes;          
        wpm += nbytes; 
        net_packet_ready = true; 
      }
      if (wpm > 0) {
        INFO("%d bytes left in buffer for next packet\n", rpm);
        memcpy(data2, &data2[wpm], rpm * sizeof(uint8_t));
      }
    } else if (n == 0) {
      rpm = 0; 
    } else {
      fprintf(stderr, "Error receiving from network\n");
      exit(-1);
    }      
  } while(n >= 0);
  return NULL;
}


int main(int argc, char **argv) {
  int nf=0, sf_idx=0, N_id_2=0;
  cf_t pss_signal[SRSLTE_PSS_LEN];
  float sss_signal0[SRSLTE_SSS_LEN]; // for subframe 0
  float sss_signal5[SRSLTE_SSS_LEN]; // for subframe 5
  uint8_t bch_payload[SRSLTE_BCH_PAYLOAD_LEN];
  int i;
  cf_t *sf_symbols[SRSLTE_MAX_PORTS];
  cf_t *slot1_symbols[SRSLTE_MAX_PORTS];
  srslte_dci_msg_t dci_msg;
  srslte_dci_location_t locations[SRSLTE_NSUBFRAMES_X_FRAME][30];
  uint32_t sfn; 
  srslte_refsignal_t csr_refs;
  srslte_refsignal_t mbsfn_refs;

  srslte_debug_handle_crash(argc, argv);

#ifdef DISABLE_RF
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
#endif

  parse_args(argc, argv);

  N_id_2 = cell.id % 3;
  sf_n_re = 2 * SRSLTE_CP_NORM_NSYMB * cell.nof_prb * SRSLTE_NRE;
  sf_n_samples = 2 * SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.nof_prb));

  cell.phich_length = SRSLTE_PHICH_NORM;
  cell.phich_resources = SRSLTE_PHICH_R_1;
  sfn = 0;

  prbset_num = (int) ceilf((float) cell.nof_prb / srslte_ra_type0_P(cell.nof_prb)); 
  last_prbset_num = prbset_num; 
  
  /* this *must* be called after setting slot_len_* */
  base_init();

  /* Generate PSS/SSS signals */
  srslte_pss_generate(pss_signal, N_id_2);
  srslte_sss_generate(sss_signal0, sss_signal5, cell.id);
  

  /* Generate reference signals */
  if(srslte_refsignal_cs_init(&csr_refs, cell.nof_prb)) {
    fprintf(stderr, "Error initializing equalizer\n");
    exit(-1);
  }
  if(mbsfn_area_id > -1) {
    if(srslte_refsignal_mbsfn_init(&mbsfn_refs, cell, mbsfn_area_id)) {
      fprintf(stderr, "Error initializing equalizer\n");
      exit(-1);
    }
  }
  
  if(srslte_refsignal_cs_set_cell(&csr_refs, cell)){
    fprintf(stderr, "Error setting cell\n");
    exit(-1);
  }
  
  
  for (i = 0; i < SRSLTE_MAX_PORTS; i++) {
    sf_symbols[i] = sf_buffer[i%cell.nof_ports];
    slot1_symbols[i] = &sf_buffer[i%cell.nof_ports][SRSLTE_SLOT_LEN_RE(cell.nof_prb, cell.cp)];
  }


#ifndef DISABLE_RF


  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  if (!output_file_name) {
    
    int srate = srslte_sampling_freq_hz(cell.nof_prb);    
    if (srate != -1) {  
      if (srate < 10e6) {          
        srslte_rf_set_master_clock_rate(&rf, 4*srate);        
      } else {
        srslte_rf_set_master_clock_rate(&rf, srate);        
      }
      printf("Setting sampling rate %.2f MHz\n", (float) srate/1000000);
      float srate_rf = srslte_rf_set_tx_srate(&rf, (double) srate);
      if (srate_rf != srate) {
        fprintf(stderr, "Could not set sampling rate\n");
        exit(-1);
      }
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell.nof_prb);
      exit(-1);
    }
    printf("Set TX gain: %.1f dB\n", srslte_rf_set_tx_gain(&rf, rf_gain));
    printf("Set TX freq: %.2f MHz\n",
        srslte_rf_set_tx_freq(&rf, rf_freq) / 1000000);
  }
#endif

  if (update_radl(sf_idx)) {
    exit(-1);
  }
  
  if (net_port > 0) {
    if (pthread_create(&net_thread, NULL, net_thread_fnc, NULL)) {
      perror("pthread_create");
      exit(-1);
    }
  }
  
  /* Initiate valid DCI locations */
  for (i=0;i<SRSLTE_NSUBFRAMES_X_FRAME;i++) {
    srslte_pdcch_ue_locations(&pdcch, locations[i], 30, i, cfi, UE_CRNTI);
  }
    
  nf = 0;
  
  bool send_data = false; 
    for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    srslte_softbuffer_tx_reset(softbuffers[i]);
  }


#ifndef DISABLE_RF
  bool start_of_burst = true; 
#endif

  while ((nf < nof_frames || nof_frames == -1) && !go_exit) {
    for (sf_idx = 0; sf_idx < SRSLTE_NSUBFRAMES_X_FRAME && (nf < nof_frames || nof_frames == -1); sf_idx++) {
      /* Set Antenna port resource elements to zero */
      bzero(sf_symbols[0], sizeof(cf_t) * sf_n_re);


      if (sf_idx == 0 || sf_idx == 5) {
        srslte_pss_put_slot(pss_signal, sf_symbols[0], cell.nof_prb, SRSLTE_CP_NORM);
        srslte_sss_put_slot(sf_idx ? sss_signal5 : sss_signal0, sf_symbols[0], cell.nof_prb,
            SRSLTE_CP_NORM);
      }
      
      /* Copy zeros, SSS, PSS into the rest of antenna ports */
      for (i = 1; i < cell.nof_ports; i++) {
        memcpy(sf_symbols[i], sf_symbols[0], sizeof(cf_t) * sf_n_re);
      }
      
      if(sf_idx == 1 && mbsfn_area_id > -1){
        srslte_refsignal_mbsfn_put_sf(cell, 0,csr_refs.pilots[0][sf_idx], mbsfn_refs.pilots[0][sf_idx],  sf_symbols[0]);
      } else { 
        for (i = 0; i < cell.nof_ports; i++) {
          srslte_refsignal_cs_put_sf(cell, (uint32_t) i, csr_refs.pilots[i / 2][sf_idx], sf_symbols[i]);
        }
      }
      
      srslte_pbch_mib_pack(&cell, sfn, bch_payload);
      if (sf_idx == 0) {
        srslte_pbch_encode(&pbch, bch_payload, slot1_symbols, nf%4);
      }

      srslte_pcfich_encode(&pcfich, cfi, sf_symbols, sf_idx);

      /* Update DL resource allocation from control port */
      if (update_control(sf_idx)) {
        fprintf(stderr, "Error updating parameters from control port\n");
      }
      
      /* Transmit PDCCH + PDSCH only when there is data to send */
      if (net_port > 0) {
        send_data = net_packet_ready; 
        if (net_packet_ready) {
          INFO("Transmitting packet\n");
        }
      } else {
        INFO("SF: %d, Generating %d random bits\n", sf_idx, pdsch_cfg.grant.mcs[0].tbs + pdsch_cfg.grant.mcs[1].tbs);
        for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
          if (pdsch_cfg.grant.tb_en[tb]) {
            for (i = 0; i < pdsch_cfg.grant.mcs[tb].tbs / 8; i++) {
              data[tb][i] = (uint8_t) rand();
            }
          }
        }
        /* Uncomment this to transmit on sf 0 and 5 only  */
        if (sf_idx != 0 && sf_idx != 5) {
          send_data = true; 
        } else {
          send_data = false;           
        }
      }      
      
      if (send_data) {
        if(sf_idx != 1 || mbsfn_area_id < 0) { // PDCCH + PDSCH
          srslte_dci_format_t dci_format;
          switch(pdsch_cfg.mimo_type) {
            case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
              dci_format = SRSLTE_DCI_FORMAT1;
              break;
            case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
            case SRSLTE_MIMO_TYPE_CDD:
              dci_format = SRSLTE_DCI_FORMAT2A;
              break;
            case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
              dci_format = SRSLTE_DCI_FORMAT2;
              if (multiplex_nof_layers == 1) {
                ra_dl.pinfo = (uint8_t) (multiplex_pmi + 1);
              } else {
                ra_dl.pinfo = (uint8_t) multiplex_pmi;
              }
              break;
            default:
              fprintf(stderr, "Wrong MIMO configuration\n");
              exit(SRSLTE_ERROR);
          }
          /* Encode PDCCH */
          INFO("Putting DCI to location: n=%d, L=%d\n", locations[sf_idx][0].ncce, locations[sf_idx][0].L);
          srslte_dci_msg_pack_pdsch(&ra_dl, dci_format, &dci_msg, cell.nof_prb, cell.nof_ports, false);
          if (srslte_pdcch_encode(&pdcch, &dci_msg, locations[sf_idx][0], UE_CRNTI, sf_symbols, sf_idx, cfi)) {
            fprintf(stderr, "Error encoding DCI message\n");
            exit(-1);
          }

          /* Configure pdsch_cfg parameters */
          srslte_ra_dl_grant_t grant; 
          srslte_ra_dl_dci_to_grant(&ra_dl, cell.nof_prb, UE_CRNTI, &grant);        
          if (srslte_pdsch_cfg_mimo(&pdsch_cfg, cell, &grant, cfi, sf_idx, rvidx, pdsch_cfg.mimo_type, multiplex_pmi)) {
            fprintf(stderr, "Error configuring PDSCH\n");
            exit(-1);
          }

          /* Encode PDSCH */
          if (srslte_pdsch_encode(&pdsch, &pdsch_cfg, softbuffers, data, UE_CRNTI, sf_symbols)) {
            fprintf(stderr, "Error encoding PDSCH\n");
            exit(-1);
          }        
          if (net_port > 0 && net_packet_ready) {
            if (null_file_sink) {
              for (uint32_t tb = 0; tb < SRSLTE_MAX_CODEWORDS; tb++) {
                srslte_bit_pack_vector(data[tb], data_tmp, pdsch_cfg.grant.mcs[tb].tbs);
                if (srslte_netsink_write(&net_sink, data_tmp, 1 + (pdsch_cfg.grant.mcs[tb].tbs - 1) / 8) < 0) {
                  fprintf(stderr, "Error sending data through UDP socket\n");
                }
              }
            }
            net_packet_ready = false; 
            sem_post(&net_sem);
          }
        }else{ // We're sending MCH on subframe 1 - PDCCH + PMCH

          /* Encode PDCCH */
          INFO("Putting DCI to location: n=%d, L=%d\n", locations[sf_idx][0].ncce, locations[sf_idx][0].L);
          srslte_dci_msg_pack_pdsch(&ra_dl, SRSLTE_DCI_FORMAT1, &dci_msg, cell.nof_prb, cell.nof_ports, false);
          if (srslte_pdcch_encode(&pdcch, &dci_msg, locations[sf_idx][0], M_CRNTI, sf_symbols, sf_idx, cfi)) {
              fprintf(stderr, "Error encoding DCI message\n");
              exit(-1);
          }
          /* Configure pmch_cfg parameters */
          srslte_ra_dl_grant_t grant;
          grant.tb_en[0] = true;
          grant.tb_en[1] = false;
          grant.mcs[0].idx = 2;
          grant.mcs[0].mod = SRSLTE_MOD_QPSK;
          grant.nof_prb = cell.nof_prb;
          grant.sf_type = SRSLTE_SF_MBSFN;
          grant.Qm[0] = srslte_mod_bits_x_symbol(grant.mcs[0].mod);
          srslte_dl_fill_ra_mcs(&grant.mcs[0], cell.nof_prb);
          for(int i = 0; i < 2; i++){
            for(int j = 0; j < grant.nof_prb; j++){
              grant.prb_idx[i][j] = true;
            }
          }
          

          if (srslte_pmch_cfg(&pmch_cfg, cell, &grant, cfi, sf_idx)) {
            fprintf(stderr, "Error configuring PMCH\n");
            exit(-1);
          }
          /* Encode PMCH */
          if (srslte_pmch_encode(&pmch, &pmch_cfg, softbuffers[0], data[0], mbsfn_area_id, sf_symbols)) {
            fprintf(stderr, "Error encoding PDSCH\n");
            exit(-1);
          }
          if (net_port > 0 && net_packet_ready) {
            if (null_file_sink) {
              srslte_bit_pack_vector(data[0], data_tmp, pmch_cfg.grant.mcs[0].tbs);
              if (srslte_netsink_write(&net_sink, data_tmp, 1+(pmch_cfg.grant.mcs[0].tbs-1)/8) < 0) {
                fprintf(stderr, "Error sending data through UDP socket\n");
              }
            }
            net_packet_ready = false;
            sem_post(&net_sem);
          }
        }
      }

      /* Transform to OFDM symbols */
      if(sf_idx != 1 || mbsfn_area_id < 0){
        for (i = 0; i < cell.nof_ports; i++) {
          srslte_ofdm_tx_sf(&ifft[i]);
        }
      }else{
        srslte_ofdm_tx_sf(&ifft_mbsfn);
      }
      
      /* send to file or usrp */
      if (output_file_name) {
        if (!null_file_sink) {
          /* Apply AWGN */
          if (output_file_snr != +INFINITY) {
            float var = powf(10.0f, -(output_file_snr + 3.0f) / 20.0f);
            for (int k = 0; k < cell.nof_ports; k++) {
              srslte_ch_awgn_c(output_buffer[k], output_buffer[k], var, sf_n_samples);
            }
          }
          srslte_filesink_write_multi(&fsink, (void**) output_buffer, sf_n_samples, cell.nof_ports);
        }
        usleep(1000);
      } else {
#ifndef DISABLE_RF
      float norm_factor = (float) cell.nof_prb/15/sqrtf(pdsch_cfg.grant.nof_prb);
      for (i = 0; i < cell.nof_ports; i++) {
        srslte_vec_sc_prod_cfc(output_buffer[i], rf_amp * norm_factor, output_buffer[i], SRSLTE_SF_LEN_PRB(cell.nof_prb));
      }
      srslte_rf_send_multi(&rf, (void**) output_buffer, sf_n_samples, true, start_of_burst, false);
      start_of_burst=false;
#endif
      }
    }
    nf++;
    sfn = (sfn + 1) % 1024;
  }

  base_free();

  printf("Done\n");
  exit(0);
}


