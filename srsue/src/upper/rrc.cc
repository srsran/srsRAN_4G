/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <srslte/asn1/liblte_rrc.h>
#include "upper/rrc.h"
#include "srslte/asn1/liblte_rrc.h"
#include "srslte/common/security.h"
#include "srslte/common/bcd_helpers.h"

using namespace srslte;

namespace srsue {


/*******************************************************************************
  Base functions 
*******************************************************************************/

rrc::rrc()
  :state(RRC_STATE_IDLE)
  ,drb_up(false)
  ,sysinfo_index(0)
  ,serving_cell(NULL)
{
  n310_cnt       = 0;
  n311_cnt       = 0;
  serving_cell = new cell_t();
}

rrc::~rrc()
{
  if (serving_cell) {
    delete(serving_cell);
  }
}

static void liblte_rrc_handler(void *ctx, char *str) {
  rrc *r = (rrc *) ctx;
  r->liblte_rrc_log(str);
}

void rrc::liblte_rrc_log(char *str) {
  if (rrc_log) {
    rrc_log->warning("[ASN]: %s\n", str);
  } else {
    printf("[ASN]: %s\n", str);
  }
}

void rrc::init(phy_interface_rrc *phy_,
               mac_interface_rrc *mac_,
               rlc_interface_rrc *rlc_,
               pdcp_interface_rrc *pdcp_,
               nas_interface_rrc *nas_,
               usim_interface_rrc *usim_,
               mac_interface_timers *mac_timers_,
               srslte::log *rrc_log_) {
  pool = byte_buffer_pool::get_instance();
  phy = phy_;
  mac = mac_;
  rlc = rlc_;
  pdcp = pdcp_;
  nas = nas_;
  usim = usim_;
  rrc_log = rrc_log_;

  // Use MAC timers
  mac_timers = mac_timers_;
  state = RRC_STATE_IDLE;
  si_acquire_state = SI_ACQUIRE_IDLE;

  thread_running = true;
  start();

  pthread_mutex_init(&mutex, NULL);

  first_stimsi_attempt = false;

  args.ue_category = SRSLTE_UE_CATEGORY;
  args.supported_bands[0] = 7;
  args.nof_supported_bands = 1;
  args.feature_group = 0xe6041c00;

  t301 = mac_timers->timer_get_unique_id();
  t310 = mac_timers->timer_get_unique_id();
  t311 = mac_timers->timer_get_unique_id();
  t304 = mac_timers->timer_get_unique_id();

  transaction_id = 0;

  // Register logging handler with liblte_rrc
  liblte_rrc_log_register_handler(this, liblte_rrc_handler);

  nof_sib1_trials = 0;
  last_win_start  = 0;

  pending_mob_reconf = false;

  // Set default values for all layers
  set_rrc_default();
  set_phy_default();
  set_mac_default();

  measurements.init(this);
  // set seed for rand (used in attach)
  srand(time(NULL));
}

void rrc::stop() {
  thread_running = false;
  wait_thread_finish();
}

void rrc::run_tti(uint32_t tti) {
  measurements.run_tti(tti);
}

rrc_state_t rrc::get_state() {
  return state;
}

bool rrc::is_connected() {
  return (RRC_STATE_CONNECTED == state);
}

bool rrc::have_drb() {
  return drb_up;
}

void rrc::set_args(rrc_args_t *args) {
  memcpy(&this->args, args, sizeof(rrc_args_t));
}

/*
 *
 * RRC State Machine
 *
 */
void rrc::run_thread() {

  uint32_t failure_test = 0;

  while (thread_running) {

    if (state >= RRC_STATE_IDLE && state < RRC_STATE_CONNECTING) {
      run_si_acquisition_procedure();
    }

    switch(state) {
      /* Procedures in IDLE state 36.304 Sec 4 */
      case RRC_STATE_IDLE:
        // If camping on the cell, it will receive SI and paging from PLMN
        if (phy->sync_status()) {
          // If attempting to attach, reselect cell
          if (nas->is_attaching()) {
            sleep(1);
            rrc_log->info("RRC IDLE: NAS is attaching and camping on cell, reselecting...\n");
            plmn_select_rrc(selected_plmn_id);
          }
        // If not camping on a cell
        } else {
          // If NAS is attached, perform cell reselection on current PLMN
          if (nas->is_attached()) {
            rrc_log->info("RRC IDLE: NAS is attached, PHY not synchronized. Re-selecting cell...\n");
            plmn_select_rrc(selected_plmn_id);
          } else if (nas->is_attaching()) {
            sleep(1);
            rrc_log->info("RRC IDLE: NAS is attaching, searching again PLMN\n");
            plmn_search();
          }
          // If not attached, PLMN selection will be triggered from higher layers
        }
        break;
      case RRC_STATE_PLMN_SELECTION:
        plmn_select_timeout++;
        if (plmn_select_timeout >= RRC_PLMN_SELECT_TIMEOUT) {
          rrc_log->info("RRC PLMN Search: timeout expired\n");
          phy->cell_search_stop();
          sleep(1);
          rrc_log->console("\nRRC PLMN Search: timeout expired. Searching again\n");
          plmn_select_timeout = 0;
          phy->cell_search_start();
        }
        break;
      case RRC_STATE_CELL_SELECTING:

        /* During cell selection, apply SIB configurations if available or receive them if not.
         * Cell is selected when all SIBs downloaded or applied.
         */
        if (phy->sync_status()) {
          if (!serving_cell->has_valid_sib1) {
            si_acquire_state = SI_ACQUIRE_SIB1;
            sysinfo_index = 0;
          } else if (!serving_cell->has_valid_sib2) {
            si_acquire_state = SI_ACQUIRE_SIB2;
          } else {
            apply_sib2_configs(&serving_cell->sib2);
            si_acquire_state = SI_ACQUIRE_IDLE;
            state = RRC_STATE_CELL_SELECTED;
          }
        }
        // Don't time out during restablishment (T311 running)
        if (!mac_timers->timer_get(t311)->is_running()) {
          select_cell_timeout++;
          if (select_cell_timeout >= RRC_SELECT_CELL_TIMEOUT) {
            rrc_log->info("RRC Cell Selecting: timeout expired. Starting Cell Search...\n");
            plmn_select_timeout = 0;
            select_cell_timeout = 0;
            serving_cell->in_sync = false;
            phy->cell_search_start();
          }
        }
        break;
      case RRC_STATE_CELL_SELECTED:

        /* The cell is selected when the SIBs are received and applied.
         * If we were in RRC_CONNECTED and arrive here it means a RLF occurred and we are in Reestablishment procedure.
         * If T311 is running means there is a reestablishment in progress, send ConnectionReestablishmentRequest.
         * If not, do a ConnectionRequest if NAS is established or go to IDLE an camp on cell otherwise.
         */
        if (mac_timers->timer_get(t311)->is_running()) {
          //
          rrc_log->info("RRC Cell Selected: Sending connection reestablishment...\n");
          con_restablish_cell_reselected();
          state = RRC_STATE_CONNECTING;
          connecting_timeout = 0;
        } else if (connection_requested) {
          connection_requested = false;
          rrc_log->info("RRC Cell Selected: Sending connection request...\n");
          send_con_request();
          state = RRC_STATE_CONNECTING;
          connecting_timeout = 0;
        } else {
          rrc_log->info("RRC Cell Selected: Starting paging and going to IDLE...\n");
          mac->pcch_start_rx();
          state = RRC_STATE_LEAVE_CONNECTED;
        }
        break;
      case RRC_STATE_CONNECTING:
        connecting_timeout++;
        if (connecting_timeout >= RRC_CONNECTING_TIMEOUT) {
          // Select another cell
          rrc_log->info("RRC Connecting: timeout expired. Selecting next cell\n");
          state = RRC_STATE_CELL_SELECTING;
        }
        break;
      case RRC_STATE_CONNECTED:
        /*
        failure_test++;
        if (failure_test >= 100) {
          mac_interface_rrc::ue_rnti_t ue_rnti;
          mac->get_rntis(&ue_rnti);
          send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_OTHER_FAILURE, ue_rnti.crnti);
        }*/
        // Take measurements, cell reselection, etc
        break;
      case RRC_STATE_HO_PREPARE:
        if (ho_prepare()) {
          state = RRC_STATE_HO_PROCESS;
        } else {
          state = RRC_STATE_CONNECTED;
        }
        break;
      case RRC_STATE_HO_PROCESS:
        // wait for HO to finish
        break;
      case RRC_STATE_LEAVE_CONNECTED:
        usleep(60000);
        leave_connected();
        // Move to RRC_IDLE
        state = RRC_STATE_IDLE;
        break;
      default:
        break;
    }
    usleep(1000);
  }
}







/*******************************************************************************
*
*
*
* System Information Acquisition procedure
*
*
*
*******************************************************************************/


// Determine SI messages scheduling as in 36.331 5.2.3 Acquisition of an SI message
uint32_t rrc::sib_start_tti(uint32_t tti, uint32_t period, uint32_t offset, uint32_t sf) {
  return (period*10*(1+tti/(period*10))+(offset*10)+sf)%10240; // the 1 means next opportunity
}

void rrc::run_si_acquisition_procedure()
{
  uint32_t tti;
  uint32_t si_win_start=0, si_win_len=0;
  uint16_t period;
  uint32_t x, sf, offset;
  const int SIB1_SEARCH_TIMEOUT = 30;

  switch (si_acquire_state) {
    case SI_ACQUIRE_SIB1:
      // Instruct MAC to look for SIB1
      tti = mac->get_current_tti();
      si_win_start = sib_start_tti(tti, 2, 0, 5);
      if (tti > last_win_start + 10) {
        last_win_start = si_win_start;
        mac->bcch_start_rx(si_win_start, 1);
        rrc_log->debug("Instructed MAC to search for SIB1, win_start=%d, win_len=%d\n",
                       si_win_start, 1);
        nof_sib1_trials++;
        if (nof_sib1_trials >= SIB1_SEARCH_TIMEOUT) {
          if (state == RRC_STATE_CELL_SELECTING) {
            select_next_cell_in_plmn();
            si_acquire_state = SI_ACQUIRE_IDLE;
          } else if (state == RRC_STATE_PLMN_SELECTION) {
            phy->cell_search_next();
          }
          nof_sib1_trials = 0;
        }
      }
      break;
    case SI_ACQUIRE_SIB2:
      // Instruct MAC to look for next SIB
      if(sysinfo_index < serving_cell->sib1.N_sched_info) {
        si_win_len   = liblte_rrc_si_window_length_num[serving_cell->sib1.si_window_length];
        x            = sysinfo_index*si_win_len;
        sf           = x%10;
        offset       = x/10;

        tti          = mac->get_current_tti();
        period       = liblte_rrc_si_periodicity_num[serving_cell->sib1.sched_info[sysinfo_index].si_periodicity];
        si_win_start = sib_start_tti(tti, period, offset, sf);

        if (tti > last_win_start + 10) {
          last_win_start = si_win_start;
          si_win_len = liblte_rrc_si_window_length_num[serving_cell->sib1.si_window_length];

          mac->bcch_start_rx(si_win_start, si_win_len);
          rrc_log->debug("Instructed MAC to search for system info, win_start=%d, win_len=%d\n",
                         si_win_start, si_win_len);
        }

      } else {
        // We've received all SIBs, move on to connection request
        si_acquire_state = SI_ACQUIRE_IDLE;
        state = RRC_STATE_CELL_SELECTED;
      }
      break;
    default:
      break;
  }
}











/*******************************************************************************
*
*
*
* PLMN selection, cell selection/reselection and acquisition of SI procedures
*
*
*
*******************************************************************************/

uint16_t rrc::get_mcc() {
  if (serving_cell->sib1.N_plmn_ids > 0) {
    return serving_cell->sib1.plmn_id[0].id.mcc;
  }
  return 0;
}

uint16_t rrc::get_mnc() {
  if (serving_cell->sib1.N_plmn_ids > 0) {
    return serving_cell->sib1.plmn_id[0].id.mnc;
  }
  return 0;
}

void rrc::plmn_search() {
  rrc_log->info("Starting PLMN search procedure\n");
  state = RRC_STATE_PLMN_SELECTION;
  phy->cell_search_start();
  plmn_select_timeout = 0;
}

/* This is the NAS interface. When NAS requests to select a PLMN we have to
 * connect to either register or because there is pending higher layer traffic.
 */
void rrc::plmn_select(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {
  connection_requested = true;
  plmn_select_rrc(plmn_id);
}

/* This is called by RRC only. In this case, we do not want to connect, just camp on the
 * selected PLMN
 */
void rrc::plmn_select_rrc(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {
  // If already camping on the selected PLMN, select this cell
  if (state == RRC_STATE_IDLE || state == RRC_STATE_CONNECTED || state == RRC_STATE_PLMN_SELECTION) {
    if (phy->sync_status() && selected_plmn_id.mcc == plmn_id.mcc && selected_plmn_id.mnc == plmn_id.mnc) {
      rrc_log->info("Already camping on selected PLMN, connecting...\n");
      state = RRC_STATE_CELL_SELECTING;
      select_cell_timeout = 0;
    } else {
      rrc_log->info("PLMN Id=%s selected\n", plmn_id_to_string(plmn_id).c_str());
      // Sort cells according to RSRP

      selected_plmn_id = plmn_id;
      select_cell_timeout = 0;

      state = RRC_STATE_CELL_SELECTING;
    }
  } else {
    rrc_log->warning("Requested PLMN select in incorrect state %s\n", rrc_state_text[state]);
  }
}

void rrc::set_serving_cell(uint32_t earfcn, uint32_t pci) {
  int cell_idx = find_neighbour_cell(earfcn, pci);
  if (cell_idx >= 0) {
    set_serving_cell(cell_idx);
  } else {
    rrc_log->error("Setting serving cell: Unkonwn cell with earfcn=%d, PCI=%d\n", earfcn, pci);
  }
}

void rrc::set_serving_cell(uint32_t cell_idx) {
  if (cell_idx < neighbour_cells.size())
  {
    // Remove future serving cell from neighbours to make space for current serving cell
    cell_t *new_serving_cell = neighbour_cells[cell_idx];
    if (!new_serving_cell) {
      rrc_log->error("Setting serving cell. Index %d is empty\n", cell_idx);
      return;
    }
    neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[cell_idx]), neighbour_cells.end());

    // Move serving cell to neighbours list
    if (serving_cell->is_valid()) {
      // Make sure it does not exist already
      int serving_idx = find_neighbour_cell(serving_cell->earfcn, serving_cell->phy_cell.id);
      if (serving_idx >= 0 && (uint32_t) serving_idx < neighbour_cells.size()) {
        printf("Error serving cell is already in the neighbour list. Removing it\n");
        neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[serving_idx]), neighbour_cells.end());
      }
      // If not in the list, add it to the list of neighbours (sorted inside the function)
      if (!add_neighbour_cell(serving_cell)) {
        rrc_log->info("Serving cell not added to list of neighbours. Worse than current neighbours\n");
      }
    }

    // Set new serving cell
    serving_cell = new_serving_cell;

    rrc_log->info("Setting serving cell idx=%d, earfcn=%d, PCI=%d, nof_neighbours=%d\n",
                  cell_idx, serving_cell->earfcn, serving_cell->phy_cell.id, neighbour_cells.size());

  } else {
    rrc_log->error("Setting invalid serving cell idx %d\n", cell_idx);
  }
}

void rrc::select_next_cell_in_plmn() {
  // Neighbour cells are sorted in descending order of RSRP
  for (uint32_t i = 0; i < neighbour_cells.size(); i++) {
    if (neighbour_cells[i]->plmn_equals(selected_plmn_id) &&
        neighbour_cells[i]->in_sync) // matches S criteria
    {
      // Try to select Cell
      if (phy->cell_select(neighbour_cells[i]->earfcn, neighbour_cells[i]->phy_cell)) {
        set_serving_cell(i);
        rrc_log->info("Selected cell PCI=%d, EARFCN=%d, Cell ID=0x%x\n",
                      serving_cell->phy_cell.id, serving_cell->earfcn,
                      serving_cell->sib1.cell_id);
        rrc_log->console("Selected cell PCI=%d, EARFCN=%d, Cell ID=0x%x\n",
                         serving_cell->phy_cell.id, serving_cell->earfcn,
                         serving_cell->sib1.cell_id);
      } else {
        // Set to out-of-sync if can't synchronize
        neighbour_cells[i]->in_sync = false;
        rrc_log->warning("Selecting cell EARFCN=%d, Cell ID=0x%x.\n",
                         neighbour_cells[i]->earfcn, neighbour_cells[i]->sib1.cell_id);
      }
      return;
    }
  }
  rrc_log->info("No more known cells. Starting again\n");
}

void rrc::new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn_i, int pci_i) {

  if (earfcn_i < 0 || pci_i < 0) {
    earfcn_i = serving_cell->earfcn;
    pci_i    = serving_cell->phy_cell.id;
  }

  uint32_t earfcn = (uint32_t) earfcn_i;
  uint32_t pci    = (uint32_t) pci_i;

  // Measurements in RRC_CONNECTED go through measuremnt class to log reports etc.
  if (state != RRC_STATE_IDLE) {
    measurements.new_phy_meas(earfcn, pci, rsrp, rsrq, tti);

  // Measurements in RRC_IDLE update serving cell and check for reselection
  } else {

    // Update serving cell 
    if (serving_cell->equals(earfcn, pci)) {
      cell_reselection_eval(rsrp, rsrq);
      serving_cell->rsrp = rsrp;
      rrc_log->info("MEAS:  New measurement serving cell in IDLE, rsrp=%f, rsrq=%f, tti=%d\n", rsrp, rsrq, tti);
      
    // Or update/add neighbour cell
    } else {
      if (add_neighbour_cell(earfcn, pci, rsrp)) {
        rrc_log->info("MEAS:  New measurement neighbour in IDLE, PCI=%d, RSRP=%.1f dBm.\n", pci, rsrp);
      } else {
        rrc_log->info("MEAS:  Neighbour Cell in IDLE PCI=%d, RSRP=%.1f dBm not added. Worse than current neighbours\n", pci, rsrp);
      }
    }

    // Verify cell selection criteria with strongest neighbour cell (always first)
    if (cell_selection_eval(neighbour_cells[0]->rsrp)     &&
        neighbour_cells[0]->rsrp > serving_cell->rsrp + 5)
    {
      set_serving_cell(0);
      rrc_log->info("Selecting best neighbour cell PCI=%d, rsrp=%.1f dBm\n", serving_cell->phy_cell.id, serving_cell->rsrp);
      state = RRC_STATE_CELL_SELECTING;
      phy->cell_select(serving_cell->earfcn, serving_cell->phy_cell);
    }
  }
}

void rrc::cell_found(uint32_t earfcn, srslte_cell_t phy_cell, float rsrp) {

  bool found = false;
  int cell_idx = -1; 
  
  if (serving_cell->equals(earfcn, phy_cell.id)) {
    serving_cell->rsrp = rsrp;
    serving_cell->in_sync = true;
    found = true;
  } else {
    // Check if cell is in our list of neighbour cells 
    cell_idx = find_neighbour_cell(earfcn, phy_cell.id);
    if (cell_idx >= 0) {
      set_serving_cell(cell_idx);
      serving_cell->rsrp = rsrp;
      serving_cell->in_sync = true;
      found = true;
    }
  }
  if (found) {

    if (!serving_cell->has_valid_sib1) {
      si_acquire_state = SI_ACQUIRE_SIB1;
    } else if (state == RRC_STATE_PLMN_SELECTION) {
      for (uint32_t j = 0; j < serving_cell->sib1.N_plmn_ids; j++) {
        nas->plmn_found(serving_cell->sib1.plmn_id[j].id, serving_cell->sib1.tracking_area_code);
      }
      usleep(5000);
      phy->cell_search_next();
    }
  } else {
    // add to list of known cells and set current_cell
    if (!add_neighbour_cell(earfcn, phy_cell, rsrp)) {
      rrc_log->info("No more space for neighbour cells (detected cell RSRP=%.1f dBm worse than current %d neighbours)\n", 
                    rsrp, NOF_NEIGHBOUR_CELLS);
      usleep(5000);
      phy->cell_search_next();
    } else {
      set_serving_cell(earfcn, phy_cell.id);

      si_acquire_state = SI_ACQUIRE_SIB1;
    }
  }

  rrc_log->info("%s %s cell EARFCN=%d, PCI=%d, RSRP=%.1f dBm\n",
                found?"Updating":"Adding",
                cell_idx>=0?"neighbour":"serving",
                serving_cell->earfcn,
                serving_cell->phy_cell.id,
                serving_cell->rsrp);
}

bool sort_rsrp(cell_t *u1, cell_t *u2) {
  return !u1->greater(u2);
}

// Sort neighbour cells by decreasing order of RSRP
void rrc::sort_neighbour_cells() {

  for (uint32_t i=1;i<neighbour_cells.size();i++) {
    if (neighbour_cells[i]->in_sync == false) {
      rrc_log->info("Removing neighbour cell PCI=%d, out_of_sync\n", neighbour_cells[i]->phy_cell.id);
      neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[i]), neighbour_cells.end());
    }
  }

  std::sort(neighbour_cells.begin(), neighbour_cells.end(), sort_rsrp);

  char ordered[512];
  int n=0;
  n += snprintf(ordered, 512, "[pci=%d, rsrsp=%.2f", neighbour_cells[0]->phy_cell.id, neighbour_cells[0]->rsrp);
  for (uint32_t i=1;i<neighbour_cells.size();i++) {
    n += snprintf(&ordered[n], 512-n, " | pci=%d, rsrp=%.2f", neighbour_cells[i]->phy_cell.id, neighbour_cells[i]->rsrp);
  }
  rrc_log->info("Sorted neighbour cells: %s]\n", ordered);
}

bool rrc::add_neighbour_cell(cell_t *new_cell) {
  bool ret = false;
  if (neighbour_cells.size() < NOF_NEIGHBOUR_CELLS - 1) {
    ret = true;
  } else if (!neighbour_cells[neighbour_cells.size()-1]->greater(new_cell)) {
    // Delete old one
    delete neighbour_cells[neighbour_cells.size()-1];
    neighbour_cells.erase(std::remove(neighbour_cells.begin(), neighbour_cells.end(), neighbour_cells[neighbour_cells.size()-1]), neighbour_cells.end());
    ret = true;
  }
  if (ret) {
    neighbour_cells.push_back(new_cell);
  }
  rrc_log->info("Added neighbour cell EARFCN=%d, PCI=%d, nof_neighbours=%d\n",
                new_cell->earfcn, new_cell->phy_cell.id, neighbour_cells.size());
  sort_neighbour_cells();
  return ret;
}

// If only neighbour PCI is provided, copy full cell from serving cell
bool rrc::add_neighbour_cell(uint32_t earfcn, uint32_t pci, float rsrp) {
  srslte_cell_t serving_phy;
  serving_phy = serving_cell->phy_cell;
  serving_phy.id = pci;
  return add_neighbour_cell(earfcn, serving_phy, rsrp);
}

bool rrc::add_neighbour_cell(uint32_t earfcn, srslte_cell_t phy_cell, float rsrp) {
  if (earfcn == 0) {
    earfcn = serving_cell->earfcn;
  }

  // First check if already exists
  int cell_idx = find_neighbour_cell(earfcn, phy_cell.id);

  rrc_log->info("Adding PCI=%d, earfcn=%d, cell_idx=%d\n", phy_cell.id, earfcn, cell_idx);

  // If exists, update RSRP, sort again and return
  if (cell_idx >= 0) {
    neighbour_cells[cell_idx]->rsrp = rsrp;
    sort_neighbour_cells();
    return true; 
  }

  // If not, create a new one
  cell_t *new_cell = new cell_t(phy_cell, earfcn, rsrp);

  return add_neighbour_cell(new_cell);
}

int rrc::find_neighbour_cell(uint32_t earfcn, uint32_t pci) {
  for (uint32_t i = 0; i < neighbour_cells.size(); i++) {
    if (neighbour_cells[i]->equals(earfcn, pci)) {
      return (int) i;
    }
  }
  return -1;
}

// PHY indicates that has gone through all known EARFCN
void rrc::earfcn_end() {
  rrc_log->info("Finished searching cells in EARFCN set while in state %s\n", rrc_state_text[state]);

  // If searching for PLMN, indicate NAS we scanned all frequencies
  if (state == RRC_STATE_PLMN_SELECTION) {
    nas->plmn_search_end();
  } else if (state == RRC_STATE_CELL_SELECTING) {
    select_cell_timeout = 0;
    rrc_log->info("Starting cell search again\n");
    phy->cell_search_start();
  }
}

// Cell reselection in IDLE Section 5.2.4 of 36.304
void rrc::cell_reselection_eval(float rsrp, float rsrq)
{
  // Intra-frequency cell-reselection criteria

  if (get_srxlev(rsrp) > cell_resel_cfg.s_intrasearchP && rsrp > -95.0) {
    // UE may not perform intra-frequency measurements.
    phy->meas_reset();
    // keep measuring serving cell
    phy->meas_start(phy->get_current_earfcn(), phy->get_current_pci());
  } else {
    // UE must start intra-frequency measurements
    phy->meas_start(phy->get_current_earfcn(), -1);
  }

  // TODO: Inter-frequency cell reselection
}

// Cell selection in IDLE Section 5.2.3.2 of 36.304
bool rrc::cell_selection_eval(float rsrp, float rsrq)
{
  if (get_srxlev(rsrp) > 0) {
    return true;
  } else {
    return false;
  }
}

float rrc::get_srxlev(float Qrxlevmeas) {
  // TODO: Do max power limitation
  float Pcompensation = 0;
  return Qrxlevmeas - (cell_resel_cfg.Qrxlevmin + cell_resel_cfg.Qrxlevminoffset) - Pcompensation;
}

float rrc::get_squal(float Qqualmeas) {
  return Qqualmeas - (cell_resel_cfg.Qqualmin + cell_resel_cfg.Qqualminoffset);
}



/*******************************************************************************
*
*
*
* Detection of Radio-Link Failures
*
*
*
*******************************************************************************/

// Detection of physical layer problems in RRC_CONNECTED (5.3.11.1)
void rrc::out_of_sync() {
  serving_cell->in_sync = false;
  if (state == RRC_STATE_CONNECTED) {
    if (!mac_timers->timer_get(t311)->is_running() && !mac_timers->timer_get(t310)->is_running()) {
      n310_cnt++;
      if (n310_cnt == N310) {
        mac_timers->timer_get(t310)->reset();
        mac_timers->timer_get(t310)->run();
        n310_cnt = 0;
        phy->sync_reset();
        rrc_log->info("Detected %d out-of-sync from PHY. Trying to resync. Starting T310 timer\n", N310);
      }
    }
  } else {
    phy->sync_reset();
  }
}

// Recovery of physical layer problems (5.3.11.2)
void rrc::in_sync() {
  serving_cell->in_sync = true;
  if (mac_timers->timer_get(t310)->is_running()) {
    n311_cnt++;
    if (n311_cnt == N311) {
      mac_timers->timer_get(t310)->stop();
      n311_cnt = 0;
      rrc_log->info("Detected %d in-sync from PHY. Stopping T310 timer\n", N311);
    }
  }
}

/* Detection of radio link failure (5.3.11.3)
 * Upon T310 expiry, RA problem or RLC max retx
 */
void rrc::radio_link_failure() {
  // TODO: Generate and store failure report

  phy->sync_reset();
  rrc_log->warning("Detected Radio-Link Failure\n");
  rrc_log->console("Warning: Detected Radio-Link Failure\n");
  if (state != RRC_STATE_CONNECTED) {
    state = RRC_STATE_LEAVE_CONNECTED;
  } else {
    mac_interface_rrc::ue_rnti_t uernti;
    mac->get_rntis(&uernti);
    send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_OTHER_FAILURE, uernti.crnti);
  }
}

/* Reception of PUCCH/SRS release procedure (Section 5.3.13) */
void rrc::release_pucch_srs() {
  // Apply default configuration for PUCCH (CQI and SR) and SRS (release)
  set_phy_default_pucch_srs();

  // Configure RX signals without pregeneration because default option is release
  phy->configure_ul_params(true);
}

void rrc::ra_problem() {
  radio_link_failure();
}

void rrc::max_retx_attempted() {
  //TODO: Handle the radio link failure
  rrc_log->warning("Max RLC reTx attempted\n");
  radio_link_failure();
}

void rrc::timer_expired(uint32_t timeout_id) {
  if (timeout_id == t310) {
    rrc_log->info("Timer T310 expired: Radio Link Failure\n");
    radio_link_failure();
  } else if (timeout_id == t311) {
    rrc_log->info("Timer T311 expired: Going to RRC IDLE\n");
    state = RRC_STATE_LEAVE_CONNECTED;
  } else if (timeout_id == t301) {
    if (state == RRC_STATE_IDLE) {
      rrc_log->info("Timer T301 expired: Already in IDLE.\n");
    } else {
      rrc_log->info("Timer T301 expired: Going to RRC IDLE\n");
      state = RRC_STATE_LEAVE_CONNECTED;
    }
  } else if (timeout_id == t304) {
    rrc_log->console("Timer T304 expired: Handover failed\n");
    ho_failed();
  // fw to measurement
  } else if (!measurements.timer_expired(timeout_id)) {
    rrc_log->error("Timeout from unknown timer id %d\n", timeout_id);
  }
}








/*******************************************************************************
*
*
*
* Connection Control: Establishment, Reconfiguration, Reestablishment and Release
*
*
*
*******************************************************************************/

void rrc::send_con_request() {
  rrc_log->debug("Preparing RRC Connection Request\n");
  LIBLTE_RRC_S_TMSI_STRUCT s_tmsi;

  // Prepare ConnectionRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ;

  if (nas->get_s_tmsi(&s_tmsi)) {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI;
    ul_ccch_msg.msg.rrc_con_req.ue_id.s_tmsi = s_tmsi;
  } else {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE;
    // TODO use proper RNG
    uint64_t random_id = 0;
    for (uint i = 0; i < 5; i++) { // fill random ID bytewise, 40 bits = 5 bytes
      random_id |= ( (uint64_t)rand() & 0xFF ) << i*8;
    }
    ul_ccch_msg.msg.rrc_con_req.ue_id.random = random_id;
  }

  ul_ccch_msg.msg.rrc_con_req.cause = LIBLTE_RRC_CON_REQ_EST_CAUSE_MO_SIGNALLING;

  send_ul_ccch_msg();

}

/* RRC connection re-establishment procedure (5.3.7) */
void rrc::send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM cause, uint16_t crnti)
{
  // Compute shortMAC-I
  uint8_t varShortMAC[128], varShortMAC_packed[16];
  bzero(varShortMAC, 128);
  bzero(varShortMAC_packed, 16);
  uint8_t *msg_ptr = varShortMAC;

  // ASN.1 encode byte-aligned VarShortMAC-Input
  liblte_rrc_pack_cell_identity_ie(serving_cell->sib1.cell_id, &msg_ptr);
  msg_ptr = &varShortMAC[4];
  liblte_rrc_pack_phys_cell_id_ie(phy->get_current_pci(), &msg_ptr);
  msg_ptr = &varShortMAC[4+2];
  liblte_rrc_pack_c_rnti_ie(crnti, &msg_ptr);
  srslte_bit_pack_vector(varShortMAC, varShortMAC_packed, (4+2+4)*8);

  rrc_log->info("Generated varShortMAC: cellId=0x%x, PCI=%d, rnti=%d\n",
                serving_cell->sib1.cell_id, phy->get_current_pci(), crnti);

  // Compute MAC-I
  uint8_t mac_key[4];
  switch(integ_algo) {
    case INTEGRITY_ALGORITHM_ID_128_EIA1:
      security_128_eia1(&k_rrc_int[16],
                        1,
                        1,
                        1,
                        varShortMAC_packed,
                        10,
                        mac_key);
      break;
    case INTEGRITY_ALGORITHM_ID_128_EIA2:
      security_128_eia2(&k_rrc_int[16],
                        1,
                        1,
                        1,
                        varShortMAC_packed,
                        10,
                        mac_key);
      break;
    default:
      rrc_log->info("Unsupported integrity algorithm during reestablishment\n");
      return;
  }

  // Prepare ConnectionRestalishmentRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.c_rnti = crnti;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.phys_cell_id = phy->get_current_pci();
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.short_mac_i = mac_key[1] << 8 | mac_key[0];
  ul_ccch_msg.msg.rrc_con_reest_req.cause = cause;

  rrc_log->info("Initiating RRC Connection Reestablishment Procedure\n");
  rrc_log->console("RRC Connection Reestablishment\n");
  mac_timers->timer_get(t310)->stop();
  mac_timers->timer_get(t311)->reset();
  mac_timers->timer_get(t311)->run();

  phy->reset();
  set_phy_default();
  mac->reset();
  set_mac_default();
  state = RRC_STATE_CELL_SELECTING;
}

// Actions following cell reselection 5.3.7.3
void rrc::con_restablish_cell_reselected()
{
  liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  rrc_log->info("Cell Selection finished. Initiating transmission of RRC Connection Reestablishment Request\n");
  mac_timers->timer_get(t301)->reset();
  mac_timers->timer_get(t301)->run();
  mac_timers->timer_get(t311)->stop();

  send_ul_ccch_msg();

}

void rrc::send_con_restablish_complete() {
  rrc_log->debug("Preparing RRC Connection Reestablishment Complete\n");

  rrc_log->console("RRC Connected\n");
  state = RRC_STATE_CONNECTED;

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reest_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg();
}

void rrc::send_con_setup_complete(byte_buffer_t *nas_msg) {
  rrc_log->debug("Preparing RRC Connection Setup Complete\n");

  state = RRC_STATE_CONNECTED;
  rrc_log->console("RRC Connected\n");

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE;
  ul_dcch_msg.msg.rrc_con_setup_complete.registered_mme_present = false;
  ul_dcch_msg.msg.rrc_con_setup_complete.rrc_transaction_id = transaction_id;
  ul_dcch_msg.msg.rrc_con_setup_complete.selected_plmn_id = 1;
  memcpy(ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.msg, nas_msg->msg, nas_msg->N_bytes);
  ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.N_bytes = nas_msg->N_bytes;

  pool->deallocate(nas_msg);
  
  send_ul_dcch_msg();
}

void rrc::send_ul_info_transfer(uint32_t lcid, byte_buffer_t *sdu) {
  rrc_log->debug("Preparing RX Info Transfer\n");

  // Prepare RX INFO packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER;
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info_type = LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS;
  memcpy(ul_dcch_msg.msg.ul_info_transfer.dedicated_info.msg, sdu->msg, sdu->N_bytes);
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info.N_bytes = sdu->N_bytes;

  send_ul_dcch_msg(sdu);
}

void rrc::send_security_mode_complete(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->debug("Preparing Security Mode Complete\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE;
  ul_dcch_msg.msg.security_mode_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg(pdu);
}

void rrc::send_rrc_con_reconfig_complete(byte_buffer_t *pdu) {
  rrc_log->debug("Preparing RRC Connection Reconfig Complete\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reconfig_complete.rrc_transaction_id = transaction_id;

  send_ul_dcch_msg(pdu);
}

bool rrc::ho_prepare() {
  if (pending_mob_reconf) {
    rrc_log->info("Processing HO command to target PCell=%d\n", mob_reconf.mob_ctrl_info.target_pci);

    int target_cell_idx = find_neighbour_cell(serving_cell->earfcn, mob_reconf.mob_ctrl_info.target_pci);
    if (target_cell_idx < 0) {
      rrc_log->error("Could not find target cell earfcn=%d, pci=%d\n", serving_cell->earfcn, mob_reconf.mob_ctrl_info.target_pci);
      return false;
    }

    // Section 5.3.5.4
    mac_timers->timer_get(t310)->stop();
    mac_timers->timer_get(t304)->set(this, liblte_rrc_t304_num[mob_reconf.mob_ctrl_info.t304]);
    if (mob_reconf.mob_ctrl_info.carrier_freq_eutra_present &&
        mob_reconf.mob_ctrl_info.carrier_freq_eutra.dl_carrier_freq != serving_cell->earfcn) {
      rrc_log->warning("Received mobilityControlInfo for inter-frequency handover\n");
    }

    // Save serving cell and current configuration
    ho_src_cell = *serving_cell;
    phy->get_config(&ho_src_phy_cfg);
    mac->get_config(&ho_src_mac_cfg);
    mac_interface_rrc::ue_rnti_t uernti;
    mac->get_rntis(&uernti);
    ho_src_rnti = uernti.crnti;

    // Reset/Reestablish stack
    phy->meas_reset();
    mac->wait_uplink();
    pdcp->reestablish();
    rlc->reestablish();
    mac->reset();
    phy->reset();
    mac->set_ho_rnti(mob_reconf.mob_ctrl_info.new_ue_id, mob_reconf.mob_ctrl_info.target_pci);
    apply_rr_config_common_dl(&mob_reconf.mob_ctrl_info.rr_cnfg_common);

    rrc_log->info("Selecting new cell pci=%d\n", neighbour_cells[target_cell_idx]->phy_cell.id);
    if (!phy->cell_handover(neighbour_cells[target_cell_idx]->phy_cell)) {
      rrc_log->error("Could not synchronize with target cell pci=%d\n", neighbour_cells[target_cell_idx]->phy_cell.id);
      return false;
    }

    if (mob_reconf.mob_ctrl_info.rach_cnfg_ded_present) {
      rrc_log->info("Starting non-contention based RA with preamble_idx=%d, mask_idx=%d\n",
                    mob_reconf.mob_ctrl_info.rach_cnfg_ded.preamble_index,
                    mob_reconf.mob_ctrl_info.rach_cnfg_ded.prach_mask_index);
      mac->start_noncont_ho(mob_reconf.mob_ctrl_info.rach_cnfg_ded.preamble_index,
                            mob_reconf.mob_ctrl_info.rach_cnfg_ded.prach_mask_index);
    } else {
      rrc_log->info("Starting contention-based RA\n");
      mac->start_cont_ho();
    }

    int ncc = -1;
    if (mob_reconf.sec_cnfg_ho_present) {
      ncc = mob_reconf.sec_cnfg_ho.intra_lte.next_hop_chaining_count;
      if (mob_reconf.sec_cnfg_ho.intra_lte.key_change_ind) {
        rrc_log->console("keyChangeIndicator in securityConfigHO not supported\n");
        return false;
      }
      if (mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg_present) {
        cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg.cipher_alg;
        integ_algo  = (INTEGRITY_ALGORITHM_ID_ENUM) mob_reconf.sec_cnfg_ho.intra_lte.sec_alg_cnfg.int_alg;
        rrc_log->info("Changed Ciphering to %s and Integrity to %s\n",
                         ciphering_algorithm_id_text[cipher_algo],
                         integrity_algorithm_id_text[integ_algo]);
      }
    }

    usim->generate_as_keys_ho(mob_reconf.mob_ctrl_info.target_pci, phy->get_current_earfcn(),
                              ncc,
                              k_rrc_enc, k_rrc_int, k_up_enc, k_up_int, cipher_algo, integ_algo);

    pdcp->config_security_all(k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
    send_rrc_con_reconfig_complete(NULL);
  }
  return true;
}

void rrc::ho_ra_completed(bool ra_successful) {
  if (pending_mob_reconf) {

    measurements.ho_finish();

    if (mob_reconf.meas_cnfg_present) {
      measurements.parse_meas_config(&mob_reconf.meas_cnfg);
    }

    if (ra_successful) {
      mac_timers->timer_get(t304)->stop();

      apply_rr_config_common_ul(&mob_reconf.mob_ctrl_info.rr_cnfg_common);
      if (mob_reconf.rr_cnfg_ded_present) {
        apply_rr_config_dedicated(&mob_reconf.rr_cnfg_ded);
      }
    }

    rrc_log->info("HO %ssuccessful\n", ra_successful?"":"un");
    rrc_log->console("HO %ssuccessful\n", ra_successful?"":"un");

    pending_mob_reconf = false;
    if (ra_successful) {
      state = RRC_STATE_CONNECTED;
    }
  } else {
    rrc_log->error("Received HO random access completed but no pending mobility reconfiguration info\n");
  }
}

// This is T304 expiry 5.3.5.6
void rrc::ho_failed() {

  // Instruct PHY to resync with source PCI
  if (!phy->cell_handover(ho_src_cell.phy_cell)) {
    rrc_log->error("Could not synchronize with target cell pci=%d\n", ho_src_cell.phy_cell.id);
    return;
  }

  // Set previous PHY/MAC configuration
  phy->set_config(&ho_src_phy_cfg);
  mac->set_config(&ho_src_mac_cfg);

  // Start the Reestablishment Procedure
  send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_HANDOVER_FAILURE, ho_src_rnti);
}

void rrc::handle_rrc_con_reconfig(uint32_t lcid, LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig,
                                  byte_buffer_t *pdu) {
  uint32_t i;

  if (reconfig->mob_ctrl_info_present) {

    if (reconfig->mob_ctrl_info.target_pci == phy->get_current_pci()) {
      rrc_log->warning("Received HO command to own cell\n");
      send_rrc_con_reconfig_complete(pdu);
    } else {
      rrc_log->info("Received HO command to target PCell=%d\n", reconfig->mob_ctrl_info.target_pci);
      rrc_log->console("Received HO command to target PCell=%d, NCC=%d\n",
                       reconfig->mob_ctrl_info.target_pci, reconfig->sec_cnfg_ho.intra_lte.next_hop_chaining_count);

      // store mobilityControlInfo
      memcpy(&mob_reconf, reconfig, sizeof(LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT));
      pending_mob_reconf = true;

      state = RRC_STATE_HO_PREPARE;
    }

  } else {
    // Section 5.3.5.3
    if (reconfig->rr_cnfg_ded_present) {
      apply_rr_config_dedicated(&reconfig->rr_cnfg_ded);
    }
    if (reconfig->meas_cnfg_present) {
      measurements.parse_meas_config(&reconfig->meas_cnfg);
    }

    send_rrc_con_reconfig_complete(pdu);

    byte_buffer_t *nas_sdu;
    for (i = 0; i < reconfig->N_ded_info_nas; i++) {
      nas_sdu = pool_allocate;
      memcpy(nas_sdu->msg, &reconfig->ded_info_nas_list[i].msg, reconfig->ded_info_nas_list[i].N_bytes);
      nas_sdu->N_bytes = reconfig->ded_info_nas_list[i].N_bytes;
      nas->write_pdu(lcid, nas_sdu);
    }
  }
}

/* Actions upon reception of RRCConnectionRelease 5.3.8.3 */
void rrc::rrc_connection_release() {
  // Save idleModeMobilityControlInfo, etc.
  state = RRC_STATE_LEAVE_CONNECTED;
  rrc_log->console("Received RRC Connection Release\n");
}

/* Actions upon leaving RRC_CONNECTED 5.3.12 */
void rrc::leave_connected()
{
  rrc_log->console("RRC IDLE\n");
  rrc_log->info("Leaving RRC_CONNECTED state\n");
  drb_up = false;
  measurements.reset();
  pdcp->reset();
  rlc->reset();
  phy->reset();
  mac->reset();
  set_phy_default();
  set_mac_default();
  mac_timers->timer_get(t301)->stop();
  mac_timers->timer_get(t310)->stop();
  mac_timers->timer_get(t311)->stop();
  mac_timers->timer_get(t304)->stop();
  if (phy->sync_status()) {
    // Instruct MAC to look for P-RNTI
    mac->pcch_start_rx();
    // Instruct PHY to measure serving cell for cell reselection
    phy->meas_start(phy->get_current_earfcn(), phy->get_current_pci());
  }
}






/*******************************************************************************
*
*
*
* Reception of Broadcast messages (MIB and SIBs)
*
*
*
*******************************************************************************/
void rrc::write_pdu_bcch_bch(byte_buffer_t *pdu) {
  pool->deallocate(pdu);
  if (state == RRC_STATE_PLMN_SELECTION) {
    // Do we need to do something with BCH?
    rrc_log->info_hex(pdu->msg, pdu->N_bytes, "BCCH BCH message received.");
  } else {
    rrc_log->warning("Received BCCH BCH in incorrect state\n");
  }
}

void rrc::write_pdu_bcch_dlsch(byte_buffer_t *pdu) {
  mac->bcch_stop_rx();

  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "BCCH DLSCH message received.");
  rrc_log->info("BCCH DLSCH message Stack latency: %ld us\n", pdu->get_latency_us());
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT dlsch_msg;
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  pool->deallocate(pdu);
  liblte_rrc_unpack_bcch_dlsch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dlsch_msg);

  for(uint32_t i=0; i<dlsch_msg.N_sibs; i++) {
    rrc_log->info("Processing SIB: %d\n", liblte_rrc_sys_info_block_type_num[dlsch_msg.sibs[i].sib_type]);

    if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1 == dlsch_msg.sibs[i].sib_type && SI_ACQUIRE_SIB1 == si_acquire_state) {
      memcpy(&serving_cell->sib1, &dlsch_msg.sibs[i].sib.sib1, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT));
      serving_cell->has_valid_sib1 = true;
      handle_sib1();
    } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_valid_sib2) {
      memcpy(&serving_cell->sib2, &dlsch_msg.sibs[i].sib.sib2, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT));
      serving_cell->has_valid_sib2 = true;
      handle_sib2();
    } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_valid_sib3) {
      memcpy(&serving_cell->sib3, &dlsch_msg.sibs[i].sib.sib3, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT));
      serving_cell->has_valid_sib3 = true;
      handle_sib3();
    }else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13 == dlsch_msg.sibs[i].sib_type && !serving_cell->has_valid_sib13) {
      memcpy(&serving_cell->sib13, &dlsch_msg.sibs[0].sib.sib13, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT));
      serving_cell->has_valid_sib13 = true;
      handle_sib13();
    }
  }
  if(serving_cell->has_valid_sib2) {
    sysinfo_index++;
  }
}

void rrc::handle_sib1()
{
  rrc_log->info("SIB1 received, CellID=%d, si_window=%d, sib2_period=%d\n",
                serving_cell->sib1.cell_id&0xfff,
                liblte_rrc_si_window_length_num[serving_cell->sib1.si_window_length],
                liblte_rrc_si_periodicity_num[serving_cell->sib1.sched_info[0].si_periodicity]);

  // Print SIB scheduling info
  uint32_t i,j;
  for(i=0;i<serving_cell->sib1.N_sched_info;i++){
    for(j=0;j<serving_cell->sib1.sched_info[i].N_sib_mapping_info;j++){
      LIBLTE_RRC_SIB_TYPE_ENUM t       = serving_cell->sib1.sched_info[i].sib_mapping_info[j].sib_type;
      LIBLTE_RRC_SI_PERIODICITY_ENUM p = serving_cell->sib1.sched_info[i].si_periodicity;
      rrc_log->debug("SIB scheduling info, sib_type=%d, si_periodicity=%d\n",
                    liblte_rrc_sib_type_num[t],
                    liblte_rrc_si_periodicity_num[p]);
    }
  }

  // Set TDD Config
  if(serving_cell->sib1.tdd) {
    phy->set_config_tdd(&serving_cell->sib1.tdd_cnfg);
  }

  serving_cell->has_valid_sib1 = true;

  // Send PLMN and TAC to NAS
  std::stringstream ss;
  for (uint32_t i = 0; i < serving_cell->sib1.N_plmn_ids; i++) {
    nas->plmn_found(serving_cell->sib1.plmn_id[i].id, serving_cell->sib1.tracking_area_code);
  }

  // Jump to next state
  switch(state) {
    case RRC_STATE_CELL_SELECTING:
      si_acquire_state = SI_ACQUIRE_SIB2;
      break;
    case RRC_STATE_PLMN_SELECTION:
      si_acquire_state = SI_ACQUIRE_IDLE;
      rrc_log->info("SI Acquisition done. Searching next cell...\n");
      usleep(5000);
      phy->cell_search_next();
      break;
    default:
      si_acquire_state = SI_ACQUIRE_IDLE;
  }
}

void rrc::handle_sib2()
{
  rrc_log->info("SIB2 received\n");

  apply_sib2_configs(&serving_cell->sib2);

}

void rrc::handle_sib3()
{
  rrc_log->info("SIB3 received\n");

  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *sib3 = &serving_cell->sib3;

  // cellReselectionInfoCommon
  cell_resel_cfg.q_hyst = liblte_rrc_q_hyst_num[sib3->q_hyst];

  // cellReselectionServingFreqInfo
  cell_resel_cfg.threshservinglow = sib3->thresh_serving_low;

  // intraFreqCellReselectionInfo
  cell_resel_cfg.Qrxlevmin       = sib3->q_rx_lev_min;
  if (sib3->s_intra_search_present) {
    cell_resel_cfg.s_intrasearchP  = sib3->s_intra_search;
  } else {
    cell_resel_cfg.s_intrasearchP  = INFINITY;
  }

}

void rrc::handle_sib13()
{
  rrc_log->info("SIB13 received\n");

//  mac->set_config_mbsfn_sib13(&serving_cell->sib13.mbsfn_area_info_list_r9[0],
//                              serving_cell->sib13.mbsfn_area_info_list_r9_size,
//                              &serving_cell->sib13.mbsfn_notification_config);
}




/*******************************************************************************
*
*
*
* Reception of Paging messages
*
*
*
*******************************************************************************/
void rrc::write_pdu_pcch(byte_buffer_t *pdu) {
  if (pdu->N_bytes > 0 && pdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BITS) {
    rrc_log->info_hex(pdu->msg, pdu->N_bytes, "PCCH message received %d bytes\n", pdu->N_bytes);
    rrc_log->info("PCCH message Stack latency: %ld us\n", pdu->get_latency_us());
    rrc_log->console("PCCH message received %d bytes\n", pdu->N_bytes);

    LIBLTE_RRC_PCCH_MSG_STRUCT pcch_msg;
    srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
    bit_buf.N_bits = pdu->N_bytes * 8;
    pool->deallocate(pdu);
    liblte_rrc_unpack_pcch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &pcch_msg);

    if (pcch_msg.paging_record_list_size > LIBLTE_RRC_MAX_PAGE_REC) {
      pcch_msg.paging_record_list_size = LIBLTE_RRC_MAX_PAGE_REC;
    }

    LIBLTE_RRC_S_TMSI_STRUCT s_tmsi;
    if (!nas->get_s_tmsi(&s_tmsi)) {
      rrc_log->info("No S-TMSI present in NAS\n");
      return;
    }

    LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi_paged;
    for (uint32_t i = 0; i < pcch_msg.paging_record_list_size; i++) {
      s_tmsi_paged = &pcch_msg.paging_record_list[i].ue_identity.s_tmsi;
      rrc_log->info("Received paging (%d/%d) for UE %x:%x\n", i + 1, pcch_msg.paging_record_list_size,
                    pcch_msg.paging_record_list[i].ue_identity.s_tmsi.mmec,
                    pcch_msg.paging_record_list[i].ue_identity.s_tmsi.m_tmsi);
      rrc_log->console("Received paging (%d/%d) for UE %x:%x\n", i + 1, pcch_msg.paging_record_list_size,
                       pcch_msg.paging_record_list[i].ue_identity.s_tmsi.mmec,
                       pcch_msg.paging_record_list[i].ue_identity.s_tmsi.m_tmsi);
      if (s_tmsi.mmec == s_tmsi_paged->mmec && s_tmsi.m_tmsi == s_tmsi_paged->m_tmsi) {
        rrc_log->info("S-TMSI match in paging message\n");
        rrc_log->console("S-TMSI match in paging message\n");
        mac->pcch_stop_rx();
        if (RRC_STATE_IDLE == state) {
          rrc_log->info("RRC in IDLE state - sending connection request.\n");
          connection_requested = true;
          state = RRC_STATE_CELL_SELECTED;
        }
      }
    }
  }
}











/*******************************************************************************
*
*
*
* Packet processing
*
*
*******************************************************************************/
byte_buffer_t* rrc::byte_align_and_pack(byte_buffer_t *pdu)
{
  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }

  // Reset and reuse sdu buffer if provided
  byte_buffer_t *pdcp_buf = pdu;

  if (pdcp_buf) {
    pdcp_buf->reset();
  } else {
    pdcp_buf = pool_allocate;
  }

  srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
  pdcp_buf->N_bytes = bit_buf.N_bits / 8;
  pdcp_buf->set_timestamp();

  return pdcp_buf;
}

void rrc::send_ul_ccch_msg(byte_buffer_t *pdu)
{
  liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);
  pdu = byte_align_and_pack(pdu);

  // Set UE contention resolution ID in MAC
  uint64_t uecri = 0;
  uint8_t *ue_cri_ptr = (uint8_t *) &uecri;
  uint32_t nbytes = 6;
  for (uint32_t i = 0; i < nbytes; i++) {
    ue_cri_ptr[nbytes - i - 1] = pdu->msg[i];
  }

  rrc_log->debug("Setting UE contention resolution ID: %d\n", uecri);
  mac->set_contention_id(uecri);

  rrc_log->info("Sending %s\n", liblte_rrc_ul_ccch_msg_type_text[ul_ccch_msg.msg_type]);
  pdcp->write_sdu(RB_ID_SRB0, pdu);
}

void rrc::send_ul_dcch_msg(byte_buffer_t *pdu)
{
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  pdu = byte_align_and_pack(pdu);

  rrc_log->info("Sending %s\n", liblte_rrc_ul_dcch_msg_type_text[ul_dcch_msg.msg_type]);
  pdcp->write_sdu(RB_ID_SRB1, pdu);
}

void rrc::write_sdu(uint32_t lcid, byte_buffer_t *sdu) {

  rrc_log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU", get_rb_name(lcid).c_str());
  switch (state) {
    case RRC_STATE_CONNECTING:
      send_con_setup_complete(sdu);
      break;
    case RRC_STATE_CONNECTED:
      send_ul_info_transfer(lcid, sdu);
      break;
    default:
      rrc_log->error("SDU received from NAS while RRC state = %s\n", rrc_state_text[state]);
      break;
  }
}

void rrc::write_pdu(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU", get_rb_name(lcid).c_str());

  switch (lcid) {
    case RB_ID_SRB0:
      parse_dl_ccch(pdu);
      break;
    case RB_ID_SRB1:
    case RB_ID_SRB2:
      parse_dl_dcch(lcid, pdu);
      break;
    default:
      rrc_log->error("RX PDU with invalid bearer id: %s", lcid);
      break;
  }
}

void rrc::parse_dl_ccch(byte_buffer_t *pdu) {
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  pool->deallocate(pdu);
  bzero(&dl_ccch_msg, sizeof(LIBLTE_RRC_DL_CCCH_MSG_STRUCT));
  liblte_rrc_unpack_dl_ccch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dl_ccch_msg);

  rrc_log->info("SRB0 - Received %s\n",
                liblte_rrc_dl_ccch_msg_type_text[dl_ccch_msg.msg_type]);

  switch (dl_ccch_msg.msg_type) {
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REJ:
      rrc_log->info("Connection Reject received. Wait time: %d\n",
                    dl_ccch_msg.msg.rrc_con_rej.wait_time);
      state = RRC_STATE_LEAVE_CONNECTED;
      break;
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_SETUP:
      rrc_log->info("Connection Setup received\n");
      transaction_id = dl_ccch_msg.msg.rrc_con_setup.rrc_transaction_id;
      handle_con_setup(&dl_ccch_msg.msg.rrc_con_setup);
      rrc_log->info("Notifying NAS of connection setup\n");
      nas->notify_connection_setup();
      break;
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST:
      rrc_log->info("Connection Reestablishment received\n");
      rrc_log->console("Reestablishment OK\n");
      transaction_id = dl_ccch_msg.msg.rrc_con_reest.rrc_transaction_id;
      handle_con_reest(&dl_ccch_msg.msg.rrc_con_reest);
      break;
      /* Reception of RRCConnectionReestablishmentReject 5.3.7.8 */
    case LIBLTE_RRC_DL_CCCH_MSG_TYPE_RRC_CON_REEST_REJ:
      rrc_log->info("Connection Reestablishment Reject received\n");
      rrc_log->console("Reestablishment Reject\n");
      state = RRC_STATE_LEAVE_CONNECTED;
      break;
    default:
      break;
  }
}

void rrc::parse_dl_dcch(uint32_t lcid, byte_buffer_t *pdu) {
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  liblte_rrc_unpack_dl_dcch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dl_dcch_msg);

  rrc_log->info("%s - Received %s\n",
                get_rb_name(lcid).c_str(),
                liblte_rrc_dl_dcch_msg_type_text[dl_dcch_msg.msg_type]);

  // Reset and reuse pdu buffer if possible
  pdu->reset();

  switch (dl_dcch_msg.msg_type) {
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_DL_INFO_TRANSFER:
      memcpy(pdu->msg, dl_dcch_msg.msg.dl_info_transfer.dedicated_info.msg,
             dl_dcch_msg.msg.dl_info_transfer.dedicated_info.N_bytes);
      pdu->N_bytes = dl_dcch_msg.msg.dl_info_transfer.dedicated_info.N_bytes;
      nas->write_pdu(lcid, pdu);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_SECURITY_MODE_COMMAND:
      transaction_id = dl_dcch_msg.msg.security_mode_cmd.rrc_transaction_id;

      cipher_algo = (CIPHERING_ALGORITHM_ID_ENUM) dl_dcch_msg.msg.security_mode_cmd.sec_algs.cipher_alg;
      integ_algo = (INTEGRITY_ALGORITHM_ID_ENUM) dl_dcch_msg.msg.security_mode_cmd.sec_algs.int_alg;

      rrc_log->info("Received Security Mode Command eea: %s, eia: %s\n",
                    ciphering_algorithm_id_text[cipher_algo],
                    integrity_algorithm_id_text[integ_algo]);

      // Generate AS security keys
      uint8_t k_asme[32];
      nas->get_k_asme(k_asme, 32);
      usim->generate_as_keys(k_asme, nas->get_ul_count()-1, k_rrc_enc, k_rrc_int, k_up_enc, k_up_int, cipher_algo, integ_algo);
      rrc_log->debug_hex(k_rrc_enc, 32, "RRC encryption key - k_rrc_enc");
      rrc_log->debug_hex(k_rrc_int, 32, "RRC integrity key  - k_rrc_int");
      rrc_log->debug_hex(k_up_enc, 32,  "UP encryption key  - k_up_enc");

      // Configure PDCP for security
      pdcp->config_security(lcid, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
      pdcp->enable_integrity(lcid);
      send_security_mode_complete(lcid, pdu);
      pdcp->enable_encryption(lcid);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG:
      transaction_id = dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id;
      handle_rrc_con_reconfig(lcid, &dl_dcch_msg.msg.rrc_con_reconfig, pdu);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY:
      transaction_id = dl_dcch_msg.msg.ue_cap_enquiry.rrc_transaction_id;
      for (uint32_t i = 0; i < dl_dcch_msg.msg.ue_cap_enquiry.N_ue_cap_reqs; i++) {
        if (LIBLTE_RRC_RAT_TYPE_EUTRA == dl_dcch_msg.msg.ue_cap_enquiry.ue_capability_request[i]) {
          send_rrc_ue_cap_info(pdu);
          break;
        }
      }
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RELEASE:
      rrc_connection_release();
      break;
    default:
      break;
  }
}









/*******************************************************************************
*
*
*
* Capabilities Message
*
*
*
*******************************************************************************/
void rrc::enable_capabilities() {
  bool enable_ul_64 = args.ue_category >= 5 && serving_cell->sib2.rr_config_common_sib.pusch_cnfg.enable_64_qam;
  rrc_log->info("%s 64QAM PUSCH\n", enable_ul_64 ? "Enabling" : "Disabling");
  phy->set_config_64qam_en(enable_ul_64);
}

void rrc::send_rrc_ue_cap_info(byte_buffer_t *pdu) {
  rrc_log->debug("Preparing UE Capability Info\n");

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO;
  ul_dcch_msg.msg.ue_capability_info.rrc_transaction_id = transaction_id;

  LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *info = &ul_dcch_msg.msg.ue_capability_info;
  info->N_ue_caps = 1;
  info->ue_capability_rat[0].rat_type = LIBLTE_RRC_RAT_TYPE_EUTRA;

  LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *cap = &info->ue_capability_rat[0].eutra_capability;
  cap->access_stratum_release = LIBLTE_RRC_ACCESS_STRATUM_RELEASE_REL8;
  cap->ue_category = args.ue_category;

  cap->pdcp_params.max_rohc_ctxts_present = false;
  cap->pdcp_params.supported_rohc_profiles[0] = false;
  cap->pdcp_params.supported_rohc_profiles[1] = false;
  cap->pdcp_params.supported_rohc_profiles[2] = false;
  cap->pdcp_params.supported_rohc_profiles[3] = false;
  cap->pdcp_params.supported_rohc_profiles[4] = false;
  cap->pdcp_params.supported_rohc_profiles[5] = false;
  cap->pdcp_params.supported_rohc_profiles[6] = false;
  cap->pdcp_params.supported_rohc_profiles[7] = false;
  cap->pdcp_params.supported_rohc_profiles[8] = false;

  cap->phy_params.specific_ref_sigs_supported = false;
  cap->phy_params.tx_antenna_selection_supported = false;

  cap->rf_params.N_supported_band_eutras = args.nof_supported_bands;
  cap->meas_params.N_band_list_eutra     = args.nof_supported_bands;
  for (uint32_t i=0;i<args.nof_supported_bands;i++) {
    cap->rf_params.supported_band_eutra[i].band_eutra = args.supported_bands[i];
    cap->rf_params.supported_band_eutra[i].half_duplex = false;
    cap->meas_params.band_list_eutra[i].N_inter_freq_need_for_gaps = 1;
    cap->meas_params.band_list_eutra[i].inter_freq_need_for_gaps[0] = true;
  }

  cap->feature_group_indicator_present = true;
  cap->feature_group_indicator = args.feature_group;
  cap->inter_rat_params.utra_fdd_present = false;
  cap->inter_rat_params.utra_tdd128_present = false;
  cap->inter_rat_params.utra_tdd384_present = false;
  cap->inter_rat_params.utra_tdd768_present = false;
  cap->inter_rat_params.geran_present = false;
  cap->inter_rat_params.cdma2000_hrpd_present = false;
  cap->inter_rat_params.cdma2000_1xrtt_present = false;

  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  send_ul_dcch_msg(pdu);
}










/*******************************************************************************
*
*
*
* PHY and MAC Radio Resource configuration
*
*
*
*******************************************************************************/

void rrc::apply_rr_config_common_dl(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config) {
  mac_interface_rrc::mac_cfg_t mac_cfg;
  mac->get_config(&mac_cfg);
  if (config->rach_cnfg_present) {
    memcpy(&mac_cfg.rach, &config->rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
  }
  mac_cfg.prach_config_index = config->prach_cnfg.root_sequence_index;
  mac_cfg.ul_harq_params.max_harq_msg3_tx = config->rach_cnfg.max_harq_msg3_tx;

  mac->set_config(&mac_cfg);

  phy_interface_rrc::phy_cfg_t phy_cfg;
  phy->get_config(&phy_cfg);
  phy_interface_rrc::phy_cfg_common_t *common = &phy_cfg.common;

  if (config->pdsch_cnfg_present) {
    memcpy(&common->pdsch_cnfg, &config->pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  }
  common->prach_cnfg.root_sequence_index = config->prach_cnfg.root_sequence_index;
  if (config->prach_cnfg.prach_cnfg_info_present) {
    memcpy(&common->prach_cnfg.prach_cnfg_info, &config->prach_cnfg.prach_cnfg_info, sizeof(LIBLTE_RRC_PRACH_CONFIG_INFO_STRUCT));
  }

  phy->set_config_common(common);
}

void rrc::apply_rr_config_common_ul(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config) {
  phy_interface_rrc::phy_cfg_t phy_cfg;
  phy->get_config(&phy_cfg);
  phy_interface_rrc::phy_cfg_common_t *common = &phy_cfg.common;

  memcpy(&common->pusch_cnfg, &config->pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  if (config->pucch_cnfg_present) {
    memcpy(&common->pucch_cnfg, &config->pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  }
  if (config->ul_pwr_ctrl_present) {
    memcpy(&common->ul_pwr_ctrl, &config->ul_pwr_ctrl, sizeof(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT));
  }
  if (config->srs_ul_cnfg.present) {
    memcpy(&common->srs_ul_cnfg, &config->srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
  } else {
    // default is release
    common->srs_ul_cnfg.present = false;
  }
  phy->set_config_common(common);
  phy->configure_ul_params();
}

void rrc::apply_sib2_configs(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2) {

  // Apply RACH timeAlginmentTimer configuration
  mac_interface_rrc::mac_cfg_t cfg;
  mac->get_config(&cfg);

  cfg.main.time_alignment_timer = sib2->time_alignment_timer;
  memcpy(&cfg.rach, &sib2->rr_config_common_sib.rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
  cfg.prach_config_index = sib2->rr_config_common_sib.prach_cnfg.root_sequence_index;
  cfg.ul_harq_params.max_harq_msg3_tx = cfg.rach.max_harq_msg3_tx;
  // Apply MBSFN configuration
//  cfg.mbsfn_subfr_cnfg_list_size = sib2->mbsfn_subfr_cnfg_list_size;
//  for(uint8_t i=0;i<sib2->mbsfn_subfr_cnfg_list_size;i++) {
//    memcpy(&cfg.mbsfn_subfr_cnfg_list[i], &sib2->mbsfn_subfr_cnfg_list[i], sizeof(LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT));
//  }

  mac->set_config(&cfg);

  rrc_log->info("Set RACH ConfigCommon: NofPreambles=%d, ResponseWindow=%d, ContentionResolutionTimer=%d ms\n",
                liblte_rrc_number_of_ra_preambles_num[sib2->rr_config_common_sib.rach_cnfg.num_ra_preambles],
                liblte_rrc_ra_response_window_size_num[sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size],
                liblte_rrc_mac_contention_resolution_timer_num[sib2->rr_config_common_sib.rach_cnfg.mac_con_res_timer]);

  // Apply PHY RR Config Common
  phy_interface_rrc::phy_cfg_common_t common;
  memcpy(&common.pdsch_cnfg, &sib2->rr_config_common_sib.pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.pusch_cnfg, &sib2->rr_config_common_sib.pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.pucch_cnfg, &sib2->rr_config_common_sib.pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  memcpy(&common.ul_pwr_ctrl, &sib2->rr_config_common_sib.ul_pwr_ctrl,
         sizeof(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT));
  memcpy(&common.prach_cnfg, &sib2->rr_config_common_sib.prach_cnfg, sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
  if (sib2->rr_config_common_sib.srs_ul_cnfg.present) {
    memcpy(&common.srs_ul_cnfg, &sib2->rr_config_common_sib.srs_ul_cnfg,
           sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
  } else {
    // default is release
    common.srs_ul_cnfg.present = false;
  }
  phy->set_config_common(&common);

  phy->configure_ul_params();

  rrc_log->info("Set PUSCH ConfigCommon: HopOffset=%d, RSGroup=%d, RSNcs=%d, N_sb=%d\n",
                sib2->rr_config_common_sib.pusch_cnfg.pusch_hopping_offset,
                sib2->rr_config_common_sib.pusch_cnfg.ul_rs.group_assignment_pusch,
                sib2->rr_config_common_sib.pusch_cnfg.ul_rs.cyclic_shift,
                sib2->rr_config_common_sib.pusch_cnfg.n_sb);

  rrc_log->info("Set PUCCH ConfigCommon: DeltaShift=%d, CyclicShift=%d, N1=%d, NRB=%d\n",
                liblte_rrc_delta_pucch_shift_num[sib2->rr_config_common_sib.pucch_cnfg.delta_pucch_shift],
                sib2->rr_config_common_sib.pucch_cnfg.n_cs_an,
                sib2->rr_config_common_sib.pucch_cnfg.n1_pucch_an,
                sib2->rr_config_common_sib.pucch_cnfg.n_rb_cqi);

  rrc_log->info("Set PRACH ConfigCommon: SeqIdx=%d, HS=%s, FreqOffset=%d, ZC=%d, ConfigIndex=%d\n",
                sib2->rr_config_common_sib.prach_cnfg.root_sequence_index,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.high_speed_flag ? "yes" : "no",
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_freq_offset,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config,
                sib2->rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_config_index);

  rrc_log->info("Set SRS ConfigCommon: BW-Configuration=%d, SF-Configuration=%d, ACKNACK=%s\n",
                liblte_rrc_srs_bw_config_num[sib2->rr_config_common_sib.srs_ul_cnfg.bw_cnfg],
                liblte_rrc_srs_subfr_config_num[sib2->rr_config_common_sib.srs_ul_cnfg.subfr_cnfg],
                sib2->rr_config_common_sib.srs_ul_cnfg.ack_nack_simul_tx ? "yes" : "no");

  mac_timers->timer_get(t301)->set(this, liblte_rrc_t301_num[sib2->ue_timers_and_constants.t301]);
  mac_timers->timer_get(t310)->set(this, liblte_rrc_t310_num[sib2->ue_timers_and_constants.t310]);
  mac_timers->timer_get(t311)->set(this, liblte_rrc_t311_num[sib2->ue_timers_and_constants.t311]);
  N310 = liblte_rrc_n310_num[sib2->ue_timers_and_constants.n310];
  N311 = liblte_rrc_n311_num[sib2->ue_timers_and_constants.n311];

  rrc_log->info("Set Constants and Timers: N310=%d, N311=%d, t301=%d, t310=%d, t311=%d\n",
                N310, N311, mac_timers->timer_get(t301)->get_timeout(),
                mac_timers->timer_get(t310)->get_timeout(), mac_timers->timer_get(t311)->get_timeout());

}

// Go through all information elements and apply defaults (9.2.4) if not defined
void rrc::apply_phy_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cnfg, bool apply_defaults) {
  // Get current configuration
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *current_cfg;
  phy_interface_rrc::phy_cfg_t c;
  phy->get_config(&c);
  current_cfg = &c.dedicated;

  if (phy_cnfg->pucch_cnfg_ded_present) {
    memcpy(&current_cfg->pucch_cnfg_ded, &phy_cnfg->pucch_cnfg_ded, sizeof(LIBLTE_RRC_PUCCH_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->pucch_cnfg_ded.tdd_ack_nack_feedback_mode = LIBLTE_RRC_TDD_ACK_NACK_FEEDBACK_MODE_BUNDLING;
    current_cfg->pucch_cnfg_ded.ack_nack_repetition_setup_present = false;
  }
  if (phy_cnfg->pusch_cnfg_ded_present) {
    memcpy(&current_cfg->pusch_cnfg_ded, &phy_cnfg->pusch_cnfg_ded, sizeof(LIBLTE_RRC_PUSCH_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->pusch_cnfg_ded.beta_offset_ack_idx = 10;
    current_cfg->pusch_cnfg_ded.beta_offset_ri_idx = 12;
    current_cfg->pusch_cnfg_ded.beta_offset_cqi_idx = 15;
  }
  if (phy_cnfg->ul_pwr_ctrl_ded_present) {
    memcpy(&current_cfg->ul_pwr_ctrl_ded, &phy_cnfg->ul_pwr_ctrl_ded,
           sizeof(LIBLTE_RRC_UL_POWER_CONTROL_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->ul_pwr_ctrl_ded.p0_ue_pusch     = 0;
    current_cfg->ul_pwr_ctrl_ded.delta_mcs_en    = LIBLTE_RRC_DELTA_MCS_ENABLED_EN0;
    current_cfg->ul_pwr_ctrl_ded.accumulation_en = true;
    current_cfg->ul_pwr_ctrl_ded.p0_ue_pucch     = 0;
    current_cfg->ul_pwr_ctrl_ded.p_srs_offset    = 7;
  }
  if (phy_cnfg->ul_pwr_ctrl_ded.filter_coeff_present) {
    current_cfg->ul_pwr_ctrl_ded.filter_coeff = phy_cnfg->ul_pwr_ctrl_ded.filter_coeff;
  } else {
    current_cfg->ul_pwr_ctrl_ded.filter_coeff = LIBLTE_RRC_FILTER_COEFFICIENT_FC4;
  }
  if (phy_cnfg->tpc_pdcch_cnfg_pucch_present) {
    memcpy(&current_cfg->tpc_pdcch_cnfg_pucch, &phy_cnfg->tpc_pdcch_cnfg_pucch,
           sizeof(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT));
  } else if (apply_defaults) {
    current_cfg->tpc_pdcch_cnfg_pucch.setup_present = false;
  }
  if (phy_cnfg->tpc_pdcch_cnfg_pusch_present) {
    memcpy(&current_cfg->tpc_pdcch_cnfg_pusch, &phy_cnfg->tpc_pdcch_cnfg_pusch,
           sizeof(LIBLTE_RRC_TPC_PDCCH_CONFIG_STRUCT));
  } else {
    current_cfg->tpc_pdcch_cnfg_pusch.setup_present = false;
  }
  if (phy_cnfg->cqi_report_cnfg_present) {
    if (phy_cnfg->cqi_report_cnfg.report_periodic_present) {
      memcpy(&current_cfg->cqi_report_cnfg.report_periodic, &phy_cnfg->cqi_report_cnfg.report_periodic,
             sizeof(LIBLTE_RRC_CQI_REPORT_PERIODIC_STRUCT));
      current_cfg->cqi_report_cnfg.report_periodic_setup_present = phy_cnfg->cqi_report_cnfg.report_periodic_setup_present;
    } else if (apply_defaults) {
      current_cfg->cqi_report_cnfg.report_periodic_setup_present = false;
    }
    if (phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present) {
      current_cfg->cqi_report_cnfg.report_mode_aperiodic = phy_cnfg->cqi_report_cnfg.report_mode_aperiodic;
      current_cfg->cqi_report_cnfg.report_mode_aperiodic_present = phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present;
    } else if (apply_defaults) {
      current_cfg->cqi_report_cnfg.report_mode_aperiodic_present = false;
    }
    current_cfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset = phy_cnfg->cqi_report_cnfg.nom_pdsch_rs_epre_offset;
  }
  if (phy_cnfg->srs_ul_cnfg_ded_present && phy_cnfg->srs_ul_cnfg_ded.setup_present) {
    memcpy(&current_cfg->srs_ul_cnfg_ded, &phy_cnfg->srs_ul_cnfg_ded,
           sizeof(LIBLTE_RRC_SRS_UL_CONFIG_DEDICATED_STRUCT));
  } else if (apply_defaults) {
    current_cfg->srs_ul_cnfg_ded.setup_present = false;
  }
  if (phy_cnfg->antenna_info_present) {
    if (!phy_cnfg->antenna_info_default_value) {
      if (phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_1 &&
          phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_2 &&
	  phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_3 &&
	  phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_4) {
        rrc_log->error("Transmission mode TM%s not currently supported by srsUE\n",
                       liblte_rrc_transmission_mode_text[phy_cnfg->antenna_info_explicit_value.tx_mode]);
      }
      memcpy(&current_cfg->antenna_info_explicit_value, &phy_cnfg->antenna_info_explicit_value,
             sizeof(LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT));
    } else if (apply_defaults) {
      current_cfg->antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_2;
      current_cfg->antenna_info_explicit_value.codebook_subset_restriction_present = false;
      current_cfg->antenna_info_explicit_value.ue_tx_antenna_selection_setup_present = false;
    }
  } else if (apply_defaults) {
    current_cfg->antenna_info_explicit_value.tx_mode = LIBLTE_RRC_TRANSMISSION_MODE_2;
    current_cfg->antenna_info_explicit_value.codebook_subset_restriction_present = false;
    current_cfg->antenna_info_explicit_value.ue_tx_antenna_selection_setup_present = false;
  }
  if (phy_cnfg->sched_request_cnfg_present) {
    memcpy(&current_cfg->sched_request_cnfg, &phy_cnfg->sched_request_cnfg,
           sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  } else if (apply_defaults) {
    current_cfg->sched_request_cnfg.setup_present = false;
  }
  if (phy_cnfg->pdsch_cnfg_ded_present) {
    current_cfg->pdsch_cnfg_ded = phy_cnfg->pdsch_cnfg_ded;
    rrc_log->info("Set PDSCH-Config=%s (present)\n", liblte_rrc_pdsch_config_p_a_text[(int) current_cfg->pdsch_cnfg_ded]);
  } else if (apply_defaults) {
    current_cfg->pdsch_cnfg_ded = LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_0;
    rrc_log->info("Set PDSCH-Config=%s (default)\n", liblte_rrc_pdsch_config_p_a_text[(int) current_cfg->pdsch_cnfg_ded]);
  }

  if (phy_cnfg->cqi_report_cnfg_present) {
    if (phy_cnfg->cqi_report_cnfg.report_periodic_present) {
      rrc_log->info("Set cqi-PUCCH-ResourceIndex=%d, cqi-pmi-ConfigIndex=%d, cqi-FormatIndicatorPeriodic=%s\n",
                    current_cfg->cqi_report_cnfg.report_periodic.pucch_resource_idx,
                    current_cfg->cqi_report_cnfg.report_periodic.pmi_cnfg_idx,
                    liblte_rrc_cqi_format_indicator_periodic_text[current_cfg->cqi_report_cnfg.report_periodic.format_ind_periodic]);
    }
    if (phy_cnfg->cqi_report_cnfg.report_mode_aperiodic_present) {
      rrc_log->info("Set cqi-ReportModeAperiodic=%s\n",
                    liblte_rrc_cqi_report_mode_aperiodic_text[current_cfg->cqi_report_cnfg.report_mode_aperiodic]);
    }

  }

  if (phy_cnfg->sched_request_cnfg_present) {
    rrc_log->info("Set PHY config ded: SR-n_pucch=%d, SR-ConfigIndex=%d, SR-TransMax=%d\n",
                  current_cfg->sched_request_cnfg.sr_pucch_resource_idx,
                  current_cfg->sched_request_cnfg.sr_cnfg_idx,
                  liblte_rrc_dsr_trans_max_num[current_cfg->sched_request_cnfg.dsr_trans_max]);
  }

  if (current_cfg->srs_ul_cnfg_ded_present) {
    rrc_log->info("Set PHY config ded: SRS-ConfigIndex=%d, SRS-bw=%s, SRS-Nrcc=%d, SRS-hop=%s, SRS-Ncs=%s\n",
                  current_cfg->srs_ul_cnfg_ded.srs_cnfg_idx,
                  liblte_rrc_srs_bandwidth_text[current_cfg->srs_ul_cnfg_ded.srs_bandwidth],
                  current_cfg->srs_ul_cnfg_ded.freq_domain_pos,
                  liblte_rrc_srs_hopping_bandwidth_text[current_cfg->srs_ul_cnfg_ded.srs_hopping_bandwidth],
                  liblte_rrc_cyclic_shift_text[current_cfg->srs_ul_cnfg_ded.cyclic_shift]);
  }

  phy->set_config_dedicated(current_cfg);

  // Apply changes to PHY
  phy->configure_ul_params();
}

void rrc::apply_mac_config_dedicated(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *mac_cnfg, bool apply_defaults) {
  // Set Default MAC main configuration (9.2.2)
  LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT default_cfg;
  bzero(&default_cfg, sizeof(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT));
  default_cfg.ulsch_cnfg.max_harq_tx = LIBLTE_RRC_MAX_HARQ_TX_N5;
  default_cfg.ulsch_cnfg.periodic_bsr_timer = LIBLTE_RRC_PERIODIC_BSR_TIMER_INFINITY;
  default_cfg.ulsch_cnfg.retx_bsr_timer = LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF2560;
  default_cfg.ulsch_cnfg.tti_bundling = false;
  default_cfg.drx_cnfg.setup_present = false;
  default_cfg.phr_cnfg.setup_present = false;
  default_cfg.time_alignment_timer = LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY;


  if (!apply_defaults) {
    if (mac_cnfg->ulsch_cnfg_present) {
      if (mac_cnfg->ulsch_cnfg.max_harq_tx_present) {
        default_cfg.ulsch_cnfg.max_harq_tx = mac_cnfg->ulsch_cnfg.max_harq_tx;
        default_cfg.ulsch_cnfg.max_harq_tx_present = true;
      }
      if (mac_cnfg->ulsch_cnfg.periodic_bsr_timer_present) {
        default_cfg.ulsch_cnfg.periodic_bsr_timer = mac_cnfg->ulsch_cnfg.periodic_bsr_timer;
        default_cfg.ulsch_cnfg.periodic_bsr_timer_present = true;
      }
      default_cfg.ulsch_cnfg.retx_bsr_timer = mac_cnfg->ulsch_cnfg.retx_bsr_timer;
      default_cfg.ulsch_cnfg.tti_bundling = mac_cnfg->ulsch_cnfg.tti_bundling;
    }
    if (mac_cnfg->drx_cnfg_present) {
      memcpy(&default_cfg.drx_cnfg, &mac_cnfg->drx_cnfg, sizeof(LIBLTE_RRC_DRX_CONFIG_STRUCT));
      default_cfg.drx_cnfg_present = true;
    }
    if (mac_cnfg->phr_cnfg_present) {
      memcpy(&default_cfg.phr_cnfg, &mac_cnfg->phr_cnfg, sizeof(LIBLTE_RRC_PHR_CONFIG_STRUCT));
      default_cfg.phr_cnfg_present = true;
    }
    default_cfg.time_alignment_timer = mac_cnfg->time_alignment_timer;
  }

  // Setup MAC configuration
  mac->set_config_main(&default_cfg);

  // Update UL HARQ config
  mac_interface_rrc::mac_cfg_t cfg;
  mac->get_config(&cfg);
  cfg.ul_harq_params.max_harq_tx = liblte_rrc_max_harq_tx_num[default_cfg.ulsch_cnfg.max_harq_tx];
  mac->set_config(&cfg);

  rrc_log->info("Set MAC main config: harq-MaxReTX=%d, bsr-TimerReTX=%d, bsr-TimerPeriodic=%d\n",
                liblte_rrc_max_harq_tx_num[default_cfg.ulsch_cnfg.max_harq_tx],
                liblte_rrc_retransmission_bsr_timer_num[default_cfg.ulsch_cnfg.retx_bsr_timer],
                liblte_rrc_periodic_bsr_timer_num[default_cfg.ulsch_cnfg.periodic_bsr_timer]);
  if (default_cfg.phr_cnfg_present) {
    rrc_log->info("Set MAC PHR config: periodicPHR-Timer=%d, prohibitPHR-Timer=%d, dl-PathlossChange=%d\n",
                  liblte_rrc_periodic_phr_timer_num[default_cfg.phr_cnfg.periodic_phr_timer],
                  liblte_rrc_prohibit_phr_timer_num[default_cfg.phr_cnfg.prohibit_phr_timer],
                  liblte_rrc_dl_pathloss_change_num[default_cfg.phr_cnfg.dl_pathloss_change]);
  }
}

void rrc::apply_rr_config_dedicated(LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT *cnfg) {
  if (cnfg->phy_cnfg_ded_present) {
    apply_phy_config_dedicated(&cnfg->phy_cnfg_ded, false);
    // Apply SR configuration to MAC
    if (cnfg->phy_cnfg_ded.sched_request_cnfg_present) {
      mac->set_config_sr(&cnfg->phy_cnfg_ded.sched_request_cnfg);
    }
  }

  if (cnfg->mac_main_cnfg_present) {
    apply_mac_config_dedicated(&cnfg->mac_main_cnfg.explicit_value, cnfg->mac_main_cnfg.default_value);
  }

  if (cnfg->sps_cnfg_present) {
    //TODO
  }
  if (cnfg->rlf_timers_and_constants_present) {
    //TODO
  }
  for (uint32_t i = 0; i < cnfg->srb_to_add_mod_list_size; i++) {
    // TODO: handle SRB modification
    add_srb(&cnfg->srb_to_add_mod_list[i]);
  }
  for (uint32_t i = 0; i < cnfg->drb_to_release_list_size; i++) {
    release_drb(cnfg->drb_to_release_list[i]);
  }
  for (uint32_t i = 0; i < cnfg->drb_to_add_mod_list_size; i++) {
    // TODO: handle DRB modification
    add_drb(&cnfg->drb_to_add_mod_list[i]);
  }
}

void rrc::handle_con_setup(LIBLTE_RRC_CONNECTION_SETUP_STRUCT *setup) {
  // Apply the Radio Resource configuration
  apply_rr_config_dedicated(&setup->rr_cnfg);
}

/* Reception of RRCConnectionReestablishment by the UE 5.3.7.5 */
void rrc::handle_con_reest(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *setup) {
  mac_timers->timer_get(t301)->stop();

  // TODO: Reestablish DRB1. Not done because never was suspended

  // Apply the Radio Resource configuration
  apply_rr_config_dedicated(&setup->rr_cnfg);

  // TODO: Some security stuff here... is it necessary?

  send_con_restablish_complete();
}


void rrc::add_srb(LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT *srb_cnfg) {
  // Setup PDCP
  pdcp->add_bearer(srb_cnfg->srb_id, srslte_pdcp_config_t(true)); // Set PDCP config control flag
  if(RB_ID_SRB2 == srb_cnfg->srb_id) {
    pdcp->config_security(srb_cnfg->srb_id, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
    pdcp->enable_integrity(srb_cnfg->srb_id);
    pdcp->enable_encryption(srb_cnfg->srb_id);
  }

  // Setup RLC
  if (srb_cnfg->rlc_cnfg_present) {
    if (srb_cnfg->rlc_default_cnfg_present) {
      rlc->add_bearer(srb_cnfg->srb_id);
    }else{
      rlc->add_bearer(srb_cnfg->srb_id, srslte_rlc_config_t(&srb_cnfg->rlc_explicit_cnfg));
    }
  }

  // Setup MAC
  uint8_t log_chan_group = 0;
  uint8_t priority = 1;
  int prioritized_bit_rate = -1;
  int bucket_size_duration = -1;

  if (srb_cnfg->lc_cnfg_present) {
    if (srb_cnfg->lc_default_cnfg_present) {
      if (RB_ID_SRB2 == srb_cnfg->srb_id)
        priority = 3;
    } else {
      if (srb_cnfg->lc_explicit_cnfg.log_chan_sr_mask_present) {
        //TODO
      }
      if (srb_cnfg->lc_explicit_cnfg.ul_specific_params_present) {
        if (srb_cnfg->lc_explicit_cnfg.ul_specific_params.log_chan_group_present)
          log_chan_group = srb_cnfg->lc_explicit_cnfg.ul_specific_params.log_chan_group;

        priority = srb_cnfg->lc_explicit_cnfg.ul_specific_params.priority;
        prioritized_bit_rate = liblte_rrc_prioritized_bit_rate_num[srb_cnfg->lc_explicit_cnfg.ul_specific_params.prioritized_bit_rate];
        bucket_size_duration = liblte_rrc_bucket_size_duration_num[srb_cnfg->lc_explicit_cnfg.ul_specific_params.bucket_size_duration];
      }
    }
    mac->setup_lcid(srb_cnfg->srb_id, log_chan_group, priority, prioritized_bit_rate, bucket_size_duration);
  }

  srbs[srb_cnfg->srb_id] = *srb_cnfg;
  rrc_log->info("Added radio bearer %s\n", get_rb_name(srb_cnfg->srb_id).c_str());
}

void rrc::add_drb(LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT *drb_cnfg) {

  if (!drb_cnfg->pdcp_cnfg_present ||
      !drb_cnfg->rlc_cnfg_present ||
      !drb_cnfg->lc_cnfg_present) {
    rrc_log->error("Cannot add DRB - incomplete configuration\n");
    return;
  }
  uint32_t lcid = 0;
  if (drb_cnfg->lc_id_present) {
    lcid = drb_cnfg->lc_id;
  } else {
    lcid = RB_ID_SRB2 + drb_cnfg->drb_id;
    rrc_log->warning("LCID not present, using %d\n", lcid);
  }

  // Setup PDCP
  srslte_pdcp_config_t pdcp_cfg;
  pdcp_cfg.is_data = true;
  if (drb_cnfg->pdcp_cnfg.rlc_um_pdcp_sn_size_present) {
    if (LIBLTE_RRC_PDCP_SN_SIZE_7_BITS == drb_cnfg->pdcp_cnfg.rlc_um_pdcp_sn_size) {
      pdcp_cfg.sn_len = 7;
    }
  }
  pdcp->add_bearer(lcid, pdcp_cfg);
  pdcp->config_security(lcid, k_up_enc, k_up_int, cipher_algo, integ_algo);
  pdcp->enable_encryption(lcid);

  // Setup RLC
  rlc->add_bearer(lcid, srslte_rlc_config_t(&drb_cnfg->rlc_cnfg));

  // Setup MAC
  uint8_t log_chan_group = 0;
  uint8_t priority = 1;
  int prioritized_bit_rate = -1;
  int bucket_size_duration = -1;
  if (drb_cnfg->lc_cnfg.ul_specific_params_present) {
    if (drb_cnfg->lc_cnfg.ul_specific_params.log_chan_group_present) {
      log_chan_group = drb_cnfg->lc_cnfg.ul_specific_params.log_chan_group;
    } else {
      rrc_log->warning("LCG not present, setting to 0\n");
    }
    priority = drb_cnfg->lc_cnfg.ul_specific_params.priority;
    prioritized_bit_rate = liblte_rrc_prioritized_bit_rate_num[drb_cnfg->lc_cnfg.ul_specific_params.prioritized_bit_rate];

    if (prioritized_bit_rate > 0) {
      rrc_log->warning("PBR>0 currently not supported. Setting it to Inifinty\n");
      prioritized_bit_rate = -1;
    }

    bucket_size_duration = liblte_rrc_bucket_size_duration_num[drb_cnfg->lc_cnfg.ul_specific_params.bucket_size_duration];
  }
  mac->setup_lcid(lcid, log_chan_group, priority, prioritized_bit_rate, bucket_size_duration);

  drbs[lcid] = *drb_cnfg;
  drb_up     = true;
  rrc_log->info("Added radio bearer %s\n", get_rb_name(lcid).c_str());
}

void rrc::release_drb(uint8_t lcid) {
  // TODO
}

// PHY CONFIG DEDICATED Defaults (3GPP 36.331 v10 9.2.4)
void rrc::set_phy_default_pucch_srs() {

  phy_interface_rrc::phy_cfg_t current_cfg;
  phy->get_config(&current_cfg);

  // Set defaults to CQI, SRS and SR
  current_cfg.dedicated.cqi_report_cnfg_present = false;
  current_cfg.dedicated.srs_ul_cnfg_ded_present = false;
  current_cfg.dedicated.sched_request_cnfg_present = false;

  apply_phy_config_dedicated(&current_cfg.dedicated, true);

  // Release SR configuration from MAC
  LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT cfg;
  bzero(&cfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  mac->set_config_sr(&cfg);
}

void rrc::set_phy_default() {
  LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT defaults;
  bzero(&defaults, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
  apply_phy_config_dedicated(&defaults, true);
}

void rrc::set_mac_default() {
  apply_mac_config_dedicated(NULL, true);
  LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT sr_cfg;
  bzero(&sr_cfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
  sr_cfg.setup_present = false;
  mac->set_config_sr(&sr_cfg);
}

void rrc::set_rrc_default() {
  N310 = 1;
  N311 = 1;
  mac_timers->timer_get(t310)->set(this, 1000);
  mac_timers->timer_get(t311)->set(this, 1000);
}



















/************************************************************************
 *
 *
 * RRC Measurements
 *
 *
 ************************************************************************/

void rrc::rrc_meas::init(rrc *parent) {
  this->parent      = parent;
  this->log_h       = parent->rrc_log;
  this->phy         = parent->phy;
  this->mac_timers  = parent->mac_timers;
  s_measure_enabled = false;
  reset();
}

void rrc::rrc_meas::reset()
{
  filter_k_rsrp = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
  filter_k_rsrq = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
  objects.clear();
  active.clear();
  reports_cfg.clear();
  phy->meas_reset();
  bzero(&pcell_measurement, sizeof(meas_value_t));
}

/* L3 filtering 5.5.3.2 */
void rrc::rrc_meas::L3_filter(meas_value_t *value, float values[NOF_MEASUREMENTS])
{
  for (int i=0;i<NOF_MEASUREMENTS;i++) {
    if (value->ms[i]) {
      value->ms[i] = SRSLTE_VEC_EMA(values[i], value->ms[i], filter_a[i]);
    } else {
      value->ms[i] = values[i];
    }
  }
}

void rrc::rrc_meas::new_phy_meas(uint32_t earfcn, uint32_t pci, float rsrp, float rsrq, uint32_t tti)
{
  float values[NOF_MEASUREMENTS] = {rsrp, rsrq};

  // This indicates serving cell
  if (parent->serving_cell->equals(earfcn, pci)) {

    log_h->info("MEAS:  New measurement serving cell, rsrp=%f, rsrq=%f, tti=%d\n", rsrp, rsrq, tti);

    L3_filter(&pcell_measurement, values);

    // Update serving cell measurement
    parent->serving_cell->rsrp = rsrp;

  } else {

    // Add to list of neighbour cells
    parent->add_neighbour_cell(earfcn, pci, rsrp);

    log_h->info("MEAS:  New measurement earfcn=%d, pci=%d, rsrp=%f, rsrq=%f, tti=%d\n", earfcn, pci, rsrp, rsrq, tti);

    // Save PHY measurement for all active measurements whose earfcn/pci matches
    for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
      meas_t *m = &iter->second;
      if (objects[m->object_id].earfcn == earfcn) {
        // If it's a newly discovered cell, add it to objects
        if (!m->cell_values.count(pci)) {
          uint32_t cell_idx = objects[m->object_id].cells.size();
          objects[m->object_id].cells[cell_idx].pci      = pci;
          objects[m->object_id].cells[cell_idx].q_offset = 0;
        }
        // Update or add cell
        L3_filter(&m->cell_values[pci], values);
        return;
      }
    }
  }
}

void rrc::rrc_meas::run_tti(uint32_t tti) {
  // Measurement Report Triggering Section 5.5.4
  calculate_triggers(tti);
}

bool rrc::rrc_meas::find_earfcn_cell(uint32_t earfcn, uint32_t pci, meas_obj_t **object, int *cell_idx) {
  if (object) {
    *object = NULL;
  }
  for (std::map<uint32_t, meas_obj_t>::iterator obj = objects.begin(); obj != objects.end(); ++obj) {
    if (obj->second.earfcn == earfcn) {
      if (object) {
        *object = &obj->second;
      }
      for (std::map<uint32_t, meas_cell_t>::iterator c = obj->second.cells.begin(); c != obj->second.cells.end(); ++c) {
        if (c->second.pci == pci) {
          if (cell_idx) {
            *cell_idx = c->first;
            return true;
          }
        }
      }
      // return true if cell idx not found but frequency is found
      if (cell_idx) {
        *cell_idx = -1;
      }
      return true;
    }
  }
  return false;
}

/* Generate report procedure 5.5.5 */
void rrc::rrc_meas::generate_report(uint32_t meas_id)
{
  parent->ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_MEASUREMENT_REPORT;
  LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT *report = &parent->ul_dcch_msg.msg.measurement_report;

  bzero(report, sizeof(LIBLTE_RRC_MEASUREMENT_REPORT_STRUCT));

  meas_t       *m   = &active[meas_id];
  report_cfg_t *cfg = &reports_cfg[m->report_id];

  report->meas_id = meas_id;
  report->pcell_rsrp_result = value_to_range(RSRP, pcell_measurement.ms[RSRP]);
  report->pcell_rsrq_result = value_to_range(RSRQ, pcell_measurement.ms[RSRQ]);

  log_h->info("MEAS:  Generate report MeasId=%d, nof_reports_send=%d, Pcell rsrp=%f rsrq=%f\n",
              report->meas_id, m->nof_reports_sent, pcell_measurement.ms[RSRP], pcell_measurement.ms[RSRQ]);

  // TODO: report up to 8 best cells
  for (std::map<uint32_t, meas_value_t>::iterator cell = m->cell_values.begin(); cell != m->cell_values.end(); ++cell)
  {
    if (cell->second.triggered && report->meas_result_neigh_cells.eutra.n_result < 8)
    {
      LIBLTE_RRC_MEAS_RESULT_EUTRA_STRUCT *rc = &report->meas_result_neigh_cells.eutra.result_eutra_list[report->meas_result_neigh_cells.eutra.n_result];

      rc->phys_cell_id = cell->first;
      rc->meas_result.have_rsrp   = cfg->report_quantity==RSRP || cfg->report_quantity==BOTH;
      rc->meas_result.have_rsrq   = cfg->report_quantity==RSRQ || cfg->report_quantity==BOTH;
      rc->meas_result.rsrp_result = value_to_range(RSRP, cell->second.ms[RSRP]);
      rc->meas_result.rsrq_result = value_to_range(RSRQ, cell->second.ms[RSRQ]);

      log_h->info("MEAS:  Adding to report neighbour=%d, pci=%d, rsrp=%f, rsrq=%f\n",
                     report->meas_result_neigh_cells.eutra.n_result, rc->phys_cell_id,
                     cell->second.ms[RSRP], cell->second.ms[RSRQ]);

      report->meas_result_neigh_cells.eutra.n_result++;
    }
  }
  report->have_meas_result_neigh_cells = report->meas_result_neigh_cells.eutra.n_result > 0;

  m->nof_reports_sent++;
  mac_timers->timer_get(m->periodic_timer)->stop();

  if (m->nof_reports_sent < cfg->amount) {
    mac_timers->timer_get(m->periodic_timer)->reset();
    mac_timers->timer_get(m->periodic_timer)->run();
  } else {
    if (cfg->trigger_type == report_cfg_t::PERIODIC) {
      m->triggered = false;
    }
  }

  // Send to lower layers
  parent->send_ul_dcch_msg();
}

/* Handle entering/leaving event conditions 5.5.4.1 */
bool rrc::rrc_meas::process_event(LIBLTE_RRC_EVENT_EUTRA_STRUCT *event, uint32_t tti,
                                  bool enter_condition, bool exit_condition,
                                  meas_t *m, meas_value_t *cell)
{
  bool generate_report = false;
  if (enter_condition && (!m->triggered || !cell->triggered)) {
    if (!cell->timer_enter_triggered) {
      cell->timer_enter_triggered = true;
      cell->enter_tti     = tti;
    } else if (srslte_tti_interval(tti, cell->enter_tti) >= event->time_to_trigger) {
      m->triggered        = true;
      cell->triggered     = true;
      m->nof_reports_sent = 0;
      generate_report     = true;
    }
  } else if (exit_condition) {
    if (!cell->timer_exit_triggered) {
      cell->timer_exit_triggered = true;
      cell->exit_tti      = tti;
    } else if (srslte_tti_interval(tti, cell->exit_tti) >= event->time_to_trigger) {
      m->triggered        = false;
      cell->triggered     = false;
      mac_timers->timer_get(m->periodic_timer)->stop();
      if (event) {
        if (event->event_id == LIBLTE_RRC_EVENT_ID_EUTRA_A3 && event->event_a3.report_on_leave) {
          generate_report = true;
        }
      }
    }
  }
  if (!enter_condition) {
    cell->timer_enter_triggered = false;
  }
  if (!enter_condition) {
    cell->timer_exit_triggered = false;
  }
  return generate_report;
}

/* Calculate trigger conditions for each cell 5.5.4 */
void rrc::rrc_meas::calculate_triggers(uint32_t tti)
{
  float Ofp = 0, Ocp = 0;
  meas_obj_t *serving_object   = NULL;
  int         serving_cell_idx = 0;

  // Get serving cell
  if (active.size()) {
    if (find_earfcn_cell(phy->get_current_earfcn(), phy->get_current_pci(), &serving_object, &serving_cell_idx)) {
      Ofp = serving_object->q_offset;
      if (serving_cell_idx >= 0) {
        Ocp = serving_object->cells[serving_cell_idx].q_offset;
      }
    } else {
      log_h->warning("Can't find current eafcn=%d, pci=%d in objects list. Using Ofp=0, Ocp=0\n",
                     phy->get_current_earfcn(), phy->get_current_pci());
    }
  }

  for (std::map<uint32_t, meas_t>::iterator m = active.begin(); m != active.end(); ++m) {
    report_cfg_t *cfg = &reports_cfg[m->second.report_id];
    float hyst = 0.5*cfg->event.hysteresis;
    float Mp   = pcell_measurement.ms[cfg->trigger_quantity];

    LIBLTE_RRC_EVENT_ID_EUTRA_ENUM event_id = cfg->event.event_id;
    const char *event_str = liblte_rrc_event_id_eutra_text[event_id];

    bool gen_report = false;

    if (cfg->trigger_type == report_cfg_t::EVENT) {

      // A1 & A2 are for serving cell only
      if (event_id < LIBLTE_RRC_EVENT_ID_EUTRA_A3) {

        bool enter_condition;
        bool exit_condition;
        if (event_id == LIBLTE_RRC_EVENT_ID_EUTRA_A1) {
          enter_condition = Mp - hyst > range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
          exit_condition  = Mp + hyst < range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
        } else {
          enter_condition = Mp + hyst < range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
          exit_condition  = Mp - hyst > range_to_value(cfg->trigger_quantity, cfg->event.event_a1.eutra.range);
        }
        gen_report |= process_event(&cfg->event, tti, enter_condition, exit_condition,
                                        &m->second, &m->second.cell_values[serving_cell_idx]);

      // Rest are evaluated for every cell in frequency
      } else {
        meas_obj_t *obj = &objects[m->second.object_id];
        for (std::map<uint32_t, meas_cell_t>::iterator cell = obj->cells.begin(); cell != obj->cells.end(); ++cell) {
          float Ofn = obj->q_offset;
          float Ocn = cell->second.q_offset;
          float Mn = m->second.cell_values[cell->second.pci].ms[cfg->trigger_quantity];
          float Off=0, th=0, th1=0, th2=0;
          bool enter_condition = false;
          bool exit_condition  = false;
          switch (event_id) {
            case LIBLTE_RRC_EVENT_ID_EUTRA_A3:
              Off = 0.5*cfg->event.event_a3.offset;
              enter_condition = Mn + Ofn + Ocn - hyst > Mp + Ofp + Ocp + Off;
              exit_condition  = Mn + Ofn + Ocn + hyst < Mp + Ofp + Ocp + Off;
              break;
            case LIBLTE_RRC_EVENT_ID_EUTRA_A4:
              th = range_to_value(cfg->trigger_quantity, cfg->event.event_a4.eutra.range);
              enter_condition = Mn + Ofn + Ocn - hyst > th;
              exit_condition  = Mn + Ofn + Ocn + hyst < th;
              break;
            case LIBLTE_RRC_EVENT_ID_EUTRA_A5:
              th1 = range_to_value(cfg->trigger_quantity, cfg->event.event_a5.eutra1.range);
              th2 = range_to_value(cfg->trigger_quantity, cfg->event.event_a5.eutra2.range);
              enter_condition = (Mp + hyst < th1) && (Mn + Ofn + Ocn - hyst > th2);
              exit_condition  = (Mp - hyst > th1) && (Mn + Ofn + Ocn + hyst < th2);
              break;
            default:
              log_h->error("Error event %s not implemented\n", event_str);
          }
          gen_report |= process_event(&cfg->event, tti, enter_condition, exit_condition,
                                      &m->second, &m->second.cell_values[cell->second.pci]);
        }
      }
    }
    if (gen_report) {
      generate_report(m->first);
    }
  }
}

// Procedure upon handover or reestablishment 5.5.6.1
void rrc::rrc_meas::ho_finish() {
  // Remove all measId with trigger periodic
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (reports_cfg[iter->second.report_id].trigger_type == report_cfg_t::PERIODIC) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }

  //TODO: Inter-frequency handover

  // Stop all reports
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    stop_reports(&iter->second);
  }
}

// 5.5.4.1 expiry of periodical reporting timer
bool rrc::rrc_meas::timer_expired(uint32_t timer_id) {
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    if (iter->second.periodic_timer == timer_id) {
      generate_report(iter->first);
      return true;
    }
  }
  return false;
}

void rrc::rrc_meas::stop_reports(meas_t *m) {
  mac_timers->timer_get(m->periodic_timer)->stop();
  m->triggered = false;
}

void rrc::rrc_meas::stop_reports_object(uint32_t object_id) {
  for (std::map<uint32_t, meas_t>::iterator iter = active.begin(); iter != active.end(); ++iter) {
    if (iter->second.object_id == object_id) {
      stop_reports(&iter->second);
    }
  }
}

void rrc::rrc_meas::remove_meas_object(uint32_t object_id) {
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (iter->second.object_id == object_id) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }
}

void rrc::rrc_meas::remove_meas_report(uint32_t report_id) {
  std::map<uint32_t, meas_t>::iterator iter = active.begin();
  while (iter != active.end()) {
    if (iter->second.report_id == report_id) {
      remove_meas_id(iter++);
    } else {
      ++iter;
    }
  }
}

void rrc::rrc_meas::remove_meas_id(uint32_t measId) {
  if (active.count(measId)) {
    mac_timers->timer_get(active[measId].periodic_timer)->stop();
    mac_timers->timer_release_id(active[measId].periodic_timer);
    log_h->info("MEAS: Removed measId=%d\n", measId);
    active.erase(measId);
  } else {
    log_h->warning("MEAS: Removing unexistent measId=%d\n", measId);
  }
}

void rrc::rrc_meas::remove_meas_id(std::map<uint32_t, meas_t>::iterator it) {
  mac_timers->timer_get(it->second.periodic_timer)->stop();
  mac_timers->timer_release_id(it->second.periodic_timer);
  log_h->info("MEAS: Removed measId=%d\n", it->first);
  active.erase(it);
}

/* Parses MeasConfig object from RRCConnectionReconfiguration message and applies configuration
 * as per section 5.5.2
 */
void rrc::rrc_meas::parse_meas_config(LIBLTE_RRC_MEAS_CONFIG_STRUCT *cfg)
{

  // Measurement object removal 5.5.2.4
  for (uint32_t i=0;i<cfg->N_meas_obj_to_remove;i++) {
    objects.erase(cfg->meas_obj_to_remove_list[i]);
    remove_meas_object(cfg->meas_obj_to_remove_list[i]);
    log_h->info("MEAS: Removed measObjectId=%d\n", cfg->meas_obj_to_remove_list[i]);
  }

  // Measurement object addition/modification Section 5.5.2.5
  if (cfg->meas_obj_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->meas_obj_to_add_mod_list.N_meas_obj;i++) {
      if (cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_type == LIBLTE_RRC_MEAS_OBJECT_TYPE_EUTRA) {
        LIBLTE_RRC_MEAS_OBJECT_EUTRA_STRUCT *src_obj = &cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_eutra;

        // Access the object if exists or create it
        meas_obj_t *dst_obj = &objects[cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id];

        dst_obj->earfcn   = src_obj->carrier_freq;;
        if (src_obj->offset_freq_not_default) {
          dst_obj->q_offset = liblte_rrc_q_offset_range_num[src_obj->offset_freq];
        } else {
          dst_obj->q_offset = 0;
        }

        if (src_obj->black_cells_to_remove_list_present) {
          for (uint32_t j=0;j<src_obj->black_cells_to_remove_list.N_cell_idx;j++) {
            dst_obj->cells.erase(src_obj->black_cells_to_remove_list.cell_idx[j]);
          }
        }

        for (uint32_t j=0;j<src_obj->N_cells_to_add_mod;j++) {
          dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].q_offset = liblte_rrc_q_offset_range_num[src_obj->cells_to_add_mod_list[j].cell_offset];
          dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].pci      = src_obj->cells_to_add_mod_list[j].pci;

          log_h->info("MEAS: Added measObjectId=%d, earfcn=%d, q_offset=%f, pci=%d, offset_cell=%f\n",
                      cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id, dst_obj->earfcn, dst_obj->q_offset,
                      dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].q_offset,
                      dst_obj->cells[src_obj->cells_to_add_mod_list[j].cell_idx].pci);

        }

        // Untrigger reports and stop timers
        stop_reports_object(cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_id);

        // TODO: Blackcells
        // TODO: meassubframepattern

      } else {
        log_h->warning("MEAS: Unsupported MeasObject type %s\n",
                       liblte_rrc_meas_object_type_text[cfg->meas_obj_to_add_mod_list.meas_obj_list[i].meas_obj_type]);
      }
    }
  }

  // Reporting configuration removal 5.5.2.6
  for (uint32_t i=0;i<cfg->N_rep_cnfg_to_remove;i++) {
    reports_cfg.erase(cfg->rep_cnfg_to_remove_list[i]);
    remove_meas_report(cfg->rep_cnfg_to_remove_list[i]);
    log_h->info("MEAS: Removed reportConfigId=%d\n", cfg->rep_cnfg_to_remove_list[i]);
  }

  // Reporting configuration addition/modification 5.5.2.7
  if (cfg->rep_cnfg_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->rep_cnfg_to_add_mod_list.N_rep_cnfg;i++) {
      if (cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_type == LIBLTE_RRC_REPORT_CONFIG_TYPE_EUTRA) {
        LIBLTE_RRC_REPORT_CONFIG_EUTRA_STRUCT *src_rep = &cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_eutra;
        // Access the object if exists or create it
        report_cfg_t *dst_rep = &reports_cfg[cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id];

        dst_rep->trigger_type = src_rep->trigger_type==LIBLTE_RRC_TRIGGER_TYPE_EUTRA_EVENT?report_cfg_t::EVENT:report_cfg_t::PERIODIC;
        dst_rep->event    = src_rep->event;
        dst_rep->amount   = liblte_rrc_report_amount_num[src_rep->report_amount];
        dst_rep->interval = liblte_rrc_report_interval_num[src_rep->report_interval];
        dst_rep->max_cell = src_rep->max_report_cells;
        dst_rep->trigger_quantity = (quantity_t) src_rep->trigger_quantity;
        dst_rep->report_quantity  = src_rep->report_quantity==LIBLTE_RRC_REPORT_QUANTITY_SAME_AS_TRIGGER_QUANTITY?dst_rep->trigger_quantity:BOTH;

        log_h->info("MEAS: Added reportConfigId=%d, event=%s, amount=%d, interval=%d\n",
                    cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id,
                    liblte_rrc_event_id_eutra_text[dst_rep->event.event_id],
                    dst_rep->amount, dst_rep->interval);

        // Reset reports counter
        for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
          if (iter->second.report_id == cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_id) {
            iter->second.nof_reports_sent = 0;
            stop_reports(&iter->second);
          }
        }
      } else {
        log_h->warning("MEAS: Unsupported reportConfigType %s\n", liblte_rrc_report_config_type_text[cfg->rep_cnfg_to_add_mod_list.rep_cnfg_list[i].rep_cnfg_type]);
      }
    }
  }

  // Quantity configuration 5.5.2.8
  if (cfg->quantity_cnfg_present && cfg->quantity_cnfg.qc_eutra_present) {
    if (cfg->quantity_cnfg.qc_eutra.fc_rsrp_not_default) {
      filter_k_rsrp = liblte_rrc_filter_coefficient_num[cfg->quantity_cnfg.qc_eutra.fc_rsrp];
    } else {
      filter_k_rsrp = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
    }
    if (cfg->quantity_cnfg.qc_eutra.fc_rsrq_not_default) {
      filter_k_rsrq = liblte_rrc_filter_coefficient_num[cfg->quantity_cnfg.qc_eutra.fc_rsrq];
    } else {
      filter_k_rsrq = liblte_rrc_filter_coefficient_num[LIBLTE_RRC_FILTER_COEFFICIENT_FC4];
    }
    filter_a[RSRP] = pow(0.5, (float) filter_k_rsrp/4);
    filter_a[RSRQ] = pow(0.5, (float) filter_k_rsrq/4);

    log_h->info("MEAS: Quantity configuration k_rsrp=%d, k_rsrq=%d\n", filter_k_rsrp, filter_k_rsrq);
  }

  // Measurement identity removal 5.5.2.2
  for (uint32_t i=0;i<cfg->N_meas_id_to_remove;i++) {
    remove_meas_id(cfg->meas_id_to_remove_list[i]);
  }

  // Measurement identity addition/modification 5.5.2.3
  if (cfg->meas_id_to_add_mod_list_present) {
    for (uint32_t i=0;i<cfg->meas_id_to_add_mod_list.N_meas_id;i++) {
      LIBLTE_RRC_MEAS_ID_TO_ADD_MOD_STRUCT *measId = &cfg->meas_id_to_add_mod_list.meas_id_list[i];
      // Stop the timer if the entry exists or create the timer if not
      bool is_new = false;
      if (active.count(measId->meas_id)) {
        mac_timers->timer_get(active[measId->meas_id].periodic_timer)->stop();
      } else {
        is_new = true;
        active[measId->meas_id].periodic_timer   = mac_timers->timer_get_unique_id();
      }
      active[measId->meas_id].object_id = measId->meas_obj_id;
      active[measId->meas_id].report_id = measId->rep_cnfg_id;
      log_h->info("MEAS: %s measId=%d, measObjectId=%d, reportConfigId=%d\n",
                  is_new?"Added":"Updated", measId->meas_id, measId->meas_obj_id, measId->rep_cnfg_id);
    }
  }

  // S-Measure
  if (cfg->s_meas_present) {
    if (cfg->s_meas) {
      s_measure_enabled = true;
      s_measure_value   = range_to_value(RSRP, cfg->s_meas);
    } else {
      s_measure_enabled = false;
    }
  }

  update_phy();
}

/* Instruct PHY to start measurement */
void rrc::rrc_meas::update_phy()
{
  phy->meas_reset();
  for(std::map<uint32_t, meas_t>::iterator iter=active.begin(); iter!=active.end(); ++iter) {
    meas_t m = iter->second;
    meas_obj_t o = objects[m.object_id];
    // Instruct PHY to look for neighbour cells on this frequency
    phy->meas_start(o.earfcn);
    for(std::map<uint32_t, meas_cell_t>::iterator iter=o.cells.begin(); iter!=o.cells.end(); ++iter) {
      // Instruct PHY to look for cells IDs on this frequency
      phy->meas_start(o.earfcn, iter->second.pci);
    }
  }
}


uint8_t rrc::rrc_meas::value_to_range(quantity_t quant, float value) {
  uint8_t range = 0;
  switch(quant) {
    case RSRP:
      if (value < -140) {
        range = 0;
      } else if (-140 <= value && value < -44) {
        range = 1 + (uint8_t) (value + 140);
      } else {
        range = 97;
      }
      break;
    case RSRQ:
      if (value < -19.5) {
        range = 0;
      } else if (-19.5 <= value && value < -3) {
        range = 1 + (uint8_t) (2*(value + 19.5));
      } else {
        range = 34;
      }
      break;
    case BOTH:
      printf("Error quantity both not supported in value_to_range\n");
      break;
  }
  return range;
}

float rrc::rrc_meas::range_to_value(quantity_t quant, uint8_t range) {
  float val = 0;
  switch(quant) {
    case RSRP:
      val = -140+(float) range;
      break;
    case RSRQ:
      val = -19.5+(float) range/2;
      break;
    case BOTH:
      printf("Error quantity both not supported in range_to_value\n");
      break;
  }
  return val;
}

const std::string rrc::rb_id_str[] = {"SRB0", "SRB1", "SRB2",
                                      "DRB1", "DRB2", "DRB3",
                                      "DRB4", "DRB5", "DRB6",
                                      "DRB7", "DRB8"};

} // namespace srsue
