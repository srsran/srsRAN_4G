/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsenb/hdr/phy/nr/worker_pool.h"

namespace srsenb {
namespace nr {

worker_pool::worker_pool(srsran::phy_common_interface& common_,
                         stack_interface_phy_nr&       stack_,
                         srslog::sink&                 log_sink_,
                         uint32_t                      max_workers) :
  pool(max_workers), common(common_), stack(stack_), log_sink(log_sink_)
{
  // Do nothing
}

bool worker_pool::init(const args_t& args, const phy_cell_cfg_list_nr_t& cell_list)
{
  // Add workers to workers pool and start threads
  srslog::basic_levels log_level = srslog::str_to_basic_level(args.log.phy_level);
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("{}PHY{}-NR", args.log.id_preamble, i), log_sink);
    log.set_level(log_level);
    log.set_hex_dump_max_size(args.log.phy_hex_limit);

    auto w = new slot_worker(common, stack, log);
    pool.init_worker(i, w, args.prio);
    workers.push_back(std::unique_ptr<slot_worker>(w));

    slot_worker::args_t w_args     = {};
    uint32_t            cell_index = 0;
    w_args.cell_index              = cell_index;
    w_args.carrier                 = cell_list[cell_index].carrier;
    w_args.nof_tx_ports            = cell_list[cell_index].carrier.max_mimo_layers;
    w_args.nof_rx_ports            = cell_list[cell_index].carrier.max_mimo_layers;
    w_args.pusch_max_nof_iter      = args.pusch_max_nof_iter;
    w_args.pdcch_cfg               = cell_list[cell_index].pdcch;

    if (not w->init(w_args)) {
      return false;
    }
  }

  return true;
}

void worker_pool::start_worker(slot_worker* w)
{
  pool.start_worker(w);
}

slot_worker* worker_pool::wait_worker(uint32_t tti)
{
  return (slot_worker*)pool.wait_worker(tti);
}

slot_worker* worker_pool::wait_worker_id(uint32_t id)
{
  return (slot_worker*)pool.wait_worker_id(id);
}

void worker_pool::stop()
{
  pool.stop();
}

} // namespace nr
} // namespace srsenb