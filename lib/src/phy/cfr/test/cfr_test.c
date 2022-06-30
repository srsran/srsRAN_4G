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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "srsran/phy/utils/random.h"
#include "srsran/srsran.h"

#define MAX_ACPR_DB -100


// Default CFR type
static char* cfr_mode_str = "manual";

static int               nof_prb         = -1;
static srsran_cp_t       cp              = SRSRAN_CP_NORM;
static int               nof_repetitions = 1;
static int               nof_frames      = 10;
static srsran_cfr_mode_t cfr_mode        = SRSRAN_CFR_THR_MANUAL;
static float             alpha           = 1.0f;
static bool              dc_empty        = true;
static float             thr_manual      = 1.5f;
static float             max_papr_db     = 8.0f;
static float             ema_alpha       = (float)1 / (float)SRSRAN_CP_NORM_NSYMB;

static uint32_t force_symbol_sz = 0;
static double   elapsed_us(struct timeval* ts_start, struct timeval* ts_end)
{
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec) * 1000000 + (double)ts_end->tv_usec -
           (double)ts_start->tv_usec;
  } else {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec - 1) * 1000000 + ((double)ts_end->tv_usec + 1000000) -
           (double)ts_start->tv_usec;
  }
}

static void usage(char* prog)
{
  printf("Usage: %s\n", prog);
  printf("\t-N Force symbol size, 0 for auto [Default %d]\n", force_symbol_sz);
  printf("\t-n Force number of Resource blocks [Default All]\n");
  printf("\t-e extended cyclic prefix [Default Normal]\n");
  printf("\t-f Number of frames [Default %d]\n", nof_frames);
  printf("\t-r Number of repetitions [Default %d]\n", nof_repetitions);
  printf("\t-m CFR mode: manual, auto_cma, auto_ema [Default %s]\n", cfr_mode_str);
  printf("\t-d Use DC subcarrier: [Default DC empty]\n");
  printf("\t-a CFR alpha: [Default %.2f]\n", alpha);
  printf("\t-t CFR manual threshold: [Default %.2f]\n", thr_manual);
  printf("\t-p CFR Max PAPR in dB (auto modes): [Default %.2f]\n", max_papr_db);
  printf("\t-E Power avg EMA alpha (EMA mode): [Default %.2f]\n", ema_alpha);
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "NnerfmatdpE")) != -1) {
    switch (opt) {
      case 'n':
        nof_prb = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'N':
        force_symbol_sz = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'e':
        cp = SRSRAN_CP_EXT;
        break;
      case 'r':
        nof_repetitions = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'f':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'm':
        cfr_mode_str = argv[optind];
        break;
      case 'a':
        alpha = strtof(argv[optind], NULL);
        break;
      case 't':
        thr_manual = strtof(argv[optind], NULL);
        break;
      case 'd':
        dc_empty = false;
        break;
      case 'p':
        max_papr_db = strtof(argv[optind], NULL);
        break;
      case 'E':
        ema_alpha = strtof(argv[optind], NULL);
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int ret = SRSRAN_ERROR;

  srsran_random_t random_gen = srsran_random_init(0);
  struct timeval  start, end;
  srsran_cfr_t    cfr         = {};
  cf_t*           input       = NULL;
  cf_t*           output      = NULL;
  cf_t*           error       = NULL;
  float*          acpr_buff   = NULL;
  float           mse_dB      = 0.0f;
  float           nmse_dB     = 0.0f;
  float           evm         = 0.0f;
  int             max_prb     = 0.0f;
  float           acpr_in_dB  = 0.0f;
  float           acpr_out_dB = 0.0f;

  srsran_dft_plan_t ofdm_ifft = {};
  srsran_dft_plan_t ofdm_fft  = {};

  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    ERROR("Error in parse_args");
    goto clean_exit;
  }

  cfr_mode = srsran_cfr_str2mode(cfr_mode_str);
  if (cfr_mode == SRSRAN_CFR_THR_INVALID) {
    ERROR("CFR mode is not recognised");
    goto clean_exit;
  }

  if (nof_prb == -1) {
    nof_prb = 6;
    max_prb = SRSRAN_MAX_PRB;
  } else {
    max_prb = nof_prb;
  }
  while (nof_prb <= max_prb) {
    const uint32_t symbol_sz      = (force_symbol_sz) ? force_symbol_sz : (uint32_t)srsran_symbol_sz(nof_prb);
    const uint32_t symbol_bw      = nof_prb * SRSRAN_NRE;
    const uint32_t nof_symb_slot  = SRSRAN_CP_NSYMB(cp);
    const uint32_t nof_symb_frame = nof_symb_slot * SRSRAN_NOF_SLOTS_PER_SF * SRSRAN_NOF_SF_X_FRAME;
    const uint32_t frame_sz       = symbol_sz * nof_symb_frame;
    const uint32_t total_nof_re   = frame_sz * nof_frames;
    const uint32_t total_nof_symb = nof_symb_frame * nof_frames;
    printf("Running test for %d PRB, %d Frames: \t", nof_prb, nof_frames);
    fflush(stdout);

    input     = srsran_vec_cf_malloc(total_nof_re);
    output    = srsran_vec_cf_malloc(total_nof_re);
    error     = srsran_vec_cf_malloc(total_nof_re);
    acpr_buff = srsran_vec_f_malloc(total_nof_symb);
    if (!input || !output || !error || !acpr_buff) {
      perror("malloc");
      goto clean_exit;
    }
    srsran_vec_cf_zero(input, total_nof_re);
    srsran_vec_cf_zero(output, total_nof_re);
    srsran_vec_cf_zero(error, total_nof_re);
    srsran_vec_f_zero(acpr_buff, total_nof_symb);

    // Set the parameters for the CFR.
    srsran_cfr_cfg_t cfr_tx_cfg = {};
    cfr_tx_cfg.cfr_enable       = true;
    cfr_tx_cfg.symbol_sz        = symbol_sz;
    cfr_tx_cfg.symbol_bw        = nof_prb * SRSRAN_NRE;
    cfr_tx_cfg.cfr_mode         = cfr_mode;
    cfr_tx_cfg.max_papr_db      = max_papr_db;
    cfr_tx_cfg.alpha            = alpha;
    cfr_tx_cfg.manual_thr       = thr_manual;
    cfr_tx_cfg.ema_alpha        = ema_alpha;
    cfr_tx_cfg.dc_sc            = dc_empty;

    if (!srsran_cfr_params_valid(&cfr_tx_cfg)) {
      ERROR("Invalid CFR configuration");
      goto clean_exit;
    }

    if (srsran_cfr_init(&cfr, &cfr_tx_cfg)) {
      ERROR("Error initializing CFR");
      goto clean_exit;
    }

    if (srsran_dft_plan_c(&ofdm_ifft, (int)symbol_sz, SRSRAN_DFT_BACKWARD)) {
      ERROR("Creating IFFT plan");
      goto clean_exit;
    }
    srsran_dft_plan_set_norm(&ofdm_ifft, true);
    if (srsran_dft_plan_c(&ofdm_fft, (int)symbol_sz, SRSRAN_DFT_FORWARD)) {
      ERROR("Creating FFT plan");
      goto clean_exit;
    }
    srsran_dft_plan_set_norm(&ofdm_fft, true);

    // Generate Random data
    cf_t* ofdm_symb = NULL;
    for (int i = 0; i < total_nof_symb; i++) {
      ofdm_symb = input + i * symbol_sz;
      srsran_random_uniform_complex_dist_vector(random_gen, ofdm_symb + dc_empty, symbol_bw / 2, -1.0f, +1.0f);
      srsran_random_uniform_complex_dist_vector(
          random_gen, ofdm_symb + symbol_sz - symbol_bw / 2, symbol_bw / 2, -1.0f, +1.0f);
      acpr_buff[i] = srsran_vec_acpr_c(ofdm_symb, symbol_bw / 2 + dc_empty, symbol_bw / 2, symbol_sz);
      srsran_dft_run_c(&ofdm_ifft, ofdm_symb, ofdm_symb);
    }
    // compute the average intput ACPR
    acpr_in_dB = srsran_vec_acc_ff(acpr_buff, total_nof_symb) / (float)total_nof_symb;
    acpr_in_dB = srsran_convert_power_to_dB(acpr_in_dB);

    // Execute CFR
    gettimeofday(&start, NULL);
    for (uint32_t i = 0; i < nof_repetitions; i++) {
      for (uint32_t j = 0; j < nof_frames; j++) {
        for (uint32_t k = 0; k < nof_symb_frame; k++) {
          srsran_cfr_process(&cfr,
                             input + (size_t)((k * symbol_sz) + (j * frame_sz)),
                             output + (size_t)((k * symbol_sz) + (j * frame_sz)));
        }
      }
    }
    gettimeofday(&end, NULL);
    printf("%.1fMsps \t", (float)(total_nof_re * nof_repetitions) / elapsed_us(&start, &end));

    // Compute metrics
    srsran_vec_sub_ccc(input, output, error, total_nof_re);

    float power_in  = srsran_vec_avg_power_cf(input, total_nof_re);
    float power_err = srsran_vec_avg_power_cf(error, total_nof_re);

    mse_dB  = srsran_convert_power_to_dB(power_err);
    nmse_dB = srsran_convert_power_to_dB(power_err / power_in);
    evm     = 100 * sqrtf(power_err / power_in);

    float snr_dB = srsran_convert_power_to_dB(power_in / power_err);

    float papr_in  = srsran_convert_power_to_dB(srsran_vec_papr_c(input, total_nof_re));
    float papr_out = srsran_convert_power_to_dB(srsran_vec_papr_c(output, total_nof_re));

    ofdm_symb = NULL;
    for (int i = 0; i < total_nof_symb; i++) {
      ofdm_symb = output + i * symbol_sz;
      srsran_dft_run_c(&ofdm_fft, ofdm_symb, ofdm_symb);
      acpr_buff[i] = srsran_vec_acpr_c(ofdm_symb, symbol_bw / 2 + dc_empty, symbol_bw / 2, symbol_sz);
    }

    // Compute the output average ACPR
    acpr_out_dB = srsran_vec_acc_ff(acpr_buff, total_nof_symb) / (float)total_nof_symb;
    acpr_out_dB = srsran_convert_power_to_dB(acpr_out_dB);

    printf("MSE=%.3fdB  NMSE=%.3fdB  EVM=%.3f%%  SNR=%.3fdB", mse_dB, nmse_dB, evm, snr_dB);
    printf("  In-PAPR=%.3fdB  Out-PAPR=%.3fdB", papr_in, papr_out);
    printf("  In-ACPR=%.3fdB  Out-ACPR=%.3fdB\n", acpr_in_dB, acpr_out_dB);

    srsran_dft_plan_free(&ofdm_ifft);
    srsran_dft_plan_free(&ofdm_fft);
    free(input);
    free(output);
    free(error);
    free(acpr_buff);
    input     = NULL;
    output    = NULL;
    error     = NULL;
    acpr_buff = NULL;

    ++nof_prb;
    if (acpr_out_dB > MAX_ACPR_DB) {
      printf("ACPR too large \n");
      goto clean_exit;
    }
  }
  ret = SRSRAN_SUCCESS;

  // Free resources
clean_exit:
  srsran_random_free(random_gen);
  srsran_cfr_free(&cfr);
  srsran_dft_plan_free(&ofdm_ifft);
  srsran_dft_plan_free(&ofdm_fft);
  if (input) {
    free(input);
  }
  if (output) {
    free(output);
  }
  if (error) {
    free(error);
  }
  if (acpr_buff) {
    free(acpr_buff);
  }
  return ret;
}
