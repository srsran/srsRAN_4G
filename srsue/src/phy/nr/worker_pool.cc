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
#include "srsue/hdr/phy/nr/worker_pool.h"

namespace srsue {
namespace nr {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers), logger(srslog::fetch_basic_logger("PHY-NR")) {}

bool worker_pool::init(const phy_args_nr_t& args, phy_common* common, stack_interface_phy_nr* stack_, int prio)
{
  phy_state.stack = stack_;
  phy_state.args  = args;

  // Set carrier attributes
  phy_state.cfg.carrier.pci     = 500;
  phy_state.cfg.carrier.nof_prb = args.max_nof_prb;

  // Set NR arguments
  phy_state.args.nof_carriers     = args.nof_carriers;
  phy_state.args.dl.nof_max_prb   = args.max_nof_prb;
  phy_state.args.dl.pdsch.max_prb = args.max_nof_prb;
  phy_state.args.ul.nof_max_prb   = args.max_nof_prb;
  phy_state.args.ul.pusch.max_prb = args.max_nof_prb;

  // Skip init of workers if no NR carriers
  if (phy_state.args.nof_carriers == 0) {
    return true;
  }

  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("PHY{}-NR", i));
    log.set_level(srslog::str_to_basic_level(args.log.phy_level));
    log.set_hex_dump_max_size(args.log.phy_hex_limit);

    auto w = new sf_worker(common, &phy_state, log);
    pool.init_worker(i, w, prio, args.worker_cpu_mask);
    workers.push_back(std::unique_ptr<sf_worker>(w));
  }

  // Set PHY loglevel
  logger.set_level(srslog::str_to_basic_level(args.log.phy_level));

  // Initialise PRACH
  prach_buffer = std::unique_ptr<prach>(new prach(logger));
  prach_buffer->init(phy_state.args.dl.nof_max_prb);

  return true;
}

void worker_pool::start_worker(sf_worker* w)
{
  // Push worker pointer for internal worker TTI synchronization
  phy_state.dl_ul_semaphore.push(w);

  // Signal worker to start processing asynchronously
  pool.start_worker(w);
}

sf_worker* worker_pool::wait_worker(uint32_t tti)
{
  logger.set_context(tti);
  sf_worker* worker = (sf_worker*)pool.wait_worker(tti);

  // Generate PRACH if ready
  if (prach_buffer->is_ready_to_send(tti, phy_state.cfg.carrier.pci)) {
    uint32_t nof_prach_sf       = 0;
    float    prach_target_power = 0.0f;
    cf_t*    prach_ptr          = prach_buffer->generate(0.0f, &nof_prach_sf, &prach_target_power);
    worker->set_prach(prach_ptr, prach_target_power);
  }

  return worker;
}

void worker_pool::stop()
{
  pool.stop();
}

void worker_pool::send_prach(uint32_t prach_occasion, uint32_t preamble_index, int preamble_received_target_power)
{
  prach_buffer->prepare_to_send(preamble_index);
}

int worker_pool::set_ul_grant(std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                              uint16_t                                       rnti,
                              srsran_rnti_type_t                             rnti_type)
{
  // Copy DCI bits and setup DCI context
  srsran_dci_msg_nr_t dci_msg = {};
  dci_msg.ctx.format          = srsran_dci_format_nr_rar; // MAC RAR grant shall be unpacked as DCI 0_0 format
  dci_msg.ctx.rnti_type       = rnti_type;
  dci_msg.ctx.ss_type         = srsran_search_space_type_rar; // This indicates it is a MAC RAR
  dci_msg.ctx.rnti            = rnti;
  dci_msg.nof_bits            = SRSRAN_RAR_UL_GRANT_NBITS;
  srsran_vec_u8_copy(dci_msg.payload, packed_ul_grant.data(), SRSRAN_RAR_UL_GRANT_NBITS);

  srsran_dci_ul_nr_t dci_ul = {};

  if (srsran_dci_nr_ul_unpack(NULL, &dci_msg, &dci_ul) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  if (logger.info.enabled()) {
    std::array<char, 512> str;
    srsran_dci_ul_nr_to_str(NULL, &dci_ul, str.data(), str.size());
    logger.set_context(phy_state.rar_grant_tti);
    logger.info("Setting RAR Grant %s", str.data());
  }

  phy_state.set_ul_pending_grant(phy_state.rar_grant_tti, dci_ul);

  return SRSRAN_SUCCESS;
}
bool worker_pool::set_config(const srsran::phy_cfg_nr_t& cfg)
{
  phy_state.cfg = cfg;

  logger.info(
      "Setting new PHY configuration ARFCN=%d, PCI=%d", cfg.carrier.absolute_frequency_point_a, cfg.carrier.pci);

  // Set carrier information
  info_metrics_t info = {};
  info.pci            = cfg.carrier.pci;
  info.dl_earfcn      = cfg.carrier.absolute_frequency_ssb;
  phy_state.set_info_metrics(info);

  // Best effort to convert NR carrier into LTE cell
  srsran_cell_t cell = {};
  int           ret  = srsran_carrier_to_cell(&phy_state.cfg.carrier, &cell);
  if (ret < SRSRAN_SUCCESS) {
    logger.error("Converting carrier to cell for PRACH (%d)", ret);
    return false;
  }

  // Request workers to run any procedure related to configuration update
  for (auto& w : workers) {
    if (not w->update_cfg(0)) {
      return false;
    }
  }

  // Best effort to set up NR-PRACH config reused for NR
  srsran_prach_cfg_t prach_cfg           = cfg.prach;
  uint32_t           lte_nr_prach_offset = (phy_state.cfg.carrier.nof_prb - cell.nof_prb) / 2;
  if (prach_cfg.freq_offset < lte_nr_prach_offset) {
    logger.error("prach_cfg.freq_offset=%d is not compatible with LTE", prach_cfg.freq_offset);
    return false;
  }
  prach_cfg.freq_offset -= lte_nr_prach_offset;

  // Set the PRACH configuration
  if (not prach_buffer->set_cell(cell, prach_cfg)) {
    logger.error("Error setting PRACH cell");
    return false;
  }

  return true;
}

bool worker_pool::has_valid_sr_resource(uint32_t sr_id)
{
  return phy_state.has_valid_sr_resource(sr_id);
}

void worker_pool::clear_pending_grants()
{
  phy_state.clear_pending_grants();
}

void worker_pool::get_metrics(phy_metrics_t& m)
{
  phy_state.get_metrics(m);
}

} // namespace nr
} // namespace srsue
