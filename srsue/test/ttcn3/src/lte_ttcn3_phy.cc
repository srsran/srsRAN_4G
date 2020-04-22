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

#include "lte_ttcn3_phy.h"

namespace srsue {

#define MIN_IN_SYNC_POWER (-120.0)
#define DEFAULT_RSRQ (-3.0)

lte_ttcn3_phy::lte_ttcn3_phy(srslte::logger* logger_) : logger(logger_) {}

int lte_ttcn3_phy::init(const phy_args_t& args_, stack_interface_phy_lte* stack_, syssim_interface_phy* syssim_)
{
  stack  = stack_;
  syssim = syssim_;

  return init(args_);
}

int lte_ttcn3_phy::init(const phy_args_t& args_, stack_interface_phy_lte* stack_, srslte::radio_interface_phy* radio_)
{

  return init(args_);
}

// ue_phy_base interface
int lte_ttcn3_phy::init(const phy_args_t& args_)
{
  log.init("PHY ", logger, true);
  log.set_level(args_.log.phy_level);

  return SRSLTE_SUCCESS;
}

void lte_ttcn3_phy::stop(){};

void lte_ttcn3_phy::wait_initialize() {}

void lte_ttcn3_phy::start_plot() {}

void lte_ttcn3_phy::get_metrics(phy_metrics_t* m) {}

// The interface for the SS
void lte_ttcn3_phy::set_cell_map(const cell_list_t& cells_)
{
  std::lock_guard<std::mutex> lock(mutex);
  cells = cells_;
}

void lte_ttcn3_phy::set_config_tdd(srslte_tdd_config_t& tdd_config) {}

void lte_ttcn3_phy::enable_pregen_signals(bool enable)
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

void lte_ttcn3_phy::set_activation_deactivation_scell(uint32_t cmd)
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

void lte_ttcn3_phy::set_config(srslte::phy_cfg_t& config, uint32_t cc_idx, uint32_t earfcn, srslte_cell_t* cell_info)
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

// Measurements interface
void lte_ttcn3_phy::meas_stop(){};

/* Cell search and selection procedures */
phy_interface_rrc_lte::cell_search_ret_t lte_ttcn3_phy::cell_search(phy_cell_t* found_cell)
{
  std::lock_guard<std::mutex> lock(mutex);

  log.info("Running cell search in PHY\n");

  cell_search_ret_t ret = {};
  ret.found             = cell_search_ret_t::CELL_NOT_FOUND;

  if (not cells.empty() && cell_idx < cells.size()) {
    log.info("Found Cell: EARFCN=%d CellId=%d\n", cells[cell_idx].earfcn, cells[cell_idx].info.id);
    if (found_cell) {
      found_cell->earfcn = cells[cell_idx].earfcn;
      found_cell->pci    = cells[cell_idx].info.id;
    }
    ret.found = cell_search_ret_t::CELL_FOUND;

    // advance index
    cell_idx++;

    if (cell_idx < cells.size()) {
      // more cells will be reported
      ret.last_freq = cell_search_ret_t::MORE_FREQS;
    } else {
      // all available cells have been reported, reset cell index
      ret.last_freq = cell_search_ret_t::NO_MORE_FREQS;
      cell_idx      = 0;
    }
  } else {
    log.warning("No cells configured yet.\n");
  }

  return ret;
};

bool lte_ttcn3_phy::cell_select(const phy_cell_t* rrc_cell)
{
  // try to find RRC cell in current cell map
  for (auto& cell : cells) {
    if (cell.info.id == rrc_cell->pci) {
      if (cell.power >= MIN_IN_SYNC_POWER) {
        pcell     = cell;
        pcell_set = true;
        syssim->select_cell(pcell.info);
        log.info("Select PCell with %.2f on PCI=%d on EARFCN=%d.\n", cell.power, rrc_cell->pci, rrc_cell->earfcn);
      } else {
        pcell_set = false;
        log.error("Power of selected cell too low (%.2f < %.2f)\n", cell.power, MIN_IN_SYNC_POWER);
      }

      return pcell_set;
    }
  }

  log.error("Couldn't fine RRC cell with PCI=%d on EARFCN=%d in cell map.\n", rrc_cell->pci, rrc_cell->earfcn);
  return false;
};

bool lte_ttcn3_phy::cell_is_camping()
{
  if (pcell_set) {
    log.info("pcell.power=%2.f\n", pcell.power);
    return (pcell.power >= MIN_IN_SYNC_POWER);
  }
  return false;
};

void lte_ttcn3_phy::reset()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
};

// The interface for MAC
void lte_ttcn3_phy::configure_prach_params()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
};

void lte_ttcn3_phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec)
{
  log.info("Sending PRACH with preamble %d on PCID=%d\n", preamble_idx, pcell.info.id);
  prach_tti_tx = current_tti;
  ra_trans_cnt++;

  syssim->prach_indication(preamble_idx, pcell.info.id);
};

std::string lte_ttcn3_phy::get_type()
{
  return "lte_ttcn3";
}

phy_interface_mac_lte::prach_info_t lte_ttcn3_phy::prach_get_info()
{
  std::lock_guard<std::mutex> lock(mutex);
  prach_info_t                info = {};
  if (prach_tti_tx != -1) {
    info.is_transmitted = true;
    info.tti_ra         = prach_tti_tx;
  }
  return info;
}

/* Indicates the transmission of a SR signal in the next opportunity */
void lte_ttcn3_phy::sr_send()
{
  sr_pending = true;
  sr_tx_tti  = -1;
}

int lte_ttcn3_phy::sr_last_tx_tti()
{
  return sr_tx_tti;
}

// The RAT-agnostic interface for MAC

/* Sets a C-RNTI allowing the PHY to pregenerate signals if necessary */
void lte_ttcn3_phy::set_crnti(uint16_t rnti)
{
  log.info("Set Temp-RNTI=%d\n", rnti);
}

/* Time advance commands */
void lte_ttcn3_phy::set_timeadv_rar(uint32_t ta_cmd)
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

void lte_ttcn3_phy::set_timeadv(uint32_t ta_cmd)
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

// Sets RAR grant payload
void lte_ttcn3_phy::set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti)
{
  // Empty, SYSSIM knows when to provide UL grant for Msg3
}

// Called from the SYSSIM to configure the current TTI
void lte_ttcn3_phy::set_current_tti(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);

  current_tti = tti;
  run_tti();
}

// Called from MAC to retrieve the current TTI
uint32_t lte_ttcn3_phy::get_current_tti()
{
  return current_tti;
}

float lte_ttcn3_phy::get_phr()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
  return 0.1;
}

float lte_ttcn3_phy::get_pathloss_db()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
  return 85.0;
}

// Only provides a new UL grant, Tx is then triggered
// Calling function hold mutex
void lte_ttcn3_phy::new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant)
{
  std::lock_guard<std::mutex> lock(mutex);

  mac_interface_phy_lte::tb_action_ul_t ul_action = {};

  // Deliver grant and retrieve payload
  stack->new_grant_ul(cc_idx, ul_mac_grant, &ul_action);

  // Deliver MAC PDU to SYSSIM
  if (ul_action.tb.enabled and ul_action.tb.payload != nullptr) {
    syssim->tx_pdu(ul_action.tb.payload, ul_mac_grant.tb.tbs, ul_mac_grant.rnti);
  }
}

// Provides DL grant, copy data into DL action and pass up to MAC
void lte_ttcn3_phy::new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t dl_grant, const uint8_t* data)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (data == nullptr) {
    log.error("Invalid data buffer passed\n");
    return;
  }

  // pass grant to MAC to retrieve DL action
  mac_interface_phy_lte::tb_action_dl_t dl_action = {};

  stack->new_grant_dl(cc_idx, dl_grant, &dl_action);

  bool dl_ack[SRSLTE_MAX_CODEWORDS] = {};

  if (dl_action.tb[0].enabled && dl_action.tb[0].payload != nullptr) {
    log.info_hex(data,
                 dl_grant.tb[0].tbs,
                 "TB received rnti=%d, tti=%d, n_bytes=%d\n",
                 dl_grant.rnti,
                 current_tti,
                 dl_grant.tb[0].tbs);

    if (dl_action.generate_ack) {
      log.debug("Calling generate ACK callback\n");
      // action.generate_ack_callback(action.generate_ack_callback_arg);
    }
    memcpy(dl_action.tb->payload, data, dl_grant.tb[0].tbs);

    // ACK first TB and pass up
    dl_ack[0] = true;

    log.info("TB processed correctly\n");
  } else {
    log.error("Couldn't get buffer for TB\n");
  }

  stack->tb_decoded(cc_idx, dl_grant, dl_ack);
}

void lte_ttcn3_phy::radio_overflow()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

void lte_ttcn3_phy::radio_failure()
{
  log.debug("%s not implemented.\n", __FUNCTION__);
}

// Calling function set_tti() is holding mutex
void lte_ttcn3_phy::run_tti()
{
  // send report for each cell
  std::vector<rrc_interface_phy_lte::phy_meas_t> phy_meas;
  for (auto& cell : cells) {
    rrc_interface_phy_lte::phy_meas_t m = {};
    m.pci                               = cell.info.id;
    m.earfcn                            = cell.earfcn;
    m.rsrp                              = cell.power;
    m.rsrq                              = DEFAULT_RSRQ;

    // Measurement for PCell needs to have EARFCN set to 0
    if (pcell_set && m.earfcn == pcell.earfcn) {
      m.earfcn = 0;
    }

    log.debug("Create cell measurement for PCI=%d, EARFCN=%d with RSRP=%.2f\n", m.pci, m.earfcn, m.rsrp);
    phy_meas.push_back(m);
  }

  if (not phy_meas.empty()) {
    stack->new_cell_meas(phy_meas);
  }

  // check if Pcell is in sync
  if (pcell_set) {
    for (auto& cell : cells) {
      if (cell.info.id == pcell.info.id) {
        if (cell.power >= MIN_IN_SYNC_POWER) {
          log.debug("PCell id=%d power=%.2f -> sync\n", pcell.info.id, cell.power);
          stack->in_sync();
        } else {
          log.debug("PCell id=%d power=%.2f -> out of sync\n", pcell.info.id, cell.power);
          stack->out_of_sync();
        }
        break; // make sure to call stack only once
      }
    }
  }

  log.step(current_tti);

  // Check for SR
  if (sr_pending) {
    syssim->sr_req(current_tti);
    sr_pending = false;
    sr_tx_tti  = current_tti;
  }

  stack->run_tti(current_tti, 1);
}

void lte_ttcn3_phy::set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) {}

} // namespace srsue
