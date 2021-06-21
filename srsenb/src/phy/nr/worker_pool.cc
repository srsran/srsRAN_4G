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

worker_pool::worker_pool(const phy_cell_cfg_list_nr_t& cell_list,
                         const args_t&                 args,
                         srsran::phy_common_interface& common,
                         stack_interface_phy_nr&       stack,
                         srslog::sink&                 log_sink) :
  pool(args.nof_workers), phy_state(cell_list, stack)
{
  // Add workers to workers pool and start threads
  srslog::basic_levels log_level = srslog::str_to_basic_level(args.log_level);
  for (uint32_t i = 0; i < args.nof_workers; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("PHY{}-NR", i), log_sink);
    log.set_level(log_level);
    log.set_hex_dump_max_size(args.log_hex_limit);

    auto w = new sf_worker(common, phy_state, log);
    pool.init_worker(i, w, args.prio);
    workers.push_back(std::unique_ptr<sf_worker>(w));
  }
}

void worker_pool::start_worker(sf_worker* w)
{
  pool.start_worker(w);
}

sf_worker* worker_pool::wait_worker(uint32_t tti)
{
  return (sf_worker*)pool.wait_worker(tti);
}

sf_worker* worker_pool::wait_worker_id(uint32_t id)
{
  return (sf_worker*)pool.wait_worker_id(id);
}

void worker_pool::stop()
{
  pool.stop();
}

} // namespace nr
} // namespace srsenb