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
