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

#include "srslte/phy/io/filesink.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/ue/ue_mib_nbiot.h"
#include "srslte/phy/ue/ue_sync_nbiot.h"
#include "srslte/phy/utils/debug.h"

static bool keep_running     = true;
char*       output_file_name = NULL;
char*       rf_args          = "";
float       rf_gain = 60.0, rf_freq = -1.0;
int         nof_prb       = 6;
int         nof_subframes = -1;

void int_handler(int dummy)
{
  keep_running = false;
}

void usage(char* prog)
{
  printf("Usage: %s [agrtnv] -f rx_frequency_hz -o output_file\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-n nof_subframes [Default %d]\n", nof_subframes);
  printf("\t-v verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "agnvfto")) != -1) {
    switch (opt) {
      case 'o':
        output_file_name = argv[optind];
        break;
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'n':
        nof_subframes = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (&rf_freq < 0 || output_file_name == NULL) {
    usage(argv[0]);
    exit(-1);
  }
}

int srslte_rf_recv_wrapper(void* h, void* data, uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  return srslte_rf_recv(h, data, nsamples, 1);
}

int main(int argc, char** argv)
{
  signal(SIGINT, int_handler);

  parse_args(argc, argv);

  srslte_filesink_t sink;
  srslte_filesink_init(&sink, output_file_name, SRSLTE_COMPLEX_FLOAT_BIN);

  printf("Opening RF device...\n");
  srslte_rf_t rf;
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  srslte_rf_set_rx_gain(&rf, rf_gain);
  printf("Set RX freq: %.6f MHz\n", srslte_rf_set_rx_freq(&rf, 0, rf_freq) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_get_rx_gain(&rf));
  int srate = srslte_sampling_freq_hz(nof_prb);
  if (srate != -1) {
    printf("Setting sampling rate %.2f MHz\n", (float)srate / 1e6);
    double srate_rf = srslte_rf_set_rx_srate(&rf, srate);
    printf("Actual sampling rate %.2f MHz\n", srate_rf / 1e6);
    // We don't check the result rate with requested rate
  } else {
    fprintf(stderr, "Invalid number of PRB %d\n", nof_prb);
    exit(-1);
  }
  srslte_rf_start_rx_stream(&rf, false);

  srslte_nbiot_cell_t cell = {};
  cell.base.nof_prb        = nof_prb;
  cell.base.nof_ports      = 1;

  cf_t* buff_ptrs[SRSLTE_MAX_PORTS] = {NULL, NULL, NULL, NULL};
  buff_ptrs[0]                      = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB_NBIOT * 10);

  srslte_nbiot_ue_sync_t ue_sync;
  if (srslte_ue_sync_nbiot_init(&ue_sync, cell, srslte_rf_recv_wrapper, (void*)&rf)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1);
  }

  int32_t  nof_warmup_subframes = 1024;
  uint32_t subframe_count       = 0;
  bool     start_capture        = false;
  bool     stop_capture         = false;
  while ((subframe_count < nof_subframes || nof_subframes == -1) && !stop_capture) {
    int n = srslte_ue_sync_nbiot_zerocopy_multi(&ue_sync, buff_ptrs);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }

    if (n == 1) {
      if (!start_capture) {
        if (nof_warmup_subframes <= 0) {
          if (srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 9) {
            printf("Starting capture ..\n");
            start_capture = true;
          }
        }
        nof_warmup_subframes--;
      } else {
        printf("Writing subframe %d (%d/%d) to file (cfo=%6.2f kHz)\n",
               srslte_ue_sync_nbiot_get_sfidx(&ue_sync),
               subframe_count,
               nof_subframes,
               srslte_ue_sync_nbiot_get_cfo(&ue_sync) / 1000);
        srslte_filesink_write(&sink, buff_ptrs[0], SRSLTE_SF_LEN_PRB(nof_prb));
        subframe_count++;
      }
    }
    if (!keep_running) {
      if (!start_capture || (start_capture && srslte_ue_sync_nbiot_get_sfidx(&ue_sync) == 9)) {
        printf("Stopping capture ..\n");
        stop_capture = true;
      }
    }
  }

  srslte_filesink_free(&sink);
  srslte_rf_close(&rf);
  srslte_ue_sync_nbiot_free(&ue_sync);

  printf("Ok - wrote %d subframes\n", subframe_count);
  exit(0);
}
