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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <zmq.h>

#include <stdbool.h>

#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"

static bool           keep_running    = true;
static uint32_t       nof_rx_antennas = 1;
static const uint32_t max_rx_antennas = 1;

static void int_handler(int dummy);
static void usage(char* prog);
static void parse_args(int argc, char** argv);
static int  init_radio(uint32_t* buf_len);
static int  rx_radio(void** buffer, uint32_t buff_len);
static void close_radio();

/* Example function to initialize ZMQ socket */
static void*       zmq_ctx  = NULL;
static void*       zmq_sock = NULL;
static const char* zmq_args = "tcp://*:5550";
static int         init_zmq()
{
  zmq_ctx = zmq_ctx_new();

  // Create socket
  zmq_sock = zmq_socket(zmq_ctx, ZMQ_PUB);
  if (!zmq_sock) {
    fprintf(stderr, "Error: creating transmitter socket\n");
    return -1;
  }

  // The transmitter starts first and creates the socket
  if (zmq_bind(zmq_sock, zmq_args)) {
    fprintf(stderr, "Error: connecting transmitter socket: %s\n", zmq_strerror(zmq_errno()));
    return -1;
  }
  return 0;
}

/* Example function to write samples to ZMQ socket */
static int tx_zmq(void** buffer, uint32_t buffer_len)
{
  // wait for request
  uint8_t dummy;
  zmq_recv(zmq_sock, &dummy, sizeof(dummy), 0);
  return zmq_send(zmq_sock, buffer[0], buffer_len, 0);
}

int main(int argc, char** argv)
{
  void*    buffer[max_rx_antennas];
  int      n           = 0;
  uint32_t buflen      = 0; // in samples
  uint32_t sample_size = 8;

  // Sets signal handlers
  signal(SIGINT, int_handler);
  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  // Parse args
  parse_args(argc, argv);

  // Initializes ZMQ
  if (init_zmq()) {
    ERROR("Initializing ZMQ\n");
    exit(-1);
  }

  if (init_radio(&buflen)) {
    ERROR("Initializing Radio\n");
    exit(-1);
  }

  // Initializes memory for input buffer
  bzero(buffer, sizeof(void*) * max_rx_antennas);
  for (int i = 0; i < nof_rx_antennas; i++) {
    buffer[i] = srslte_vec_cf_malloc(buflen);
    if (!buffer[i]) {
      perror("malloc");
      exit(-1);
    }
  }

  printf("Streaming samples...\n");
  uint32_t print_cnt = 0;
  while (keep_running) {
    n = rx_radio(buffer, buflen);
    if (n < 0) {
      ERROR("Error receiving samples\n");
      exit(-1);
    }
    if (srslte_verbose == SRSLTE_VERBOSE_INFO) {
      printf("Received %d samples from radio\n", n);
    }

    n = tx_zmq((void**)buffer, n * sample_size);

    if (n == -1) {
      print_cnt++;
      if (print_cnt == 1000) {
        printf("ZMQ socket not connected\n");
        print_cnt = 0;
      }
    } else {
      if (srslte_verbose == SRSLTE_VERBOSE_INFO) {
        printf("Transmitted %d bytes to ZMQ\n", n);
      }
    }
  }

  // Cleanup memory and close RF device
  for (int i = 0; i < nof_rx_antennas; i++) {
    if (buffer[i]) {
      free(buffer[i]);
    }
  }
  close_radio();

  printf("Exit Ok\n");
  exit(0);
}

/* Example function to initialize the Radio frontend. In this case, we use srsLTE RF API to open a device,
 * which automatically picks UHD, bladeRF, limeSDR, etc.
 */
static srslte_rf_t radio   = {};
static char*       rf_args = "fastpath";
static float       rf_gain = 40.0, rf_freq = -1.0, rf_rate = 11.52e6;
static uint32_t    rf_recv_frame_size_ms = 1;
static int         init_radio(uint32_t* buffer_len)
{
  // Uses srsLTE RF API to open a device, could use other code here
  printf("Opening RF device...\n");
  if (srslte_rf_open_multi(&radio, rf_args, nof_rx_antennas)) {
    ERROR("Error opening rf\n");
    return -1;
  }
  srslte_rf_set_rx_gain(&radio, rf_gain);
  srslte_rf_set_rx_freq(&radio, nof_rx_antennas, rf_freq);

  printf("Set RX freq: %.2f MHz\n", rf_freq / 1000000);
  printf("Set RX gain: %.2f dB\n", rf_gain);
  float srate = srslte_rf_set_rx_srate(&radio, rf_rate);
  if (srate != rf_rate) {
    ERROR("Error setting samplign frequency %.2f MHz\n", rf_rate * 1e-6);
    return -1;
  }

  if (buffer_len) {
    *buffer_len = srate * rf_recv_frame_size_ms * 1e-3;
  }

  printf("Set RX rate: %.2f MHz\n", srate * 1e-6);
  srslte_rf_start_rx_stream(&radio, false);
  return 0;
}

/* Example implementation to receive from Radio frontend. In this case we use srsLTE
 */
static int rx_radio(void** buffer, uint32_t buf_len)
{
  return srslte_rf_recv_with_time_multi(&radio, buffer, buf_len, true, NULL, NULL);
}

static void close_radio()
{
  srslte_rf_close(&radio);
}

static void int_handler(int dummy)
{
  keep_running = false;
}

static void usage(char* prog)
{
  printf("Usage: %s [agrAzv] -f rx_frequency_hz\n", prog);
  printf("\t-a RF args [Default %s]\n", rf_args);
  printf("\t-g RF Gain [Default %.2f dB]\n", rf_gain);
  printf("\t-r RF Rate [Default %.6f Hz]\n", rf_rate);
  printf("\t-m RF receive frame size in ms [Default %d ms]\n", rf_recv_frame_size_ms);
  printf("\t-A Number of antennas [Max %d, Default %d]\n", max_rx_antennas, nof_rx_antennas);
  printf("\t-z ZMQ args [Default %s]\n", zmq_args);
  printf("\t-v srslte_verbose\n");
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "agrfvmzA")) != -1) {
    switch (opt) {
      case 'a':
        rf_args = argv[optind];
        break;
      case 'g':
        rf_gain = strtof(argv[optind], NULL);
        break;
      case 'm':
        rf_recv_frame_size_ms = strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rf_rate = strtof(argv[optind], NULL);
        break;
      case 'f':
        rf_freq = strtof(argv[optind], NULL);
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'z':
        zmq_args = argv[optind];
        break;
      case 'A':
        nof_rx_antennas = strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (nof_rx_antennas > max_rx_antennas || nof_rx_antennas < 1) {
    fprintf(stderr, "Invalid number of antennas\n");
    usage(argv[0]);
    exit(-1);
  }
  if (rf_freq < 0) {
    usage(argv[0]);
    exit(-1);
  }
}
