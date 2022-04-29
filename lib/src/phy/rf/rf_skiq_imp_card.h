/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#ifndef SRSRAN_LIB_SRC_PHY_RF_RF_SKIQ_IMP_CARD_H_
#define SRSRAN_LIB_SRC_PHY_RF_RF_SKIQ_IMP_CARD_H_

#include "rf_skiq_imp_port.h"

typedef struct {
  uint8_t          card;
  uint8_t          nof_ports;
  skiq_chan_mode_t mode;

  skiq_param_t      param;
  rf_skiq_tx_port_t tx_ports[RF_SKIQ_MAX_PORTS_CARD];
  rf_skiq_rx_port_t rx_ports[RF_SKIQ_MAX_PORTS_CARD];

  double cur_rx_gain_db;
  bool   suspend;

  uint64_t             start_rx_stream_ts;
  rf_skiq_port_state_t state;
  pthread_mutex_t      mutex; ///< Protect concurrent access to start/stop rx stream and receive
  pthread_cond_t       cvar;
  pthread_t            thread;

} rf_skiq_card_t;

int rf_skiq_card_init(rf_skiq_card_t* q, uint8_t card, uint8_t nof_ports, const rf_skiq_port_opts_t* opts);

void rf_skiq_card_set_error_handler(rf_skiq_card_t* q, srsran_rf_error_handler_t error_handler, void* arg);

double rf_skiq_card_set_tx_gain_db(rf_skiq_card_t* q, uint32_t port_idx, double gain_db);

double rf_skiq_card_set_rx_gain_db(rf_skiq_card_t* q, uint32_t port_idx, double gain_db);

int rf_skiq_card_update_timestamp(rf_skiq_card_t* q, bool use_1pps, uint64_t new_ts);

uint64_t rf_skiq_card_read_sys_timestamp(rf_skiq_card_t* q);

uint64_t rf_skiq_card_read_rf_timestamp(rf_skiq_card_t* q);

int rf_skiq_card_start_rx_streaming(rf_skiq_card_t* q, uint64_t timestamp);

int rf_skiq_card_stop_rx_streaming(rf_skiq_card_t* q);

void rf_skiq_card_end_of_burst(rf_skiq_card_t* q);

int rf_skiq_card_set_srate_hz(rf_skiq_card_t* q, uint32_t srate_hz);

double rf_skiq_card_set_tx_freq_hz(rf_skiq_card_t* q, uint32_t port_idx, double freq_hz);

double rf_skiq_card_set_rx_freq_hz(rf_skiq_card_t* q, uint32_t port_idx, double freq_hz);

void rf_skiq_card_close(rf_skiq_card_t* q);

int rf_skiq_card_receive(rf_skiq_card_t* q, uint32_t port_idx, cf_t* dst, uint32_t nsamples, uint64_t* ts);

uint64_t rf_skiq_card_get_rx_timestamp(rf_skiq_card_t* q, uint32_t port_idx);

bool rf_skiq_card_is_streaming(rf_skiq_card_t* q);

int rf_skiq_card_send(rf_skiq_card_t* q, uint32_t port_idx, const cf_t* data, uint32_t nsamples, uint64_t timestamp);

#endif // SRSRAN_LIB_SRC_PHY_RF_RF_SKIQ_IMP_CARD_H_
