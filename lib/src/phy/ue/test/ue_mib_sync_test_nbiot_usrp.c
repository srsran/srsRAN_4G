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

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/sync/sync_nbiot.h"
#include "srslte/phy/ue/ue_mib_nbiot.h"
#include "srslte/phy/utils/debug.h"

char*               rf_args = "";
float               rf_gain = 70.0, rf_freq = -1.0;
int                 nof_frames = -1;
uint32_t            fft_size   = 128;
float               threshold  = 20.0;
srslte_cp_t         cp         = SRSLTE_CP_NORM;
srslte_nbiot_cell_t cell       = {};

void usage(char* prog)
{
  printf("Usage: %s [aedgtvnpR] -f rx_frequency_hz\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l n_id_ncell to sync [Default %d]\n", cell.n_id_ncell);
  printf("\t-R Is R14 cell [Default %s]\n", cell.is_r14 ? "Yes" : "No");
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "adgtvsfilR")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 't':
        threshold = strtof(argv[optind], NULL);
        break;
      case 'l':
        cell.n_id_ncell = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        cell.is_r14 = true;
        break;
      case 's':
        fft_size = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
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

int srslte_rf_recv_wrapper(void* h, cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  void* ptr[SRSLTE_MAX_PORTS];
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    ptr[i] = data[i];
  }
  return srslte_rf_recv_with_time_multi(h, ptr, nsamples, true, NULL, NULL);
}

int main(int argc, char** argv)
{
  // init cell struct
  cell.base.nof_prb = 1;

  parse_args(argc, argv);

  signal(SIGINT, sig_int_handler);

  float   srate = 15000.0 * fft_size;
  int32_t flen  = srate * 10 / 1000;

  printf("Frame length %d samples\n", flen);

  printf("Opening RF device...\n");
  srslte_rf_t rf_device;
  if (srslte_rf_open(&rf_device, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  printf("Set RX rate: %.2f MHz\n", srslte_rf_set_rx_srate(&rf_device, srate) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf_device, rf_gain));
  printf("Set RX freq: %.2f MHz\n", srslte_rf_set_rx_freq(&rf_device, 0, rf_freq) / 1000000);

  srslte_ue_mib_sync_nbiot_t mib_sync;
  if (srslte_ue_mib_sync_nbiot_init_multi(
          &mib_sync, srslte_rf_recv_wrapper, SRSLTE_NBIOT_NUM_RX_ANTENNAS, (void*)&rf_device)) {
    fprintf(stderr, "Error initializing MIB sync object\n");
    exit(-1);
  }

  if (srslte_ue_mib_sync_nbiot_set_cell(&mib_sync, cell) != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error setting cell for MIB sync object\n");
    exit(-1);
  }

  srslte_rf_start_rx_stream(&rf_device, false);

  int max_frames = 2 * SRSLTE_NPBCH_NUM_FRAMES;

  printf("Trying to receive MIB-NB for n_id_ncell=%d for at most %d frames\n", cell.n_id_ncell, nof_frames);
  int     sfn_offset                        = 0;
  uint8_t bch_payload_rx[SRSLTE_MIB_NB_LEN] = {};
  int     ret = srslte_ue_mib_sync_nbiot_decode(&mib_sync, max_frames, bch_payload_rx, &cell.nof_ports, &sfn_offset);
  if (ret == SRSLTE_UE_MIB_NBIOT_FOUND) {
    srslte_mib_nb_t mib_nb;
    srslte_npbch_mib_unpack(bch_payload_rx, &mib_nb);
    srslte_mib_nb_printf(stdout, cell, &mib_nb);
    printf("CFO: %+6.2f kHz\n", srslte_ue_sync_nbiot_get_cfo(&mib_sync.ue_sync) / 1000);
  } else {
    printf("Failed!\n");
  }

  srslte_ue_mib_sync_nbiot_free(&mib_sync);
  srslte_rf_close(&rf_device);

  return SRSLTE_SUCCESS;
}
