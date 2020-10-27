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

#include <complex.h>
#include <math.h>
#include <srslte/common/test_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

#define MAX_LEN 70176

// PRACH Parameters
static uint32_t nof_prb          = 25;
static uint32_t preamble_format  = 0;
static uint32_t root_seq_idx     = 0;
static uint32_t seq_idx          = 0;
static uint32_t frequency_offset = 0;
static uint32_t zero_corr_zone   = 11;
static bool     high_speed_flag  = false;

// Simulation parameters
static float          timeadv          = 0;
static uint32_t       num_ra_preambles = 0; // use default
static uint32_t       nof_repetitions  = 1;
static bool           continous_tx     = true;
static char*          output_filename  = NULL;
static const uint32_t nof_frames       = 20;
static const uint32_t tx_delay_ms      = 4;

// RF parameters
static float uhd_rx_gain = 40, uhd_tx_gain = 60, uhd_freq = 2.4e9;
static char* uhd_args = "";
static char* device_name = "";

// SRSLTE Verbose
SRSLTE_API extern int srslte_verbose;

void usage(char* prog)
{
  printf("Usage: %s \n", prog);
  printf("  -d RF device name [Default %s]\n", device_name);
  printf("  -a UHD args [Default %s]\n", uhd_args);
  printf("  -c Continous Tx? [Default %s]\n", continous_tx ? "true" : "false");
  printf("  -f UHD TX/RX frequency [Default %.2f MHz]\n", uhd_freq / 1e6);
  printf("  -g UHD RX gain [Default %.1f dB]\n", uhd_rx_gain);
  printf("  -G UHD TX gain [Default %.1f dB]\n", uhd_tx_gain);
  printf("  -p Number of UL RB [Default %d]\n", nof_prb);
  printf("  -F Preamble format [Default %d]\n", preamble_format);
  printf("  -O Frequency offset [Default %d]\n", frequency_offset);
  printf("  -s sequence index [Default %d]\n", seq_idx);
  printf("  -r Root sequence index [Default %d]\n", root_seq_idx);
  printf("  -R Number of repetitions of %d ms [Default %d]\n", nof_frames, nof_repetitions);
  printf("  -t Time advance (us) [Default %.1f us]\n", timeadv);
  printf("  -z Zero correlation zone config [Default %d]\n", zero_corr_zone);
  printf("  -o Save transmitted PRACH in file [Default no]\n");
  printf("  -v [set srslte_verbose to info, debug, default none]\n");
  printf("\n");
  printf("Device arguments for:\n");
  printf("  X300: type=x300,addr=192.168.40.2,send_frame_size=2000,recv_frame_size=2000\n");
  printf("  B200: type=b200\n");
  printf("\n");
  printf("Scripted example:\n");
  printf("  Iterate over all bandwidth for 5 minutes and saving all stdout and displays delay stats at the end:\n");
  printf("    for i in 6 15 25 50 75 100;  do sudo ./lib/src/phy/phch/test/prach_test_usrp -a "
         "type=x300,addr=192.168.40.2,send_frame_size=2000,recv_frame_size=2000 -g 20 -G 20 -p $i -f 875e6 -R 15000 | "
         "tee prach_test_usrp_$i.txt; done; grep \"delay:\" prach_*\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "acdpfFgGrRstoPOvz")) != -1) {
    switch (opt) {
      case 'd':
        device_name = argv[optind];
        break;
      case 'a':
        uhd_args = argv[optind];
        break;
      case 'c':
        continous_tx = !continous_tx;
        break;
      case 'o':
        output_filename = argv[optind];
        break;
      case 'f':
        uhd_freq = strtof(argv[optind], NULL);
        break;
      case 'g':
        uhd_rx_gain = strtof(argv[optind], NULL);
        break;
      case 'G':
        uhd_tx_gain = strtof(argv[optind], NULL);
        break;
      case 'P':
        preamble_format = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'O':
        frequency_offset = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        timeadv = strtof(argv[optind], NULL);
        break;
      case 'p':
        nof_prb = (int)strtol(argv[optind], NULL, 10);
        if (!srslte_nofprb_isvalid(nof_prb)) {
          ERROR("Invalid number of UL RB %d\n", nof_prb);
          exit(-1);
        }
        break;
      case 'F':
        preamble_format = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        root_seq_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        nof_repetitions = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        seq_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'z':
        zero_corr_zone = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

void rf_msg_callback(void* arg, srslte_rf_error_t error)
{
  switch (error.type) {

    case SRSLTE_RF_ERROR_LATE:
      printf("L");
      break;
    case SRSLTE_RF_ERROR_UNDERFLOW:
      printf("U");
      break;
    case SRSLTE_RF_ERROR_OVERFLOW:
      printf("O");
      break;
    case SRSLTE_RF_ERROR_RX:
      printf("R");
      break;
    case SRSLTE_RF_ERROR_OTHER:
      printf("X");
      break;
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  srslte_prach_t prach = {};
  int            srate = srslte_sampling_freq_hz(nof_prb);
  uint32_t       flen  = srate / 1000;

  // Allocate buffers
  cf_t* preamble = srslte_vec_cf_malloc(MAX_LEN);
  cf_t* zeros    = srslte_vec_cf_malloc(flen);
  cf_t* buffer   = srslte_vec_cf_malloc(flen * nof_frames);

  if (!preamble || !zeros || !buffer) {
    return SRSLTE_ERROR;
  }

  srslte_vec_cf_zero(preamble, MAX_LEN);
  srslte_vec_cf_zero(zeros, flen);
  srslte_vec_cf_zero(buffer, flen * nof_frames);

  srslte_prach_cfg_t prach_cfg = {};
  prach_cfg.config_idx         = preamble_format;
  prach_cfg.hs_flag            = high_speed_flag;
  prach_cfg.freq_offset        = 0;
  prach_cfg.root_seq_idx       = root_seq_idx;
  prach_cfg.zero_corr_zone     = zero_corr_zone;
  prach_cfg.num_ra_preambles   = num_ra_preambles;

  if (srslte_prach_init(&prach, srslte_symbol_sz(nof_prb))) {
    return SRSLTE_ERROR;
  }

  if (srslte_prach_set_cfg(&prach, &prach_cfg, nof_prb)) {
    ERROR("Error initiating PRACH object\n");
    return SRSLTE_ERROR;
  }

  printf("Generating PRACH\n");
  srslte_prach_gen(&prach, seq_idx, frequency_offset, preamble);

  // Send through UHD
  srslte_rf_t rf;
  printf("Opening RF device...\n");
  if (srslte_rf_open_devname(&rf, device_name, uhd_args, 1)) {
    ERROR("Error opening &uhd\n");
    exit(-1);
  }
  printf("Test summary:\n");
  printf("   Sub-frame len: %d samples\n", flen);
  printf("     Set RX gain: %.1f dB\n", uhd_rx_gain);
  printf("     Set TX gain: %.1f dB\n", uhd_tx_gain);
  printf("   Sampling rate: %.3f MHz\n", srate / 1e6);
  printf("  Set TX/RX freq: %.2f MHz\n", uhd_freq / 1000000);
  printf("  Total duration: %d milli-seconds\n", nof_repetitions * nof_frames);
  printf("   Continuous Tx: %s\n", continous_tx ? "true" : "false");
  printf("\n");

  srslte_rf_set_rx_gain(&rf, uhd_rx_gain);
  srslte_rf_set_tx_gain(&rf, uhd_tx_gain);
  srslte_rf_set_rx_freq(&rf, 0, uhd_freq);
  srslte_rf_set_tx_freq(&rf, 0, uhd_freq);

  printf("Setting sampling rate %.2f MHz\n", (float)srate / 1000000);
  int srate_rf = (int)srslte_rf_set_rx_srate(&rf, (double)srate);
  if (srate_rf != srate) {
    ERROR("Could not set sampling rate\n");
    exit(-1);
  }
  srslte_rf_set_tx_srate(&rf, (double)srate);
  sleep(1);

  srslte_timestamp_t tstamp;

  // Register error handler
  srslte_rf_register_error_handler(&rf, rf_msg_callback, NULL);

  // Start streaming
  srslte_rf_start_rx_stream(&rf, false);

  // Print Table legend
  printf("%8s; %5s; %5s; %5s; %6s; %5s; %9s;\n", "Time", "i", "count", "index", "usec", "samp", "Norm Peak");

  // Statistics variables
  double   delay_us_min = +INFINITY;
  double   delay_us_max = -INFINITY;
  double   delay_us_avg = 0.0;
  double   peak_min     = +INFINITY;
  double   peak_max     = -INFINITY;
  double   peak_avg     = 0.0;
  uint32_t count        = 0;

  // First transmission shall be flagged as start of burst
  bool is_start_of_burst = true;

  // Perform experiment for given a number of repetitions
  for (uint32_t rep = 0; rep < nof_repetitions; rep++) {
    is_start_of_burst |= !continous_tx;

    // For a the number of frames
    for (uint32_t nframe = 0; nframe < nof_frames; nframe++) {
      INFO("Rep %d. Receiving frame %d\n", rep, nframe);
      srslte_rf_recv_with_time(&rf, &buffer[flen * nframe], flen, true, &tstamp.full_secs, &tstamp.frac_secs);

      srslte_timestamp_add(&tstamp, 0, tx_delay_ms * 1e-3 - timeadv * 1e-6);
      if (nframe == 10 - tx_delay_ms) {
        srslte_rf_send_timed2(&rf, preamble, flen, tstamp.full_secs, tstamp.frac_secs, false, !continous_tx);
        INFO("Rep %d. Transmitting PRACH\n", rep);
      } else if (nframe == 10 - tx_delay_ms - 1 || continous_tx) {
        srslte_rf_send_timed2(&rf, zeros, flen, tstamp.full_secs, tstamp.frac_secs, is_start_of_burst, false);
        INFO("Rep %d. Transmitting Zeros\n", rep);
        is_start_of_burst = false;
      }
    }

    // PRACH detection
    uint32_t indices[1024]     = {};
    float    offsets[1024]     = {};
    float    peak_to_avg[1024] = {};
    uint32_t nof_detected      = 0;
    if (srslte_prach_detect_offset(&prach,
                                   frequency_offset,
                                   &buffer[flen * 10 + prach.N_cp],
                                   flen,
                                   indices,
                                   offsets,
                                   peak_to_avg,
                                   &nof_detected)) {
      printf("Rep %d. Error detecting prach\n", rep);
    }

    // Prompt detected PRACH
    INFO("Rep %d. Nof detected PRACHs: %d\n", rep, nof_detected);
    for (int i = 0; i < nof_detected; i++) {
      INFO("%d/%d index=%d, offset=%.2f us (%d samples)\n",
           i,
           nof_detected,
           indices[i],
           offsets[i] * 1e6,
           (int)(offsets[i] * srate));
      printf("%8.3f; %5d; %5d; %5d; %6.2f; %5d; %9.3f;\n",
             (double)(rep * nof_frames) * 1e-3,
             i + 1,
             nof_detected,
             indices[i],
             offsets[i] * 1e6,
             (int)(offsets[i] * srate),
             peak_to_avg[i]);

      // Update stats
      delay_us_min = SRSLTE_MIN(delay_us_min, offsets[i] * 1e6);
      delay_us_max = SRSLTE_MAX(delay_us_max, offsets[i] * 1e6);
      delay_us_avg = SRSLTE_VEC_CMA(offsets[i] * 1e6, delay_us_avg, count);

      peak_min = SRSLTE_MIN(peak_min, peak_to_avg[i]);
      peak_max = SRSLTE_MAX(peak_max, peak_to_avg[i]);
      peak_avg = SRSLTE_VEC_CMA(peak_to_avg[i], peak_avg, count);

      count++;
    }
  }

  // End burst
  if (continous_tx) {
    srslte_rf_send_timed2(&rf, zeros, 0, tstamp.full_secs, tstamp.frac_secs, false, true);
  }

  // Print statistics
  printf("\n");
  printf("Statistics:\n");
  printf("    PRACH count: %d\n", count);
  printf("  Minimum delay: %.2f us\n", delay_us_min);
  printf("  Maximum delay: %.2f us\n", delay_us_max);
  printf("  Average delay: %.2f us\n", delay_us_avg);
  printf("   Minimum peak: %.2f\n", peak_min);
  printf("   Maximum peak: %.2f\n", peak_max);
  printf("   Average peak: %.2f\n", peak_avg);

  // Save in file if filename is not empty
  if (output_filename) {
    // Save generated PRACH signal
    srslte_vec_save_file("generated", preamble, (prach.N_seq + prach.N_cp) * sizeof(cf_t));

    // Save last received buffer
    srslte_vec_save_file(output_filename, buffer, 11 * flen * (uint32_t)sizeof(cf_t));
  }

  srslte_rf_close(&rf);
  srslte_prach_free(&prach);

  TESTASSERT(count == nof_repetitions);
  TESTASSERT(delay_us_max - delay_us_min < 0.5);
  TESTASSERT(isnormal(peak_min));
  TESTASSERT(isnormal(peak_max));
  TESTASSERT(peak_max < peak_min * 2);

  printf("Done\n");
  exit(0);
}
