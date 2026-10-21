// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_PDCP_METRICS_H
#define SRSRAN_PDCP_METRICS_H

#include "srsran/common/common.h"
#include <iostream>

namespace srsran {

typedef struct {
  // PDU metrics
  uint32_t num_tx_pdus;
  uint32_t num_rx_pdus;
  uint64_t num_tx_pdu_bytes;
  uint64_t num_rx_pdu_bytes;

  // ACK specific metrics (requires RLC AM)
  uint64_t num_tx_acked_bytes;         //< Cumulative number of bytes that the PDCP knows to be acknowledged
  uint64_t tx_notification_latency_ms; //< Average time in ms from PDU delivery to RLC to ACK notification from RLC
  uint32_t num_tx_buffered_pdus;       //< Number of PDUs waiting for ACK
  uint32_t num_tx_buffered_pdus_bytes; //< Number of bytes of PDUs waiting for ACK
} pdcp_bearer_metrics_t;

typedef struct {
  pdcp_bearer_metrics_t bearer[SRSRAN_N_RADIO_BEARERS];
} pdcp_metrics_t;

} // namespace srsran

#endif // SRSRAN_RLC_METRICS_H
