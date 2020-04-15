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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

static bool           keep_running     = true;
char*                 output_file_name = NULL;
static char           rf_devname[64]   = "";
static char           rf_args[64]      = "auto";
float                 rf_gain = 60.0, rf_freq = -1.0;
int                   nof_prb                = 6;
int                   nof_subframes          = -1;
int                   N_id_2                 = -1;
uint32_t              nof_rx_antennas        = 1;
bool                  use_standard_lte_rates = false;
srslte_ue_sync_mode_t sync_mode              = SYNC_MODE_PSS;

void int_handler(int dummy)
{
  keep_running = false;
}

void usage(char* prog)
{
  printf("Usage: %s [agrnv] -l N_id_2 -f rx_frequency_hz -o output_file\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-d RF devicename [Default %s]\n", rf_devname);
  printf("\t-e use_standard_lte_rates [Default %i]\n", use_standard_lte_rates);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-p nof_prb [Default %d]\n", nof_prb);
  printf("\t-n nof_subframes [Default %d]\n", nof_subframes);
  printf("\t-m Use GPS sync mode [Default %s]\n", sync_mode == SYNC_MODE_PSS ? "false" : "true");
  printf("\t-A nof_rx_antennas [Default %d]\n", nof_rx_antennas);
  printf("\t-v verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "adegpnmvfolA")) != -1) {
    switch (opt) {
      case 'o':
        output_file_name = argv[optind];
        break;
      case 'a':
        strncpy(rf_args, argv[optind], 63);
        rf_args[63] = '\0';
        break;
      case 'd':
        strncpy(rf_devname, argv[optind], 63);
        rf_devname[63] = '\0';
        break;
      case 'e':
        use_standard_lte_rates = true;
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'p':
        nof_prb = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'n':
        nof_subframes = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        sync_mode = SYNC_MODE_GNSS;
        break;
      case 'l':
        N_id_2 = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'A':
        nof_rx_antennas = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (&rf_freq < 0 || N_id_2 == -1 || output_file_name == NULL) {
    usage(argv[0]);
    exit(-1);
  }
}

int srslte_rf_recv_wrapper(void* h, cf_t* data[SRSLTE_MAX_PORTS], uint32_t nsamples, srslte_timestamp_t* t)
{
  DEBUG(" ----  Receive %d samples  ---- \n", nsamples);
  void* ptr[SRSLTE_MAX_PORTS];
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    ptr[i] = data[i];
  }
  return srslte_rf_recv_with_time_multi(h, ptr, nsamples, true, &t->full_secs, &t->frac_secs);
}

int main(int argc, char** argv)
{
  cf_t*             buffer[SRSLTE_MAX_CHANNELS] = {NULL};
  int               n;
  srslte_rf_t       rf;
  srslte_filesink_t sink;
  srslte_ue_sync_t  ue_sync;
  srslte_cell_t     cell;

  signal(SIGINT, int_handler);

  parse_args(argc, argv);

  srslte_use_standard_symbol_size(use_standard_lte_rates);

  srslte_filesink_init(&sink, output_file_name, SRSLTE_COMPLEX_FLOAT_BIN);

  printf("Opening RF device...\n");
  if (srslte_rf_open_multi(&rf, rf_args, nof_rx_antennas)) {
    ERROR("Error opening rf\n");
    exit(-1);
  }

  uint32_t max_num_samples = 3 * SRSLTE_SF_LEN_MAX;
  for (int i = 0; i < nof_rx_antennas; i++) {
    buffer[i] = srslte_vec_cf_malloc(max_num_samples);
  }

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  printf("Set RX freq: %.6f MHz\n", srslte_rf_set_rx_freq(&rf, nof_rx_antennas, rf_freq) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, rf_gain));
  int srate = srslte_sampling_freq_hz(nof_prb);
  if (srate != -1) {
    printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
    float srate_rf = srslte_rf_set_rx_srate(&rf, (double)srate);
    if (srate_rf != srate) {
      ERROR("Could not set sampling rate\n");
      exit(-1);
    }
  } else {
    ERROR("Invalid number of PRB %d\n", nof_prb);
    exit(-1);
  }
  srslte_rf_start_rx_stream(&rf, false);

  cell.cp        = SRSLTE_CP_NORM;
  cell.id        = N_id_2;
  cell.nof_prb   = nof_prb;
  cell.nof_ports = 1;

  if (srslte_ue_sync_init_multi_decim_mode(
          &ue_sync, cell.nof_prb, cell.id == 1000, srslte_rf_recv_wrapper, nof_rx_antennas, (void*)&rf, 1, sync_mode)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    exit(-1);
  }
  if (srslte_ue_sync_set_cell(&ue_sync, cell)) {
    ERROR("Error initiating ue_sync\n");
    exit(-1);
  }

  uint32_t           subframe_count = 0;
  bool               start_capture  = false;
  bool               stop_capture   = false;
  srslte_timestamp_t ts_rx_start    = {};
  while ((subframe_count < nof_subframes || nof_subframes == -1) && !stop_capture) {
    n = srslte_ue_sync_zerocopy(&ue_sync, buffer, max_num_samples);
    if (n < 0) {
      ERROR("Error receiving samples\n");
      exit(-1);
    }
    if (n == 1) {
      if (!start_capture) {
        if (srslte_ue_sync_get_sfidx(&ue_sync) == 9) {
          start_capture = true;
        }
      } else {
        printf("Writing to file %6d subframes...\r", subframe_count);
        srslte_filesink_write_multi(&sink, (void**)buffer, SRSLTE_SF_LEN_PRB(nof_prb), nof_rx_antennas);

        // store time stamp of first subframe
        if (subframe_count == 0) {
          srslte_ue_sync_get_last_timestamp(&ue_sync, &ts_rx_start);
        }
        subframe_count++;
      }
    }
    if (!keep_running) {
      if (!start_capture || (start_capture && srslte_ue_sync_get_sfidx(&ue_sync) == 9)) {
        stop_capture = true;
      }
    }
  }

  srslte_filesink_free(&sink);
  srslte_rf_close(&rf);
  srslte_ue_sync_free(&ue_sync);

  for (int i = 0; i < nof_rx_antennas; i++) {
    if (buffer[i]) {
      free(buffer[i]);
    }
  }

  printf("\nOk - wrote %d subframes\n", subframe_count);

  srslte_ue_sync_set_tti_from_timestamp(&ue_sync, &ts_rx_start);
  printf("Start of capture at %ld+%.3f. TTI=%d.%d\n",
         ts_rx_start.full_secs,
         ts_rx_start.frac_secs,
         srslte_ue_sync_get_sfn(&ue_sync),
         srslte_ue_sync_get_sfidx(&ue_sync));

  return SRSLTE_SUCCESS;
}
