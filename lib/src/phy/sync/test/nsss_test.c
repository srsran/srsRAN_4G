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

#include "srslte/phy/sync/nsss.h"
#include "srslte/srslte.h"

#define OUTPUT_FILENAME "nsss_test.m"
void write_to_file();

int      sfn        = 0;
uint32_t n_id_ncell = SRSLTE_CELL_ID_UNKNOWN;
char*    input_file_name;

int max_num_sf = 20;

#define NOF_PRB 1

#define DUMP_SIGNALS 0
#define MAX_CORR_LEN 10000
#define SFLEN (1 * SRSLTE_SF_LEN(SRSLTE_NBIOT_FFT_SIZE))

void usage(char* prog)
{
  printf("Usage: %s [fcpoev]\n", prog);
  printf("\t-f file to read from\n");
  printf("\t-c cell ID\n");
  printf("\t-n SFN\n");
  printf("\t-r Maximum number of subframes to read from file [default: %d]\n", max_num_sf);
  printf("\t-v srslte_verbose\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fcprnov")) != -1) {
    switch (opt) {
      case 'f':
        input_file_name = argv[optind];
        break;
      case 'c':
        n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        sfn = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        max_num_sf = (int)strtol(argv[optind], NULL, 10);
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
  cf_t*               fft_buffer = NULL;
  cf_t*               buffer     = NULL;
  srslte_nsss_synch_t syncobj    = {};
  srslte_ofdm_t       ifft       = {};
  int                 fft_size   = 0;
  float               peak_value = 0.0;
  int                 num_sf     = 1;
  int                 ret        = SRSLTE_ERROR;

  parse_args(argc, argv);

  buffer = srslte_vec_cf_malloc(SFLEN * max_num_sf);
  if (!buffer) {
    perror("malloc");
    return ret;
  }
  memset(buffer, 0, sizeof(cf_t) * SFLEN * max_num_sf);

  fft_size = srslte_symbol_sz(NOF_PRB);
  if (fft_size < 0) {
    fprintf(stderr, "Invalid nof_prb=%d\n", NOF_PRB);
    goto exit;
  }

  printf("SFLEN is %d samples\n", SFLEN);
  fft_buffer = srslte_vec_cf_malloc(SFLEN * max_num_sf);
  if (!fft_buffer) {
    perror("malloc");
    goto exit;
  }
  memset(fft_buffer, 0, sizeof(cf_t) * SFLEN * max_num_sf);

  if (srslte_ofdm_tx_init(&ifft, SRSLTE_CP_NORM, buffer, fft_buffer, NOF_PRB)) {
    fprintf(stderr, "Error creating iFFT object\n");
    goto exit;
  }

  if (input_file_name != NULL) {
    srslte_filesource_t fsrc;
    printf("Opening file %s\n", input_file_name);
    if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
      fprintf(stderr, "Error opening file %s\n", input_file_name);
      goto exit;
    }
    num_sf = 0;

    // offset file
    int file_offset = 0;
    srslte_filesource_read(&fsrc, buffer, file_offset);

    // now read
    while (num_sf < max_num_sf) {
      int n = srslte_filesource_read(&fsrc, &fft_buffer[num_sf * SFLEN], SFLEN);
      if (n < 0) {
        fprintf(stderr, "Error reading samples\n");
        goto exit;
      }
      if (n < SFLEN) {
        fprintf(stdout, "End of file (n=%d, sflen=%d)\n", n, SFLEN);
        break;
      }
      num_sf++;
    }
    srslte_filesource_free(&fsrc);
    printf("Read %d sumbframes from file.\n", num_sf);
  }

  // initialize NSSS object with actual input length
  printf("Initializing NSSS synch with %dx%d samples.\n", num_sf, SFLEN);
  if (srslte_nsss_synch_init(&syncobj, num_sf * SFLEN, fft_size)) {
    fprintf(stderr, "Error initializing NSSS object\n");
    goto exit;
  }

  // write single NSSS sequence if not reading from input file
  if (!input_file_name) {
    // generate NPSS/NSSS signals
    printf("Generating NSSS sequence for n_id_ncell=%d\n", n_id_ncell);
    cf_t nsss_signals[SRSLTE_NSSS_TOT_LEN] = {};
    srslte_nsss_generate(nsss_signals, n_id_ncell == SRSLTE_CELL_ID_UNKNOWN ? 0 : n_id_ncell);

#if DUMP_SIGNALS
    srslte_vec_save_file("nsss_signal_freq.bin", nsss_signals, SRSLTE_NSSS_LEN * sizeof(cf_t));
#endif

    srslte_nsss_put_subframe(
        &syncobj, nsss_signals, buffer, sfn, SRSLTE_NBIOT_DEFAULT_NUM_PRB_BASECELL, SRSLTE_NBIOT_DEFAULT_PRB_OFFSET);

    // Transform to OFDM symbols
    srslte_ofdm_tx_sf(&ifft);
  }

  // look for NSSS signal
  uint32_t n_id_ncell_detected = SRSLTE_CELL_ID_UNKNOWN;
  uint32_t sfn_partial         = 0;
  srslte_nsss_sync_find(&syncobj, fft_buffer, &peak_value, &n_id_ncell_detected, &sfn_partial);
  printf("NSSS with peak=%f, n_id_ncell: %d, partial SFN: %x\n", peak_value, n_id_ncell_detected, sfn_partial);

  if (n_id_ncell_detected == (n_id_ncell == SRSLTE_CELL_ID_UNKNOWN ? 0 : n_id_ncell)) {
    printf("Ok\n");
    ret = SRSLTE_SUCCESS;
  } else {
    printf("Failed\n");
  }

#if DUMP_SIGNALS
// dump signals
#define MAX_FNAME_LEN 40
  char fname[MAX_FNAME_LEN] = {};
  snprintf(fname, MAX_FNAME_LEN, "nsss_find_input.bin");
  printf("Saving entire sub-frame to %s\n", fname);
  srslte_vec_save_file(fname, fft_buffer, num_sf * SFLEN * sizeof(cf_t));
  srslte_vec_save_file("nsss_corr_seq_time.bin",
                       syncobj.nsss_signal_time[n_id_ncell == SRSLTE_CELL_ID_UNKNOWN ? 0 : n_id_ncell],
                       SRSLTE_NSSS_CORR_FILTER_LEN * sizeof(cf_t));
  if (n_id_ncell_detected != SRSLTE_CELL_ID_UNKNOWN) {
    // run correlation again with found cell to populate conv_output_abs
    srslte_nsss_sync_find(&syncobj, fft_buffer, &peak_value, &n_id_ncell_detected, &sfn_partial);
  }
  srslte_vec_save_file("nsss_find_conv_output_abs.bin",
                       syncobj.conv_output_abs,
                       (SRSLTE_NSSS_CORR_FILTER_LEN + num_sf * SFLEN) * sizeof(float));

  // write Octave script
  write_to_file();
#endif

  // cleanup
exit:
  srslte_nsss_synch_free(&syncobj);
  srslte_ofdm_tx_free(&ifft);
  if (buffer) {
    free(buffer);
  }
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
  fprintf(debug_fsink.f, "max_len = 1920 * 100;\n");

  // the correlation sequence
  fprintf(debug_fsink.f, "corr_seq=read_complex('nsss_corr_seq_time.bin', max_len);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t=1:length(corr_seq);\n");
  fprintf(debug_fsink.f, "plot(t,real(corr_seq),t,imag(corr_seq));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Correlation sequence time-domain');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the generated subframe
  fprintf(debug_fsink.f, "input=read_complex('nsss_find_input.bin', max_len);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "t=1:length(input);\n");
  fprintf(debug_fsink.f, "plot(t,real(input),t,imag(input));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Subframe time-domain');\n");
  fprintf(debug_fsink.f, "\n\n");

  // the correlation output
  fprintf(debug_fsink.f, "corr_srslte = read_real('nsss_find_conv_output_abs.bin', max_len);\n");
  fprintf(debug_fsink.f, "t=1:length(corr_srslte);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(t,corr_srslte);\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Convolution output absolute');\n");
  fprintf(debug_fsink.f, "ylabel('Auto-correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  // correlation in octave
  fprintf(debug_fsink.f, "[corr_oct, lag] = xcorr(input,corr_seq);\n");
  fprintf(debug_fsink.f, "figure;\n");
  fprintf(debug_fsink.f, "plot(abs(corr_oct));\n");
  fprintf(debug_fsink.f, "xlabel('sample index');\n");
  fprintf(debug_fsink.f, "title('Correlation in Octave');\n");
  fprintf(debug_fsink.f, "ylabel('Correlation magnitude');\n");
  fprintf(debug_fsink.f, "\n\n");

  srslte_filesink_free(&debug_fsink);
  printf("data written to %s\n", OUTPUT_FILENAME);
}
