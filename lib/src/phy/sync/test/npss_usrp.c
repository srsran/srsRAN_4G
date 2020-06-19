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

#include "srslte/phy/rf/rf.h"
#include "srslte/phy/sync/npss.h"
#include "srslte/srslte.h"

#ifndef DISABLE_GRAPHICS
void init_plots();
void do_plots_npss(float* corr, float energy, uint32_t size);
#endif

bool     disable_plots = false;
char*    rf_args       = "";
float    rf_gain = 40.0, rf_freq = -1.0;
int      nof_frames         = -1;
uint32_t fft_size           = 128;
float    threshold          = 0.4;
bool     save_frame_to_file = false;
float    cfo_fixed          = 0.0;
bool     has_cfo_corr       = true;

srslte_nbiot_cell_t cell = {
    .base       = {.nof_prb = 1, .cp = SRSLTE_CP_NORM, .nof_ports = 1, .id = 0},
    .nbiot_prb  = 0,
    .n_id_ncell = 0,
};

void usage(char* prog)
{
  printf("Usage: %s [adgtvnp] -f rx_frequency_hz -i cell_id\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-C Disable CFO correction [Default %s]\n", has_cfo_corr ? "Enabled" : "Disabled");
  printf("\t-c Manual CFO offset [Default %.0f Hz]\n", cfo_fixed);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-s Save frame to file [Default %d]\n", save_frame_to_file);
  printf("\t-t threshold [Default %.2f]\n", threshold);
#ifndef DISABLE_GRAPHICS
  printf("\t-d disable plots [Default enabled]\n");
#else
  printf("\t plots are disabled. Graphics library not available\n");
#endif
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "aCcdgtvsfi")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'C':
        has_cfo_corr = false;
        break;
      case 'c':
        cfo_fixed = strtof(argv[optind], NULL);
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
      case 'i':
        cell.base.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        save_frame_to_file = true;
        disable_plots      = true;
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
  if (rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}

float m0_value, m1_value;

bool go_exit = false;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  }
}

int main(int argc, char** argv)
{
  cf_t*               buffer;
  int                 frame_cnt, n;
  srslte_rf_t         rf;
  srslte_cfo_t        cfocorr;
  srslte_npss_synch_t npss;
  int32_t             flen;
  int                 peak_idx;
  float               peak_value;
  float               mean_peak;
  uint32_t            nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;

  parse_args(argc, argv);

#ifndef DISABLE_GRAPHICS
  if (!disable_plots)
    init_plots();
#endif

  signal(SIGINT, sig_int_handler);

  float srate = 15000.0 * fft_size;
  flen        = srate * 10 / 1000;

  printf("Frame length %d samples\n", flen);
  printf("NPSS detection threshold: %.2f\n", threshold);

  if (cfo_fixed) {
    printf("Manually compensating %.0f Hz CFO offset\n", cfo_fixed);
  }

  if (srslte_cfo_init(&cfocorr, flen)) {
    fprintf(stderr, "Error initiating CFO\n");
    exit(-1);
  }
  srslte_cfo_set_tol(&cfocorr, 50.0 / (15000.0 * fft_size));

  printf("Opening RF device...\n");
  if (srslte_rf_open(&rf, rf_args)) {
    fprintf(stderr, "Error opening rf\n");
    exit(-1);
  }

  srslte_rf_set_rx_gain(&rf, rf_gain);
  printf("Set RX rate: %.2f MHz\n", srslte_rf_set_rx_srate(&rf, srate) / 1000000);
  printf("Set RX gain: %.1f dB\n", srslte_rf_get_rx_gain(&rf));
  printf("Set RX freq: %.2f MHz\n", srslte_rf_set_rx_freq(&rf, 0, rf_freq) / 1000000);

  buffer = srslte_vec_cf_malloc(flen * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }
  srslte_vec_cf_zero(buffer, flen * 2);

  if (srslte_npss_synch_init(&npss, flen, fft_size)) {
    fprintf(stderr, "Error initializing NPSS object\n");
    exit(-1);
  }

  srslte_rf_start_rx_stream(&rf, false);

  nof_det = nof_nodet = nof_nopeak = nof_nopeakdet = 0;
  frame_cnt                                        = 0;
  mean_peak                                        = 0;
  peak_idx                                         = 0;
  int peak_offset                                  = 0;

  bool save_and_exit = false;
  while ((frame_cnt < nof_frames || nof_frames == -1) && !go_exit) {
    n = srslte_rf_recv(&rf, buffer, flen - peak_offset, 1);
    if (n < 0) {
      fprintf(stderr, "Error receiving samples\n");
      exit(-1);
    }

    frame_cnt++;

    if (save_frame_to_file && save_and_exit) {
      char* filename = "frame_hyp.bin";
      printf("Saving entire frame to %s\n", filename);
      srslte_vec_save_file(filename, buffer, flen * sizeof(cf_t));
      go_exit = true;
    }

    // perform CFO correction
    if (has_cfo_corr) {
      srslte_cfo_correct(&cfocorr, buffer, buffer, -cfo_fixed / (15000 * fft_size));
    }

    peak_idx = srslte_npss_sync_find(&npss, buffer, &peak_value);
    if (peak_idx < 0) {
      fprintf(stderr, "Error finding NPSS peak\n");
      exit(-1);
    }

    mean_peak = SRSLTE_VEC_CMA(peak_value, mean_peak, frame_cnt);

    if (peak_value >= threshold) {
      nof_det++;

      // try to align frame
      if (save_frame_to_file && !save_and_exit) {
        cf_t dummy[flen]; // full frame
        printf("Peak_idx at %d\n", peak_idx);
        int num_drop = peak_idx - SRSLTE_NPSS_CORR_OFFSET + flen / 2;
        printf("Dropping %d samples!\n", num_drop);

        if (num_drop > flen) {
          printf("wrapping num drop to %d\n", num_drop);
          num_drop = num_drop % flen;
        }

        srslte_rf_recv(&rf, dummy, num_drop, 1);
        save_and_exit = true;
      }
    } else {
      nof_nodet++;
    }
    printf("[%5d]: Pos: %5d, PSR: %4.1f (~%4.1f) Pdet: %4.2f, "
           "FA: %4.2f\r",
           frame_cnt,
           (peak_value > threshold) ? (peak_idx - SRSLTE_NPSS_CORR_OFFSET - flen / 10 / 2) : 0,
           peak_value,
           mean_peak,
           (float)nof_det / frame_cnt,
           (float)nof_nopeakdet / frame_cnt);

    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }

#ifndef DISABLE_GRAPHICS
    if (!disable_plots) {
      int len = SRSLTE_NPSS_CORR_FILTER_LEN + npss.frame_size - 1;
      do_plots_npss(npss.conv_output_avg, npss.conv_output_avg[peak_idx], len);
    }
#endif
  }

  printf("NPSS detected #%d\n", nof_det);

  srslte_npss_synch_free(&npss);
  srslte_cfo_free(&cfocorr);
  free(buffer);
  srslte_rf_close(&rf);

  printf("Ok\n");
  exit(0);
}

/**********************************************************************
 *  Plotting Functions
 ***********************************************************************/
#ifndef DISABLE_GRAPHICS

#include "srsgui/srsgui.h"
plot_real_t pssout;
plot_real_t psss1;

float tmp[1000000];

void init_plots()
{
  sdrgui_init();
  plot_real_init(&pssout);
  plot_real_setTitle(&pssout, "NPSS xCorr");
  plot_real_setLabels(&pssout, "Index", "Absolute value");
  plot_real_setYAxisScale(&pssout, 0, 1);
}

void do_plots_npss(float* corr, float peak, uint32_t size)
{
  srslte_vec_sc_prod_fff(corr, 1. / peak, tmp, size);
  plot_real_setNewData(&pssout, tmp, size);
}

#endif
