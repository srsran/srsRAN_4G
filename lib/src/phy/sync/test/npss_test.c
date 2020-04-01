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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srslte/phy/sync/npss.h"
#include "srslte/srslte.h"

#define OUTPUT_FILENAME "npss_test.m"
void write_to_file();

#define DUMP_SIGNALS 0

int input_len = SRSLTE_SF_LEN(SRSLTE_NBIOT_FFT_SIZE);

void usage(char* prog)
{
  printf("Usage: %s [cpoev]\n", prog);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "lv")) != -1) {
    switch (opt) {
      case 'l':
        input_len = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'v':
        srslte_verbose = SRSLTE_VERBOSE_DEBUG;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  cf_t* fft_buffer;
  cf_t* input_buffer;

  srslte_npss_synch_t syncobj;
  srslte_ofdm_t       ifft;
  struct timeval      t[3];
  int                 fft_size;
  int                 peak_pos;
  float               peak_value;
  int                 ret = SRSLTE_ERROR;

  parse_args(argc, argv);

  if (input_len < SRSLTE_SF_LEN(SRSLTE_NBIOT_FFT_SIZE)) {
    fprintf(stderr, "Input len too small (%d), must be at least one subframe\n", input_len);
    exit(-1);
  }

  fft_size = srslte_symbol_sz(SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL);
  if (fft_size < 0) {
    fprintf(stderr, "Invalid nof_prb=%d\n", SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL);
    exit(-1);
  }

  printf("Input buffer length is %d samples\n", input_len);
  uint32_t buffer_len = input_len + SRSLTE_NPSS_CORR_FILTER_LEN + 1;
  fft_buffer          = srslte_vec_cf_malloc(buffer_len);
  if (!fft_buffer) {
    perror("malloc");
    exit(-1);
  }
  srslte_vec_cf_zero(fft_buffer, buffer_len);

  input_buffer = srslte_vec_cf_malloc(input_len);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }
  srslte_vec_cf_zero(input_buffer, input_len);

  if (srslte_ofdm_tx_init(&ifft, SRSLTE_CP_NORM, input_buffer, fft_buffer, SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL)) {
    fprintf(stderr, "Error creating iFFT object\n");
    exit(-1);
  }
  srslte_ofdm_set_freq_shift(&ifft, -SRSLTE_NBIOT_FREQ_SHIFT_FACTOR);

  if (srslte_npss_synch_init(&syncobj, input_len, fft_size)) {
    fprintf(stderr, "Error initializing NPSS object\n");
    return SRSLTE_ERROR;
  }

  // generate NPSS/NSSS signals
  _Complex float npss_signal[SRSLTE_NPSS_TOT_LEN];
  srslte_npss_generate(npss_signal);
  srslte_npss_put_subframe(
      &syncobj, npss_signal, input_buffer, SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL, SRSLTE_NBIOT_DEFAULT_PRB_OFFSET);

  // Transform to OFDM symbols
  srslte_ofdm_tx_sf(&ifft);

  // look for NPSS signal
  gettimeofday(&t[1], NULL);
  peak_pos = srslte_npss_sync_find(&syncobj, fft_buffer, &peak_value);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("NPPS with peak=%f found at: %d (in %.0f usec)\n",
         peak_value,
         peak_pos,
         (int)t[0].tv_sec * 1e6 + (int)t[0].tv_usec);

  // write results to file
#if DUMP_SIGNALS
  srslte_vec_save_file("npss_find_conv_output_abs.bin", syncobj.conv_output_abs, buffer_len * sizeof(float));
  srslte_vec_save_file("npss_sf_time.bin", fft_buffer, input_len * sizeof(cf_t));
  srslte_vec_save_file("npss_corr_seq_time.bin", syncobj.npss_signal_time, SRSLTE_NPSS_CORR_FILTER_LEN * sizeof(cf_t));
  write_to_file();
#endif

  // cleanup
  srslte_npss_synch_free(&syncobj);
  free(fft_buffer);
  free(input_buffer);
  srslte_ofdm_tx_free(&ifft);


  if (peak_pos == SRSLTE_NPSS_CORR_OFFSET) {
    printf("Ok\n");
    ret = SRSLTE_SUCCESS;
  } else {
    printf("Failed\n");
  }

  return ret;
}

void write_to_file()
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
  fprintf(debug_fsink.f, "title('Correlation sequence time-domain');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the generated subframe
  fprintf(debug_fsink.f, "len = %u;\n", input_len);
  fprintf(debug_fsink.f, "sig=read_complex('npss_sf_time.bin', len);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t= 1:len;\n");
  fprintf(debug_fsink.f, "plot(t,real(sig),t,imag(sig));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Subframe time-domain');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the correlation output
  fprintf(debug_fsink.f, "num_samples = %u;\n", SRSLTE_NPSS_CORR_FILTER_LEN + input_len - 1);
  fprintf(debug_fsink.f, "sig = read_real('npss_find_conv_output_abs.bin', num_samples);\n");
  fprintf(debug_fsink.f, "t=1:num_samples;\n");
  fprintf(debug_fsink.f, "\n\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(t,sig);\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Convolution output absolute');\n");
  fprintf(debug_fsink.f, "ylabel('Correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  // cross-correlation in octave
  fprintf(debug_fsink.f, "[corr, lag] = xcorr(sig1,sig1);\n");
  fprintf(debug_fsink.f, "\n\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(abs(corr));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Correlation in Octave');\n");
  fprintf(debug_fsink.f, "ylabel('Correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  srslte_filesink_free(&debug_fsink);
  printf("data written to %s\n", OUTPUT_FILENAME);
}
