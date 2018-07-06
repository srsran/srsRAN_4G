/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <unistd.h>
#include <signal.h>
#include "srslte/srslte.h"
#include "srslte/radio/radio_multi.h"

using namespace srslte;

std::string device_args = "auto";

double freq = 2630e6;
uint32_t nof_ports = 1;
double srate = 1.92e6; /* Hz */
double duration = 0.01; /* in seconds, 10 ms by default */
cf_t *buffers[SRSLTE_MAX_PORTS];
bool tx_enable = false;

uint32_t num_lates = 0;
uint32_t num_overflows = 0;
uint32_t num_underflows = 0;
uint32_t num_other_error = 0;


void usage(char *prog) {
  printf("Usage: %s [rpstvh]\n", prog);
  printf("\t-f Carrier frequency in Hz [Default %f]\n", freq);
  printf("\t-a Arguments for first radio [Default %s]\n", device_args.c_str());
  printf("\t-p number of ports 1-%d [Default %d]\n", SRSLTE_MAX_PORTS, nof_ports);
  printf("\t-s sampling rate [Default %.0f]\n", srate);
  printf("\t-t duration in seconds [Default %.3f]\n", duration);
  printf("\t-x enable transmit [Default %s]\n", (tx_enable) ? "enabled" : "disabled");
  printf("\t-v Set srslte_verbose to info (v) or debug (vv) [Default none]\n");
  printf("\t-h show this message\n");
}

void parse_args(int argc, char **argv) {
  int opt;
  while ((opt = getopt(argc, argv, "foabcderpstvhmxw")) != -1) {
    switch (opt) {
      case 'f':
        freq = atof(argv[optind]);
        break;
      case 'a':
        device_args = std::string(argv[optind]);
        break;
      case 'p':
        nof_ports = (uint32_t) atoi(argv[optind]);
        break;
      case 's':
        srate = atof(argv[optind]);
        break;
      case 't':
        duration = atof(argv[optind]);
        break;
      case 'x':
        tx_enable ^= true;
        break;
      case 'v':
        srslte_verbose++;
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

bool go_exit = false;
void sig_int_handler(int signo)
{
  printf("SIGINT received. Exiting...\n");
  if (signo == SIGINT) {
    go_exit = true;
  } else if (signo == SIGSEGV) {
    exit(1);
  }
}

void rf_msg(srslte_rf_error_t error)
{
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    num_overflows++;
  } else
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW) {
    num_underflows++;
  } else
  if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_LATE) {
    num_lates++;
  } else {
    num_other_error++;
  }
}

void print_rf_summary(void)
{
  printf("#lates=%d\n", num_lates);
  printf("#overflows=%d\n", num_overflows);
  printf("#underflows=%d\n", num_underflows);
  printf("#num_other_error=%d\n", num_other_error);
}

int main(int argc, char **argv)
{
  int ret = SRSLTE_ERROR;
  srslte::radio_multi *radio_h = NULL;
  srslte_timestamp_t ts_rx = {}, ts_tx = {};

  signal(SIGINT, sig_int_handler);

  /* Parse args */
  parse_args(argc, argv);

  uint32_t nof_samples = (uint32_t) (duration * srate);
  uint32_t frame_size = (uint32_t) (srate / 1000.0); /* 1 ms at srate */
  uint32_t nof_frames = duration * 1e3;

  radio_h = new radio_multi();
  if (!radio_h) {
    fprintf(stderr, "Error: Calling radio_multi constructor\n");
    goto clean_exit;
  }

  for (uint32_t p = 0; p < SRSLTE_MAX_PORTS; p++) {
    buffers[p] = NULL;
  }

  for (uint32_t p = 0; p < nof_ports; p++) {
    buffers[p] = (cf_t *) srslte_vec_malloc(sizeof(cf_t) * frame_size);
    if (!buffers[p]) {
      fprintf(stderr, "Error: Allocating buffer (%d)\n", p);
      goto clean_exit;
    }
  }

  /* Initialise instances */
  printf("Initialising instances...\n");
  if (!radio_h->init((char*)device_args.c_str(), NULL, nof_ports)) {
    fprintf(stderr, "Error: Calling radio_multi constructor\n");
    goto clean_exit;
  }

  radio_h->register_error_handler(rf_msg);

  radio_h->set_rx_freq(freq);

  /* Set radio */
  printf("Setting radio...\n");
  if (srate < 10e6) {
    radio_h->set_master_clock_rate(4 * srate);
  } else {
    radio_h->set_master_clock_rate(srate);
  }

  radio_h->set_rx_srate(srate);
  if (tx_enable) {
    radio_h->set_tx_srate(srate);
  }

  /* Receive */
  printf("Initial receive for aligning radios...\n");
  radio_h->rx_now(buffers, frame_size, &ts_rx);

  printf("Start capturing %d frames of %d samples...\n", nof_frames, frame_size);

  for (uint32_t i = 0; i < nof_frames; i++) {
    frame_size = SRSLTE_MIN(frame_size, nof_samples);
    radio_h->rx_now(buffers, frame_size, &ts_rx);

    if (tx_enable) {
      srslte_timestamp_copy(&ts_tx, &ts_rx);
      srslte_timestamp_add(&ts_tx, 0, 0.004);
      radio_h->tx_single(buffers[0], frame_size, ts_tx);
    }

    nof_samples -= frame_size;

    if (go_exit) break;
  }

  printf("Finished streaming ...\n");

  ret = SRSLTE_SUCCESS;

clean_exit:
  printf("Tearing down...\n");

  radio_h->stop();

  for (uint32_t p = 0; p < nof_ports; p++) {
    if (buffers[p]) {
      free(buffers[p]);
    }
  }

  if (ret) {
    printf("Failed!\n");
  } else {
    printf("Ok!\n");
  }

  print_rf_summary();

  return ret;
}
