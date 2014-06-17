/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>

#include "liblte/phy/phy.h"
#include "liblte/cuhd/cuhd.h"

int nof_frames=1000;
int band;

cf_t *input_buffer, *fft_buffer;
void *uhd;
int earfcn_start = -1, earfcn_end = -1;

#define MAX_EARFCN 1000
lte_earfcn_t channels[MAX_EARFCN];

#define MHZ 1000000
#define SAMP_FREQ 1920000

void usage(char *prog) {
  printf("Usage: %s [nvse] -b band\n", prog);
  printf("\t-s earfcn_start [Default All]\n");
  printf("\t-e earfcn_end [Default All]\n");
  printf("\t-n number of frames [Default %d]\n", nof_frames);
  printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "sebnv")) != -1) {
    switch(opt) {
    case 'b':
      band = atoi(argv[optind]);
      break;
    case 's':
      earfcn_start = atoi(argv[optind]);
      break;
    case 'e':
      earfcn_end = atoi(argv[optind]);
      break;
    case 'n':
      nof_frames = atoi(argv[optind]);
      break;
    case 'v':
      verbose++;
      break;
    default:
      usage(argv[0]);
      exit(-1);
    }
  }
}

int base_init() {

  input_buffer = malloc(4 * 960 * sizeof(cf_t));
  if (!input_buffer) {
    perror("malloc");
    exit(-1);
  }

  /* open UHD device */
  printf("Opening UHD device...\n");
  if (cuhd_open("",&uhd)) {
    fprintf(stderr, "Error opening uhd\n");
    exit(-1);
  }

  printf("Setting sampling frequency %.2f MHz\n", (float) SAMP_FREQ/MHZ);
  cuhd_set_rx_srate(uhd, SAMP_FREQ);

  printf("Starting receiver...\n");
  cuhd_start_rx_stream(uhd);
  return 0;
}



int main(int argc, char **argv) {
  int frame_cnt;
  int i;
  int nsamples;
  float rssi[MAX_EARFCN];

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }

  parse_args(argc,argv);

  if (base_init()) {
    fprintf(stderr, "Error initializing memory\n");
    exit(-1);
  }

  int nof_bands = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
  printf("Scanning %d freqs in band %d\n", nof_bands, band);
  for (i=0;i<nof_bands;i++) {
    cuhd_set_rx_freq(uhd, (double) channels[i].fd * MHZ);
    frame_cnt = 0;
    nsamples=0;
    rssi[i]=0;
    while(frame_cnt < nof_frames) {
      nsamples += cuhd_recv(uhd, input_buffer, 1920, 1);
      rssi[i] += vec_avg_power_cf(input_buffer, 1920);
      frame_cnt++;
    }
    printf("[%3d/%d]: Scanning earfcn %d freq %.2f MHz RSSI %.2f dBm\n", i, nof_bands,
        channels[i].id, channels[i].fd, 10*log10f(rssi[i]) + 30);
  }

  FILE *f = fopen("output.m", "w");
  if (!f) {
    perror("fopen");
    exit(-1);
  }
  fprintf(f, "fd=[");
  for (i=0;i<nof_bands;i++) {
    fprintf(f, "%g, ", channels[i].fd);
  }
  fprintf(f, "];\n");

  fprintf(f, "rssi=[");
  for (i=0;i<nof_bands;i++) {
    fprintf(f, "%g, ", rssi[i]);
  }
  fprintf(f, "];\n");
  fprintf(f, "plot(fd/1000, 10*log10(rssi)+30)\ngrid on\nxlabel('f_d [Ghz]')\nylabel('RSSI [dBm]')\n");
  fclose(f);

  free(input_buffer);

  printf("Done\n");
  exit(0);
}
