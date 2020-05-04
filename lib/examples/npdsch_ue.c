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
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

#include "npdsch_ue_helper.h"
#include "srslte/phy/ch_estimation/chest_dl_nbiot.h"
#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/io/filesink.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/ue/ue_dl_nbiot.h"
#include "srslte/phy/ue/ue_mib_nbiot.h"
#include "srslte/phy/ue/ue_sync_nbiot.h"
#include "srslte/phy/utils/bit.h"

#undef ENABLE_AGC_DEFAULT

#ifndef DISABLE_RF
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/rf/rf_utils.h"

#define ENABLE_MANUAL_NSSS_SEARCH 0
#define HAVE_PCAP 1

#if HAVE_PCAP
#include "srslte/common/pcap.h"
#endif

cell_search_cfg_t cell_detect_config = {.max_frames_pbch      = SRSLTE_DEFAULT_MAX_FRAMES_NPBCH,
                                        .max_frames_pss       = SRSLTE_DEFAULT_MAX_FRAMES_NPSS,
                                        .nof_valid_pss_frames = SRSLTE_DEFAULT_NOF_VALID_NPSS_FRAMES,
                                        .init_agc             = 0,
                                        .force_tdd            = false};

#else
#pragma message "Compiling npdsch_ue with no RF support"
#endif

#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
void      init_plots();
pthread_t plot_thread;
sem_t     plot_sem;
uint32_t  plot_sf_idx = 0;
bool      plot_track  = true;
#define HAVE_RSRP_PLOT 0
#endif // ENABLE_GUI

#define PRINT_CHANGE_SCHEDULIGN
#define NPSS_FIND_PLOT_WIDTH 80
//#define CORRECT_SAMPLE_OFFSET

static srslte_nbiot_si_params_t sib2_params;

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
typedef struct {
  int      nof_subframes;
  bool     disable_plots;
  bool     disable_plots_except_constellation;
  bool     disable_cfo;
  uint32_t time_offset;
  int      n_id_ncell;
  bool     is_r14;
  bool     skip_sib2;
  uint16_t rnti;
  char*    input_file_name;
  int      file_offset_time;
  float    file_offset_freq;
  uint32_t file_nof_prb;
  uint32_t file_nof_ports;
  uint32_t file_cell_id;
  char*    rf_args;
  double   rf_freq;
  float    rf_gain;
} prog_args_t;

void args_default(prog_args_t* args)
{
  args->disable_plots                      = false;
  args->disable_plots_except_constellation = false;
  args->nof_subframes                      = -1;
  args->rnti                               = SRSLTE_SIRNTI;
  args->n_id_ncell                         = SRSLTE_CELL_ID_UNKNOWN;
  args->is_r14                             = true;
  args->input_file_name                    = NULL;
  args->disable_cfo                        = false;
  args->time_offset                        = 0;
  args->file_nof_prb                       = 1;
  args->file_nof_ports                     = 0;
  args->file_cell_id                       = 0;
  args->file_offset_time                   = 0;
  args->file_offset_freq                   = 0;
  args->rf_args                            = "";
  args->rf_freq                            = -1.0;
#ifdef ENABLE_AGC_DEFAULT
  args->rf_gain = -1.0;
#else
  args->rf_gain = 70.0;
#endif
}

void usage(prog_args_t* args, char* prog)
{
  printf("Usage: %s [agpRPoOildtDnrBuHvqwzxc] -f rx_frequency (in Hz) | -i input_file\n", prog);
#ifndef DISABLE_RF
  printf("\t-a RF args [Default %s]\n", args->rf_args);
#ifdef ENABLE_AGC_DEFAULT
  printf("\t-g RF fix RX gain [Default AGC]\n");
#else
  printf("\t-g Set RX gain [Default %.1f dB]\n", args->rf_gain);
#endif
#else
  printf("\t   RF is disabled.\n");
#endif
  printf("\t-i input_file [Default use RF board]\n");
  printf("\t-o offset frequency correction (in Hz) for input file [Default %.1f Hz]\n", args->file_offset_freq);
  printf("\t-O offset samples for input file [Default %d]\n", args->file_offset_time);
  printf("\t-p nof_prb for input file [Default %d]\n", args->file_nof_prb);
  printf("\t-P nof_ports for input file [Default %d]\n", args->file_nof_ports);
  printf("\t-r RNTI in Hex [Default 0x%x]\n", args->rnti);
  printf("\t-l n_id_ncell [Default %d]\n", args->n_id_ncell);
  printf("\t-R Is R14 cell [Default %s]\n", args->is_r14 ? "Yes" : "No");
  printf("\t-C Disable CFO correction [Default %s]\n", args->disable_cfo ? "Disabled" : "Enabled");
  printf("\t-t Add time offset [Default %d]\n", args->time_offset);
  printf("\t-s Skip SIB2 decoding[Default %d]\n", args->skip_sib2);
#ifdef ENABLE_GUI
  printf("\t-d disable plots [Default enabled]\n");
  printf("\t-D disable all but constellation plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif // ENABLE_GUI
  printf("\t-n nof_subframes [Default %d]\n", args->nof_subframes);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(prog_args_t* args, int argc, char** argv)
{
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "aogRBlipHPOCtdDsnvrfqwzxc")) != -1) {
    switch (opt) {
      case 'i':
        args->input_file_name = argv[optind];
        break;
      case 'p':
        args->file_nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'P':
        args->file_nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        args->file_offset_freq = strtof(argv[optind], NULL);
        break;
      case 'O':
        args->file_offset_time = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'a':
        args->rf_args = argv[optind];
        break;
      case 'g':
        args->rf_gain = strtof(argv[optind], NULL);
        break;
      case 'C':
        args->disable_cfo = true;
        break;
      case 't':
        args->time_offset = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        args->rf_freq = strtod(argv[optind], NULL);
        break;
      case 'n':
        args->nof_subframes = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        args->rnti = strtol(argv[optind], NULL, 16);
        break;
      case 'l':
        args->n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        args->is_r14 = !args->is_r14;
        break;
      case 'd':
        args->disable_plots = true;
        break;
      case 's':
        args->skip_sib2 = !args->skip_sib2;
        break;
      case 'D':
        args->disable_plots_except_constellation = true;
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(args, argv[0]);
        exit(-1);
    }
  }
  if (args->rf_freq < 0 && args->input_file_name == NULL) {
    usage(args, argv[0]);
    exit(-1);
  }
}
/**********************************************************************/

static uint8_t rx_tb[SRSLTE_MAX_DL_BITS_CAT_NB1]; // Byte buffer for rx'ed transport blocks

bool go_exit = false;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

#if HAVE_PCAP
void pcap_pack_and_write(FILE*    pcap_file,
                         uint8_t* pdu,
                         uint32_t pdu_len_bytes,
                         uint8_t  reTX,
                         bool     crc_ok,
                         uint32_t tti,
                         uint16_t crnti,
                         uint8_t  direction,
                         uint8_t  rnti_type)
{
  MAC_Context_Info_t context = {.radioType      = FDD_RADIO,
                                .direction      = direction,
                                .rntiType       = rnti_type,
                                .rnti           = crnti,
                                .ueid           = 1,
                                .isRetx         = reTX,
                                .crcStatusOK    = crc_ok,
                                .sysFrameNumber = (uint16_t)(tti / SRSLTE_NOF_SF_X_FRAME),
                                .subFrameNumber = (uint16_t)(tti % SRSLTE_NOF_SF_X_FRAME),
                                .nbiotMode      = 1};
  if (pdu) {
    LTE_PCAP_MAC_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
  }
}
#endif

#ifndef DISABLE_RF
int srslte_rf_recv_wrapper(void* h, void* data, uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return srslte_rf_recv_with_time(h, data, nsamples, true, &t->full_secs, &t->frac_secs);
}

void srslte_rf_set_rx_gain_th_wrapper_(void* h, float f)
{
  srslte_rf_set_rx_gain_th((srslte_rf_t*)h, f);
}

#endif

enum receiver_state { DECODE_MIB, DECODE_SIB, DECODE_NPDSCH } state;

static srslte_nbiot_cell_t cell = {};

srslte_nbiot_ue_dl_t   ue_dl;
srslte_nbiot_ue_sync_t ue_sync;
prog_args_t            prog_args;

bool have_sib1 = false;
bool have_sib2 = false;

#ifdef ENABLE_GUI
#define MAX_MSG_BUF (8192)
static char mib_buffer_disp[MAX_MSG_BUF], mib_buffer_decode[MAX_MSG_BUF];
static char sib1_buffer_disp[MAX_MSG_BUF], sib1_buffer_decode[MAX_MSG_BUF];
static char sib2_buffer_disp[MAX_MSG_BUF], sib2_buffer_decode[MAX_MSG_BUF];

#if HAVE_RSRP_PLOT
#define RSRP_TABLE_MAX_IDX 1024
static float    rsrp_table[RSRP_TABLE_MAX_IDX];
static uint32_t rsrp_table_index = 0;
static uint32_t rsrp_num_plot    = RSRP_TABLE_MAX_IDX;
#endif // HAVE_RSRP_PLOT
#endif // ENABLE_GUI

static uint32_t system_frame_number = 0;
static uint32_t hyper_frame_number  = 0;

int main(int argc, char** argv)
{
  int                   ret;
  int64_t               sf_cnt;
  srslte_ue_mib_nbiot_t ue_mib;
#ifndef DISABLE_RF
  srslte_rf_t rf;
#endif
  uint32_t nof_trials = 0;
  int      n;
  uint8_t  bch_payload[SRSLTE_MIB_NB_LEN] = {};
  int      sfn_offset;
  float    cfo = 0;

  parse_args(&prog_args, argc, argv);

#if HAVE_PCAP
  FILE* pcap_file = LTE_PCAP_Open(MAC_LTE_DLT, "/tmp/npdsch.pcap");
#endif

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
  signal(SIGINT, sig_int_handler);

  cell.base.nof_prb = SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL;
  cell.nbiot_prb    = SRSLTE_NBIOT_DEFAULT_PRB_OFFSET;
  cell.n_id_ncell   = prog_args.n_id_ncell;
  cell.is_r14       = prog_args.is_r14;

#ifndef DISABLE_RF
  if (!prog_args.input_file_name) {

    printf("Opening RF device...\n");
    if (srslte_rf_open(&rf, prog_args.rf_args)) {
      fprintf(stderr, "Error opening rf\n");
      exit(-1);
    }
    /* Set receiver gain */
    if (prog_args.rf_gain > 0) {
      printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, prog_args.rf_gain));
    } else {
      printf("Starting AGC thread...\n");
      if (srslte_rf_start_gain_thread(&rf, false)) {
        fprintf(stderr, "Error opening rf\n");
        exit(-1);
      }
      srslte_rf_set_rx_gain(&rf, 50);
      cell_detect_config.init_agc = 50;
    }

    // set transceiver frequency
    printf("Set RX freq: %.6f MHz\n", srslte_rf_set_rx_freq(&rf, 0, prog_args.rf_freq) / 1000000);

    // set sampling frequency
    int srate = srslte_sampling_freq_hz(cell.base.nof_prb);
    if (srate != -1) {
      printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
      float srate_rf = srslte_rf_set_rx_srate(&rf, (double)srate);
      if (srate_rf != srate) {
        fprintf(stderr, "Could not set sampling rate\n");
        exit(-1);
      }
    } else {
      fprintf(stderr, "Invalid number of PRB %d\n", cell.base.nof_prb);
      exit(-1);
    }

    INFO("Stopping RF and flushing buffer...\r");
    srslte_rf_stop_rx_stream(&rf);

#if ENABLE_MANUAL_NSSS_SEARCH
    // determine n_id_ncell
    if (prog_args.n_id_ncell == SRSLTE_CELL_ID_UNKNOWN) {
      srslte_nsss_synch_t nsss;
      float               nsss_peak_value;
      int                 input_len = srate * 10 / 1000 * 2; // capture two full frames to make sure we have one NSSS

      cf_t* buffer = srslte_vec_cf_malloc(input_len * 2);
      if (!buffer) {
        perror("malloc");
        exit(-1);
      }

      if (srslte_nsss_synch_init(&nsss, input_len, srate / 15000)) {
        fprintf(stderr, "Error initializing NSSS object\n");
        exit(-1);
      }

      srslte_rf_start_rx_stream(&rf, false);
      n = srslte_rf_recv(&rf, buffer, input_len, 1);
      if (n != input_len) {
        fprintf(stderr, "Error receiving samples\n");
        exit(-1);
      }
      srslte_rf_stop_rx_stream(&rf);

      // trying to find NSSS
      printf("Detecting NSSS signal .. ");
      fflush(stdout);
      uint32_t sfn_partial;
      srslte_nsss_sync_find(&nsss, buffer, &nsss_peak_value, (uint32_t*)&cell.n_id_ncell, &sfn_partial);
      printf("done!");
      srslte_nsss_synch_free(&nsss);
      free(buffer);
    } else {
      cell.n_id_ncell = prog_args.n_id_ncell;
    }
    printf("\nSetting n_id_ncell to %d.\n", cell.n_id_ncell);
#else
    if (cell.n_id_ncell == SRSLTE_CELL_ID_UNKNOWN) {
      uint32_t ntrial = 0;
      do {
        ret = rf_cell_search_nbiot(&rf, &cell_detect_config, &cell, &cfo);
        if (ret != SRSLTE_SUCCESS) {
          printf("Cell not found after %d trials. Trying again (Press Ctrl+C to exit)\n", ntrial++);
        }
      } while (ret != SRSLTE_SUCCESS && !go_exit);
    }
#endif

    if (go_exit) {
      exit(0);
    }
  }
#endif

  /* If reading from file, go straight to PDSCH decoding. Otherwise, decode MIB first */
  if (prog_args.input_file_name) {
    // set file specific params
    cell.base.nof_ports = prog_args.file_nof_ports;
    cell.nof_ports      = prog_args.file_nof_ports;

    if (srslte_ue_sync_nbiot_init_file(
            &ue_sync, cell, prog_args.input_file_name, prog_args.file_offset_time, prog_args.file_offset_freq)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      exit(-1);
    }
  } else {
#ifndef DISABLE_RF
    if (srslte_ue_sync_nbiot_init(&ue_sync, cell, srslte_rf_recv_wrapper, (void*)&rf)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      exit(-1);
    }
    // reduce AGC period to every 10th frame
    srslte_ue_sync_nbiot_set_agc_period(&ue_sync, 10);
#endif
  }

  // Allocate memory to fit a full frame (needed for time re-alignment)
  cf_t* buff_ptrs[SRSLTE_MAX_PORTS] = {NULL};
  buff_ptrs[0]                      = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB_NBIOT * 10);

  if (srslte_ue_mib_nbiot_init(&ue_mib, buff_ptrs, SRSLTE_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initaiting UE MIB decoder\n");
    exit(-1);
  }
  if (srslte_ue_mib_nbiot_set_cell(&ue_mib, cell) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error setting cell configuration in UE MIB decoder\n");
    exit(-1);
  }

  // Initialize subframe counter
  sf_cnt = 0;

#ifdef ENABLE_GUI
  if (!prog_args.disable_plots) {
    init_plots();
  }
#endif // ENABLE_GUI

#ifndef DISABLE_RF
  if (!prog_args.input_file_name) {
    srslte_rf_start_rx_stream(&rf, false);
  }
#endif

  // Variables for measurements
  uint32_t nframes = 0;
  float    rsrp = 0.0, rsrq = 0.0, noise = 0.0;

#ifndef DISABLE_RF
  if (prog_args.rf_gain < 0) {
    srslte_ue_sync_nbiot_start_agc(&ue_sync, srslte_rf_set_rx_gain_th_wrapper_, cell_detect_config.init_agc);
  }
#endif
#ifdef PRINT_CHANGE_SCHEDULIGN
  srslte_ra_nbiot_dl_dci_t old_dl_dci;
  bzero(&old_dl_dci, sizeof(srslte_ra_nbiot_dl_dci_t));
#endif

  ue_sync.correct_cfo = !prog_args.disable_cfo;

  // Set initial CFO for ue_sync
  srslte_ue_sync_nbiot_set_cfo(&ue_sync, cfo);

  srslte_npbch_decode_reset(&ue_mib.npbch);

  INFO("\nEntering main loop...\n\n");
  while (!go_exit && (sf_cnt < prog_args.nof_subframes || prog_args.nof_subframes == -1)) {

    ret = srslte_ue_sync_nbiot_zerocopy_multi(&ue_sync, buff_ptrs);
    if (ret < 0) {
      fprintf(stderr, "Error calling srslte_nbiot_ue_sync_zerocopy_multi()\n");
      break;
    }

#ifdef CORRECT_SAMPLE_OFFSET
    float sample_offset =
        (float)srslte_ue_sync_get_last_sample_offset(&ue_sync) + srslte_ue_sync_get_sfo(&ue_sync) / 1000;
    srslte_ue_dl_set_sample_offset(&ue_dl, sample_offset);
#endif

    // srslte_ue_sync_nbiot_zerocopy_multi() returns 1 if successfully read 1 aligned subframe
    if (ret == 1) {
      switch (state) {
        case DECODE_MIB:
          if (srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 0) {
            n = srslte_ue_mib_nbiot_decode(&ue_mib, buff_ptrs[0], bch_payload, &cell.nof_ports, &sfn_offset);
            if (n < 0) {
              fprintf(stderr, "Error decoding UE MIB\n");
              exit(-1);
            } else if (n == SRSLTE_UE_MIB_NBIOT_FOUND) {
              printf("MIB received (CFO: %+6.2f kHz)\n", srslte_ue_sync_nbiot_get_cfo(&ue_sync) / 1000);
              srslte_mib_nb_t mib;
              srslte_npbch_mib_unpack(bch_payload, &mib);

              // update SFN and set deployment mode
              system_frame_number = (mib.sfn + sfn_offset) % 1024;
              cell.mode = mib.mode;

              // set number of ports of base cell to that of NB-IoT cell (FIXME: read eutra-NumCRS-Ports-r13)
              cell.base.nof_ports = cell.nof_ports;

              if (cell.mode == SRSLTE_NBIOT_MODE_INBAND_SAME_PCI) {
                cell.base.id = cell.n_id_ncell;
              }

              if (SRSLTE_VERBOSE_ISINFO()) {
                srslte_mib_nb_printf(stdout, cell, &mib);
              }

              // Initialize DL
              if (srslte_nbiot_ue_dl_init(&ue_dl, buff_ptrs, SRSLTE_NBIOT_MAX_PRB, SRSLTE_NBIOT_NUM_RX_ANTENNAS)) {
                fprintf(stderr, "Error initiating UE downlink processing module\n");
                exit(-1);
              }

              if (srslte_nbiot_ue_dl_set_cell(&ue_dl, cell)) {
                fprintf(stderr, "Configuring cell in UE DL\n");
                exit(-1);
              }

              // Configure downlink receiver with the MIB params and the RNTI we use
              srslte_nbiot_ue_dl_set_mib(&ue_dl, mib);
              srslte_nbiot_ue_dl_set_rnti(&ue_dl, prog_args.rnti);

              // Pretty-print MIB
              srslte_bit_pack_vector(bch_payload, rx_tb, SRSLTE_MIB_NB_CRC_LEN);
#ifdef ENABLE_GUI
              if (bcch_bch_to_pretty_string(
                      rx_tb, SRSLTE_MIB_NB_CRC_LEN, mib_buffer_decode, sizeof(mib_buffer_decode))) {
                fprintf(stderr, "Error decoding MIB\n");
              }
#endif

#if HAVE_PCAP
              // write to PCAP
              pcap_pack_and_write(pcap_file,
                                  rx_tb,
                                  SRSLTE_MIB_NB_CRC_LEN,
                                  0,
                                  true,
                                  system_frame_number * SRSLTE_NOF_SF_X_FRAME,
                                  0,
                                  DIRECTION_DOWNLINK,
                                  NO_RNTI);
#endif
              // activate SIB1 decoding
              srslte_nbiot_ue_dl_decode_sib1(&ue_dl, system_frame_number);
              state = DECODE_SIB;
            }
          }
          break;

        case DECODE_SIB:
          if (!have_sib1) {
            int dec_ret = srslte_nbiot_ue_dl_decode_npdsch(&ue_dl,
                                                           &buff_ptrs[0][prog_args.time_offset],
                                                           rx_tb,
                                                           system_frame_number,
                                                           srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                           SRSLTE_SIRNTI);
            if (dec_ret == SRSLTE_SUCCESS) {
              printf("SIB1 received\n");
              srslte_sys_info_block_type_1_nb_t sib = {};
              srslte_npdsch_sib1_unpack(rx_tb, &sib);
              hyper_frame_number = sib.hyper_sfn;

              have_sib1 = true;

#ifdef ENABLE_GUI
              if (bcch_dl_sch_to_pretty_string(
                      rx_tb, ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8, sib1_buffer_decode, sizeof(sib1_buffer_decode))) {
                fprintf(stderr, "Error decoding SIB1\n");
              }
#endif

              // Decode SIB1 and extract SIB2 scheduling params
              get_sib2_params(rx_tb, ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8, &sib2_params);

              // Activate SIB2 decoding
              srslte_nbiot_ue_dl_decode_sib(
                  &ue_dl, hyper_frame_number, system_frame_number, SRSLTE_NBIOT_SI_TYPE_SIB2, sib2_params);
#if HAVE_PCAP
              pcap_pack_and_write(pcap_file,
                                  rx_tb,
                                  ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8,
                                  0,
                                  true,
                                  system_frame_number * 10 + srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                  SRSLTE_SIRNTI,
                                  DIRECTION_DOWNLINK,
                                  SI_RNTI);
#endif
              // if SIB1 was decoded in this subframe, skip processing it further
              break;
            } else if (dec_ret == SRSLTE_ERROR) {
              // reactivate SIB1 grant
              if (srslte_nbiot_ue_dl_has_grant(&ue_dl) == false) {
                srslte_nbiot_ue_dl_decode_sib1(&ue_dl, system_frame_number);
              }
            }
          } else if (!have_sib2 && !srslte_nbiot_ue_dl_is_sib1_sf(
                                       &ue_dl, system_frame_number, srslte_ue_sync_nbiot_get_sfidx(&ue_sync))) {
            // SIB2 is transmitted over multiple subframes, so this needs to be called more than once ..
            int dec_ret = srslte_nbiot_ue_dl_decode_npdsch(&ue_dl,
                                                           &buff_ptrs[0][prog_args.time_offset],
                                                           rx_tb,
                                                           system_frame_number,
                                                           srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                           SRSLTE_SIRNTI);
            if (dec_ret == SRSLTE_SUCCESS) {
              printf("SIB2 received\n");
              have_sib2 = true;

#ifdef ENABLE_GUI
              if (bcch_dl_sch_to_pretty_string(
                      rx_tb, ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8, sib2_buffer_decode, sizeof(sib2_buffer_decode))) {
                fprintf(stderr, "Error decoding SIB2\n");
              }
#endif

#if HAVE_PCAP
              pcap_pack_and_write(pcap_file,
                                  rx_tb,
                                  ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8,
                                  0,
                                  true,
                                  system_frame_number * 10 + srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                  SRSLTE_SIRNTI,
                                  DIRECTION_DOWNLINK,
                                  SI_RNTI);
#endif
            } else {
              // reactivate SIB2 grant
              if (srslte_nbiot_ue_dl_has_grant(&ue_dl) == false) {
                srslte_nbiot_ue_dl_decode_sib(
                    &ue_dl, hyper_frame_number, system_frame_number, SRSLTE_NBIOT_SI_TYPE_SIB2, sib2_params);
              }
            }
          }

          if (have_sib1 && (have_sib2 || prog_args.skip_sib2)) {
            if (prog_args.rnti == SRSLTE_SIRNTI) {
              srslte_nbiot_ue_dl_decode_sib1(&ue_dl, system_frame_number);
            }
            state = DECODE_NPDSCH;
          }
          break;
        case DECODE_NPDSCH:
          if (prog_args.rnti != SRSLTE_SIRNTI) {
            if (srslte_nbiot_ue_dl_has_grant(&ue_dl)) {
              // attempt to decode NPDSCH
              n = srslte_nbiot_ue_dl_decode_npdsch(&ue_dl,
                                                   &buff_ptrs[0][prog_args.time_offset],
                                                   rx_tb,
                                                   system_frame_number,
                                                   srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                   prog_args.rnti);
              if (n == SRSLTE_SUCCESS) {
                INFO("NPDSCH decoded ok.\n");
              }
            } else {
              // decode NPDCCH
              srslte_dci_msg_t dci_msg;
              n = srslte_nbiot_ue_dl_decode_npdcch(&ue_dl,
                                                   &buff_ptrs[0][prog_args.time_offset],
                                                   system_frame_number,
                                                   srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                   prog_args.rnti,
                                                   &dci_msg);
              if (n == SRSLTE_NBIOT_UE_DL_FOUND_DCI) {
                INFO("DCI found for rnti=%d\n", prog_args.rnti);
                // convert DCI to grant
                srslte_ra_nbiot_dl_dci_t   dci_unpacked;
                srslte_ra_nbiot_dl_grant_t grant;
                if (srslte_nbiot_dci_msg_to_dl_grant(&dci_msg,
                                                     prog_args.rnti,
                                                     &dci_unpacked,
                                                     &grant,
                                                     system_frame_number,
                                                     srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                     64 /* TODO: remove */,
                                                     cell.mode)) {
                  fprintf(stderr, "Error unpacking DCI\n");
                  return SRSLTE_ERROR;
                }
                // activate grant
                srslte_nbiot_ue_dl_set_grant(&ue_dl, &grant);
              }
            }
          } else {
            // decode SIB1 over and over again
            n = srslte_nbiot_ue_dl_decode_npdsch(&ue_dl,
                                                 &buff_ptrs[0][prog_args.time_offset],
                                                 rx_tb,
                                                 system_frame_number,
                                                 srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
                                                 prog_args.rnti);

#ifdef ENABLE_GUI
            if (n == SRSLTE_SUCCESS) {
              if (bcch_dl_sch_to_pretty_string(
                      rx_tb, ue_dl.npdsch_cfg.grant.mcs[0].tbs / 8, sib1_buffer_decode, sizeof(sib1_buffer_decode))) {
                fprintf(stderr, "Error decoding SIB1\n");
              }
            }
#endif // ENABLE_GUI

            // reactivate SIB1 grant
            if (srslte_nbiot_ue_dl_has_grant(&ue_dl) == false) {
              srslte_nbiot_ue_dl_decode_sib1(&ue_dl, system_frame_number);
            }
          }

          nof_trials++;

          rsrq  = SRSLTE_VEC_EMA(srslte_chest_dl_nbiot_get_rsrq(&ue_dl.chest), rsrq, 0.1);
          rsrp  = SRSLTE_VEC_EMA(srslte_chest_dl_nbiot_get_rsrp(&ue_dl.chest), rsrp, 0.05);
          noise = SRSLTE_VEC_EMA(srslte_chest_dl_nbiot_get_noise_estimate(&ue_dl.chest), noise, 0.05);
          nframes++;
          if (isnan(rsrq)) {
            rsrq = 0;
          }
          if (isnan(noise)) {
            noise = 0;
          }
          if (isnan(rsrp)) {
            rsrp = 0;
          }

          // Plot and Printf
          if (srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 5) {
            float gain = prog_args.rf_gain;
            if (gain < 0) {
              gain = 10 * log10(srslte_agc_get_gain(&ue_sync.agc));
            }
            printf(
                "CFO: %+6.2f kHz, RSRP: %4.1f dBm "
                "SNR: %4.1f dB, RSRQ: %4.1f dB, "
                "NPDCCH detected: %d, NPDSCH-BLER: %5.2f%% (%d of total %d), NPDSCH-Rate: %5.2f kbit/s\r",
                srslte_ue_sync_nbiot_get_cfo(&ue_sync) / 1000,
                10 * log10(rsrp),
                10 * log10(rsrp / noise),
                10 * log10(rsrq),
                ue_dl.nof_detected,
                (float)100 * ue_dl.pkt_errors / ue_dl.pkts_total,
                ue_dl.pkt_errors,
                ue_dl.pkts_total,
                (ue_dl.bits_total / ((system_frame_number * 10 + srslte_ue_sync_nbiot_get_sfidx(&ue_sync)) / 1000.0)) /
                    1000.0);
          }
          break;
      }
      if (srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 9) {
        system_frame_number++;
        if (system_frame_number == 1024) {
          system_frame_number = 0;
          hyper_frame_number++;
          printf("\n");

          // don't reset counter when reading from file to maintain complete stats
          if (!prog_args.input_file_name) {
            ue_dl.pkt_errors   = 0;
            ue_dl.pkts_total   = 0;
            ue_dl.nof_detected = 0;
            ue_dl.bits_total   = 0;
            nof_trials         = 0;
          }
        }
      }

#ifdef ENABLE_GUI
      if (!prog_args.disable_plots) {
        if ((system_frame_number % 4) == 0) {
          plot_sf_idx = srslte_ue_sync_nbiot_get_sfidx(&ue_sync);
          plot_track  = true;
          sem_post(&plot_sem);
        }
      }
#endif // ENABLE_GUI
    } else if (ret == 0) {
      state = DECODE_MIB;
      printf("Finding PSS... Peak: %8.1f, FrameCnt: %d, State: %d\r",
             srslte_sync_nbiot_get_peak_value(&ue_sync.sfind),
             ue_sync.frame_total_cnt,
             ue_sync.state);
#ifdef ENABLE_GUI
      if (!prog_args.disable_plots) {
        plot_sf_idx = srslte_ue_sync_nbiot_get_sfidx(&ue_sync);
        plot_track  = false;
        sem_post(&plot_sem);
      }
#endif // ENABLE_GUI
    }

    sf_cnt++;
  } // Main loop

  // print statistics
  if (prog_args.input_file_name) {
    printf("pkt_total=%d\n", ue_dl.pkts_total);
    printf("pkt_ok=%d\n", ue_dl.pkts_total - ue_dl.pkt_errors);
    printf("pkt_errors=%d\n", ue_dl.pkt_errors);
    printf("bler=%.2f\n", ue_dl.pkts_total ? (float)100 * ue_dl.pkt_errors / ue_dl.pkts_total : 0);
    printf("rate=%.2f\n", ((ue_dl.bits_total / ((sf_cnt) / 1000.0)) / 1000.0));
    printf("dci_detected=%d\n", ue_dl.nof_detected);
  }

  srslte_nbiot_ue_dl_free(&ue_dl);
  srslte_ue_sync_nbiot_free(&ue_sync);

#if HAVE_PCAP
  printf("Saving PCAP file\n");
  LTE_PCAP_Close(pcap_file);
#endif

#ifndef DISABLE_RF
  if (!prog_args.input_file_name) {
    srslte_ue_mib_nbiot_free(&ue_mib);
    srslte_rf_close(&rf);
    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      if (buff_ptrs[i] != NULL)
        free(buff_ptrs[i]);
    }
  }
#endif

#ifdef ENABLE_GUI
  if (!prog_args.disable_plots) {
    sem_post(&plot_sem);
    if (!pthread_kill(plot_thread, 0)) {
      pthread_kill(plot_thread, SIGHUP);
      pthread_join(plot_thread, NULL);
    }
  }
#endif // ENABLE_GUI

  printf("\nBye\n");
  return SRSLTE_SUCCESS;
}

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifdef ENABLE_GUI

plot_real_t p_sync, pce;
#if HAVE_RSRP_PLOT
plot_real_t rsrp_plot;
#endif
plot_scatter_t constellation_plot;
text_edit_t    miblog, sib1log, sib2log;
key_value_t    id_label, mode_label, hfn_label;

#define LABLE_MAX_LEN (10)
static char lable_buf[LABLE_MAX_LEN];

float tmp_plot[110 * 15 * 2048];
float tmp_plot2[110 * 15 * 2048];

void* plot_thread_run(void* arg)
{
  uint32_t nof_re   = SRSLTE_SF_LEN_RE(ue_dl.cell.base.nof_prb, ue_dl.cell.base.cp);
#if HAVE_RSRP_PLOT
  float    rsrp_lin = 0;
#endif

  sdrgui_init_title("Software Radio Systems NB-IoT Receiver");

  plot_scatter_init(&constellation_plot);
  plot_scatter_setTitle(&constellation_plot, "NPDCCH/NPDSCH - Equalized Symbols");
  plot_scatter_setXAxisScale(&constellation_plot, -2, 2);
  plot_scatter_setYAxisScale(&constellation_plot, -2, 2);

  plot_scatter_addToWindowGrid(&constellation_plot, (char*)"npdsch_ue", 1, 0);

  if (!prog_args.disable_plots_except_constellation) {
    plot_real_init(&pce);
    plot_real_setTitle(&pce, "Channel Response - Magnitude");
    plot_real_setLabels(&pce, "Index", "dB");
    plot_real_setYAxisScale(&pce, -40, 40);

    plot_real_init(&p_sync);
    plot_real_setTitle(&p_sync, "NPSS Cross-Corr abs value");
    plot_real_setYAxisScale(&p_sync, 0, 1);

#if HAVE_RSRP_PLOT
    plot_real_init(&rsrp_plot);
    plot_real_setTitle(&rsrp_plot, "RSRP");
    plot_real_setLabels(&rsrp_plot, "subframe index", "dBm");
    plot_real_setYAxisScale(&rsrp_plot, 20, 50);
#endif

    plot_real_addToWindowGrid(&pce, (char*)"npdsch_ue", 1, 2);
    plot_real_addToWindowGrid(&p_sync, (char*)"npdsch_ue", 1, 1);

#if HAVE_RSRP_PLOT
    plot_real_addToWindowGrid(&rsrp_plot, (char*)"npdsch_ue", 1, 3);
#endif

    // add log
    text_edit_init(&miblog);
    text_edit_addToWindowGrid(&miblog, (char*)"npdsch_ue", 2, 0);
    text_edit_setTitle(&miblog, "Master Information Block - NB");

    text_edit_init(&sib1log);
    text_edit_addToWindowGrid(&sib1log, (char*)"npdsch_ue", 2, 1);
    text_edit_setTitle(&sib1log, "System Information Block 1 - NB");

    text_edit_init(&sib2log);
    text_edit_addToWindowGrid(&sib2log, (char*)"npdsch_ue", 2, 2);
    text_edit_setTitle(&sib2log, "System Information - NB");

    key_value_init(&id_label);
    text_edit_addToWindowGrid(&id_label, (char*)"npdsch_ue", 0, 0);
    key_value_setKeyText(&id_label, "Cell ID:");

    key_value_init(&mode_label);
    text_edit_addToWindowGrid(&mode_label, (char*)"npdsch_ue", 0, 1);
    key_value_setKeyText(&mode_label, "Operation Mode:");

    key_value_init(&hfn_label);
    text_edit_addToWindowGrid(&hfn_label, (char*)"npdsch_ue", 0, 2);
    key_value_setKeyText(&hfn_label, "Hyper/System Frame Number:");
  }

  while (!go_exit) {
    sem_wait(&plot_sem);

    if (!prog_args.disable_plots_except_constellation) {
      for (int i = 0; i < nof_re; i++) {
        tmp_plot[i] = 20 * log10f(cabsf(ue_dl.sf_symbols[i]));
        if (isinf(tmp_plot[i])) {
          tmp_plot[i] = -80;
        }
      }
      int numpoints = SRSLTE_NRE * 2;
      bzero(tmp_plot2, sizeof(float) * numpoints);
      int g = (numpoints - SRSLTE_NRE) / 2;
      for (int i = 0; i < 12 * ue_dl.cell.base.nof_prb; i++) {
        tmp_plot2[g + i] = 20 * log10(cabsf(ue_dl.ce[0][i]));
        if (isinf(tmp_plot2[g + i])) {
          tmp_plot2[g + i] = -80;
        }
      }
      plot_real_setNewData(&pce, tmp_plot2, numpoints);

      if (!prog_args.input_file_name) {
        if (plot_track) {
          srslte_npss_synch_t* pss_obj = &ue_sync.strack.npss;
          int max = srslte_vec_max_fi(pss_obj->conv_output_avg, pss_obj->frame_size + pss_obj->fft_size - 1);
          srslte_vec_sc_prod_fff(pss_obj->conv_output_avg,
                                 1 / pss_obj->conv_output_avg[max],
                                 tmp_plot2,
                                 pss_obj->frame_size + pss_obj->fft_size - 1);
          plot_real_setNewData(&p_sync, &tmp_plot2[max - NPSS_FIND_PLOT_WIDTH / 2], NPSS_FIND_PLOT_WIDTH);
        } else {
          int len = SRSLTE_NPSS_CORR_FILTER_LEN + ue_sync.sfind.npss.frame_size - 1;
          int max = srslte_vec_max_fi(ue_sync.sfind.npss.conv_output_avg, len);
          srslte_vec_sc_prod_fff(
              ue_sync.sfind.npss.conv_output_avg, 1 / ue_sync.sfind.npss.conv_output_avg[max], tmp_plot2, len);
          plot_real_setNewData(&p_sync, tmp_plot2, len);
        }
      }

#if HAVE_RSRP_PLOT
      // get current RSRP estimate
      rsrp_lin                       = SRSLTE_VEC_EMA(srslte_chest_dl_nbiot_get_rsrp(&ue_dl.chest), rsrp_lin, 0.05);
      rsrp_table[rsrp_table_index++] = 10 * log10(rsrp_lin);
      if (rsrp_table_index == rsrp_num_plot) {
        rsrp_table_index = 0;
      }
      plot_real_setNewData(&rsrp_plot, rsrp_table, rsrp_num_plot);
#endif

      // update MIB and SIB widget only if their content changed
      if (memcmp(mib_buffer_disp, mib_buffer_decode, sizeof(mib_buffer_disp)) != 0) {
        memcpy(mib_buffer_disp, mib_buffer_decode, sizeof(mib_buffer_disp));
        text_edit_setMessage(&miblog, mib_buffer_disp);
      }
      if (memcmp(sib1_buffer_disp, sib1_buffer_decode, sizeof(sib1_buffer_disp)) != 0) {
        memcpy(sib1_buffer_disp, sib1_buffer_decode, sizeof(sib1_buffer_disp));
        text_edit_setMessage(&sib1log, sib1_buffer_disp);
      }
      if (memcmp(sib2_buffer_disp, sib2_buffer_decode, sizeof(sib2_buffer_disp)) != 0) {
        memcpy(sib2_buffer_disp, sib2_buffer_decode, sizeof(sib2_buffer_disp));
        text_edit_setMessage(&sib2log, sib2_buffer_disp);
      }

      snprintf(lable_buf, LABLE_MAX_LEN, "%d", cell.n_id_ncell);
      key_value_setValueText(&id_label, lable_buf);

      key_value_setValueText(&mode_label, srslte_nbiot_mode_string(cell.mode));

      snprintf(lable_buf, LABLE_MAX_LEN, "%d / %d", hyper_frame_number, system_frame_number);
      key_value_setValueText(&hfn_label, lable_buf);
    }

    // check if NPDSCH or NPDCCH has been received
    if (ue_dl.npdsch_cfg.nbits.nof_re) {
      // plot NPDSCH
      plot_scatter_setNewData(&constellation_plot, ue_dl.npdsch.d, ue_dl.npdsch_cfg.nbits.nof_re);

    } else if (ue_dl.npdcch.num_decoded_symbols) {
      // plot NPDCCH
      plot_scatter_setNewData(&constellation_plot, ue_dl.npdcch.d, ue_dl.npdcch.num_decoded_symbols);
    }
  }

  return NULL;
}

void init_plots()
{
  if (sem_init(&plot_sem, 0, 0)) {
    perror("sem_init");
    exit(-1);
  }

  pthread_attr_t     attr;
  struct sched_param param;
  param.sched_priority = 0;
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
  pthread_attr_setschedparam(&attr, &param);
  if (pthread_create(&plot_thread, NULL, plot_thread_run, NULL)) {
    perror("pthread_create");
    exit(-1);
  }
}

#endif // ENABLE_GUI
