/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/sync/npss.h"
#include "srsran/srsran.h"

#define OUTPUT_FILENAME "npss_test.m"
void write_to_file();

#define DUMP_SIGNALS 0

int input_len = SRSRAN_SF_LEN(SRSRAN_NBIOT_FFT_SIZE);

void usage(char* prog)
{
  printf("Usage: %s [cpoev]\n", prog);
  printf("\t-v srsran_verbose\n");
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
        set_srsran_verbose_level(SRSRAN_VERBOSE_DEBUG);
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

  srsran_npss_synch_t syncobj;
  srsran_ofdm_t       ifft;
  struct timeval      t[3];
  int                 fft_size;
  int                 peak_pos;
  float               peak_value;
  int                 ret = SRSRAN_ERROR;

  parse_args(argc, argv);

  if (input_len < SRSRAN_SF_LEN(SRSRAN_NBIOT_FFT_SIZE)) {
    fprintf(stderr, "Input len too small (%d), must be at least one subframe\n", input_len);
    exit(-1);
  }

  fft_size = srsran_symbol_sz(SRSRAN_NBIOT_DEFAULT_NUM_PRB_BASECELL);
  if (fft_size < 0) {
    fprintf(stderr, "Invalid nof_prb=%d\n", SRSRAN_NBIOT_DEFAULT_NUM_PRB_BASECELL);
    exit(-1);
  }

  printf("Input buffer length is %d samples\n", input_len);
  uint32_t buffer_len = input_len + SRSRAN_NPSS_CORR_FILTER_LEN + 1;
  fft_buffer          = srsran_vec_cf_malloc(buffer_len);
  if (!fft_buffer) {
    perror("malloc");
    exit(-1);
  }
  srsran_vec_cf_zero(fft_buffer, buffer_len);

  input_buffer = srsran_vec_cf_malloc(input_len);
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }
  srsran_vec_cf_zero(input_buffer, input_len);

  if (srsran_ofdm_tx_init(&ifft, SRSRAN_CP_NORM, input_buffer, fft_buffer, SRSRAN_NBIOT_DEFAULT_NUM_PRB_BASECELL)) {
    fprintf(stderr, "Error creating iFFT object\n");
    exit(-1);
  }
  srsran_ofdm_set_freq_shift(&ifft, -SRSRAN_NBIOT_FREQ_SHIFT_FACTOR);

  if (srsran_npss_synch_init(&syncobj, input_len, fft_size)) {
    fprintf(stderr, "Error initializing NPSS object\n");
    return SRSRAN_ERROR;
  }

  // generate NPSS/NSSS signals
  _Complex float npss_signal[SRSRAN_NPSS_TOT_LEN];
  srsran_npss_generate(npss_signal);
  srsran_npss_put_subframe(
      &syncobj, npss_signal, input_buffer, SRSRAN_NBIOT_DEFAULT_NUM_PRB_BASECELL, SRSRAN_NBIOT_DEFAULT_PRB_OFFSET);

  // Transform to OFDM symbols
  srsran_ofdm_tx_sf(&ifft);

  // look for NPSS signal
  gettimeofday(&t[1], NULL);
  peak_pos = srsran_npss_sync_find(&syncobj, fft_buffer, &peak_value);
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("NPPS with peak=%f found at: %d (in %.0f usec)\n",
         peak_value,
         peak_pos,
         (int)t[0].tv_sec * 1e6 + (int)t[0].tv_usec);

  // write results to file
#if DUMP_SIGNALS
  srsran_vec_save_file("npss_find_conv_output_abs.bin", syncobj.conv_output_abs, buffer_len * sizeof(float));
  srsran_vec_save_file("npss_sf_time.bin", fft_buffer, input_len * sizeof(cf_t));
  srsran_vec_save_file("npss_corr_seq_time.bin", syncobj.npss_signal_time, SRSRAN_NPSS_CORR_FILTER_LEN * sizeof(cf_t));
  write_to_file();
#endif

  // cleanup
  srsran_npss_synch_free(&syncobj);
  free(fft_buffer);
  free(input_buffer);
  srsran_ofdm_tx_free(&ifft);

  if (peak_pos == SRSRAN_NPSS_CORR_OFFSET) {
    printf("Ok\n");
    ret = SRSRAN_SUCCESS;
  } else {
    printf("Failed\n");
  }

  return ret;
}

void write_to_file()
{
  srsran_filesink_t debug_fsink;
  char              fname[] = OUTPUT_FILENAME;
  if (srsran_filesink_init(&debug_fsink, fname, SRSRAN_TEXT)) {
    fprintf(stderr, "Error opening file %s\n", fname);
    exit(-1);
  }

  fprintf(debug_fsink.f, "%% %s : auto-generated file\n", OUTPUT_FILENAME);
  fprintf(debug_fsink.f, "clear all;\n");
  fprintf(debug_fsink.f, "close all;\n");
  fprintf(debug_fsink.f, "pkg load signal;\n\n");

  // the correlation sequence
  fprintf(debug_fsink.f, "len = %u;\n", SRSRAN_NPSS_CORR_FILTER_LEN);
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
  fprintf(debug_fsink.f, "num_samples = %u;\n", SRSRAN_NPSS_CORR_FILTER_LEN + input_len - 1);
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

  srsran_filesink_free(&debug_fsink);
  printf("data written to %s\n", OUTPUT_FILENAME);
}
