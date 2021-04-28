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

#include "srsenb/hdr/phy/nr/cc_worker.h"
#include "srsran/srsran.h"

namespace srsenb {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslog::basic_logger& log, phy_nr_state* phy_state_) :
  cc_idx(cc_idx_), phy_state(phy_state_), logger(log)
{
  cf_t* buffer_c[SRSRAN_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSRAN_SF_LEN_PRB(phy_state->args.dl.nof_max_prb);
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_tx_antennas; i++) {
    tx_buffer[i] = srsran_vec_cf_malloc(buffer_sz);
    rx_buffer[i] = srsran_vec_cf_malloc(buffer_sz);
    buffer_c[i]  = tx_buffer[i];
  }

  if (srsran_enb_dl_nr_init(&enb_dl, buffer_c, &phy_state_->args.dl)) {
    ERROR("Error initiating UE DL NR");
    return;
  }
}

cc_worker::~cc_worker()
{
  srsran_enb_dl_nr_free(&enb_dl);
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

bool cc_worker::set_carrier(const srsran_carrier_nr_t* carrier)
{
  if (srsran_enb_dl_nr_set_carrier(&enb_dl, carrier) < SRSRAN_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  srsran_coreset_t coreset  = {};
  coreset.freq_resources[0] = true; // Enable the bottom 6 PRB for PDCCH
  coreset.duration          = 2;

  srsran_dci_cfg_nr_t dci_cfg = phy_state->cfg.get_dci_cfg();
  if (srsran_enb_dl_nr_set_pdcch_config(&enb_dl, &phy_state->cfg.pdcch, &dci_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error setting coreset");
    return false;
  }

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = TTI_ADD(tti, FDD_HARQ_DELAY_UL_MS);
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

int cc_worker::encode_pdcch_dl(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    // Get PHY config for UE
    // ...

    // Put actual DCI
    if (srsran_enb_dl_nr_pdcch_put(&enb_dl, &dl_slot_cfg, &grants[i].dci) < SRSRAN_SUCCESS) {
      ERROR("Error putting PDCCH");
      return SRSRAN_ERROR;
    }

    if (logger.info.enabled()) {
      logger.info("PDCCH: cc=%d, ...", cc_idx);
    }
  }

  return SRSRAN_SUCCESS;
}

int cc_worker::encode_pdsch(stack_interface_phy_nr::dl_sched_grant_t* grants, uint32_t nof_grants)
{
  for (uint32_t i = 0; i < nof_grants; i++) {
    // Get PHY config for UE
    // ...
    srsran_sch_hl_cfg_nr_t pdsch_hl_cfg = {};
    srsran_sch_cfg_nr_t    pdsch_cfg    = {};

    // Compute DL grant
    if (srsran_ra_dl_dci_to_grant_nr(
            &enb_dl.carrier, &dl_slot_cfg, &pdsch_hl_cfg, &grants[i].dci, &pdsch_cfg, &pdsch_cfg.grant) <
        SRSRAN_SUCCESS) {
      ERROR("Computing DL grant");
      return false;
    }

    // Set soft buffer
    for (uint32_t j = 0; j < SRSRAN_MAX_CODEWORDS; j++) {
      pdsch_cfg.grant.tb[j].softbuffer.tx = grants[i].softbuffer_tx[j];
    }

    if (srsran_enb_dl_nr_pdsch_put(&enb_dl, &dl_slot_cfg, &pdsch_cfg, grants[i].data) < SRSRAN_SUCCESS) {
      ERROR("Error putting PDSCH");
      return false;
    }

    // Logging
    if (logger.info.enabled()) {
      char str[512];
      srsran_enb_dl_nr_pdsch_info(&enb_dl, &pdsch_cfg, str, sizeof(str));
      logger.info("PDSCH: cc=%d, %s", cc_idx, str);
    }
  }

  return SRSRAN_SUCCESS;
}

bool cc_worker::work_dl(const srsran_slot_cfg_t& dl_sf_cfg, stack_interface_phy_nr::dl_sched_t& dl_grants)
{
  // Reset resource grid
  if (srsran_enb_dl_nr_base_zero(&enb_dl) < SRSRAN_SUCCESS) {
    ERROR("Error setting base to zero");
    return SRSRAN_ERROR;
  }

  // Put DL grants to resource grid. PDSCH data will be encoded as well.
  encode_pdcch_dl(dl_grants.pdsch, dl_grants.nof_grants);
  encode_pdsch(dl_grants.pdsch, dl_grants.nof_grants);

  // Generate signal
  srsran_enb_dl_nr_gen_signal(&enb_dl);

  return true;
}

} // namespace nr
} // namespace srsenb
