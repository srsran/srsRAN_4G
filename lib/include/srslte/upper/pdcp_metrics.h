/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_PDCP_METRICS_H
#define SRSLTE_PDCP_METRICS_H

#include "srslte/common/common.h"
#include <iostream>

namespace srslte {

typedef struct {
  // ACK specific metrics (requires RLC AM)
  uint64_t num_tx_acked_bytes;         //< Cumulative number of bytes that the PDCP knows to be acknowledged
  uint64_t tx_notification_latency_ms; //< Average time in ms from PDU delivery to RLC to ACK notification from RLC
  uint32_t num_tx_buffered_pdus;       //< Number of PDUs waiting for ACK
  uint32_t num_tx_buffered_pdus_bytes; //< Number of bytes of PDUs waiting for ACK
} pdcp_bearer_metrics_t;

typedef struct {
  pdcp_bearer_metrics_t bearer[SRSLTE_N_RADIO_BEARERS];
} pdcp_metrics_t;

} // namespace srslte

#endif // SRSLTE_RLC_METRICS_H
