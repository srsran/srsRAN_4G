// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSENB_MAC_METRICS_H
#define SRSENB_MAC_METRICS_H

#include <cstdint>
#include <vector>

namespace srsenb {

/// MAC metrics per user
struct mac_ue_metrics_t {
  uint16_t rnti;
  uint32_t pci;
  uint32_t nof_tti;
  uint32_t cc_idx;
  int      tx_pkts;
  int      tx_errors;
  int      tx_brate;
  int      rx_pkts;
  int      rx_errors;
  int      rx_brate;
  int      ul_buffer;
  int      dl_buffer;
  float    dl_cqi;
  float    dl_ri;
  float    dl_pmi;
  float    phr;
  float    dl_cqi_offset;
  float    ul_snr_offset;

  // NR-only UL PHY metrics
  float pusch_sinr;
  float pucch_sinr;
  float ul_rssi;
  float fec_iters;
  float dl_mcs;
  int   dl_mcs_samples;
  float ul_mcs;
  int   ul_mcs_samples;
};
/// MAC misc information for each cc.
struct mac_cc_info_t {
  /// PCI value.
  uint32_t pci;
  /// RACH preamble counter per cc.
  uint32_t cc_rach_counter;
};

/// Main MAC metrics.
struct mac_metrics_t {
  /// Per CC info.
  std::vector<mac_cc_info_t> cc_info;
  /// Per UE MAC metrics.
  std::vector<mac_ue_metrics_t> ues;
};

} // namespace srsenb

#endif // SRSENB_MAC_METRICS_H
