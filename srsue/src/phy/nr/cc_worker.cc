/**
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

#include "srsue/hdr/phy/nr/cc_worker.h"
#include "srslte/srslte.h"

namespace srsue {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslte::log* log, phy_nr_state* phy_state_) :
  cc_idx(cc_idx_), phy_state(phy_state_), log_h(log)
{
  cf_t* buffer_c[SRSLTE_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSLTE_SF_LEN_PRB(phy_state->args.dl.nof_max_prb) * 5;
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_rx_antennas; i++) {
    rx_buffer[i] = srslte_vec_cf_malloc(buffer_sz);
    tx_buffer[i] = srslte_vec_cf_malloc(buffer_sz);
    buffer_c[i]  = rx_buffer[i];
  }

  if (srslte_ue_dl_nr_init(&ue_dl, buffer_c, &phy_state_->args.dl) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR\n");
    return;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    return;
  }
  data.resize(SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8);
}

cc_worker::~cc_worker()
{
  srslte_ue_dl_nr_free(&ue_dl);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  for (cf_t* p : rx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
  for (cf_t* p : tx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
}

bool cc_worker::set_carrier(const srslte_carrier_nr_t* carrier)
{
  if (srslte_ue_dl_nr_set_carrier(&ue_dl, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier\n");
    return false;
  }

  coreset.freq_resources[0] = true; // Enable the bottom 6 PRB for PDCCH
  coreset.duration          = 2;

  if (srslte_ue_dl_nr_set_coreset(&ue_dl, &coreset) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier\n");
    return false;
  }

  // Set default PDSCH config
  phy_state->cfg.pdsch.rbg_size_cfg_1        = false;
  phy_state->cfg.pdsch.pdsch_time_is_default = true;

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = tti;
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy_state->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return buffer_sz;
}

bool cc_worker::work_dl()
{
  srslte_dci_dl_nr_t                                      dci_dl      = {};
  srslte_sch_cfg_nr_t                                     pdsch_cfg   = phy_state->cfg.pdsch;
  std::array<srslte_pdsch_res_nr_t, SRSLTE_MAX_CODEWORDS> pdsch_res   = {};

  // Run FFT
  srslte_ue_dl_nr_estimate_fft(&ue_dl, &dl_slot_cfg);

  // Set rnti
  rnti = 0x1234;

  // Configure Search space
  search_space.type = srslte_search_space_type_ue;
  for (uint32_t L = 0; L < SRSLTE_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR; L++) {
    search_space.nof_candidates[L] = srslte_pdcch_nr_max_candidates_coreset(&coreset, L);
  }

  srslte_dci_dl_nr_t dci_dl_rx = {};
  int nof_found_dci            = srslte_ue_dl_nr_find_dl_dci(&ue_dl, &search_space, &dl_slot_cfg, rnti, &dci_dl_rx, 1);
  if (nof_found_dci < SRSLTE_SUCCESS) {
    ERROR("Error decoding\n");
    return SRSLTE_ERROR;
  }

  if (nof_found_dci < 1) {
    ERROR("Error DCI not found\n");
  }

  dci_dl.rnti   = 0x1234;
  dci_dl.format = srslte_dci_format_nr_1_0;

  dci_dl.freq_domain_assigment = 0x1FFF;
  dci_dl.time_domain_assigment = 0;
  dci_dl.mcs                   = 27;

  // Compute DL grant
  if (srslte_ra_dl_dci_to_grant_nr(&ue_dl.carrier, &pdsch_cfg, &dci_dl, &pdsch_cfg.grant)) {
    ERROR("Computing DL grant\n");
  }

  pdsch_res[0].payload                = data.data();
  pdsch_cfg.grant.tb[0].softbuffer.rx = &softbuffer_rx;
  srslte_softbuffer_rx_reset(pdsch_cfg.grant.tb[0].softbuffer.rx);

  if (srslte_ue_dl_nr_decode_pdsch(&ue_dl, &dl_slot_cfg, &pdsch_cfg, pdsch_res.data()) < SRSLTE_SUCCESS) {
    ERROR("Error decoding PDSCH\n");
    return false;
  }

  // Logging
  if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
    char str[512];
    srslte_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, pdsch_res.data(), str, sizeof(str));
    log_h->info("PDSCH: cc=%d, %s\n", cc_idx, str);
  }

  return true;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  uint32_t nof_re = ue_dl.carrier.nof_prb * SRSLTE_NRE * 12;
  srslte_vec_cf_copy(pdsch_d, ue_dl.pdsch.d[0], nof_re);
  return nof_re;
}

} // namespace nr
} // namespace srsue
