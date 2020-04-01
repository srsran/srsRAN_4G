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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <stdbool.h>

#include "srslte/phy/sync/sync_nbiot.h"
#include "srslte/srslte.h"

int   offset = 0;
float cfo    = 0.0;
float snr    = -1.0;

#define OUTPUT_FILENAME "sync_nbiot_test.m"
void  write_to_file();
char* input_file_name;

#define DUMP_SIGNALS 1
#define MAX_CORR_LEN 10000
#define SFLEN (10 * SRSLTE_SF_LEN(128))

void usage(char* prog)
{
  printf("Usage: %s [cgofv] -f input_file_name\n", prog);
  printf("\t-c add CFO [Default %f]\n", cfo);
  printf("\t-g add AWGN with target SNR [Default off]\n");
  printf("\t-o offset [Default %d]\n", offset);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "cgofv")) != -1) {
    switch (opt) {
      case 'f':
        input_file_name = argv[optind];
        break;
      case 'c':
        cfo = strtof(argv[optind], NULL);
        break;
      case 'g':
        snr = strtof(argv[optind], NULL);
        break;
      case 'o':
        offset = (int)strtol(argv[optind], NULL, 10);
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
  int            ret        = SRSLTE_ERROR;
  int            sf_idx     = 0;
  cf_t*          fft_buffer = NULL;
  _Complex float buffer[SFLEN]; // FLEN + fft_size

  srslte_filesource_t fsrc     = {};
  uint32_t            find_idx = 0;
  srslte_sync_nbiot_t syncobj  = {};
  srslte_ofdm_t       ifft     = {};
  srslte_cfo_t        cfocorr  = {};
  int                 fft_size = 0;

  input_file_name = NULL;

  parse_args(argc, argv);

  if (input_file_name != NULL) {
    printf("Opening file...\n");
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", input_file_name);
      goto exit;
    }
  }

  fft_size = srslte_symbol_sz(SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL);
  if (fft_size < 0) {
    fprintf(stderr, "Invalid nof_prb=%d\n", SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL);
    exit(-1);
  }

  printf("SFLEN is %d samples\n", SFLEN);
  fft_buffer = srslte_vec_cf_malloc(SFLEN * 2);
  if (!fft_buffer) {
    perror("malloc");
    goto exit;
  }

  memset(buffer, 0, sizeof(cf_t) * SFLEN);

  if (srslte_cfo_init(&cfocorr, SFLEN)) {
    fprintf(stderr, "Error initiating CFO\n");
    goto exit;
  }
  // Set a CFO tolerance of approx 100 Hz
  srslte_cfo_set_tol(&cfocorr, 100.0 / (15000.0 * fft_size));

  // init synch object for a maximum SFLEN samples
  if (srslte_sync_nbiot_init(&syncobj, SFLEN, SFLEN, fft_size)) {
    fprintf(stderr, "Error initiating NPSS/NSSS\n");
    goto exit;
  }
  srslte_sync_nbiot_set_cfo_enable(&syncobj, true);

  if (input_file_name == NULL) {
    // generating test sequence
    if (srslte_ofdm_tx_init(
            &ifft, SRSLTE_CP_NORM, buffer, &fft_buffer[offset], SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL)) {
      fprintf(stderr, "Error creating iFFT object\n");
      goto exit;
    }
    srslte_ofdm_set_normalize(&ifft, true);
    srslte_ofdm_set_freq_shift(&ifft, -SRSLTE_NBIOT_FREQ_SHIFT_FACTOR);

    // generate NPSS/NSSS signals
    cf_t npss_signal[SRSLTE_NPSS_TOT_LEN];
    srslte_npss_generate(npss_signal);
    srslte_npss_put_subframe(
        &syncobj.npss, npss_signal, buffer, SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL, SRSLTE_NBIOT_DEFAULT_PRB_OFFSET);

    // Transform to OFDM symbols
    memset(fft_buffer, 0, sizeof(cf_t) * SFLEN * 2);
    srslte_ofdm_tx_sf(&ifft);

    srslte_ofdm_tx_free(&ifft);
  } else {
    // read samples from file
    printf("Reading %d samples from file.\n", SFLEN);
    int n = srslte_filesource_read(&fsrc, fft_buffer, SFLEN);
    if (n < 0) {
      fprintf(stderr, "Error reading samples\n");
      goto exit;
    }
  }

#ifdef DUMP_SIGNALS
  srslte_vec_save_file(
      "npss_corr_seq_time.bin", syncobj.npss.npss_signal_time, SRSLTE_NPSS_CORR_FILTER_LEN * sizeof(cf_t));
  srslte_vec_save_file("npss_sf_time.bin", fft_buffer, SFLEN * sizeof(cf_t));
#endif

  if (cfo > 0.0) {
    float delta_freq = cfo / 15000 / SRSLTE_NBIOT_FFT_SIZE;
    printf("Adding CFO with target: %.4f\n", delta_freq);
    printf("WARNING: not working at the moment!\n");
    srslte_cfo_correct(&cfocorr, fft_buffer, fft_buffer, delta_freq);
  }

  // add some noise to the signal
  if (snr != -1.0) {
    snr -= 10.0;
    printf("Adding AWGN with target SNR: %.2fdB\n", snr);
    float nstd = srslte_convert_dB_to_amplitude(-snr);
    srslte_ch_awgn_c(fft_buffer, fft_buffer, nstd, SFLEN);
  }

  // look for NPSS signal
  if (srslte_sync_nbiot_find(&syncobj, fft_buffer, 0, &find_idx) < 0) {
    fprintf(stderr, "Error running srslte_sync_nbiot_find()\n");
    exit(-1);
  }

  printf("NPPS with peak=%f found at: %d, offset: %d, SF starts at %d\n", syncobj.peak_value, find_idx, offset, sf_idx);

  // write results to file
  write_to_file();

#ifdef DUMP_SIGNALS
  srslte_vec_save_file("npss_find_conv_output_abs.bin",
                       syncobj.npss.conv_output_abs,
                       (SFLEN + SRSLTE_NPSS_CORR_FILTER_LEN - 1) * sizeof(float));
#endif

  printf("Ok\n");

  ret = SRSLTE_SUCCESS;

  // cleanup
exit:
  if (input_file_name != NULL) {
    srslte_filesource_free(&fsrc);
  }
  srslte_sync_nbiot_free(&syncobj);
  srslte_cfo_free(&cfocorr);

  if (fft_buffer) {
    free(fft_buffer);
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
  fprintf(debug_fsink.f, "set(0,'DefaultFigureWindowStyle','docked');\n\n");

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
  fprintf(debug_fsink.f, "len = %u;\n", SFLEN);
  fprintf(debug_fsink.f, "sig=read_complex('npss_sf_time.bin', len);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t= 1:len;\n");
  fprintf(debug_fsink.f, "plot(t,real(sig),t,imag(sig));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Subframe time-domain');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the correlation output
  fprintf(debug_fsink.f, "num_samples = %u;\n", SRSLTE_NPSS_CORR_FILTER_LEN + SFLEN - 1);
  fprintf(debug_fsink.f, "sig = read_real('npss_find_conv_output_abs.bin', num_samples);\n");
  fprintf(debug_fsink.f, "t=1:num_samples;\n");
  fprintf(debug_fsink.f, "\n\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(t,sig);\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Convolution output absolute');\n");
  fprintf(debug_fsink.f, "ylabel('Auto-correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  srslte_filesink_free(&debug_fsink);
  printf("data written to %s\n", OUTPUT_FILENAME);
}
