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
#include "srsue/hdr/phy/nr/worker_pool.h"

namespace srsue {
namespace nr {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(const phy_args_nr_t&    args,
                       phy_common*             common,
                       stack_interface_phy_nr* stack_,
                       srslog::sink&           log_sink,
                       int                     prio)
{
  phy_state.stack = stack_;
  phy_state.args  = args;

  // Set carrier attributes
  phy_state.carrier.id      = 500;
  phy_state.carrier.nof_prb = args.nof_prb;

  // Set NR arguments
  phy_state.args.nof_carriers   = args.nof_carriers;
  phy_state.args.dl.nof_max_prb = args.nof_prb;

  // Skip init of workers if no NR carriers
  if (phy_state.args.nof_carriers == 0) {
    return true;
  }

  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("PHY{}", i), log_sink);
    log.set_level(srslog::str_to_basic_level(args.log.phy_level));
    log.set_hex_dump_max_size(args.log.phy_hex_limit);

    auto w = new sf_worker(common, &phy_state, log);
    pool.init_worker(i, w, prio, args.worker_cpu_mask);
    workers.push_back(std::unique_ptr<sf_worker>(w));

    if (not w->set_carrier_unlocked(0, &phy_state.carrier)) {
      return false;
    }
  }

  // Initialise PRACH
  auto& prach_log = srslog::fetch_basic_logger("NR-PRACH", log_sink);
  prach_log.set_level(srslog::str_to_basic_level(args.log.phy_level));
  prach_buffer = std::unique_ptr<prach>(new prach(prach_log));
  prach_buffer->init(phy_state.args.dl.nof_max_prb);

  // Set PRACH hard-coded cell
  srslte_cell_t cell = {};
  cell.nof_prb       = 50;
  cell.id            = phy_state.carrier.id;
  if (not prach_buffer->set_cell(cell, phy_state.cfg.prach)) {
    prach_log.error("Setting PRACH cell");
    return false;
  }

  return true;
}

void worker_pool::start_worker(sf_worker* w)
{
  pool.start_worker(w);
}

sf_worker* worker_pool::wait_worker(uint32_t tti)
{
  sf_worker* worker = (sf_worker*)pool.wait_worker(tti);

  // Generate PRACH if ready
  if (prach_buffer->is_ready_to_send(tti, phy_state.carrier.id)) {
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

int worker_pool::set_ul_grant(std::array<uint8_t, SRSLTE_RAR_UL_GRANT_NBITS> array)
{
  // Copy DCI bits and setup DCI context
  srslte_dci_msg_nr_t dci_msg = {};
  dci_msg.format              = srslte_dci_format_nr_rar;
  dci_msg.rnti_type           = srslte_rnti_type_ra;
  dci_msg.rnti                = phy_state.ra_rnti;
  dci_msg.nof_bits            = SRSLTE_RAR_UL_GRANT_NBITS;
  srslte_vec_u8_copy(dci_msg.payload, array.data(), SRSLTE_RAR_UL_GRANT_NBITS);

  srslte_dci_ul_nr_t dci_ul = {};

  if (srslte_dci_nr_format_0_0_unpack(&phy_state.carrier, &phy_state.cfg.pdcch.coreset[1], &dci_msg, &dci_ul) <
      SRSLTE_SUCCESS) {
    return SRSLTE_ERROR;
  }

  phy_state.set_ul_pending_grant(phy_state.rar_grant_tti, dci_ul);

  return SRSLTE_SUCCESS;
}
bool worker_pool::set_config(const srslte::phy_cfg_nr_t& cfg)
{
  phy_state.cfg = cfg;
  return true;
}
} // namespace nr
} // namespace srsue