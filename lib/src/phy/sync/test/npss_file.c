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

#include "srslte/phy/io/filesink.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/sync/npss.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#define OUTPUT_FILENAME "npss_file.m"
void write_to_file();

srslte_nbiot_cell_t cell = {
    .base           = {.nof_prb = 1, .cp = SRSLTE_CP_NORM, .id = 0},
    .base.nof_ports = 1,
    .nbiot_prb      = 0,
};

bool        disable_plots = false;
char*       input_file_name;
int         cell_id            = -1;
int         nof_frames         = 1;
uint32_t    fft_size           = 128;
float       threshold          = 0.4;
int         N_id_2_sync        = -1;
srslte_cp_t cp                 = SRSLTE_CP_NORM;
int         file_offset        = 0;
bool        save_frame_to_file = false;

#define FLEN (fft_size * 15 * 10) // for one entire frame

void usage(char* prog)
{
  printf("Usage: %s [nlestodv] -i cell_id -f input_file_name\n", prog);
  printf("\t-n nof_frames [Default %d]\n", nof_frames);
  printf("\t-l N_id_2 to sync [Default use cell_id]\n");
  printf("\t-s Safe to aligned frame to file [Default %d]\n", save_frame_to_file);
  printf("\t-t threshold [Default %.2f]\n", threshold);
  printf("\t-o file read offset [Default %d]\n", file_offset);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nlstvof")) != -1) {
    switch (opt) {
      case 'f':
        input_file_name = argv[optind];
        break;
      case 't':
        threshold = strtof(argv[optind], NULL);
        break;
      case 'o':
        file_offset = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'l':
        N_id_2_sync = (int)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        save_frame_to_file = true;
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
}
float m0_value, m1_value;

int main(int argc, char** argv)
{
  srslte_filesource_t fsrc;
  cf_t*               buffer;
  int                 frame_cnt, n;
  srslte_npss_synch_t npss;
  int                 peak_idx, last_peak;
  float               peak_value;
  float               mean_peak;
  uint32_t            nof_det, nof_nodet, nof_nopeak, nof_nopeakdet;

  parse_args(argc, argv);

  buffer = srslte_vec_cf_malloc(FLEN * 2);
  if (!buffer) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_npss_synch_init(&npss, FLEN, fft_size)) {
    fprintf(stderr, "Error initializing NPSS object\n");
    exit(-1);
  }

  printf("Opening file...\n");
  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }

  printf("Frame length %d samples\n", FLEN);
  printf("NPSS detection threshold: %.2f\n", threshold);

  nof_det = nof_nodet = nof_nopeak = nof_nopeakdet = 0;
  frame_cnt                                        = 0;
  last_peak                                        = 0;
  mean_peak                                        = 0;
  int peak_offset                                  = 0;

  n = srslte_filesource_read(&fsrc, buffer, file_offset);

  bool save_and_exit = false;
  while (frame_cnt < nof_frames || nof_frames == -1) {
    n = srslte_filesource_read(&fsrc, buffer, FLEN - peak_offset);
    if (n < 0) {
      fprintf(stderr, "Error reading samples\n");
      exit(-1);
    }
    if (n < FLEN / 10) {
      fprintf(stdout, "End of file (n=%d, flen=%d, peak=%d)\n", n, FLEN, peak_offset);
      break;
    }

    if (save_frame_to_file && save_and_exit) {
      char* filename = "frame_hyp.bin";
      printf("Saving entire frame to %s\n", filename);
      srslte_vec_save_file(filename, buffer, FLEN * sizeof(cf_t));
      exit(-1);
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
        cf_t dummy[FLEN]; // full frame
        printf("Peak_idx at %d\n", peak_idx);
        int num_drop = peak_idx - SRSLTE_NPSS_CORR_OFFSET + FLEN / 2;
        printf("Dropping %d samples!\n", num_drop);

        if (num_drop > FLEN) {
          printf("wrapping num drop to %d\n", num_drop);
          num_drop = num_drop % FLEN;
        }

        srslte_filesource_read(&fsrc, dummy, num_drop);
        save_and_exit = true;
      }

    } else {
      nof_nodet++;
    }

    if (frame_cnt > 100) {
      if (abs(last_peak - peak_idx) > 4) {
        if (peak_value >= threshold) {
          nof_nopeakdet++;
        }
        nof_nopeak++;
      }
    }

    frame_cnt++;

    printf("[%5d]: Pos: %5d, PSR: %4.1f (~%4.1f) Pdet: %4.2f, "
           "FA: %4.2f\n",
           frame_cnt,
           (peak_value > threshold) ? peak_idx : 0,
           peak_value,
           mean_peak,
           (float)nof_det / frame_cnt,
           (float)nof_nopeakdet / frame_cnt);

    if (SRSLTE_VERBOSE_ISINFO()) {
      printf("\n");
    }

    usleep(10000);

    last_peak = peak_idx;
  }

  printf("NPSS detected #%d\n", nof_det);

  srslte_vec_save_file(
      "npss_find_conv_output_abs.bin", npss.conv_output_abs, (FLEN + SRSLTE_NPSS_CORR_FILTER_LEN) * sizeof(float));
  srslte_vec_save_file("npss_corr_seq_time.bin", npss.npss_signal_time, SRSLTE_NPSS_CORR_FILTER_LEN * sizeof(cf_t));

  write_to_file();

  srslte_npss_synch_free(&npss);
  free(buffer);
  srslte_filesource_free(&fsrc);

  printf("Ok\n");
  exit(0);
}

float tmp[1000000];
void  write_to_file()
{
  srslte_filesink_t debug_fsink;
  char              fname[] = OUTPUT_FILENAME;
  if (srslte_filesink_init(&debug_fsink, fname, SRSLTE_TEXT)) {
    fprintf(stderr, "Error opening file %s\n", fname);
    exit(-1);
  }

  fprintf(debug_fsink.f, "%% %s : auto-generated file\n", OUTPUT_FILENAME);
  fprintf(debug_fsink.f, "clear all;\n");
  fprintf(debug_fsink.f, "close all;\n");
  fprintf(debug_fsink.f, "pkg load signal;\n\n");

  // the correlation sequence
  fprintf(debug_fsink.f, "len = %u;\n", SRSLTE_NPSS_CORR_FILTER_LEN);
  fprintf(debug_fsink.f, "sig1=read_complex('npss_corr_seq_time.bin', len);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t=1:len;\n");
  fprintf(debug_fsink.f, "plot(t,real(sig1),t,imag(sig1));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title(\"Correlation sequence time-domain\");\n");
  fprintf(debug_fsink.f, "\n\n");

  // the FFT of the first symbol after the frequency correction
  fprintf(debug_fsink.f, "npss_sym0=sig1(10:137);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(real(fftshift(fft(npss_sym0.*exp(-2*pi*1i*(0:127)'*.5/128), 128))));\n");
  fprintf(debug_fsink.f, "title(\"FFT of first symbol after frequency correction\");\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the input signal
  fprintf(debug_fsink.f, "len = %u;\n", FLEN);
  fprintf(debug_fsink.f, "sig=read_complex('%s', len);\n", input_file_name);
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t= 1:length(sig);\n");
  fprintf(debug_fsink.f, "plot(t,real(sig),t,imag(sig));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title(\"Subframe time-domain\");\n");
  fprintf(debug_fsink.f, "\n\n");

  // the correlation output
  fprintf(debug_fsink.f, "num_samples = %u;\n", SRSLTE_NPSS_CORR_FILTER_LEN + FLEN - 1);
  fprintf(debug_fsink.f, "conv = read_real('npss_find_conv_output_abs.bin', num_samples);\n");
  fprintf(debug_fsink.f, "t=1:length(conv);\n");
  fprintf(debug_fsink.f, "\n\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(t,conv);\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title(\"Convolution output absolute\");\n");
  fprintf(debug_fsink.f, "ylabel('Correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  // cross-correlation in octave
  fprintf(debug_fsink.f, "[corr, lag] = xcorr(sig,sig1);\n");
  fprintf(debug_fsink.f, "\n\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(abs(corr));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title(\"Correlation in Octave\");\n");
  fprintf(debug_fsink.f, "ylabel('Correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  srslte_filesink_free(&debug_fsink);
  printf("data written to %s\n", OUTPUT_FILENAME);
}
