/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>

#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/io/filesink.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/sync/npss.h"
#include "srslte/phy/sync/nsss.h"
#include "srslte/phy/ue/ue_dl_nbiot.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/random.h"

#define UE_CRNTI 0x1234

#define HAVE_NPDSCH 1
#define NPDCCH_SF_IDX 1

static const uint8_t dummy_sib1_payload[] = {0x43, 0x4d, 0xd0, 0x92, 0x22, 0x06, 0x04, 0x30, 0x28,
                                             0x6e, 0x87, 0xd0, 0x4b, 0x13, 0x90, 0xb4, 0x12, 0xa1,
                                             0x02, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#ifndef DISABLE_RF
#include "srslte/phy/rf/rf.h"
static srslte_rf_t radio;
#else
#pragma message "Compiling npdsch_ue with no RF support"
#endif

static char* output_file_name = NULL;

static srslte_nbiot_cell_t cell = {
    .base       = {.nof_ports = 1, .nof_prb = SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL, .cp = SRSLTE_CP_NORM, .id = 0},
    .nbiot_prb  = SRSLTE_NBIOT_DEFAULT_PRB_OFFSET,
    .n_id_ncell = 0,
    .nof_ports  = 1,
    .mode       = SRSLTE_NBIOT_MODE_STANDALONE,
    .is_r14     = true};

static uint32_t i_tbs_val = 1, last_i_tbs_val = 1;
static int      nof_frames = -1;
static uint32_t i_sf_val   = 0;
static uint32_t i_rep_val  = 0;

static char* rf_args = "";
static float rf_amp = 0.8, rf_gain = 70.0, rf_freq = 0;
static float file_snr = -100.0;

static bool                     null_file_sink = false;
static srslte_random_t*         random_gen;
static srslte_filesink_t        fsink;
static srslte_ofdm_t            ifft;
static srslte_npss_synch_t      npss_sync;
static srslte_nsss_synch_t      nsss_sync;
static srslte_npbch_t           npbch;
static srslte_npdcch_t          npdcch;
static srslte_npdsch_t          npdsch;
static srslte_npdsch_cfg_t      sib1_npdsch_cfg;
static srslte_npdsch_cfg_t      npdsch_cfg;
static srslte_nbiot_ue_dl_t     ue_dl;
static srslte_softbuffer_tx_t   softbuffer;
static srslte_ra_nbiot_dl_dci_t ra_dl;
static srslte_ra_nbiot_dl_dci_t ra_dl_sib1;
static srslte_chest_dl_nbiot_t  ch_est;
static srslte_mib_nb_t          mib_nb;
static uint32_t                 sched_info_tag =
    0; // according to Table 16.4.1.3-3 in 36.213, 0 means 4 NPDSCH repetitions with TBS 208

static cf_t *sf_buffer = NULL, *output_buffer = NULL;
static int   sf_n_re = 0, sf_n_samples = 0;

void usage(char* prog)
{
  printf("Usage: %s [agmiftlReosncvrpu]\n", prog);
#ifndef DISABLE_RF
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-e RF amplitude [Default %.2f]\n", rf_amp);
  printf("\t-g RF TX gain [Default %.2f dB]\n", rf_gain);
  printf("\t-f RF TX frequency [Default %.1f MHz]\n", rf_freq / 1000000);
#else
  printf("\t   RF is disabled.\n");
#endif
  printf("\t-o output_file [Default use RF board]\n");
  printf("\t-s SNR-10 (only if output to file) [Default %f]\n", file_snr);
  printf("\t-t Value of schedulingInfoSIB1-NB-r13 [Default %d]\n", sched_info_tag);
  printf("\t-m Value of i_tbs (translates to MCS) [Default %d]\n", i_tbs_val);
  printf("\t-i Value of i_sf [Default %d]\n", i_sf_val);
  printf("\t-r Value of i_rep [Default %d]\n", i_rep_val);
  printf("\t-n number of frames [Default %d]\n", nof_frames);
  printf("\t-l n_id_ncell [Default %d]\n", cell.n_id_ncell);
  printf("\t-R Is R14 cell [Default %s]\n", cell.is_r14 ? "Yes" : "No");
  printf("\t-p NB-IoT PRB id [Default %d]\n", cell.nbiot_prb);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "aglfmiosncrtvpuR")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'e':
        rf_amp = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'o':
        output_file_name = argv[optind];
        break;
      case 's':
        file_snr = strtof(argv[optind], NULL);
        break;
      case 't':
        sched_info_tag = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        i_tbs_val = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'i':
        i_sf_val = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        i_rep_val = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        cell.n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        cell.is_r14 = !cell.is_r14;
        break;
      case 'p':
        cell.nbiot_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }

  if (!output_file_name && rf_freq == 0) {
    usage(argv[0]);
    printf("\nError! Either RF frequency or output filename need to be specified.\n");
    exit(-1);
  }

#ifdef DISABLE_RF
  if (!output_file_name) {
    usage(argv[0]);
    exit(-1);
  }
#endif
}

void base_init()
{
  // init memory
  sf_buffer = srslte_vec_cf_malloc(sf_n_re);
  if (!sf_buffer) {
    perror("malloc");
    exit(-1);
  }
  output_buffer = srslte_vec_cf_malloc(sf_n_samples);
  if (!output_buffer) {
    perror("malloc");
    exit(-1);
  }
  // open file or USRP
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
    if (srslte_rf_open(&radio, rf_args)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
#else
    printf("Error RF not available. Select an output file\n");
    exit(-1);
#endif
  }

  if (srslte_ofdm_tx_init(&ifft, SRSLTE_CP_NORM, sf_buffer, output_buffer, cell.base.nof_prb)) {
    fprintf(stderr, "Error creating iFFT object\n");
    exit(-1);
  }
  srslte_ofdm_set_normalize(&ifft, true);
  srslte_ofdm_set_freq_shift(&ifft, -SRSLTE_NBIOT_FREQ_SHIFT_FACTOR);

  if (srslte_npss_synch_init(&npss_sync, sf_n_samples, srslte_symbol_sz(cell.base.nof_prb))) {
    fprintf(stderr, "Error initializing NPSS object\n");
    exit(-1);
  }

  if (srslte_nsss_synch_init(&nsss_sync, sf_n_samples, srslte_symbol_sz(cell.base.nof_prb))) {
    fprintf(stderr, "Error initializing NSSS object\n");
    exit(-1);
  }

  if (srslte_npbch_init(&npbch)) {
    fprintf(stderr, "Error creating NPBCH object\n");
    exit(-1);
  }
  if (srslte_npbch_set_cell(&npbch, cell)) {
    fprintf(stderr, "Error setting cell in NPBCH object\n");
    exit(-1);
  }

  if (srslte_npdcch_init(&npdcch)) {
    fprintf(stderr, "Error creating NPDCCH object\n");
    exit(-1);
  }

  if (srslte_npdcch_set_cell(&npdcch, cell)) {
    fprintf(stderr, "Configuring cell in NPDCCH\n");
    exit(-1);
  }

  if (srslte_npdsch_init(&npdsch)) {
    fprintf(stderr, "Error creating NPDSCH object\n");
    exit(-1);
  }

  if (srslte_npdsch_set_cell(&npdsch, cell)) {
    fprintf(stderr, "Configuring cell in NPDSCH\n");
    exit(-1);
  }
  srslte_npdsch_set_rnti(&npdsch, UE_CRNTI);

  if (srslte_softbuffer_tx_init(&softbuffer, cell.base.nof_prb)) {
    fprintf(stderr, "Error initiating soft buffer\n");
    exit(-1);
  }

  random_gen = srslte_random_init(time(NULL));
}

void base_free()
{
  srslte_random_free(random_gen);
  srslte_softbuffer_tx_free(&softbuffer);
  srslte_npdsch_free(&npdsch);
  srslte_npdcch_free(&npdcch);
  srslte_npbch_free(&npbch);
  srslte_chest_dl_nbiot_free(&ch_est);
  srslte_npss_synch_free(&npss_sync);
  srslte_nsss_synch_free(&nsss_sync);
  srslte_ofdm_tx_free(&ifft);

  if (sf_buffer) {
    free(sf_buffer);
  }
  if (output_buffer) {
    free(output_buffer);
  }
  if (output_file_name) {
    if (!null_file_sink) {
      srslte_filesink_free(&fsink);
    }
  } else {
#ifndef DISABLE_RF
    srslte_rf_close(&radio);
#endif
  }
}

bool go_exit = false;
#ifndef DISABLE_RF
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}
#endif

static int update_radl(void)
{
  bzero(&ra_dl_sib1, sizeof(srslte_ra_nbiot_dl_dci_t));

  // NB-IoT specific fields
  ra_dl_sib1.alloc.has_sib1        = true;
  ra_dl_sib1.alloc.sched_info_sib1 = mib_nb.sched_info_sib1;
  ra_dl_sib1.mcs_idx               = 1;

  bzero(&ra_dl, sizeof(srslte_ra_nbiot_dl_dci_t));
  ra_dl.mcs_idx = i_tbs_val;

  // NB-IoT specific fields
  ra_dl.format          = 1; // FormatN1 DCI
  ra_dl.alloc.has_sib1  = false;
  ra_dl.alloc.is_ra     = false;
  ra_dl.alloc.i_delay   = 0;
  ra_dl.alloc.i_sf      = i_sf_val;
  ra_dl.alloc.i_rep     = i_rep_val;
  ra_dl.alloc.harq_ack  = 1;
  ra_dl.alloc.i_n_start = 0;

  srslte_nbiot_dl_dci_fprint(stdout, &ra_dl);
  srslte_ra_nbiot_dl_grant_t dummy_grant;
  srslte_ra_nbits_t          dummy_nbits;

#define DUMMY_SFIDX 1
#define DUMMY_SFN 0
  srslte_ra_nbiot_dl_dci_to_grant(&ra_dl, &dummy_grant, DUMMY_SFN, DUMMY_SFIDX, DUMMY_R_MAX, false, cell.mode);
  srslte_ra_nbiot_dl_grant_to_nbits(&dummy_grant, cell, 0, &dummy_nbits);
  srslte_ra_nbiot_dl_grant_fprint(stdout, &dummy_grant);
  printf("Type new MCS index and press Enter: ");
  fflush(stdout);

  return SRSLTE_SUCCESS;
}

/* Read new MCS from stdin */
static int update_control(void)
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
      last_i_tbs_val = i_tbs_val;
      i_tbs_val      = atoi(input);
      bzero(input, sizeof(input));
      if (update_radl()) {
        printf("Trying with last known MCS index\n");
        i_tbs_val = last_i_tbs_val;
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

#define DATA_BUFF_SZ 1024 * 128
uint8_t data[8 * DATA_BUFF_SZ] = {};
uint8_t data2[DATA_BUFF_SZ]    = {};
uint8_t data_tmp[DATA_BUFF_SZ] = {};

int main(int argc, char** argv)
{
  int  nf = 0, sf_idx = 0;
  cf_t npss_signal[SRSLTE_NPSS_TOT_LEN];
  cf_t nsss_signal[SRSLTE_NSSS_LEN * SRSLTE_NSSS_NUM_SEQ]; // for subframe 9

  uint8_t bch_payload[SRSLTE_MIB_NB_LEN];
  uint8_t sib1_nb_payload[SRSLTE_NPDSCH_MAX_TBS];

  srslte_dci_msg_t      dci_msg;
  srslte_dci_location_t locations[SRSLTE_NOF_SF_X_FRAME][30];

  uint32_t sfn = 0;
  uint32_t hfn = 0; // Hyper frame number is incremented when sfn wraps and is also 10bit wide

#ifdef DISABLE_RF
  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
#endif

  parse_args(argc, argv);

  // adjust SNR input to allow for negative values
  if (output_file_name && file_snr != -100.0) {
    file_snr -= 10.0;
    printf("Target SNR: %.2fdB\n", file_snr);
  }

  sf_n_re      = 2 * SRSLTE_CP_NORM_NSYMB * cell.base.nof_prb * SRSLTE_NRE;
  sf_n_samples = 2 * SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.base.nof_prb));

  /* this *must* be called after setting slot_len_* */
  base_init();

  // buffer for outputting signal in RE representation (using sf_n_re)
  cf_t* sf_re_symbols[SRSLTE_MAX_PORTS] = {NULL, NULL, NULL, NULL};
  sf_re_symbols[0]                      = sf_buffer;

  // buffer for outputting actual IQ samples (using sf_n_samples)
  cf_t* sf_symbols[SRSLTE_MAX_PORTS] = {NULL, NULL, NULL, NULL};
  sf_symbols[0]                      = output_buffer;

  // construct MIB-NB
  mib_nb.sched_info_sib1 = sched_info_tag;
  mib_nb.sys_info_tag    = 0;
  mib_nb.ac_barring      = false;
  mib_nb.mode            = SRSLTE_NBIOT_MODE_STANDALONE;

  // Initialize UE DL
  if (srslte_nbiot_ue_dl_init(&ue_dl, sf_symbols, SRSLTE_NBIOT_MAX_PRB, SRSLTE_NBIOT_NUM_RX_ANTENNAS)) {
    fprintf(stderr, "Error initiating UE downlink processing module\n");
    exit(-1);
  }

  if (srslte_nbiot_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Setting cell in UE DL\n");
    return -1;
  }

  srslte_nbiot_ue_dl_set_mib(&ue_dl, mib_nb);

  /* Generate NPSS/NSSS signals */
  srslte_npss_generate(npss_signal);
  srslte_nsss_generate(nsss_signal, cell.n_id_ncell);

#ifdef NPSS_DUMP
  srslte_filesink_t debug_fsink;
  char              fname[] = "npss.bin";
  if (srslte_filesink_init(&debug_fsink, fname, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", fname);
    exit(-1);
  }
  srslte_filesink_write(&debug_fsink, npss_signal, SRSLTE_NPSS_LEN * 11);
  srslte_filesink_free(&debug_fsink);
#endif

  /* Generate CRS+NRS signals */
  if (srslte_chest_dl_nbiot_init(&ch_est, SRSLTE_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initializing equalizer\n");
    exit(-1);
  }
  if (srslte_chest_dl_nbiot_set_cell(&ch_est, cell) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error setting channel estimator's cell configuration\n");
    return -1;
  }

#ifndef DISABLE_RF
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  if (!output_file_name) {

    int srate = srslte_sampling_freq_hz(cell.base.nof_prb);
    if (srate != -1) {
      printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
      float srate_rf = srslte_rf_set_tx_srate(&radio, (double)srate);
      if (srate_rf != srate) {
        fprintf(stderr, "Could not set sampling rate\n");
        exit(-1);
      }
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell.base.nof_prb);
      exit(-1);
    }
    printf("Set TX gain: %.1f dB\n", srslte_rf_set_tx_gain(&radio, rf_gain));
    printf("Set TX freq: %.2f MHz\n", srslte_rf_set_tx_freq(&radio, 0, rf_freq) / 1000000);
  }
#endif

  if (update_radl()) {
    exit(-1);
  }

  /* Initiate valid DCI locations */
  for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
    locations[i][0].L    = 1; // Agg-level 2, i.e. both NCEEs used
    locations[i][0].ncce = 0;
  }

  nf = 0;

  bool send_data     = false;
  bool npdsch_active = false;
  srslte_softbuffer_tx_reset(&softbuffer);
  bzero(&sib1_npdsch_cfg, sizeof(srslte_npdsch_cfg_t));
  bzero(&npdsch_cfg, sizeof(srslte_npdsch_cfg_t));

#ifndef DISABLE_RF
  bool start_of_burst = true;
#endif

  while ((nf < nof_frames || nof_frames == -1) && !go_exit) {
    for (sf_idx = 0; sf_idx < SRSLTE_NOF_SF_X_FRAME && (nf < nof_frames || nof_frames == -1); sf_idx++) {
      srslte_vec_cf_zero(sf_buffer, sf_n_re);

      // Transmit NPBCH in subframe 0
      if (sf_idx == 0) {
        if ((sfn % SRSLTE_NPBCH_NUM_FRAMES) == 0) {
          srslte_npbch_mib_pack(hfn, sfn, mib_nb, bch_payload);
        }
        srslte_npbch_put_subframe(&npbch, bch_payload, sf_re_symbols, sfn);
        if (SRSLTE_VERBOSE_ISDEBUG()) {
          printf("MIB payload: ");
          srslte_vec_fprint_hex(stdout, bch_payload, SRSLTE_MIB_NB_LEN);
        }
      }

      // Transmit NPSS, NSSS and NRS
      if (sf_idx == 5) {
        // NPSS at subframe 5
        srslte_npss_put_subframe(&npss_sync, npss_signal, sf_buffer, cell.base.nof_prb, cell.nbiot_prb);
      } else if ((sfn % 2 == 0) && sf_idx == 9) {
        // NSSS in every even numbered frame at subframe 9
        srslte_nsss_put_subframe(&nsss_sync, nsss_signal, sf_buffer, sfn, cell.base.nof_prb, cell.nbiot_prb);
      } else {
        // NRS in all other subframes (using CSR signal intentionally)
        // DEBUG("%d.%d: Putting %d NRS pilots\n", sfn, sf_idx, SRSLTE_REFSIGNAL_NUM_SF(1, cell.nof_ports));
        srslte_refsignal_nrs_put_sf(cell, 0, ch_est.nrs_signal.pilots[0][sf_idx], sf_buffer);
      }

#if HAVE_NPDSCH
      // only transmit in subframes not used for NPBCH, NPSS or NSSS and only use subframe 4 when there is no SIB1
      // transmission
      if (sf_idx != 0 && sf_idx != 5 && (!(sf_idx == 9 && sfn % 2 == 0)) &&
          !srslte_nbiot_ue_dl_is_sib1_sf(&ue_dl, sfn, sf_idx)) {
        send_data = true;
      } else {
        send_data = false;
      }

      // SIB1-NB content
      if (hfn % 4 == 0 && sfn == 0 && sf_idx == 0) {
        // copy captured SIB1
        memcpy(sib1_nb_payload, dummy_sib1_payload, sizeof(dummy_sib1_payload));

        // overwrite Hyper Frame Number (HFN), 8 MSB
        uint8_t unpacked_hfn[4 * 8];
        srslte_bit_unpack_vector(sib1_nb_payload, unpacked_hfn, 4 * 8);
        uint8_t* tmp = unpacked_hfn;
        tmp += 12;
        srslte_bit_unpack(hfn >> 2, &tmp, 8);
        uint8_t packed_hfn[4];
        srslte_bit_pack_vector(unpacked_hfn, packed_hfn, 32);
        memcpy(sib1_nb_payload, packed_hfn, sizeof(packed_hfn));

        if (SRSLTE_VERBOSE_ISDEBUG()) {
          printf("SIB1-NB payload: ");
          srslte_vec_fprint_byte(stdout, sib1_nb_payload, sizeof(dummy_sib1_payload));
        }
      }

      if (srslte_nbiot_ue_dl_is_sib1_sf(&ue_dl, sfn, sf_idx)) {
        INFO("%d.%d: Transmitting SIB1-NB.\n", sfn, sf_idx);
        assert(send_data == false);

        // configure DL grant for SIB1-NB transmission
        if (sib1_npdsch_cfg.sf_idx == 0) {
          srslte_ra_nbiot_dl_grant_t grant;
          srslte_ra_nbiot_dl_dci_to_grant(
              &ra_dl_sib1, &grant, sfn, sf_idx, DUMMY_R_MAX, true, cell.mode);
          if (srslte_npdsch_cfg(&sib1_npdsch_cfg, cell, &grant, sf_idx)) {
            fprintf(stderr, "Error configuring NPDSCH\n");
            exit(-1);
          }
        }

        // Encode SIB1 content
        if (srslte_npdsch_encode_rnti(
                &npdsch, &sib1_npdsch_cfg, &softbuffer, sib1_nb_payload, SRSLTE_SIRNTI, sf_re_symbols)) {
          fprintf(stderr, "Error encoding NPDSCH\n");
          exit(-1);
        }

        if (sib1_npdsch_cfg.sf_idx == sib1_npdsch_cfg.grant.nof_sf) {
          bzero(&sib1_npdsch_cfg, sizeof(srslte_npdsch_cfg_t));
        }
      }

      // Update DL resource allocation from control port
      if (update_control()) {
        fprintf(stderr, "Error updating parameters from control port\n");
      }

      if (send_data) {
        // always transmit NPDCCH on fixed positions if no transmission is going on
        if (sf_idx == NPDCCH_SF_IDX && !npdsch_active) {
          // Encode NPDCCH
          INFO("Putting DCI to location: n=%d, L=%d\n", locations[sf_idx][0].ncce, locations[sf_idx][0].L);
          srslte_dci_msg_pack_npdsch(&ra_dl, SRSLTE_DCI_FORMATN1, &dci_msg, false);
          if (srslte_npdcch_encode(&npdcch, &dci_msg, locations[sf_idx][0], UE_CRNTI, sf_re_symbols, sf_idx)) {
            fprintf(stderr, "Error encoding DCI message\n");
            exit(-1);
          }

          // Configure NPDSCH accordingly
          srslte_ra_nbiot_dl_grant_t grant;
          srslte_ra_nbiot_dl_dci_to_grant(&ra_dl, &grant, sfn, sf_idx, DUMMY_R_MAX, false, cell.mode);
          if (srslte_npdsch_cfg(&npdsch_cfg, cell, &grant, sf_idx)) {
            fprintf(stderr, "Error configuring NPDSCH\n");
            exit(-1);
          }
        }

        // catch start of "user" NPDSCH
        if (!npdsch_active && (sf_idx == npdsch_cfg.grant.start_sfidx && sfn == npdsch_cfg.grant.start_sfn)) {
          // generate data only in first sf
          INFO("%d.%d: Generating %d random bits\n", sfn, sf_idx, npdsch_cfg.grant.mcs[0].tbs);
          for (int i = 0; i < npdsch_cfg.grant.mcs[0].tbs / 8; i++) {
            data[i] = srslte_random_uniform_int_dist(random_gen, 0, 255);
          }
          if (SRSLTE_VERBOSE_ISDEBUG()) {
            printf("Tx payload: ");
            srslte_vec_fprint_b(stdout, data, npdsch_cfg.grant.mcs[0].tbs / 8);
          }
          npdsch_active = true;
        }

        if (npdsch_active) {
          DEBUG("Current sf_idx=%d, Encoding npdsch.sf_idx=%d start=%d, nof=%d\n",
                sf_idx,
                npdsch_cfg.sf_idx,
                npdsch_cfg.grant.start_sfidx,
                npdsch_cfg.grant.nof_sf);
          // Encode NPDSCH
          if (srslte_npdsch_encode(&npdsch, &npdsch_cfg, &softbuffer, data, sf_re_symbols)) {
            fprintf(stderr, "Error encoding NPDSCH\n");
            exit(-1);
          }
          if (npdsch_cfg.num_sf == npdsch_cfg.grant.nof_sf * npdsch_cfg.grant.nof_rep) {
            INFO("Deactive current NPDSCH\n");
            npdsch_active = false;
          }
        }
      }
#endif

      /* Transform to OFDM symbols */
      srslte_ofdm_tx_sf(&ifft);

      if (output_file_name && file_snr != -100.0) {
        // compute average energy per symbol
        float abs_avg = srslte_vec_avg_power_cf(output_buffer, sf_n_samples);

        // find the noise spectral density
        float snr_lin = srslte_convert_dB_to_power(file_snr);
        float n0      = abs_avg / snr_lin;
        float nstd    = sqrtf(n0 / 2);

        // add some noise to the signal
        srslte_ch_awgn_c(output_buffer, output_buffer, nstd, sf_n_samples);
      }

      /* send to file or usrp */
      if (output_file_name) {
        // write to file
        if (!null_file_sink) {
          srslte_filesink_write(&fsink, output_buffer, sf_n_samples);
        }
        usleep(1000);
      } else {
#ifndef DISABLE_RF
        // TODO: output scaling needed?
        srslte_rf_send2(&radio, output_buffer, sf_n_samples, true, start_of_burst, false);
        start_of_burst = false;
#endif
      }
    }
    nf++;
    sfn++;
    if (sfn >= 1024) {
      sfn = 0;
      hfn = (hfn + 1) % 1024;
      printf("NB-IoT HFN: %d\n", hfn);
    }
  }

  base_free();

  printf("Done\n");

  return SRSLTE_SUCCESS;
}
