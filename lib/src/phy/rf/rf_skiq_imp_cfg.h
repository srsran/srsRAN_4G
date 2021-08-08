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

#ifndef SRSRAN_RF_SKIQ_IMP_CFG_H
#define SRSRAN_RF_SKIQ_IMP_CFG_H

/**
 * RF_SKIQ_MAX_PORTS_CARD sets the maximum number of ports per card
 */
#define RF_SKIQ_MAX_PORTS_CARD 2

/**
 * SKIQ_CARD_SYNCH_MAX_TRIALS defines the maximum number of trials to synchronize multiple boards
 */
#define SKIQ_CARD_SYNCH_MAX_TRIALS 10

/**
 * SKIQ_CARD_SYNC_MAX_ERROR sets the maximum number of system "ticks" error between boards during synchronization check.
 * Consider the communication medium delay between the host and SidekIQ cards.
 */
#define SKIQ_CARD_SYNC_MAX_ERROR (SKIQ_SYS_TIMESTAMP_FREQ / 2)

/**
 * Maximum gap allowed in number of samples between ports
 */
#define SKIQ_PORT_SYNC_MAX_GAP (1024 * 1024)

/**
 * For checking the number of Tx lattes in the FPGA set the next line to the desired check period in number of blocks.
 * Example: set to 1000 for checking it every 1000 blocks.
 * WARNING: A low period may cause a reduction of performance in the Host-FPGA communication
 */
#define SKIQ_TX_LATES_CHECK_PERIOD (1920 * 10)

/**
 * Minimum number of channels that this RF device can reach
 */
#define SKIQ_MIN_CHANNELS (1)

/**
 * Maximum number of channels that this RF device can reach
 */
#define SKIQ_MAX_CHANNELS (SKIQ_MAX_NUM_CARDS * RF_SKIQ_MAX_PORTS_CARD)

/**
 * Dummy receive buffer size in samples
 */
#define RF_SKIQ_DUMMY_BUFFER_SIZE (1024)

/**
 * Magic word value as a ring buffer check
 */
#define SKIQ_RX_BUFFFER_MAGIC_WORD 0xABCD1234

/**
 * Normalization value between fixed and floating point conversion
 */
#define SKIQ_NORM 2048.0

/**
 * Default Rx gain in decibels (dB)
 */
#define SKIQ_RX_GAIN_DEFAULT_dB (+50.0f)

/**
 * Default sampling rate in samples per second (Hz)
 */
#define SKIQ_DEFAULT_SAMPLING_RATE_HZ (30.72e6)

/**
 *
 */
#define SKIQ_TX_PACKET_SIZE(N, MODE) (SKIQ_TX_PACKET_SIZE_INCREMENT_IN_WORDS * (N)-SKIQ_TX_HEADER_SIZE_IN_WORDS)

/**
 * SKIQ driver standard output MACRO
 */
extern uint32_t rf_skiq_logging_level;

#define SKIQ_RF_INFO(...)                                                                                              \
  do {                                                                                                                 \
    if (rf_skiq_logging_level >= SKIQ_LOG_INFO) {                                                                      \
      fprintf(stdout, "[SKIQ RF INFO] " __VA_ARGS__);                                                                  \
    }                                                                                                                  \
  } while (false)

#define SKIQ_RF_DEBUG(...)                                                                                             \
  do {                                                                                                                 \
    if (rf_skiq_logging_level >= SKIQ_LOG_DEBUG) {                                                                     \
      fprintf(stdout, "[SKIQ RF INFO] " __VA_ARGS__);                                                                  \
    }                                                                                                                  \
  } while (false)

#define SKIQ_RF_ERROR(...)                                                                                             \
  do {                                                                                                                 \
    if (rf_skiq_logging_level >= SKIQ_LOG_ERROR) {                                                                     \
      fprintf(stdout, "[SKIQ RF ERROR] " __VA_ARGS__);                                                                 \
    }                                                                                                                  \
  } while (false)

#endif // SRSLTE_RF_SKIQ_IMP_CFG_H
