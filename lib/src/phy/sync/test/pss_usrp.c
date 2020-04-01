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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

#ifdef ENABLE_GUI
void init_plots();
void do_plots_pss(float* corr, float energy, uint32_t size);
void do_plots_sss(float* corr_m0, float* corr_m1);
#endif /* ENABLE_GUI */

bool        tdd_mode      = false;
bool        disable_plots = false;
int         cell_id       = -1;
char*       rf_args       = "";
float       rf_gain = 40.0, rf_freq = -1.0;
int         nof_frames  = -1;
uint32_t    fft_size    = 64;
float       threshold   = 0.4;
int         N_id_2_sync = -1;
srslte_cp_t cp          = SRSLTE_CP_NORM;

void usage(char* prog)
{
  printf("Usage: %s [aedgtvnp] -f rx_frequency_hz -i cell_id\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l N_id_2 to sync [Default use cell_id]\n");
  printf("\t-e Extended CP [Default Normal]\n");
  printf("\t-s symbol_sz [Default %d]\n", fft_size);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-T TDD mode [Default FDD]\n");
#ifdef ENABLE_GUI
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "adgetTvnsfil")) != -1) {
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
      case 'e':
        cp = SRSLTE_CP_EXT;
        break;
      case 'i':
        cell_id = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'T':
        tdd_mode = true;
        break;
      case 'l':
        N_id_2_sync = (int)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        fft_size = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        nof_frames = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'd':
        disable_plots = true;
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (cell_id < 0 || rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}
float m0_value, m1_value;

int main(int argc, char** argv)
{
  cf_t*        buffer;
  int          frame_cnt, n;
  srslte_rf_t  rf;
  srslte_pss_t pss;
  srslte_cfo_t cfocorr, cfocorr64;
  srslte_sss_t sss;
  int32_t      flen;
  int          peak_idx, last_peak;
  float        peak_value;
  float        mean_peak;
  uint32_t     nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;
  cf_t         ce[SRSLTE_PSS_LEN];
  float        sfo = 0;

  parse_args(argc, argv);

  if (N_id_2_sync == -1) {
    N_id_2_sync = cell_id % 3;
  }
  uint32_t N_id_2 = cell_id % 3;
  uint32_t N_id_1 = cell_id / 3;

#ifdef ENABLE_GUI
  if (!disable_plots)
    init_plots();
#endif /* ENABLE_GUI */

  float srate = 15000.0 * fft_size;

  flen = srate * 5 / 1000;

  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    ERROR("Error opening rf\n");
    exit(-1);
  }

  printf("Set RX rate: %.2f MHz\n", srslte_rf_set_rx_srate(&rf, srate) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_set_rx_gain(&rf, rf_gain));
  printf("Set RX freq: %.2f MHz\n", srslte_rf_set_rx_freq(&rf, 0, rf_freq) / 1000000);

  buffer = srslte_vec_cf_malloc(flen * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_pss_init_fft(&pss, flen, fft_size)) {
    ERROR("Error initiating PSS\n");
    exit(-1);
  }

  if (srslte_pss_set_N_id_2(&pss, N_id_2_sync)) {
    ERROR("Error setting N_id_2=%d\n", N_id_2_sync);
    exit(-1);
  }

  srslte_cfo_init(&cfocorr, flen);
  srslte_cfo_init(&cfocorr64, flen);

  if (srslte_sss_init(&sss, fft_size)) {
    ERROR("Error initializing SSS object\n");
    exit(-1);
  }

  srslte_sss_set_N_id_2(&sss, N_id_2);

  printf("N_id_2: %d\n", N_id_2);

  srslte_rf_start_rx_stream(&rf, false);

  printf("Frame length %d samples\n", flen);
  printf("PSS detection threshold: %.2f\n", threshold);

  nof_det = nof_nodet = nof_nopeak = nof_nopeakdet = 0;
  frame_cnt                                        = 0;
  last_peak                                        = 0;
  mean_peak                                        = 0;
  int      peak_offset                             = 0;
  float    cfo;
  float    mean_cfo = 0;
  uint32_t m0, m1;
  uint32_t sss_error1 = 0, sss_error2 = 0, sss_error3 = 0;
  uint32_t cp_is_norm = 0;

  srslte_sync_t ssync;
  bzero(&ssync, sizeof(srslte_sync_t));
  ssync.fft_size = fft_size;

  uint32_t max_peak  = 0;
  uint32_t max_peak_ = 0;
  uint32_t min_peak  = fft_size;
  uint32_t min_peak_ = fft_size;

  pss.filter_pss_enable = true;

  while (frame_cnt < nof_frames || nof_frames == -1) {
    n = srslte_rf_recv(&rf, buffer, flen - peak_offset, 1);
    if (n < 0) {
      ERROR("Error receiving samples\n");
      exit(-1);
    }

    peak_idx = srslte_pss_find_pss(&pss, buffer, &peak_value);
    if (peak_idx < 0) {
      ERROR("Error finding PSS peak\n");
      exit(-1);
    }

    mean_peak = SRSLTE_VEC_CMA(peak_value, mean_peak, frame_cnt);

    if (peak_value >= threshold) {
      nof_det++;

      if (peak_idx >= fft_size) {

        // Estimate CFO
        cfo      = srslte_pss_cfo_compute(&pss, &buffer[peak_idx - fft_size]);
        mean_cfo = SRSLTE_VEC_CMA(cfo, mean_cfo, frame_cnt);

        // Correct CFO
        srslte_cfo_correct(&cfocorr, buffer, buffer, -mean_cfo / fft_size);

        // Estimate channel
        if (srslte_pss_chest(&pss, &buffer[peak_idx - fft_size], ce)) {
          ERROR("Error computing channel estimation\n");
          exit(-1);
        }

        // Find SSS
        int sss_idx;
        if (!tdd_mode) {
          sss_idx = peak_idx - 2 * fft_size -
                    (SRSLTE_CP_ISNORM(cp) ? SRSLTE_CP_LEN(fft_size, SRSLTE_CP_NORM_LEN)
                                          : SRSLTE_CP_LEN(fft_size, SRSLTE_CP_EXT_LEN));
        } else {
          sss_idx = peak_idx - 4 * fft_size -
                    3 * (SRSLTE_CP_ISNORM(cp) ? SRSLTE_CP_LEN(fft_size, SRSLTE_CP_NORM_LEN)
                                              : SRSLTE_CP_LEN(fft_size, SRSLTE_CP_EXT_LEN));
        }
        if (sss_idx >= 0 && sss_idx < flen - fft_size) {

          // Filter SSS
          srslte_pss_filter(&pss, &buffer[sss_idx], &buffer[sss_idx]);

          INFO("Full N_id_1: %d\n", srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value));
          srslte_sss_m0m1_partial(&sss, &buffer[sss_idx], 1, ce, &m0, &m0_value, &m1, &m1_value);
          if (srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value) != N_id_1) {
            sss_error2++;
          }
          INFO("Partial N_id_1: %d\n", srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value));
          srslte_sss_m0m1_diff_coh(&sss, &buffer[sss_idx], ce, &m0, &m0_value, &m1, &m1_value);
          if (srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value) != N_id_1) {
            sss_error3++;
          }
          INFO("Diff N_id_1: %d\n", srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value));
        }
        srslte_sss_m0m1_partial(&sss, &buffer[sss_idx], 1, NULL, &m0, &m0_value, &m1, &m1_value);
        if (srslte_sss_N_id_1(&sss, m0, m1, m1_value + m0_value) != N_id_1) {
          sss_error1++;
        }

        // Estimate CP
        if (peak_idx > 2 * (fft_size + SRSLTE_CP_LEN_EXT(fft_size))) {
          srslte_cp_t cp = srslte_sync_detect_cp(&ssync, buffer, peak_idx);
          if (SRSLTE_CP_ISNORM(cp)) {
            cp_is_norm++;
          }
        }

      } else {
        INFO("No space for CFO computation. Frame starts at \n");
      }

      if (srslte_sss_subframe(m0, m1) == 0) {
#ifdef ENABLE_GUI
        if (!disable_plots)
          do_plots_sss(sss.corr_output_m0, sss.corr_output_m1);
#endif /* ENABLE_GUI */
      }

    } else {
      nof_nodet++;
    }

    printf("[%5d]: Pos: %5d (%d-%d), PSR: %4.1f (~%4.1f) Pdet: %4.2f, "
           "FA: %4.2f, CFO: %+7.1f Hz, SFO: %+.2f Hz SSSmiss: %4.2f/%4.2f/%4.2f CPNorm: %.0f%%\r",
           frame_cnt,
           peak_idx,
           min_peak_,
           max_peak_,
           peak_value,
           mean_peak,
           (float)nof_det / frame_cnt,
           (float)nof_nopeakdet / frame_cnt,
           mean_cfo * 15000,
           sfo,
           (float)sss_error1 / nof_det,
           (float)sss_error2 / nof_det,
           (float)sss_error3 / nof_det,
           (float)cp_is_norm / nof_det * 100);

    if (frame_cnt > 100) {
      if (abs(last_peak - peak_idx) > 4) {
        if (peak_value >= threshold) {
          nof_nopeakdet++;
        }
        nof_nopeak++;
      }

      sfo = SRSLTE_VEC_CMA((peak_idx - last_peak) / 5e-3, sfo, frame_cnt);

      int      frame_idx          = frame_cnt % 200;
      uint32_t peak_offset_symbol = peak_idx % fft_size;
      if (peak_offset_symbol > max_peak) {
        max_peak = peak_offset_symbol;
      }
      if (peak_offset_symbol < min_peak) {
        min_peak = peak_offset_symbol;
      }
      if (!frame_idx) {
        max_peak_ = max_peak;
        min_peak_ = min_peak;
        max_peak  = 0;
        min_peak  = fft_size;
      }
    }

    frame_cnt++;

    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }

#ifdef ENABLE_GUI
    if (!disable_plots) {
      do_plots_pss(pss.conv_output_avg, pss.conv_output_avg[peak_idx], pss.fft_size + pss.frame_size - 1);
    }
#endif /* ENABLE_GUI */

    last_peak = peak_idx;
  }

  srslte_sss_free(&sss);
  srslte_pss_free(&pss);
  free(buffer);
  srslte_rf_close(&rf);

  printf("Ok\n");
  exit(0);
}

extern cf_t* tmp2;

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifdef ENABLE_GUI

#include "srsgui/srsgui.h"
plot_real_t pssout;
// plot_complex_t pce;

plot_real_t psss1;

float tmp[1000000];
cf_t  tmpce[SRSLTE_PSS_LEN];

void init_plots()
{
  sdrgui_init();
  plot_real_init(&pssout);
  plot_real_setTitle(&pssout, "PSS xCorr");
  plot_real_setLabels(&pssout, "Index", "Absolute value");
  plot_real_setYAxisScale(&pssout, 0, 1);

  /*
  plot_complex_init(&pce);
  plot_complex_setTitle(&pce, "Channel Estimates");
  plot_complex_setYAxisScale(&pce, Ip, -2, 2);
  plot_complex_setYAxisScale(&pce, Q, -2, 2);
  plot_complex_setYAxisScale(&pce, Magnitude, 0, 2);
  plot_complex_setYAxisScale(&pce, Phase, -M_PI, M_PI);
  */

  plot_real_init(&psss1);
  plot_real_setTitle(&psss1, "SSS xCorr m0");
  plot_real_setLabels(&psss1, "Index", "Absolute value");
  plot_real_setYAxisScale(&psss1, 0, 1);
}

void do_plots_pss(float* corr, float peak, uint32_t size)
{
  srslte_vec_sc_prod_fff(corr, 1. / peak, tmp, size);
  plot_real_setNewData(&pssout, tmp, size);
}

void do_plots_sss(float* corr_m0, float* corr_m1)
{
  if (m0_value > 0)
    srslte_vec_sc_prod_fff(corr_m0, 1. / m0_value, corr_m0, SRSLTE_SSS_N);
  plot_real_setNewData(&psss1, corr_m0, SRSLTE_SSS_N);
}

#endif /* ENABLE_GUI */
