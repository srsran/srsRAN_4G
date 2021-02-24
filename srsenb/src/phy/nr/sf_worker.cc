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

#include "srsenb/hdr/phy/nr/sf_worker.h"

namespace srsenb {
namespace nr {
sf_worker::sf_worker(phy_common* phy_, phy_nr_state* phy_state_, srslog::basic_logger& logger) :
  phy(phy_), phy_state(phy_state_), logger(logger)
{
  for (uint32_t i = 0; i < phy_state->args.nof_carriers; i++) {
    cc_worker* w = new cc_worker(i, logger, phy_state);
    cc_workers.push_back(std::unique_ptr<cc_worker>(w));
  }

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }
  data.resize(SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8);
  srslte_vec_u8_zero(data.data(), SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8);
  snprintf((char*)data.data(), SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8, "hello world!");
}

sf_worker::~sf_worker()
{
  srslte_softbuffer_tx_free(&softbuffer_tx);
}

bool sf_worker::set_carrier_unlocked(uint32_t cc_idx, const srslte_carrier_nr_t* carrier_)
{
  if (cc_idx >= cc_workers.size()) {
    return false;
  }

  return cc_workers.at(cc_idx)->set_carrier(carrier_);
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

void sf_worker::set_tti(uint32_t tti)
{
  logger.set_context(tti);
  for (auto& w : cc_workers) {
    w->set_tti(tti);
  }
}

void sf_worker::work_imp()
{
  // Get Transmission buffers
  srslte::rf_buffer_t    tx_buffer = {};
  srslte::rf_timestamp_t dummy_ts  = {};
  for (uint32_t cc = 0; cc < phy->get_nof_carriers_nr(); cc++) {
    for (uint32_t ant = 0; ant < phy->get_nof_ports(0); ant++) {
      tx_buffer.set(cc, ant, phy->get_nof_ports(0), cc_workers[cc]->get_tx_buffer(ant));
    }
  }

  // Configure user
  phy_state->cfg.pdsch.rbg_size_cfg_1 = false;

  // Fill grant (this comes from the scheduler)
  srslte_slot_cfg_t                  dl_cfg = {};
  stack_interface_phy_nr::dl_sched_t grants = {};

  grants.nof_grants                = 1;
  grants.pdsch[0].data[0]          = data.data();
  grants.pdsch[0].softbuffer_tx[0] = &softbuffer_tx;
  srslte_softbuffer_tx_reset(&softbuffer_tx);

  grants.pdsch[0].dci.rnti   = 0x1234;
  grants.pdsch[0].dci.format = srslte_dci_format_nr_1_0;

  grants.pdsch[0].dci.freq_domain_assigment = 0x1FFF;
  grants.pdsch[0].dci.time_domain_assigment = 0;
  grants.pdsch[0].dci.mcs                   = 27;

  grants.pdsch[0].dci.search_space  = srslte_search_space_type_ue;
  grants.pdsch[0].dci.coreset_id    = 1;
  grants.pdsch[0].dci.location.L    = 0;
  grants.pdsch[0].dci.location.ncce = 0;

  for (auto& w : cc_workers) {
    w->work_dl(dl_cfg, grants);
  }

  phy->worker_end(this, tx_buffer, dummy_ts, true);
}

} // namespace nr
} // namespace srsenb