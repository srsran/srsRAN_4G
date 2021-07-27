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

#ifndef SRSRAN_SCHED_NR_H
#define SRSRAN_SCHED_NR_H

#include "sched_nr_cfg.h"
#include "sched_nr_interface.h"
#include "sched_nr_ue.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/common/slot_point.h"
#include <array>
extern "C" {
#include "srsran/config.h"
}

namespace srsenb {

namespace sched_nr_impl {
class sched_worker_manager;
class serv_cell_manager;
} // namespace sched_nr_impl

class ue_event_manager;
class sched_result_manager;

class sched_nr final : public sched_nr_interface
{
public:
  explicit sched_nr(const sched_cfg_t& sched_cfg);
  ~sched_nr() override;
  int  cell_cfg(srsran::const_span<cell_cfg_t> cell_list) override;
  void ue_cfg(uint16_t rnti, const ue_cfg_t& cfg) override;

  int dl_rach_info(uint32_t cc, const dl_sched_rar_info_t& rar_info);

  void dl_ack_info(uint16_t rnti, uint32_t cc, uint32_t pid, uint32_t tb_idx, bool ack) override;
  void ul_crc_info(uint16_t rnti, uint32_t cc, uint32_t pid, bool crc) override;
  void ul_sr_info(slot_point slot_rx, uint16_t rnti) override;

  int get_dl_sched(slot_point pdsch_tti, uint32_t cc, dl_sched_t& result) override;
  int get_ul_sched(slot_point pusch_tti, uint32_t cc, ul_sched_t& result) override;

private:
  int  generate_slot_result(slot_point pdcch_tti, uint32_t cc);
  void ue_cfg_impl(uint16_t rnti, const ue_cfg_t& cfg);

  // args
  sched_nr_impl::sched_params cfg;
  srslog::basic_logger&       logger;

  using sched_worker_manager = sched_nr_impl::sched_worker_manager;
  std::unique_ptr<sched_worker_manager> sched_workers;

  using ue_map_t = sched_nr_impl::ue_map_t;
  std::mutex ue_db_mutex;
  ue_map_t   ue_db;

  // management of Sched Result buffering
  std::unique_ptr<sched_result_manager> pending_results;

  // management of cell resources
  std::vector<std::unique_ptr<sched_nr_impl::serv_cell_manager> > cells;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_H
