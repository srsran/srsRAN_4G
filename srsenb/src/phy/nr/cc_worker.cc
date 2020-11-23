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

#include "srsenb/hdr/phy/nr/cc_worker.h"
#include "srslte/phy/enb/enb_dl_nr.h"
#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/ue/ue_dl_nr_data.h"

namespace srsenb {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslte::log* log, phy_nr_state* phy_state_) :
  cc_idx(cc_idx_), phy_state(phy_state_), log_h(log)
{
  cf_t* buffer_c[SRSLTE_MAX_PORTS] = {};

  // Allocate buffers
  uint32_t sf_len = SRSLTE_SF_LEN_PRB(phy_state->args.max_prb);
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_tx_antennas; i++) {
    tx_buffer[i] = srslte_vec_cf_malloc(sf_len);
    rx_buffer[i] = srslte_vec_cf_malloc(sf_len);
    buffer_c[i]  = tx_buffer[i];
  }

  if (srslte_enb_dl_nr_init(&enb_dl, buffer_c, &phy_state_->args.dl)) {
    ERROR("Error initiating UE DL NR\n");
    return;
  }

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer\n");
    return;
  }
  data.resize(SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB);
  srslte_random_t r = srslte_random_init(1234);
  for (uint32_t i = 0; i < SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB; i++) {
    data[i] = srslte_random_uniform_int_dist(r, 0, UINT8_MAX);
  }
  srslte_random_free(r);
}

cc_worker::~cc_worker()
{
  srslte_enb_dl_nr_free(&enb_dl);
  srslte_softbuffer_tx_free(&softbuffer_tx);
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
  if (srslte_enb_dl_nr_set_carrier(&enb_dl, carrier) < SRSLTE_SUCCESS) {
    return false;
  }

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = tti;
}

cf_t* cc_worker::get_tx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy_state->args.dl.nof_tx_antennas) {
    return nullptr;
  }

  return tx_buffer.at(antenna_idx);
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy_state->args.dl.nof_tx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return tx_buffer.size();
}

bool cc_worker::work_dl()
{
  srslte_pdsch_grant_nr_t pdsch_grant = {};
  srslte_pdsch_cfg_nr_t   pdsch_cfg   = phy_state->cfg.pdsch;
  // Use grant default A time resources with m=0
  if (srslte_ue_dl_nr_pdsch_time_resource_default_A(0, pdsch_cfg.dmrs_cfg_typeA.typeA_pos, &pdsch_grant) <
      SRSLTE_SUCCESS) {
    ERROR("Error loading default grant\n");
    return false;
  }
  pdsch_grant.nof_layers = enb_dl.carrier.max_mimo_layers;
  pdsch_grant.dci_format = srslte_dci_format_nr_1_0;
  pdsch_grant.rnti       = 0x1234;

  for (uint32_t i = 0; i < enb_dl.carrier.nof_prb; i++) {
    pdsch_grant.prb_idx[i] = true;
  }

  if (srslte_ra_nr_fill_tb(&pdsch_cfg, &pdsch_grant, 27, &pdsch_grant.tb[0]) < SRSLTE_SUCCESS) {
    ERROR("Error filing tb\n");
    return false;
  }

  uint8_t* data2[SRSLTE_MAX_TB]   = {data.data(), data.data()};
  pdsch_grant.tb[0].softbuffer.tx = &softbuffer_tx;
  srslte_softbuffer_tx_reset(pdsch_grant.tb[0].softbuffer.tx);

  if (srslte_enb_dl_nr_pdsch_put(&enb_dl, &dl_slot_cfg, &pdsch_cfg, &pdsch_grant, data2) < SRSLTE_SUCCESS) {
    ERROR("Error decoding PDSCH\n");
    return false;
  }

  // Logging
  if (log_h->get_level() >= srslte::LOG_LEVEL_INFO) {
    char str[512];
    srslte_enb_dl_nr_pdsch_info(&enb_dl, &pdsch_cfg, &pdsch_grant, str, sizeof(str));
    log_h->info("PDSCH: cc=%d, %s\n", cc_idx, str);
  }

  srslte_enb_dl_nr_gen_signal(&enb_dl);

  return true;
}

} // namespace nr
} // namespace srsenb