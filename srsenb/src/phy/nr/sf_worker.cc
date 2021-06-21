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

#include "srsenb/hdr/phy/nr/sf_worker.h"

namespace srsenb {
namespace nr {
sf_worker::sf_worker(srsran::phy_common_interface& common_, phy_nr_state& phy_state_, srslog::basic_logger& logger) :
  common(common_), phy_state(phy_state_), logger(logger)
{
  const phy_cell_cfg_list_nr_t& carrier_list = phy_state.get_carrier_list();
  for (uint32_t i = 0; i < (uint32_t)carrier_list.size(); i++) {
    cc_worker::args_t cc_args  = {};
    cc_args.cc_idx             = i;
    cc_args.carrier            = carrier_list[i].carrier;
    cc_args.dl.nof_tx_antennas = 1;
    cc_args.dl.nof_max_prb     = cc_args.carrier.nof_prb;

    cc_worker* w = new cc_worker(cc_args, logger, phy_state);
    cc_workers.push_back(std::unique_ptr<cc_worker>(w));
  }

  if (srsran_softbuffer_tx_init_guru(&softbuffer_tx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }
  data.resize(SRSRAN_SCH_NR_MAX_NOF_CB_LDPC * SRSRAN_LDPC_MAX_LEN_ENCODED_CB / 8);
  srsran_vec_u8_zero(data.data(), SRSRAN_SCH_NR_MAX_NOF_CB_LDPC * SRSRAN_LDPC_MAX_LEN_ENCODED_CB / 8);
  snprintf((char*)data.data(), SRSRAN_SCH_NR_MAX_NOF_CB_LDPC * SRSRAN_LDPC_MAX_LEN_ENCODED_CB / 8, "hello world!");
}

sf_worker::~sf_worker()
{
  srsran_softbuffer_tx_free(&softbuffer_tx);
}

cf_t* sf_worker::get_buffer_rx(uint32_t cc_idx, uint32_t antenna_idx)
{
  if (cc_idx >= cc_workers.size()) {
    return nullptr;
  }

  return cc_workers.at(cc_idx)->get_rx_buffer(antenna_idx);
}

cf_t* sf_worker::get_buffer_tx(uint32_t cc_idx, uint32_t antenna_idx)
{
  if (cc_idx >= cc_workers.size()) {
    return nullptr;
  }

  return cc_workers.at(cc_idx)->get_tx_buffer(antenna_idx);
}

uint32_t sf_worker::get_buffer_len()
{
  return cc_workers.at(0)->get_buffer_len();
}

void sf_worker::set_time(const uint32_t& tti, const srsran::rf_timestamp_t& timestamp)
{
  logger.set_context(tti);
  for (auto& w : cc_workers) {
    w->set_tti(tti);
  }
  ul_slot_cfg.idx = tti;
  dl_slot_cfg.idx = TTI_ADD(tti, FDD_HARQ_DELAY_UL_MS);
  tx_time.copy(timestamp);
}

void sf_worker::work_imp()
{
  // Get Transmission buffers
  srsran::rf_buffer_t    tx_buffer = {};
  srsran::rf_timestamp_t dummy_ts  = {};
  for (uint32_t cc = 0; cc < (uint32_t)phy_state.get_carrier_list().size(); cc++) {
    tx_buffer.set(cc, 0, 1, cc_workers[cc]->get_tx_buffer(0));
  }

  // Get UL Scheduling
  mac_interface_phy_nr::ul_sched_list_t ul_sched_list = {};
  ul_sched_list.resize(1);
  if (phy_state.get_stack().get_ul_sched(ul_slot_cfg.idx, ul_sched_list) < SRSRAN_SUCCESS) {
    logger.error("DL Scheduling error");
    common.worker_end(this, true, tx_buffer, dummy_ts, true);
    return;
  }

  // Get DL scheduling
  mac_interface_phy_nr::dl_sched_list_t dl_sched_list = {};
  dl_sched_list.resize(1);
  if (phy_state.get_stack().get_dl_sched(ul_slot_cfg.idx, dl_sched_list) < SRSRAN_SUCCESS) {
    logger.error("DL Scheduling error");
    common.worker_end(this, true, tx_buffer, dummy_ts, true);
    return;
  }

  for (auto& w : cc_workers) {
    w->work_dl(dl_sched_list[0], ul_sched_list[0]);
  }

  common.worker_end(this, true, tx_buffer, dummy_ts, true);
}

} // namespace nr
} // namespace srsenb