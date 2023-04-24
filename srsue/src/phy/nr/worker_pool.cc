/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/common/band_helper.h"

namespace srsue {
namespace nr {

worker_pool::worker_pool(srslog::basic_logger& logger_, uint32_t max_workers) : pool(max_workers), logger(logger_) {}

bool worker_pool::init(const phy_args_nr_t& args, srsran::phy_common_interface& common, stack_interface_phy_nr* stack_)
{
  phy_state.stack = stack_;
  phy_state.args  = args;

  {
    std::lock_guard<std::mutex> lock(cfg_mutex);
    pending_cfgs.resize(args.nof_phy_threads);
    for (auto&& b : pending_cfgs) {
      b = false;
    }
    // Set carrier attributes
    cfg.carrier.pci     = 500;
    cfg.carrier.nof_prb = args.max_nof_prb;
  }

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
    auto& log = srslog::fetch_basic_logger(fmt::format("{}PHY{}-NR", args.log.id_preamble, i));
    log.set_level(srslog::str_to_basic_level(args.log.phy_level));
    log.set_hex_dump_max_size(args.log.phy_hex_limit);

    sf_worker* w = nullptr;
    {
      std::lock_guard<std::mutex> lock(cfg_mutex);
      w = new sf_worker(common, phy_state, cfg, log);
    }
    pool.init_worker(i, w, args.workers_thread_prio, args.worker_cpu_mask);
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

  uint32_t pci = 0;
  {
    std::lock_guard<std::mutex> lock(cfg_mutex);
    pci = cfg.carrier.pci;
    if (pending_cfgs[worker->get_id()]) {
      pending_cfgs[worker->get_id()] = false;
      worker->set_cfg(cfg);
    }
  }

  // Generate PRACH if ready
  if (prach_buffer->is_ready_to_send(tti, pci)) {
    prach_ptr = prach_buffer->generate(phy_state.get_ul_cfo() / 15000, &prach_nof_sf, &prach_target_power);

    // Scale signal to maximum
    {
      float* ptr   = (float*)prach_ptr;
      int    max_i = srsran_vec_max_abs_fi(ptr, 2 * sf_sz);
      float  max   = ptr[max_i];
      if (std::isnormal(max)) {
        srsran_vec_sc_prod_cfc(prach_ptr, 0.99f / max, prach_ptr, sf_sz * prach_nof_sf);
      }
    }

    uint32_t                    config_idx = 0;
    srsran_duplex_mode_t        mode       = SRSRAN_DUPLEX_MODE_FDD;
    srsran_subcarrier_spacing_t scs        = srsran_subcarrier_spacing_15kHz;
    {
      std::lock_guard<std::mutex> lock(cfg_mutex);
      config_idx = cfg.prach.config_idx;
      mode       = cfg.duplex.mode;
      scs        = cfg.carrier.scs;
    }

    // Notify MAC about PRACH transmission
    phy_state.stack->prach_sent(
        TTI_TX(tti), srsran_prach_nr_start_symbol(config_idx, mode), SRSRAN_SLOT_NR_MOD(scs, TTI_TX(tti)), 0, 0);
  }

  // Set PRACH transmission buffer in workers if it is pending
  if (prach_nof_sf > 0) {
    // Set worker PRACH buffer
    worker->set_prach(&prach_ptr[sf_sz * prach_sf_count], prach_target_power);

    // Increment SF counter
    prach_sf_count++;

    // Reset PRACH pending subframe count
    if (prach_sf_count >= prach_nof_sf) {
      prach_nof_sf   = 0;
      prach_sf_count = 0;
    }
  }

  return worker;
}

void worker_pool::stop()
{
  pool.stop();
}

void worker_pool::send_prach(const uint32_t prach_occasion,
                             const int      preamble_index,
                             const float    preamble_received_target_power,
                             const float    ta_base_sec)
{
  prach_buffer->prepare_to_send(preamble_index);
}

// called from Stack thread when processing RAR PDU
int worker_pool::set_rar_grant(uint32_t                                       rar_slot_idx,
                               std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
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
    logger.error("Couldn't unpack UL grant");
    return SRSRAN_ERROR;
  }

  // initialize with Rx TTI of RAR
  srsran_slot_cfg_t msg3_slot_cfg = {};
  msg3_slot_cfg.idx               = rar_slot_idx;

  if (logger.info.enabled()) {
    std::array<char, 512> str;
    srsran_dci_nr_t       dci = {};
    srsran_dci_ul_nr_to_str(&dci, &dci_ul, str.data(), str.size());
    logger.set_context(msg3_slot_cfg.idx);
    logger.info("Setting RAR Grant: %s", str.data());
  }

  std::lock_guard<std::mutex> lock(cfg_mutex);
  phy_state.set_ul_pending_grant(cfg, msg3_slot_cfg, dci_ul);

  return SRSRAN_SUCCESS;
}

bool worker_pool::set_config(const srsran::phy_cfg_nr_t& new_cfg)
{
  uint32_t dl_arfcn = srsran::srsran_band_helper().freq_to_nr_arfcn(new_cfg.carrier.dl_center_frequency_hz);
  sf_sz             = SRSRAN_SF_LEN_PRB_NR(new_cfg.carrier.nof_prb);

  bool carrier_equal;
  {
    std::lock_guard<std::mutex> lock(cfg_mutex);

    // Check if the carrier has changed
    carrier_equal = cfg.carrier_is_equal(new_cfg);

    // If the carrier has not changed, reset pending flags. Configuration will be copied when the worker is reserved
    // from the real-time thread
    for (auto&& b : pending_cfgs) {
      b = carrier_equal;
    }

    // Update configuration
    cfg = new_cfg;
  }

  // If the carrier has changed, the configuration cannot be set from the real-time thread
  if (not carrier_equal) {
    // Configure each worker with the new configuration
    for (uint32_t i = 0; i < (uint32_t)workers.size(); i++) {
      // Wait for each worker to avoid concurrency issues
      sf_worker* w = (sf_worker*)pool.wait_worker_id(i);
      if (w == nullptr) {
        // Unlikely to happen
        continue;
      }

      // Configure worker
      w->set_cfg(new_cfg);

      // Release worker
      w->release();

      // As the worker has been configured, there is no need to load new configuration from the real-time thread
      {
        std::lock_guard<std::mutex> lock(cfg_mutex);
        pending_cfgs[i] = false;
      }
    }
  }

  logger.info("Setting new PHY configuration ARFCN=%d, PCI=%d", dl_arfcn, new_cfg.carrier.pci);

  // Set carrier information
  info_metrics_t info = {};
  info.pci            = new_cfg.carrier.pci;
  info.dl_earfcn      = dl_arfcn;
  phy_state.set_info_metrics(info);

  // Best effort to convert NR carrier into LTE cell
  srsran_cell_t cell = {};
  int           ret  = srsran_carrier_to_cell(&new_cfg.carrier, &cell);
  if (ret < SRSRAN_SUCCESS) {
    logger.error("Converting carrier to cell for PRACH (%d)", ret);
    return false;
  }

  // Best effort to set up NR-PRACH config reused for NR
  srsran_prach_cfg_t prach_cfg           = new_cfg.prach;
  uint32_t           lte_nr_prach_offset = (new_cfg.carrier.nof_prb - cell.nof_prb) / 2;
  if (prach_cfg.freq_offset < lte_nr_prach_offset) {
    logger.error("prach_cfg.freq_offset=%d is not compatible with LTE", prach_cfg.freq_offset);
    return false;
  }
  prach_cfg.freq_offset -= lte_nr_prach_offset;
  prach_cfg.tdd_config.configured = (new_cfg.duplex.mode == SRSRAN_DUPLEX_MODE_TDD);

  // Set the PRACH configuration
  if (not prach_buffer->set_cell(cell, prach_cfg)) {
    logger.error("Error setting PRACH cell");
    return false;
  }

  return true;
}

bool worker_pool::has_valid_sr_resource(uint32_t sr_id)
{
  std::lock_guard<std::mutex> lock(cfg_mutex);
  return phy_state.has_valid_sr_resource(cfg, sr_id);
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
