/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/common/crash_handler.h"
#include "srsran/common/gen_mch_tables.h"
#include "srsran/srsran.h"
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>

#define UE_CRNTI 0x1234
#define M_CRNTI 0xFFFD

#ifndef DISABLE_RF
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/rf/rf.h"
srsran_rf_t radio;
#else
#pragma message "Compiling pdsch_ue with no RF support"
#endif

static char* output_file_name = NULL;

#define LEFT_KEY 68
#define RIGHT_KEY 67
#define UP_KEY 65
#define DOWN_KEY 66
#define PAGE_UP 53
#define PAGE_DOWN 54

static srsran_cell_t cell = {
    25,                // nof_prb
    1,                 // nof_ports
    0,                 // cell_id
    SRSRAN_CP_NORM,    // cyclic prefix
    SRSRAN_PHICH_NORM, // PHICH length
    SRSRAN_PHICH_R_1,  // PHICH resources
    SRSRAN_FDD,
};

static int      net_port = -1; // -1 generates random dataThat means there is some problem sending samples to the device
static uint32_t cfi      = 2;
static uint32_t mcs_idx = 1, last_mcs_idx = 1;
static int      nof_frames              = -1;
static srsran_tm_t transmission_mode    = SRSRAN_TM1;
static uint32_t    nof_tb               = 1;
static uint32_t    multiplex_pmi        = 0;
static uint32_t    multiplex_nof_layers = 1;
static uint8_t     mbsfn_sf_mask        = 32;
static int         mbsfn_area_id        = -1;
static char*       rf_args              = "";
static char*       rf_dev               = "";
static float       rf_amp = 0.8, rf_gain = 60.0, rf_freq = 2400000000;
static bool        enable_256qam         = false;
static float       output_file_snr       = +INFINITY;
static bool        use_standard_lte_rate = false;

static bool                    null_file_sink = false;
static srsran_filesink_t       fsink;
static srsran_ofdm_t           ifft[SRSRAN_MAX_PORTS];
static srsran_ofdm_t           ifft_mbsfn;
static srsran_pbch_t           pbch;
static srsran_pcfich_t         pcfich;
static srsran_pdcch_t          pdcch;
static srsran_pdsch_t          pdsch;
static srsran_pdsch_cfg_t      pdsch_cfg;
static srsran_pmch_t           pmch;
static srsran_pmch_cfg_t       pmch_cfg;
static srsran_softbuffer_tx_t* softbuffers[SRSRAN_MAX_CODEWORDS];
static srsran_regs_t           regs;
static srsran_dci_dl_t         dci_dl;
static int                     rvidx[SRSRAN_MAX_CODEWORDS] = {0, 0};

static cf_t *   sf_buffer[SRSRAN_MAX_PORTS] = {NULL}, *output_buffer[SRSRAN_MAX_PORTS] = {NULL};
static uint32_t sf_n_re, sf_n_samples;

static pthread_t          net_thread;
static void*              net_thread_fnc(void* arg);
static sem_t              net_sem;
static bool               net_packet_ready = false;
static srsran_netsource_t net_source;
static srsran_netsink_t   net_sink;

static int prbset_num = 1, last_prbset_num = 1;
static int prbset_orig = 0;
//#define DATA_BUFF_SZ    1024*128
// uint8_t data[8*DATA_BUFF_SZ], data2[DATA_BUFF_SZ];
// uint8_t data_tmp[DATA_BUFF_SZ];

#define DATA_BUFF_SZ 1024 * 1024
static uint8_t *data_mbms, *data[2], data2[DATA_BUFF_SZ];
static uint8_t  data_tmp[DATA_BUFF_SZ];

static void usage(char* prog)
{
  printf("Usage: %s [Iagmfoncvpuxb]\n", prog);
#ifndef DISABLE_RF
  printf("\t-I RF device [Default %s]\n", rf_dev);
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
  printf("\t-x Transmission mode [1-4] [Default %d]\n", transmission_mode + 1);
  printf("\t-b Precoding Matrix Index (multiplex mode only)* [Default %d]\n", multiplex_pmi);
  printf("\t-w Number of codewords/layers (multiplex mode only)* [Default %d]\n", multiplex_nof_layers);
  printf("\t-u listen TCP/UDP port for input data (if mbsfn is active then the stream is over mbsfn only) (-1 is "
         "random) [Default %d]\n",
         net_port);
  printf("\t-v [set srsran_verbose to debug, default none]\n");
  printf("\t-s output file SNR [Default %f]\n", output_file_snr);
  printf("\t-q Enable/Disable 256QAM modulation (default %s)\n", enable_256qam ? "enabled" : "disabled");
  printf("\t-Q Use standard LTE sample rates (default %s)\n", use_standard_lte_rate ? "enabled" : "disabled");
  printf("\n");
  printf("\t*: See 3GPP 36.212 Table  5.3.3.1.5-4 for more information\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "IadglfmoncpqvutxbwMsBQ")) != -1) {
    switch (opt) {
      case 'I':
        rf_dev = argv[optind];
        break;
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'l':
        rf_amp = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'o':
        output_file_name = argv[optind];
        break;
      case 'm':
        mcs_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'u':
        net_port = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'p':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'x':
        transmission_mode = (srsran_tm_t)(strtol(argv[optind], NULL, 10) - 1);
        break;
      case 'b':
        multiplex_pmi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'w':
        multiplex_nof_layers = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'M':
        mbsfn_area_id = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      case 's':
        output_file_snr = strtof(argv[optind], NULL);
        break;
      case 'B':
        mbsfn_sf_mask = (uint8_t)strtol(argv[optind], NULL, 10);
        break;
      case 'q':
        enable_256qam ^= true;
        break;
      case 'Q':
        use_standard_lte_rate ^= true;
        break;
      case 'E':
        cell.cp = SRSRAN_CP_EXT;
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

static void base_init()
{
  int i;

  /* Configure cell and PDSCH in function of the transmission mode */
  switch (transmission_mode) {
    case SRSRAN_TM1:
      cell.nof_ports = 1;
      break;
    case SRSRAN_TM2:
    case SRSRAN_TM3:
    case SRSRAN_TM4:
      cell.nof_ports = 2;
      break;
    default:
      ERROR("Transmission mode %d not implemented or invalid", transmission_mode);
      exit(-1);
  }

  /* Allocate memory */
  for (i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    data[i] = srsran_vec_u8_malloc(SOFTBUFFER_SIZE);
    if (!data[i]) {
      perror("malloc");
      exit(-1);
    }
    bzero(data[i], sizeof(uint8_t) * SOFTBUFFER_SIZE);
  }
  data_mbms = srsran_vec_u8_malloc(SOFTBUFFER_SIZE);

  /* init memory */
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    sf_buffer[i] = srsran_vec_cf_malloc(sf_n_re);
    if (!sf_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    output_buffer[i] = srsran_vec_cf_malloc(sf_n_samples);
    if (!output_buffer[i]) {
      perror("malloc");
      exit(-1);
    }
    srsran_vec_cf_zero(output_buffer[i], sf_n_samples);
  }

  /* open file or USRP */
  if (output_file_name) {
    if (strcmp(output_file_name, "NULL")) {
      if (srsran_filesink_init(&fsink, output_file_name, SRSRAN_COMPLEX_FLOAT_BIN)) {
        ERROR("Error opening file %s", output_file_name);
        exit(-1);
      }
      null_file_sink = false;
    } else {
      null_file_sink = true;
    }
  } else {
#ifndef DISABLE_RF
    printf("Opening RF device...\n");
    if (srsran_rf_open_devname(&radio, rf_dev, rf_args, cell.nof_ports)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
#else
    printf("Error RF not available. Select an output file\n");
    exit(-1);
#endif
  }

  if (net_port > 0) {
    if (srsran_netsource_init(&net_source, "127.0.0.1", net_port, SRSRAN_NETSOURCE_UDP)) {
      ERROR("Error creating input UDP socket at port %d", net_port);
      exit(-1);
    }
    if (null_file_sink) {
      if (srsran_netsink_init(&net_sink, "127.0.0.1", net_port + 1, SRSRAN_NETSINK_TCP)) {
        ERROR("Error sink");
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
    if (srsran_ofdm_tx_init(&ifft[i], cell.cp, sf_buffer[i], output_buffer[i], cell.nof_prb)) {
      ERROR("Error creating iFFT object");
      exit(-1);
    }

    srsran_ofdm_set_normalize(&ifft[i], true);
  }

  if (srsran_ofdm_tx_init_mbsfn(&ifft_mbsfn, SRSRAN_CP_EXT, sf_buffer[0], output_buffer[0], cell.nof_prb)) {
    ERROR("Error creating iFFT object");
    exit(-1);
  }
  srsran_ofdm_set_non_mbsfn_region(&ifft_mbsfn, 2);
  srsran_ofdm_set_normalize(&ifft_mbsfn, true);

  if (srsran_pbch_init(&pbch)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }
  if (srsran_pbch_set_cell(&pbch, cell)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }

  if (srsran_regs_init(&regs, cell)) {
    ERROR("Error initiating regs");
    exit(-1);
  }
  if (srsran_pcfich_init(&pcfich, 1)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }
  if (srsran_pcfich_set_cell(&pcfich, &regs, cell)) {
    ERROR("Error creating PBCH object");
    exit(-1);
  }

  if (srsran_pdcch_init_enb(&pdcch, cell.nof_prb)) {
    ERROR("Error creating PDCCH object");
    exit(-1);
  }
  if (srsran_pdcch_set_cell(&pdcch, &regs, cell)) {
    ERROR("Error creating PDCCH object");
    exit(-1);
  }

  if (srsran_pdsch_init_enb(&pdsch, cell.nof_prb)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }
  if (srsran_pdsch_set_cell(&pdsch, cell)) {
    ERROR("Error creating PDSCH object");
    exit(-1);
  }

  if (mbsfn_area_id > -1) {
    if (srsran_pmch_init(&pmch, cell.nof_prb, 1)) {
      ERROR("Error creating PMCH object");
    }
    srsran_pmch_set_area_id(&pmch, mbsfn_area_id);
  }

  for (i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    softbuffers[i] = calloc(sizeof(srsran_softbuffer_tx_t), 1);
    if (!softbuffers[i]) {
      ERROR("Error allocating soft buffer");
      exit(-1);
    }

    if (srsran_softbuffer_tx_init(softbuffers[i], cell.nof_prb)) {
      ERROR("Error initiating soft buffer");
      exit(-1);
    }
  }
}

static void base_free()
{
  int i;
  for (i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    srsran_softbuffer_tx_free(softbuffers[i]);
    if (softbuffers[i]) {
      free(softbuffers[i]);
    }
  }
  srsran_pdsch_free(&pdsch);
  srsran_pdcch_free(&pdcch);
  srsran_regs_free(&regs);
  srsran_pbch_free(&pbch);
  if (mbsfn_area_id > -1) {
    srsran_pmch_free(&pmch);
  }
  srsran_ofdm_tx_free(&ifft_mbsfn);
  for (i = 0; i < cell.nof_ports; i++) {
    srsran_ofdm_tx_free(&ifft[i]);
  }

  for (i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    if (data[i]) {
      free(data[i]);
    }
  }

  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (sf_buffer[i]) {
      free(sf_buffer[i]);
    }

    if (output_buffer[i]) {
      free(output_buffer[i]);
    }
  }
  if (output_file_name) {
    if (!null_file_sink) {
      srsran_filesink_free(&fsink);
    }
  } else {
#ifndef DISABLE_RF
    srsran_rf_close(&radio);
#endif
  }

  if (net_port > 0) {
    srsran_netsource_free(&net_source);
    sem_close(&net_sem);
  }
}

bool go_exit = false;
#ifndef DISABLE_RF
static void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}
#endif /* DISABLE_RF */

static unsigned int reverse(register unsigned int x)
{
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return ((x >> 16) | (x << 16));
}

static uint32_t prbset_to_bitmask()
{
  uint32_t mask = 0;
  int      nb   = (int)ceilf((float)cell.nof_prb / srsran_ra_type0_P(cell.nof_prb));
  for (int i = 0; i < nb; i++) {
    if (i >= prbset_orig && i < prbset_orig + prbset_num) {
      mask = mask | (0x1 << i);
    }
  }
  return reverse(mask) >> (32 - nb);
}

static int update_radl()
{
  ZERO_OBJECT(dci_dl);

  /* Configure cell and PDSCH in function of the transmission mode */
  switch (transmission_mode) {
    case SRSRAN_TM1:
    case SRSRAN_TM2:
      nof_tb        = 1;
      dci_dl.format = SRSRAN_DCI_FORMAT1;
      break;
    case SRSRAN_TM3:
      dci_dl.format = SRSRAN_DCI_FORMAT2A;
      nof_tb        = 2;
      break;
    case SRSRAN_TM4:
      dci_dl.format = SRSRAN_DCI_FORMAT2;
      nof_tb        = multiplex_nof_layers;
      if (multiplex_nof_layers == 1) {
        dci_dl.pinfo = (uint8_t)(multiplex_pmi + 1);
      } else {
        dci_dl.pinfo = (uint8_t)multiplex_pmi;
      }
      break;
    default:
      ERROR("Transmission mode not implemented.");
      exit(-1);
  }

  dci_dl.rnti                    = UE_CRNTI;
  dci_dl.pid                     = 0;
  dci_dl.tb[0].mcs_idx           = mcs_idx;
  dci_dl.tb[0].ndi               = 0;
  dci_dl.tb[0].rv                = rvidx[0];
  dci_dl.tb[0].cw_idx            = 0;
  dci_dl.alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
  dci_dl.type0_alloc.rbg_bitmask = prbset_to_bitmask();

  if (nof_tb > 1) {
    dci_dl.tb[1].mcs_idx = mcs_idx;
    dci_dl.tb[1].ndi     = 0;
    dci_dl.tb[1].rv      = rvidx[1];
    dci_dl.tb[1].cw_idx  = 1;
  } else {
    SRSRAN_DCI_TB_DISABLE(dci_dl.tb[1]);
  }

  srsran_dci_dl_fprint(stdout, &dci_dl, cell.nof_prb);
  if (transmission_mode != SRSRAN_TM1) {
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
static int update_control()
{
  char input[128];

  fd_set set;
  FD_ZERO(&set);
  FD_SET(0, &set);

  struct timeval to;
  to.tv_sec  = 0;
  to.tv_usec = 0;

  int n = select(1, &set, NULL, NULL, &to);
  if (n == 1) {
    // stdin ready
    if (fgets(input, sizeof(input), stdin)) {
      if (input[0] == 27) {
        switch (input[2]) {
          case RIGHT_KEY:
            if (prbset_orig + prbset_num < (int)ceilf((float)cell.nof_prb / srsran_ra_type0_P(cell.nof_prb)))
              prbset_orig++;
            break;
          case LEFT_KEY:
            if (prbset_orig > 0)
              prbset_orig--;
            break;
          case UP_KEY:
            if (prbset_num < (int)ceilf((float)cell.nof_prb / srsran_ra_type0_P(cell.nof_prb)))
              prbset_num++;
            break;
          case DOWN_KEY:
            last_prbset_num = prbset_num;
            if (prbset_num > 0)
              prbset_num--;
            break;
#ifndef DISABLE_RF
          case PAGE_UP:
            if (!output_file_name) {
              rf_gain++;
              srsran_rf_set_tx_gain(&radio, rf_gain);
              printf("Set TX gain: %.1f dB\n", srsran_rf_get_tx_gain(&radio));
            }
            break;
          case PAGE_DOWN:
            if (!output_file_name) {
              rf_gain--;
              srsran_rf_set_tx_gain(&radio, rf_gain);
              printf("Set TX gain: %.1f dB\n", srsran_rf_get_tx_gain(&radio));
            }
            break;
#endif
        }
      } else {
        switch (input[0]) {
          case 'q':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 0;
            multiplex_nof_layers = 1;
            break;
          case 'w':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 1;
            multiplex_nof_layers = 1;
            break;
          case 'e':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 2;
            multiplex_nof_layers = 1;
            break;
          case 'r':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 3;
            multiplex_nof_layers = 1;
            break;
          case 'a':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 0;
            multiplex_nof_layers = 2;
            break;
          case 's':
            transmission_mode    = SRSRAN_TM4;
            multiplex_pmi        = 1;
            multiplex_nof_layers = 2;
            break;
          case 'z':
            transmission_mode = SRSRAN_TM3;
            break;
          case 'x':
            transmission_mode = SRSRAN_TM2;
            break;
          default:
            last_mcs_idx = mcs_idx;
            mcs_idx      = strtol(input, NULL, 10);
        }
      }
      bzero(input, sizeof(input));
      if (update_radl()) {
        printf("Trying with last known MCS index\n");
        mcs_idx    = last_mcs_idx;
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
static void* net_thread_fnc(void* arg)
{
  int n;
  int rpm = 0, wpm = 0;

  do {
    n = srsran_netsource_read(&net_source, &data2[rpm], DATA_BUFF_SZ - rpm);
    if (n > 0) {
      // TODO: I assume that both transport blocks have same size in case of 2 tb are active

      int nbytes = 1 + (((mbsfn_area_id > -1) ? (pmch_cfg.pdsch_cfg.grant.tb[0].tbs)
                                              : (pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs)) -
                        1) /
                           8;
      rpm += n;
      INFO("received %d bytes. rpm=%d/%d", n, rpm, nbytes);
      wpm = 0;
      while (rpm >= nbytes) {
        // wait for packet to be transmitted
        sem_wait(&net_sem);
        if (mbsfn_area_id > -1) {
          memcpy(data_mbms, &data2[wpm], nbytes);
        } else {
          memcpy(data[0], &data2[wpm], nbytes / (size_t)2);
          memcpy(data[1], &data2[wpm], nbytes / (size_t)2);
        }
        INFO("Sent %d/%d bytes ready", nbytes, rpm);
        rpm -= nbytes;
        wpm += nbytes;
        net_packet_ready = true;
      }
      if (wpm > 0) {
        INFO("%d bytes left in buffer for next packet", rpm);
        memcpy(data2, &data2[wpm], rpm * sizeof(uint8_t));
      }
    } else if (n == 0) {
      rpm = 0;
    } else {
      ERROR("Error receiving from network");
      exit(-1);
    }
  } while (true);
}

int main(int argc, char** argv)
{
  int                   nf = 0, sf_idx = 0, N_id_2 = 0;
  cf_t                  pss_signal[SRSRAN_PSS_LEN];
  float                 sss_signal0[SRSRAN_SSS_LEN]; // for subframe 0
  float                 sss_signal5[SRSRAN_SSS_LEN]; // for subframe 5
  uint8_t               bch_payload[SRSRAN_BCH_PAYLOAD_LEN];
  int                   i;
  cf_t*                 sf_symbols[SRSRAN_MAX_PORTS];
  srsran_dci_msg_t      dci_msg;
  srsran_dci_location_t locations[SRSRAN_NOF_SF_X_FRAME][30];
  uint32_t              sfn;
  srsran_refsignal_t    csr_refs;
  srsran_refsignal_t    mbsfn_refs;

  srsran_debug_handle_crash(argc, argv);

#ifdef DISABLE_RF
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
#endif

  parse_args(argc, argv);

  srsran_use_standard_symbol_size(use_standard_lte_rate);

  uint8_t mch_table[10];
  bzero(&mch_table[0], sizeof(uint8_t) * 10);
  if (mbsfn_area_id > -1) {
    generate_mcch_table(mch_table, mbsfn_sf_mask);
  }
  N_id_2       = cell.id % 3;
  sf_n_re      = SRSRAN_SF_LEN_RE(cell.nof_prb, cell.cp);
  sf_n_samples = 2 * SRSRAN_SLOT_LEN(srsran_symbol_sz(cell.nof_prb));

  cell.phich_length    = SRSRAN_PHICH_NORM;
  cell.phich_resources = SRSRAN_PHICH_R_1;
  sfn                  = 0;

  prbset_num      = (int)ceilf((float)cell.nof_prb / srsran_ra_type0_P(cell.nof_prb));
  last_prbset_num = prbset_num;

  /* this *must* be called after setting slot_len_* */
  base_init();

  /* Generate PSS/SSS signals */
  srsran_pss_generate(pss_signal, N_id_2);
  srsran_sss_generate(sss_signal0, sss_signal5, cell.id);

  /* Generate reference signals */
  if (srsran_refsignal_cs_init(&csr_refs, cell.nof_prb)) {
    ERROR("Error initializing equalizer");
    exit(-1);
  }
  if (mbsfn_area_id > -1) {
    if (srsran_refsignal_mbsfn_init(&mbsfn_refs, cell.nof_prb)) {
      ERROR("Error initializing equalizer");
      exit(-1);
    }
    if (srsran_refsignal_mbsfn_set_cell(&mbsfn_refs, cell, mbsfn_area_id)) {
      ERROR("Error initializing MBSFNR signal");
      exit(-1);
    }
  }

  if (srsran_refsignal_cs_set_cell(&csr_refs, cell)) {
    ERROR("Error setting cell");
    exit(-1);
  }

  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    sf_symbols[i] = sf_buffer[i % cell.nof_ports];
  }

#ifndef DISABLE_RF

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  if (!output_file_name) {
    int srate = srsran_sampling_freq_hz(cell.nof_prb);
    if (srate != -1) {
      printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
      float srate_rf = srsran_rf_set_tx_srate(&radio, (double)srate);
      if (srate_rf != srate) {
        ERROR("Could not set sampling rate");
        exit(-1);
      }
    } else {
      ERROR("Invalid number of PRB %d", cell.nof_prb);
      exit(-1);
    }
    srsran_rf_set_tx_gain(&radio, rf_gain);
    printf("Set TX gain: %.1f dB\n", srsran_rf_get_tx_gain(&radio));
    printf("Set TX freq: %.2f MHz\n", srsran_rf_set_tx_freq(&radio, cell.nof_ports, rf_freq) / 1000000);
  }
#endif

  if (update_radl()) {
    exit(-1);
  }

  if (net_port > 0) {
    if (pthread_create(&net_thread, NULL, net_thread_fnc, NULL)) {
      perror("pthread_create");
      exit(-1);
    }
  }
  pmch_cfg.pdsch_cfg.grant.tb[0].tbs = 1096;

  srsran_dl_sf_cfg_t dl_sf;
  ZERO_OBJECT(dl_sf);

  /* Initiate valid DCI locations */
  for (i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
    dl_sf.cfi = cfi;
    dl_sf.tti = i;
    srsran_pdcch_ue_locations(&pdcch, &dl_sf, locations[i], 30, UE_CRNTI);
  }

  nf = 0;

  bool send_data = false;
  for (i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    srsran_softbuffer_tx_reset(softbuffers[i]);
  }

#ifndef DISABLE_RF
  bool start_of_burst = true;
#endif

  ZERO_OBJECT(pdsch_cfg);
  for (uint32_t j = 0; j < SRSRAN_MAX_CODEWORDS; j++) {
    pdsch_cfg.softbuffers.tx[j] = softbuffers[j];
  }
  pdsch_cfg.rnti = UE_CRNTI;

  pmch_cfg.pdsch_cfg = pdsch_cfg;

  while ((nf < nof_frames || nof_frames == -1) && !go_exit) {
    for (sf_idx = 0; sf_idx < SRSRAN_NOF_SF_X_FRAME && (nf < nof_frames || nof_frames == -1) && !go_exit; sf_idx++) {
      /* Set Antenna port resource elements to zero */
      srsran_vec_cf_zero(sf_symbols[0], sf_n_re);

      if (sf_idx == 0 || sf_idx == 5) {
        srsran_pss_put_slot(pss_signal, sf_symbols[0], cell.nof_prb, cell.cp);
        srsran_sss_put_slot(sf_idx ? sss_signal5 : sss_signal0, sf_symbols[0], cell.nof_prb, cell.cp);
      }

      /* Copy zeros, SSS, PSS into the rest of antenna ports */
      for (i = 1; i < cell.nof_ports; i++) {
        memcpy(sf_symbols[i], sf_symbols[0], sizeof(cf_t) * sf_n_re);
      }

      if (mch_table[sf_idx] == 1 && mbsfn_area_id > -1) {
        srsran_refsignal_mbsfn_put_sf(cell, 0, csr_refs.pilots[0][sf_idx], mbsfn_refs.pilots[0][sf_idx], sf_symbols[0]);
      } else {
        dl_sf.tti = nf * 10 + sf_idx;
        for (i = 0; i < cell.nof_ports; i++) {
          srsran_refsignal_cs_put_sf(&csr_refs, &dl_sf, (uint32_t)i, sf_symbols[i]);
        }
      }

      srsran_pbch_mib_pack(&cell, sfn, bch_payload);
      if (sf_idx == 0) {
        srsran_pbch_encode(&pbch, bch_payload, sf_symbols, nf % 4);
      }

      dl_sf.tti = nf * 10 + sf_idx;
      dl_sf.cfi = cfi;

      srsran_pcfich_encode(&pcfich, &dl_sf, sf_symbols);

      /* Update DL resource allocation from control port */
      if (update_control()) {
        ERROR("Error updating parameters from control port");
      }

      /* Transmit PDCCH + PDSCH only when there is data to send */
      if ((net_port > 0) && (mch_table[sf_idx] == 1 && mbsfn_area_id > -1)) {
        send_data = net_packet_ready;
        if (net_packet_ready) {
          INFO("Transmitting packet from port");
        }
      } else {
        INFO("SF: %d, Generating %d random bits", sf_idx, pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs);
        for (uint32_t tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
          if (pdsch_cfg.grant.tb[tb].enabled) {
            for (i = 0; i < pdsch_cfg.grant.tb[tb].tbs / 8; i++) {
              data[tb][i] = (uint8_t)rand();
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
        if (mch_table[sf_idx] == 0 || mbsfn_area_id < 0) { // PDCCH + PDSCH
          dl_sf.sf_type = SRSRAN_SF_NORM;

          /* Encode PDCCH */
          INFO("Putting DCI to location: n=%d, L=%d", locations[sf_idx][0].ncce, locations[sf_idx][0].L);

          srsran_dci_msg_pack_pdsch(&cell, &dl_sf, NULL, &dci_dl, &dci_msg);
          dci_msg.location = locations[sf_idx][0];
          if (srsran_pdcch_encode(&pdcch, &dl_sf, &dci_msg, sf_symbols)) {
            ERROR("Error encoding DCI message");
            exit(-1);
          }

          /* Configure pdsch_cfg parameters */
          if (srsran_ra_dl_dci_to_grant(&cell, &dl_sf, transmission_mode, enable_256qam, &dci_dl, &pdsch_cfg.grant)) {
            ERROR("Error configuring PDSCH");
            exit(-1);
          }

          /* Encode PDSCH */
          if (srsran_pdsch_encode(&pdsch, &dl_sf, &pdsch_cfg, data, sf_symbols)) {
            ERROR("Error encoding PDSCH");
            exit(-1);
          }
          if (net_port > 0 && net_packet_ready) {
            if (null_file_sink) {
              for (uint32_t tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
                srsran_bit_pack_vector(data[tb], data_tmp, pdsch_cfg.grant.tb[tb].tbs);
                if (srsran_netsink_write(&net_sink, data_tmp, 1 + (pdsch_cfg.grant.tb[tb].tbs - 1) / 8) < 0) {
                  ERROR("Error sending data through UDP socket");
                }
              }
            }
            if (mbsfn_area_id < 0) {
              net_packet_ready = false;
              sem_post(&net_sem);
            }
          }
        } else { // We're sending MCH on subframe 1 - PDCCH + PMCH
          dl_sf.sf_type = SRSRAN_SF_MBSFN;

          /* Force 1 word and MCS 2 */
          dci_dl.rnti                    = SRSRAN_MRNTI;
          dci_dl.alloc_type              = SRSRAN_RA_ALLOC_TYPE0;
          dci_dl.type0_alloc.rbg_bitmask = 0xffffffff;
          dci_dl.tb[0].mcs_idx           = 2;
          dci_dl.format                  = SRSRAN_DCI_FORMAT1;

          /* Configure pdsch_cfg parameters */
          if (srsran_ra_dl_dci_to_grant(&cell, &dl_sf, SRSRAN_TM1, enable_256qam, &dci_dl, &pmch_cfg.pdsch_cfg.grant)) {
            ERROR("Error configuring PDSCH");
            exit(-1);
          }

          for (int j = 0; j < pmch_cfg.pdsch_cfg.grant.tb[0].tbs / 8; j++) {
            data_mbms[j] = j % 255;
          }

          pmch_cfg.area_id = mbsfn_area_id;

          /* Encode PMCH */
          if (srsran_pmch_encode(&pmch, &dl_sf, &pmch_cfg, data_mbms, sf_symbols)) {
            ERROR("Error encoding PDSCH");
            exit(-1);
          }
          if (net_port > 0 && net_packet_ready) {
            if (null_file_sink) {
              srsran_bit_pack_vector(data[0], data_tmp, pmch_cfg.pdsch_cfg.grant.tb[0].tbs);
              if (srsran_netsink_write(&net_sink, data_tmp, 1 + (pmch_cfg.pdsch_cfg.grant.tb[0].tbs - 1) / 8) < 0) {
                ERROR("Error sending data through UDP socket");
              }
            }
            net_packet_ready = false;
            sem_post(&net_sem);
          }
        }
      }

      /* Transform to OFDM symbols */
      if (mch_table[sf_idx] == 0 || mbsfn_area_id < 0) {
        for (i = 0; i < cell.nof_ports; i++) {
          srsran_ofdm_tx_sf(&ifft[i]);
        }
      } else {
        srsran_ofdm_tx_sf(&ifft_mbsfn);
      }

      /* send to file or usrp */
      if (output_file_name) {
        if (!null_file_sink) {
          /* Apply AWGN */
          if (output_file_snr != +INFINITY) {
            float var = srsran_convert_dB_to_amplitude(-(output_file_snr + 3.0f));
            for (int k = 0; k < cell.nof_ports; k++) {
              srsran_ch_awgn_c(output_buffer[k], output_buffer[k], var, sf_n_samples);
            }
          }
          srsran_filesink_write_multi(&fsink, (void**)output_buffer, sf_n_samples, cell.nof_ports);
        }
        usleep(1000);
      } else {
#ifndef DISABLE_RF
        float norm_factor = (float)cell.nof_prb / 15 / sqrtf(pdsch_cfg.grant.nof_prb);
        for (i = 0; i < cell.nof_ports; i++) {
          srsran_vec_sc_prod_cfc(
              output_buffer[i], rf_amp * norm_factor, output_buffer[i], SRSRAN_SF_LEN_PRB(cell.nof_prb));
        }
        srsran_rf_send_multi(&radio, (void**)output_buffer, sf_n_samples, true, start_of_burst, false);
        start_of_burst = false;
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
