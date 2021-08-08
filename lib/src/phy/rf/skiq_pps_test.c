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

#include <sidekiq_api.h>
#include <stdio.h>
#include <unistd.h>

#define ERROR(...) fprintf(stderr, __VA_ARGS__);

const static uint8_t nof_cards = 1;

int main(int argc, char* argv[])
{
  int     ret  = -1;
  uint8_t card = 0;

  if (argc > 1) {
    card = (uint8_t)strtol(argv[1], NULL, 10);
  }

  uint8_t nof_available_cards                 = 0;
  uint8_t available_cards[SKIQ_MAX_NUM_CARDS] = {};
  if (skiq_get_cards(skiq_xport_type_auto, &nof_available_cards, available_cards)) {
    ERROR("Getting available cards\n");
    goto clean_exit;
  }

  // Check number of cards bounds
  if (nof_cards > nof_available_cards) {
    ERROR("The number of cards (%d) exceeds available cards (%d)\n", nof_cards, nof_available_cards);
    goto clean_exit;
  }

  // Initialise driver
  if (skiq_init(skiq_xport_type_auto, skiq_xport_init_level_full, &card, 1)) {
    ERROR("Unable to initialise libsidekiq driver\n");
    goto clean_exit;
  }

  // Programming FPGA from flash ensures the FPGA and transceiver are completely reseted
  if (skiq_prog_fpga_from_flash(card)) {
    ERROR("Error programming FPGA from flash\n");
    goto clean_exit;
  }

  // Read 1pps source
  skiq_1pps_source_t pps_source = skiq_1pps_source_unavailable;
  if (skiq_read_1pps_source(card, &pps_source)) {
    ERROR("Error reading card %d 1PPS source\n", card);
    goto clean_exit;
  }

  // Make sure the source is external
  if (pps_source != skiq_1pps_source_external) {
    ERROR("Error card %d is not configured with external 1PPS source\n", card);
    goto clean_exit;
  }

  // Sleeping 5 seconds
  sleep(5);

  // Get last time a PPS was received
  uint64_t ts_sys_1pps = 0;
  if (skiq_read_last_1pps_timestamp(card, NULL, &ts_sys_1pps)) {
    ERROR("Reading card %d last 1PPS timestamp", card);
    goto clean_exit;
  }

  // Read current system time
  uint64_t ts = 0;
  if (skiq_read_curr_sys_timestamp(card, &ts)) {
    ERROR("Reading card %d system timestamp", card);
    goto clean_exit;
  }

  // Make sure a 1PPS was received less than 2 seconds ago
  if (ts - ts_sys_1pps > 2 * SKIQ_SYS_TIMESTAMP_FREQ) {
    ERROR("Error card %d last PPS was received %.1f seconds ago (ts=%ld)\n",
          card,
          (double)(ts - ts_sys_1pps) / (double)SKIQ_SYS_TIMESTAMP_FREQ,
          ts_sys_1pps);
    goto clean_exit;
  }

  // Set a given time in the future, a 100th of a second (10ms)
  ts += SKIQ_SYS_TIMESTAMP_FREQ / 100;

  // Reset timestamp in a near future on next PPS signal
  if (skiq_write_timestamp_reset_on_1pps(card, ts)) {
    ERROR("Error reseting card %d timestamp on 1 PPS", card);
    goto clean_exit;
  }

  // Give time to pass 1PPS
  sleep(1);

  // Read current system time
  if (skiq_read_curr_sys_timestamp(card, &ts)) {
    ERROR("Reading card %d system timestamp", card);
    goto clean_exit;
  }

  // The current system timestamp should be below 2s
  if (ts > 2 * SKIQ_SYS_TIMESTAMP_FREQ) {
    ERROR("Timestamp of card %d is greater than 2 seconds (%.1fs)!\n",
          card,
          (double)ts / (double)SKIQ_SYS_TIMESTAMP_FREQ);
    goto clean_exit;
  }

  // Success
  printf("Success!\n");
  ret = 0;

clean_exit:
  if (skiq_disable_cards(&card, 1)) {
    ERROR("Unable to disable cards\n");
  }

  // Close sidekiq SDK
  skiq_exit();

  return ret;
}
