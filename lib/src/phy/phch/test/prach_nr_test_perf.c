/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

/**
 * \file prach_nr_test_perf.c
 * \brief Performance test for PRACH NR.
 *
 * This program simulates several PRACH preamble transmissions (so far, burst format 0 only)
 * to estimate the probability of detection and of false alarm. The probability of detection
 * is the conditional probability of detecting the preamble when the preamble is present.
 * An error consists in detecting no preambles, detecting only preambles different from the
 * reference one, or detecting the correct preamble with a timing error beyond tolerance.
 * The probability of false alarm is the probability of detecting any preamble when input
 * is only noise.
 *
 * The simulation setup can be controlled by means of the following arguments.
 *   - <tt>-N num</tt>: sets the number of experiments to \c num.
 *   - <tt>-n num</tt>: sets the total number of UL PRBs to \c num.
 *   - <tt>-f num</tt>: sets the preamble format to \c num (for now, format 0 only).
 *   - <tt>-s val</tt>: sets the nominal SNR to \c val dB.
 *   - <tt>-v </tt>: activates verbose output.
 *
 * Example:
 * \code{.cpp}
 * prach_nr_test_perf -n 52 -s -14.6
 * \endcode
 *
 * \todo Restricted preamble formats not implemented yet. Fading channel and SIMO.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/srsran.h"

#define MAX_LEN 70176

static uint32_t nof_prb    = 52;
static uint32_t config_idx = 0;
static int      nof_runs   = 100;
static float    snr_dB     = -14.5F;
static bool     is_verbose = false;

static void usage(char* prog)
{
  printf("Usage: %s\n", prog);
  printf("\t-N Number of experiments [Default %d]\n", nof_runs);
  printf("\t-n Uplink number of PRB [Default %d]\n", nof_prb);
  printf("\t-f Preamble format [Default %d]\n", config_idx);
  printf("\t-s SNR in dB [Default %.2f]\n", snr_dB);
  printf("\t-v Activate verbose output [Default %s]\n", is_verbose ? "true" : "false");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "N:n:f:s:v")) != -1) {
    switch (opt) {
      case 'N':
        nof_runs = (int)strtol(optarg, NULL, 10);
        break;
      case 'n':
        nof_prb = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 'f':
        config_idx = (uint32_t)strtol(optarg, NULL, 10);
        break;
      case 's':
        snr_dB = strtof(optarg, NULL);
        break;
      case 'v':
        is_verbose = true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);
  if (config_idx != 0) {
    ERROR("Preamble format not yet implemented");
    return SRSRAN_ERROR;
  }
  srsran_prach_t prach;

  const int   fft_size         = srsran_symbol_sz(nof_prb);
  const float main_scs_kHz     = 15; // UL subcarrier spacing (i.e., Delta f)
  const float sampling_time_us = 1000.0F / (main_scs_kHz * (float)fft_size);
  const int   slot_length      = 15 * fft_size; // number of samples in a slot

  if (srsran_prach_init(&prach, fft_size)) {
    ERROR("Initializing PRACH");
    srsran_prach_free(&prach);
    return SRSRAN_ERROR;
  }

  cf_t preamble[MAX_LEN];
  srsran_vec_cf_zero(preamble, MAX_LEN);

  srsran_prach_cfg_t prach_cfg;
  ZERO_OBJECT(prach_cfg);

  // Setup according to TS38.104 Section 8.4
  prach_cfg.is_nr                  = true;
  prach_cfg.config_idx             = 0;     // preamble format 0
  prach_cfg.hs_flag                = false; // no high speed
  prach_cfg.freq_offset            = 0;
  prach_cfg.root_seq_idx           = 22;    // logical (root sequence) index i
  prach_cfg.zero_corr_zone         = 1;     // zero correlation zone -> implies Ncs = 13
  prach_cfg.num_ra_preambles       = 0;     // use default
  const uint32_t seq_index         = 32;    // sequence index "v"
  const float    prach_scs_kHz     = 1.25F; // PRACH subcarrier spacing (i.e., Delta f^RA)
  const float    max_time_error_us = 1.04F; // time error tolerance
  const int      nof_offset_steps  = 10;

  if (srsran_prach_set_cfg(&prach, &prach_cfg, nof_prb)) {
    ERROR("Error initiating PRACH object");
    srsran_prach_free(&prach);
    return SRSRAN_ERROR;
  }

  if (srsran_prach_gen(&prach, seq_index, 0, preamble) < SRSRAN_SUCCESS) {
    ERROR("Generating PRACH preamble");
    srsran_prach_free(&prach);
    return SRSRAN_ERROR;
  }

  const uint32_t preamble_length = prach.N_seq;

  float prach_pwr_sqrt = sqrtf(srsran_vec_avg_power_cf(preamble, preamble_length));
  if (!isnormal(prach_pwr_sqrt)) {
    ERROR("PRACH preamble power is not a finite, nonzero value");
    srsran_prach_free(&prach);
    return SRSRAN_ERROR;
  }
  srsran_vec_sc_prod_cfc(preamble, 1.0F / prach_pwr_sqrt, preamble, preamble_length);

  int  vector_length = 2 * slot_length;
  cf_t symbols[vector_length];
  cf_t noise_vec[vector_length];

  uint32_t indices[64]    = {0};
  float    offset_est[64] = {0};
  uint32_t n_indices      = 0;

  float time_offset_us             = 0;
  int   offset_samples             = 0;
  float noise_var                  = srsran_convert_dB_to_power(-snr_dB);
  int   ok_detection               = 0;
  int   missed_detection           = 0;
  int   false_detection_signal_tmp = 0;
  int   false_detection_signal     = 0;
  int   false_detection_noise      = 0;
  int   offset_est_error           = 0;

  // Timing offset base value is equivalent to N_cs/2
  const uint32_t ZC_length           = prach.N_zc; // Zadoff-Chu sequence length (i.e., L_RA)
  const float    base_time_offset_us = (float)prach.N_cs * 1000 / (2.0F * (float)ZC_length * prach_scs_kHz);

  int step = SRSRAN_MAX(nof_runs / 100, 1);
  for (int i_run = 0; i_run < nof_runs; i_run++) {
    // show we are doing something
    if (i_run % (20 * step) == 0) {
      printf("\n");
    }
    if (i_run % step == 0) {
      printf("*");
      fflush(stdout);
    }

    srsran_vec_cf_zero(noise_vec, vector_length);
    srsran_ch_awgn_c(noise_vec, noise_vec, noise_var, vector_length);
    if (is_verbose) {
      float prach_pwr = srsran_vec_avg_power_cf(preamble, preamble_length);
      float noise_pwr = srsran_vec_avg_power_cf(noise_vec, vector_length);
      printf("    Tx power: %.3f\n", prach_pwr);
      printf("    Noise power: %.3f\n", noise_pwr);
      printf("    Target/measured SNR: %.3f / %.3f dB\n", snr_dB, srsran_convert_power_to_dB(prach_pwr / noise_pwr));
    }
    // Cycle timing offset with a 0.1-us step starting from the base value
    for (int i = 0; i < nof_offset_steps; i++) {
      time_offset_us = base_time_offset_us + (float)i * 0.1F;
      offset_samples = (int)roundf(time_offset_us / sampling_time_us);
      srsran_vec_cf_copy(symbols, noise_vec, vector_length);
      srsran_vec_sum_ccc(&symbols[offset_samples], preamble, &symbols[offset_samples], preamble_length);

      srsran_prach_detect_offset(&prach, 0, &symbols[prach.N_cp], slot_length, indices, offset_est, NULL, &n_indices);
      false_detection_signal_tmp = 0;
      for (int j = 0; j < n_indices; j++) {
        if (indices[j] != seq_index) {
          false_detection_signal_tmp++;
        } else if (fabsf(offset_est[j] * 1.0e6F - time_offset_us) > max_time_error_us) {
          offset_est_error++;
        } else {
          ok_detection++;
        }
      }
      false_detection_signal += (n_indices > 1 || false_detection_signal_tmp == 1);
      // Missed detection if no preamble was detected or no detected preamble is the right one
      missed_detection += (n_indices == 0 || n_indices == false_detection_signal_tmp);
    }

    srsran_prach_detect_offset(&prach, 0, &noise_vec[prach.N_cp], slot_length, indices, offset_est, NULL, &n_indices);
    false_detection_noise += (n_indices > 0);
  }
  int total_runs = nof_offset_steps * nof_runs;
  if (missed_detection + ok_detection + offset_est_error != total_runs) {
    srsran_prach_free(&prach);
    ERROR("Counting detection errors");
    return SRSRAN_ERROR;
  }

  printf("\n\nPRACH performance test: format 0, %d PRB, AWGN channel, SNR=%.1f dB\n", nof_prb, snr_dB);
  printf("\nMissed detection probability: %.3e (%d out of %d)\n",
         (float)missed_detection / (float)total_runs,
         missed_detection,
         total_runs);
  printf("Probability of timing error: %.3e (%d out of %d)\n",
         (float)offset_est_error / (float)total_runs,
         offset_est_error,
         total_runs);
  printf("Probability of OK detection: %.3e (%d out of %d)\n",
         (float)ok_detection / (float)total_runs,
         ok_detection,
         total_runs);
  printf("\nProbability of false detection with preamble: %.3e (%d out of %d)\n",
         (float)false_detection_signal / (float)total_runs,
         false_detection_signal,
         total_runs);
  printf("Probability of false detection without preamble: %.3e (%d out of %d)\n",
         (float)false_detection_noise / (float)nof_runs,
         false_detection_noise,
         nof_runs);

  srsran_prach_free(&prach);

  printf("Done\n");
}
