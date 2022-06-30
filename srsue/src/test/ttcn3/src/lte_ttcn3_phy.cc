/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "lte_ttcn3_phy.h"

namespace srsue {

#define SUITABLE_CELL_RS_EPRE (-97.0)      // Threshold for cell-search and cell-select
#define NON_SUITABLE_CELL_RS_EPRE (-115.0) // Threshold until sync can be maintained
#define NON_SUITABLE_OFF_CELL_RS_EPRE (-145.0)
#define SUITABLE_NEIGHBOR_INTRAFREQ_RS_EPRE (-91)
#define DEFAULT_RSRQ (-3.0)

lte_ttcn3_phy::lte_ttcn3_phy() : logger(srslog::fetch_basic_logger("PHY")) {}

int lte_ttcn3_phy::init(const phy_args_t& args_, stack_interface_phy_lte* stack_, syssim_interface_phy* syssim_)
{
  stack  = stack_;
  syssim = syssim_;

  logger.set_level(srslog::str_to_basic_level(args_.log.phy_level));
  logger.set_hex_dump_max_size(-1);

  return SRSRAN_SUCCESS;
}

void lte_ttcn3_phy::stop(){};

bool lte_ttcn3_phy::is_initialized()
{
  return true;
}

void lte_ttcn3_phy::wait_initialize() {}

void lte_ttcn3_phy::start_plot() {}

void lte_ttcn3_phy::get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m) {}

// The interface for the SS
void lte_ttcn3_phy::set_cell_map(const cell_list_t& cells_)
{
  std::lock_guard<std::mutex> lock(phy_mutex);
  cells = cells_;
}

void lte_ttcn3_phy::set_config_tdd(srsran_tdd_config_t& tdd_config) {}

void lte_ttcn3_phy::deactivate_scells()
{
  logger.debug("%s not implemented.", __FUNCTION__);
}

void lte_ttcn3_phy::set_activation_deactivation_scell(uint32_t cmd, uint32_t tti)
{
  logger.debug("%s not implemented.", __FUNCTION__);
}

bool lte_ttcn3_phy::set_config(const srsran::phy_cfg_t& config, uint32_t cc_idx_)
{
  logger.debug("%s not implemented.", __FUNCTION__);
  task_sched.defer_task([this]() { stack->set_config_complete(true); });
  return true;
}

bool lte_ttcn3_phy::set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn)
{
  logger.debug("%s not implemented.", __FUNCTION__);
  task_sched.defer_task([this]() { stack->set_scell_complete(true); });
  return true;
}

// Measurements interface
void lte_ttcn3_phy::meas_stop() {}

// Cell search and selection procedures
// Note that in contrast to a real PHY, we have visibility of all existing cells
// configured by the SS, including the ones that we should not even detect because
// their power is too weak. The cell search should only report the cells that
// are actually visible though.
bool lte_ttcn3_phy::cell_search(int earfcn)
{
  std::lock_guard<std::mutex> lock(phy_mutex);

  logger.info("Running cell search in PHY");

  rrc_interface_phy_lte::cell_search_ret_t ret = {};
  ret.found                                    = rrc_interface_phy_lte::cell_search_ret_t::CELL_NOT_FOUND;
  phy_cell_t found_cell                        = {};

  if (not cells.empty() && cell_idx < cells.size()) {
    // only find suitable cells
    if (cells[cell_idx].power >= SUITABLE_CELL_RS_EPRE) {
      logger.info("Found Cell: EARFCN=%d CellId=%d power=%.2f",
                  cells[cell_idx].earfcn,
                  cells[cell_idx].info.id,
                  cells[cell_idx].power);
      found_cell.earfcn = cells[cell_idx].earfcn;
      found_cell.pci    = cells[cell_idx].info.id;
      ret.found         = rrc_interface_phy_lte::cell_search_ret_t::CELL_FOUND;
    }

    // advance index
    cell_idx++;

    if (cell_idx < cells.size()) {
      // more cells will be reported
      ret.last_freq = rrc_interface_phy_lte::cell_search_ret_t::MORE_FREQS;
    } else {
      // all available cells have been reported, reset cell index
      ret.last_freq = rrc_interface_phy_lte::cell_search_ret_t::NO_MORE_FREQS;
      cell_idx      = 0;
    }
  } else {
    logger.warning("No cells configured yet.");
  }

  stack->cell_search_complete(ret, found_cell);

  return true;
}

// Called from RRC/Stack thread
bool lte_ttcn3_phy::cell_select(phy_cell_t rrc_cell)
{
  bool ret = false;

  {
    std::lock_guard<std::mutex> lock(phy_mutex);
    // try to find RRC cell in current cell map
    for (auto& cell : cells) {
      if (cell.info.id == rrc_cell.pci && cell.earfcn == rrc_cell.earfcn) {
        if (cell.power >= SUITABLE_CELL_RS_EPRE) {
          pcell     = cell;
          pcell_set = true;
          logger.info("Select PCell with %.2f on PCI=%d on EARFCN=%d.", cell.power, rrc_cell.pci, rrc_cell.earfcn);
        } else {
          pcell_set = false;
          logger.error("Power of selected cell too low (%.2f < %.2f)", cell.power, SUITABLE_CELL_RS_EPRE);
        }
        // update return value
        ret = pcell_set;
        break;
      }
    }
  }

  if (ret) {
    // cell has been selected
    syssim->select_cell(pcell.info);
  } else {
    logger.error(
        "Couldn't find (suitable) RRC cell with PCI=%d on EARFCN=%d in cell map.", rrc_cell.pci, rrc_cell.earfcn);
  }

  // inform stack about result asynchronously
  task_sched.defer_task([this, ret]() { stack->cell_select_complete(ret); });

  // regardless of actual result, return True to tell RRC that we entered the cell select state at PHY
  return true;
}

bool lte_ttcn3_phy::cell_is_camping()
{
  if (pcell_set) {
    logger.info("pcell.power=%2.f", pcell.power);
    return (pcell.power >= SUITABLE_CELL_RS_EPRE);
  }
  return false;
}

// The interface for MAC (called from Stack thread context)
void lte_ttcn3_phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec)
{
  uint32_t pcell_pci = 0;
  {
    std::lock_guard<std::mutex> lock(phy_mutex);
    pcell_pci = pcell.info.id;

    logger.info("Sending PRACH with preamble %d on PCID=%d", preamble_idx, pcell_pci);
    prach_tti_tx = current_tti;
    ra_trans_cnt++;
  }

  syssim->prach_indication(preamble_idx, pcell_pci);
};

std::string lte_ttcn3_phy::get_type()
{
  return "lte_ttcn3";
}

phy_interface_mac_lte::prach_info_t lte_ttcn3_phy::prach_get_info()
{
  std::lock_guard<std::mutex> lock(phy_mutex);
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

/* Time advance commands */
void lte_ttcn3_phy::set_timeadv_rar(uint32_t tti, uint32_t ta_cmd)
{
  logger.debug("%s not implemented.", __FUNCTION__);
}

void lte_ttcn3_phy::set_timeadv(uint32_t tti, uint32_t ta_cmd)
{
  logger.debug("%s not implemented.", __FUNCTION__);
}

// Sets RAR grant payload
void lte_ttcn3_phy::set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti)
{
  // Empty, SYSSIM knows when to provide UL grant for Msg3
  logger.debug("%s not implemented.", __FUNCTION__);
}

// Called from the SYSSIM to configure the current TTI
void lte_ttcn3_phy::set_current_tti(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(phy_mutex);

  current_tti = tti;
  run_tti();
}

// Called from MAC to retrieve the current TTI
uint32_t lte_ttcn3_phy::get_current_tti()
{
  std::lock_guard<std::mutex> lock(phy_mutex);
  return current_tti;
}

float lte_ttcn3_phy::get_phr()
{
  logger.debug("%s not implemented.", __FUNCTION__);
  return 0.1;
}

float lte_ttcn3_phy::get_pathloss_db()
{
  logger.debug("%s not implemented.", __FUNCTION__);
  return 85.0;
}

// Only provides a new UL grant, Tx is then triggered
// Calling function hold mutex
void lte_ttcn3_phy::new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t ul_mac_grant)
{
  std::lock_guard<std::mutex> lock(phy_mutex);

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
  std::lock_guard<std::mutex> lock(phy_mutex);

  if (data == nullptr) {
    logger.error("Invalid data buffer passed");
    return;
  }

  // pass grant to MAC to retrieve DL action
  mac_interface_phy_lte::tb_action_dl_t dl_action = {};

  stack->new_grant_dl(cc_idx, dl_grant, &dl_action);

  bool dl_ack[SRSRAN_MAX_CODEWORDS] = {};

  if (dl_action.tb[0].enabled && dl_action.tb[0].payload != nullptr) {
    logger.info(data,
                dl_grant.tb[0].tbs,
                "TB received rnti=%d, tti=%d, n_bytes=%d",
                dl_grant.rnti,
                current_tti,
                dl_grant.tb[0].tbs);

    if (dl_action.generate_ack) {
      logger.debug("Calling generate ACK callback");
      // action.generate_ack_callback(action.generate_ack_callback_arg);
    }
    memcpy(dl_action.tb->payload, data, dl_grant.tb[0].tbs);

    // ACK first TB and pass up
    dl_ack[0] = true;

    logger.info("TB processed correctly");
  } else {
    logger.error("Couldn't get buffer for TB");
  }

  stack->tb_decoded(cc_idx, dl_grant, dl_ack);
}

// Calling function set_tti() is holding mutex
void lte_ttcn3_phy::run_tti()
{
  // send report for all cells stronger than non-suitable cell RS
  std::vector<phy_meas_t> phy_meas;
  for (auto& cell : cells) {
    if (cell.power >= NON_SUITABLE_CELL_RS_EPRE) {
      phy_meas_t m = {};
      m.pci        = cell.info.id;
      m.earfcn     = cell.earfcn;
      m.rsrp       = cell.power;
      m.rsrq       = DEFAULT_RSRQ;

      // Measurement for PCell needs to have EARFCN set to 0
      if (pcell_set && m.earfcn == pcell.earfcn && m.pci == pcell.info.id) {
        logger.debug("Creating Pcell measurement for PCI=%d, EARFCN=%d with RSRP=%.2f", m.pci, m.earfcn, m.rsrp);
        m.earfcn = 0;
      } else {
        logger.debug("Create cell measurement for PCI=%d, EARFCN=%d with RSRP=%.2f", m.pci, m.earfcn, m.rsrp);
      }
      phy_meas.push_back(m);
    }
  }

  if (not phy_meas.empty()) {
    stack->new_cell_meas(phy_meas);
  }

  // check if Pcell is in sync
  if (pcell_set) {
    for (auto& cell : cells) {
      if (cell.info.id == pcell.info.id) {
        // consider Pcell in-sync until reaching threshold
        if (cell.power >= NON_SUITABLE_CELL_RS_EPRE) {
          logger.debug("PCell id=%d power=%.2f -> sync", pcell.info.id, cell.power);
          stack->in_sync();
        } else {
          logger.debug("PCell id=%d power=%.2f -> out of sync", pcell.info.id, cell.power);
          stack->out_of_sync();
        }
        break; // make sure to call stack only once
      }
    }
  }

  logger.set_context(current_tti);

  // Check for SR
  if (sr_pending) {
    syssim->sr_req(current_tti);
    sr_pending = false;
    sr_tx_tti  = current_tti;
  }

  task_sched.run_pending_tasks();
}

void lte_ttcn3_phy::set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci)
{
  logger.debug("%s not implemented.", __FUNCTION__);
}

} // namespace srsue
