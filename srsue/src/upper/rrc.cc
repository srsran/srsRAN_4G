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
#include <sstream>
#include "srslte/asn1/liblte_rrc.h"
#include "upper/rrc.h"
#include <boost/assign.hpp>
#include <upper/rrc.h>
#include <srslte/asn1/liblte_rrc.h>
#include "srslte/common/security.h"
#include "srslte/common/bcd_helpers.h"

#define TIMEOUT_RESYNC_REESTABLISH 100

using namespace srslte;

namespace srsue {


/*******************************************************************************
  Base functions 
*******************************************************************************/

rrc::rrc()
  :state(RRC_STATE_IDLE)
  ,drb_up(false)
{
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

  ue_category = SRSLTE_UE_CATEGORY;
  t301 = mac_timers->timer_get_unique_id();
  t310 = mac_timers->timer_get_unique_id();
  t311 = mac_timers->timer_get_unique_id();

  transaction_id = 0;

  // Register logging handler with liblte_rrc
  liblte_rrc_log_register_handler(this, liblte_rrc_handler);

  nof_sib1_trials = 0;
  last_win_start  = 0;

  // Set default values for all layers
  set_rrc_default();
  set_phy_default();
  set_mac_default();
}

void rrc::stop() {
  thread_running = false;
  wait_thread_finish();
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

void rrc::set_ue_category(int category) {
  if (category >= 1 && category <= 5) {
    ue_category = category;
  } else {
    rrc_log->error("Unsupported UE category %d\n", category);
  }
}

/*
 *
 * RRC State Machine
 *
 */
void rrc::run_thread() {

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
            plmn_select(selected_plmn_id);
          }
        // If not camping on a cell
        } else {
          // If NAS is attached, perform cell reselection on current PLMN
          if (nas->is_attached()) {
            rrc_log->info("RRC IDLE: NAS is attached, PHY not synchronized. Re-selecting cell...\n");
            plmn_select(selected_plmn_id);
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
        if (phy->sync_status()) {
          if (!current_cell->has_valid_sib1) {
            si_acquire_state = SI_ACQUIRE_SIB1;
          } else if (!current_cell->has_valid_sib2) {
            si_acquire_state = SI_ACQUIRE_SIB2;
          } else {
            apply_sib2_configs(&current_cell->sib2);
            si_acquire_state = SI_ACQUIRE_IDLE;
            state = RRC_STATE_CELL_SELECTED;
          }
        }
        select_cell_timeout++;
        if (select_cell_timeout >= RRC_SELECT_CELL_TIMEOUT) {
          rrc_log->info("RRC Cell Selecting: timeout expired. Starting Cell Search...\n");
          state = RRC_STATE_PLMN_SELECTION;
          plmn_select_timeout = 0;
          phy->cell_search_start();
        }
        break;
      case RRC_STATE_CELL_SELECTED:
        rrc_log->info("RRC Cell Selected: Sending connection request...\n");
        send_con_request();
        state = RRC_STATE_CONNECTING;
        connecting_timeout = 0;
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
        // Take measurements, cell reselection, etc
        break;
      case RRC_STATE_LEAVE_CONNECTED:
        usleep(60000);
        rrc_log->info("Leaving RRC_CONNECTED state\n");
        drb_up = false;
        pdcp->reset();
        rlc->reset();
        phy->reset();
        mac->reset();
        set_phy_default();
        set_mac_default();
        mac->pcch_start_rx();
        mac_timers->timer_get(t311)->run();
        mac_timers->timer_get(t310)->stop();
        mac_timers->timer_get(t311)->stop();
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
uint32_t rrc::sib_start_tti(uint32_t tti, uint32_t period, uint32_t x) {
  return (period * 10 * (1 + tti / (period * 10)) + x) % 10240; // the 1 means next opportunity
}

void rrc::run_si_acquisition_procedure()
{
  uint32_t tti;
  uint32_t si_win_start=0, si_win_len=0;
  uint16_t period;
  const int SIB1_SEARCH_TIMEOUT = 30;

  switch (si_acquire_state) {
    case SI_ACQUIRE_SIB1:
      // Instruct MAC to look for SIB1
      tti = mac->get_current_tti();
      si_win_start = sib_start_tti(tti, 2, 5);
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
      // Instruct MAC to look for SIB2 only when selecting a cell
      tti = mac->get_current_tti();
      period = liblte_rrc_si_periodicity_num[current_cell->sib1.sched_info[0].si_periodicity];
      si_win_start = sib_start_tti(tti, period, 0);
      if (tti > last_win_start + 10) {
        last_win_start = si_win_start;
        si_win_len = liblte_rrc_si_window_length_num[current_cell->sib1.si_window_length];

        mac->bcch_start_rx(si_win_start, si_win_len);
        rrc_log->debug("Instructed MAC to search for SIB2, win_start=%d, win_len=%d\n",
                       si_win_start, si_win_len);
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
  if (current_cell) {
    if (current_cell->sib1.N_plmn_ids > 0) {
      return current_cell->sib1.plmn_id[0].id.mcc;
    }
  }
  return 0;
}

uint16_t rrc::get_mnc() {
  if (current_cell) {
    if (current_cell->sib1.N_plmn_ids > 0) {
      return current_cell->sib1.plmn_id[0].id.mnc;
    }
  }
  return 0;
}

void rrc::plmn_search() {
  rrc_log->info("Starting PLMN search procedure\n");
  state = RRC_STATE_PLMN_SELECTION;
  phy->cell_search_start();
  plmn_select_timeout = 0;
}

void rrc::plmn_select(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {

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
      last_selected_cell = -1;
      select_cell_timeout = 0;

      state = RRC_STATE_CELL_SELECTING;
      select_next_cell_in_plmn();
    }
  } else {
    rrc_log->warning("Requested PLMN select in incorrect state %s\n", rrc_state_text[state]);
  }
}

void rrc::select_next_cell_in_plmn() {
  for (uint32_t i = last_selected_cell + 1; i < known_cells.size(); i++) {
    for (uint32_t j = 0; j < known_cells[i].sib1.N_plmn_ids; j++) {
      if (known_cells[i].sib1.plmn_id[j].id.mcc == selected_plmn_id.mcc ||
          known_cells[i].sib1.plmn_id[j].id.mnc == selected_plmn_id.mnc) {
        rrc_log->info("Selecting cell PCI=%d, EARFCN=%d, Cell ID=0x%x\n",
                      known_cells[i].phy_cell.id, known_cells[i].earfcn,
                      known_cells[i].sib1.cell_id);
        rrc_log->console("Select cell: PCI=%d, EARFCN=%d, Cell ID=0x%x\n",
                      known_cells[i].phy_cell.id, known_cells[i].earfcn,
                      known_cells[i].sib1.cell_id);
        // Check that cell satisfies S criteria
        if (known_cells[i].in_sync) { // %% rsrp > S dbm
          // Try to select Cell
          if (phy->cell_select(known_cells[i].earfcn, known_cells[i].phy_cell))
          {
            last_selected_cell = i;
            current_cell = &known_cells[i];
            return;
          } else {
            rrc_log->warning("Selecting cell EARFCN=%d, Cell ID=0x%x.\n",
                           known_cells[i].earfcn, known_cells[i].sib1.cell_id);
          }
        }
      }
    }
  }
  rrc_log->info("No more known cells...\n");
}

void rrc::cell_found(uint32_t earfcn, srslte_cell_t phy_cell, float rsrp) {

  // find if cell_id-earfcn combination already exists
  for (uint32_t i = 0; i < known_cells.size(); i++) {
    if (earfcn == known_cells[i].earfcn && phy_cell.id == known_cells[i].phy_cell.id) {
      known_cells[i].rsrp    = rsrp;
      known_cells[i].in_sync = true;
      current_cell = &known_cells[i];
      rrc_log->info("Updating cell EARFCN=%d, PCI=%d, RSRP=%.1f dBm\n", known_cells[i].earfcn,
                    known_cells[i].phy_cell.id, known_cells[i].rsrp);

      if (!known_cells[i].has_valid_sib1) {
        si_acquire_state = SI_ACQUIRE_SIB1;
      } else if (state == RRC_STATE_PLMN_SELECTION) {
        for (uint32_t i = 0; i < current_cell->sib1.N_plmn_ids; i++) {
          nas->plmn_found(current_cell->sib1.plmn_id[i].id, current_cell->sib1.tracking_area_code);
        }
        usleep(5000);
        phy->cell_search_next();
      }
      return;
    }
  }
  // add to list of known cells
  cell_t cell;
  cell.phy_cell = phy_cell;
  cell.rsrp = rsrp;
  cell.earfcn = earfcn;
  cell.has_valid_sib1 = false;
  cell.has_valid_sib2 = false;
  known_cells.push_back(cell);

  // save current cell
  current_cell = &known_cells.back();

  si_acquire_state = SI_ACQUIRE_SIB1;

  rrc_log->info("New Cell: PCI=%d, PRB=%d, Ports=%d, EARFCN=%d, RSRP=%.1f dBm\n",
                cell.phy_cell.id, cell.phy_cell.nof_prb, cell.phy_cell.nof_ports,
                cell.earfcn, cell.rsrp);
}

// PHY indicates that has gone through all known EARFCN
void rrc::earfcn_end() {
  rrc_log->debug("Finished searching cells in EARFCN set while in state %s\n", rrc_state_text[state]);

  // If searching for PLMN, indicate NAS we scanned all frequencies
  if (state == RRC_STATE_PLMN_SELECTION) {
    nas->plmn_search_end();
  }
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

// Detection of physical layer problems (5.3.11.1)
void rrc::out_of_sync() {
    current_cell->in_sync = false;
  if (!mac_timers->timer_get(t311)->is_running() && !mac_timers->timer_get(t310)->is_running()) {
    n310_cnt++;
    if (n310_cnt == N310) {
      // attempt resync
      phy->sync_reset();

      mac_timers->timer_get(t310)->reset();
      mac_timers->timer_get(t310)->run();
      n310_cnt = 0;
      rrc_log->info("Detected %d out-of-sync from PHY. Starting T310 timer\n", N310);
    }
  }
}

// Recovery of physical layer problems (5.3.11.2)
void rrc::in_sync() {
  current_cell->in_sync = true;
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
    send_con_restablish_request();
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
    rrc_log->info("Timer T301 expired: Going to RRC IDLE\n");
    state = RRC_STATE_LEAVE_CONNECTED;
  } else {
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
  LIBLTE_RRC_UL_CCCH_MSG_STRUCT ul_ccch_msg;
  LIBLTE_RRC_S_TMSI_STRUCT s_tmsi;

  // Prepare ConnectionRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ;
  if (nas->get_s_tmsi(&s_tmsi)) {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI;
    ul_ccch_msg.msg.rrc_con_req.ue_id.s_tmsi = s_tmsi;
  } else {
    ul_ccch_msg.msg.rrc_con_req.ue_id_type = LIBLTE_RRC_CON_REQ_UE_ID_TYPE_RANDOM_VALUE;
    ul_ccch_msg.msg.rrc_con_req.ue_id.random = 1000;
  }
  ul_ccch_msg.msg.rrc_con_req.cause = LIBLTE_RRC_CON_REQ_EST_CAUSE_MO_SIGNALLING;
  liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  byte_buffer_t *pdcp_buf = pool_allocate;;
  srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
  pdcp_buf->N_bytes = bit_buf.N_bits / 8;
  pdcp_buf->set_timestamp();

  // Set UE contention resolution ID in MAC
  uint64_t uecri = 0;
  uint8_t *ue_cri_ptr = (uint8_t *) &uecri;
  uint32_t nbytes = 6;
  for (uint32_t i = 0; i < nbytes; i++) {
    ue_cri_ptr[nbytes - i - 1] = pdcp_buf->msg[i];
  }
  rrc_log->debug("Setting UE contention resolution ID: %d\n", uecri);

  mac->set_contention_id(uecri);

  rrc_log->info("Sending RRC Connection Request on SRB0\n");
  pdcp->write_sdu(RB_ID_SRB0, pdcp_buf);
}

/* RRC connection re-establishment procedure (5.3.7) */
void rrc::send_con_restablish_request() {

  srslte_cell_t cell;
  phy->get_current_cell(&cell);

  LIBLTE_RRC_UL_CCCH_MSG_STRUCT ul_ccch_msg;
  LIBLTE_RRC_S_TMSI_STRUCT s_tmsi;

  // Compute shortMAC-I
  uint8_t varShortMAC[128], varShortMAC_packed[16];
  bzero(varShortMAC, 128);
  bzero(varShortMAC_packed, 16);
  uint8_t *msg_ptr = varShortMAC;
  liblte_rrc_pack_cell_identity_ie(0x1a2d0, &msg_ptr);
  liblte_rrc_pack_phys_cell_id_ie(cell.id, &msg_ptr);
  mac_interface_rrc::ue_rnti_t ue_rnti;
  mac->get_rntis(&ue_rnti);
  liblte_rrc_pack_c_rnti_ie(ue_rnti.crnti, &msg_ptr);
  srslte_bit_pack_vector(varShortMAC, varShortMAC_packed, msg_ptr - varShortMAC);

  uint8_t mac_key[4];
  security_128_eia2(&k_rrc_int[16],
                    1,
                    1,
                    1,
                    varShortMAC_packed,
                    7,
                    mac_key);

  mac_interface_rrc::ue_rnti_t uernti;
  mac->get_rntis(&uernti);

  // Prepare ConnectionRestalishmentRequest packet
  ul_ccch_msg.msg_type = LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REEST_REQ;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.c_rnti = uernti.crnti;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.phys_cell_id = cell.id;
  ul_ccch_msg.msg.rrc_con_reest_req.ue_id.short_mac_i = mac_key[2] << 8 | mac_key[3];
  ul_ccch_msg.msg.rrc_con_reest_req.cause = LIBLTE_RRC_CON_REEST_REQ_CAUSE_OTHER_FAILURE;
  liblte_rrc_pack_ul_ccch_msg(&ul_ccch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  rrc_log->info("Initiating RRC Connection Reestablishment Procedure\n");
  rrc_log->console("RRC Connection Reestablishment\n");
  mac_timers->timer_get(t310)->stop();
  mac_timers->timer_get(t311)->reset();
  mac_timers->timer_get(t311)->run();

  phy->reset();
  set_phy_default();
  mac->reset();
  set_mac_default();

  // FIXME: Cell selection should be different??

  // Wait for cell re-synchronization
  uint32_t timeout_cnt = 0;
  while (!phy->sync_status() && timeout_cnt < TIMEOUT_RESYNC_REESTABLISH) {
    usleep(10000);
    timeout_cnt++;
  }
  mac_timers->timer_get(t301)->reset();
  mac_timers->timer_get(t301)->run();
  mac_timers->timer_get(t311)->stop();
  rrc_log->info("Cell Selection finished. Initiating transmission of RRC Connection Reestablishment Request\n");

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  byte_buffer_t *pdcp_buf = pool_allocate;;
  srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
  pdcp_buf->N_bytes = bit_buf.N_bits / 8;

  // Set UE contention resolution ID in MAC
  uint64_t uecri = 0;
  uint8_t *ue_cri_ptr = (uint8_t *) &uecri;
  uint32_t nbytes = 6;
  for (uint32_t i = 0; i < nbytes; i++) {
    ue_cri_ptr[nbytes - i - 1] = pdcp_buf->msg[i];
  }
  rrc_log->debug("Setting UE contention resolution ID: %d\n", uecri);
  mac->set_contention_id(uecri);

  rrc_log->info("Sending RRC Connection Reestablishment Request on SRB0\n");
  pdcp->write_sdu(RB_ID_SRB0, pdcp_buf);
}

void rrc::send_con_restablish_complete() {
  rrc_log->debug("Preparing RRC Connection Reestablishment Complete\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_REEST_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reest_complete.rrc_transaction_id = transaction_id;
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  byte_buffer_t *pdcp_buf = pool_allocate;;
  srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
  pdcp_buf->N_bytes = bit_buf.N_bits / 8;

  state = RRC_STATE_CONNECTED;
  rrc_log->console("RRC Connected\n");
  rrc_log->info("Sending RRC Connection Reestablishment Complete\n");
  pdcp->write_sdu(RB_ID_SRB1, pdcp_buf);
}

void rrc::send_con_setup_complete(byte_buffer_t *nas_msg) {
  rrc_log->debug("Preparing RRC Connection Setup Complete\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;

  // Prepare ConnectionSetupComplete packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE;
  ul_dcch_msg.msg.rrc_con_setup_complete.registered_mme_present = false;
  ul_dcch_msg.msg.rrc_con_setup_complete.rrc_transaction_id = transaction_id;
  ul_dcch_msg.msg.rrc_con_setup_complete.selected_plmn_id = 1;
  memcpy(ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.msg, nas_msg->msg, nas_msg->N_bytes);
  ul_dcch_msg.msg.rrc_con_setup_complete.dedicated_info_nas.N_bytes = nas_msg->N_bytes;
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  byte_buffer_t *pdcp_buf = pool_allocate;;
  srslte_bit_pack_vector(bit_buf.msg, pdcp_buf->msg, bit_buf.N_bits);
  pdcp_buf->N_bytes = bit_buf.N_bits / 8;
  pdcp_buf->set_timestamp();

  state = RRC_STATE_CONNECTED;
  rrc_log->console("RRC Connected\n");
  rrc_log->info("Sending RRC Connection Setup Complete\n");
  pdcp->write_sdu(RB_ID_SRB1, pdcp_buf);
}

void rrc::send_ul_info_transfer(uint32_t lcid, byte_buffer_t *sdu) {
  rrc_log->debug("Preparing RX Info Transfer\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;

  // Prepare RX INFO packet
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER;
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info_type = LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS;
  memcpy(ul_dcch_msg.msg.ul_info_transfer.dedicated_info.msg, sdu->msg, sdu->N_bytes);
  ul_dcch_msg.msg.ul_info_transfer.dedicated_info.N_bytes = sdu->N_bytes;
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Reset and reuse sdu buffer
  byte_buffer_t *pdu = sdu;
  pdu->reset();

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  srslte_bit_pack_vector(bit_buf.msg, pdu->msg, bit_buf.N_bits);
  pdu->N_bytes = bit_buf.N_bits / 8;
  pdu->set_timestamp();
  pdu->set_timestamp();

  rrc_log->info("Sending RX Info Transfer\n");
  pdcp->write_sdu(lcid, pdu);
}

void rrc::send_security_mode_complete(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->debug("Preparing Security Mode Complete\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;
  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_SECURITY_MODE_COMPLETE;
  ul_dcch_msg.msg.security_mode_complete.rrc_transaction_id = transaction_id;
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  srslte_bit_pack_vector(bit_buf.msg, pdu->msg, bit_buf.N_bits);
  pdu->N_bytes = bit_buf.N_bits / 8;
  pdu->set_timestamp();

  rrc_log->info("Sending Security Mode Complete\n");
  pdcp->write_sdu(lcid, pdu);
}

void rrc::send_rrc_con_reconfig_complete(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->debug("Preparing RRC Connection Reconfig Complete\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_RRC_CON_RECONFIG_COMPLETE;
  ul_dcch_msg.msg.rrc_con_reconfig_complete.rrc_transaction_id = transaction_id;
  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  srslte_bit_pack_vector(bit_buf.msg, pdu->msg, bit_buf.N_bits);
  pdu->N_bytes = bit_buf.N_bits / 8;
  pdu->set_timestamp();

  rrc_log->info("Sending RRC Connection Reconfig Complete\n");
  pdcp->write_sdu(lcid, pdu);
}


void rrc::handle_rrc_con_reconfig(uint32_t lcid, LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig,
                                  byte_buffer_t *pdu) {
  uint32_t i;

  if (reconfig->rr_cnfg_ded_present) {
    apply_rr_config_dedicated(&reconfig->rr_cnfg_ded);
  } else {
    printf("received con reconfig no rr confg present\n");
  }
  if (reconfig->meas_cnfg_present) {
    //TODO: handle meas_cnfg
  }
  if (reconfig->mob_ctrl_info_present) {
    //TODO: handle mob_ctrl_info
  }

  send_rrc_con_reconfig_complete(lcid, pdu);

  byte_buffer_t *nas_sdu;
  for (i = 0; i < reconfig->N_ded_info_nas; i++) {
    nas_sdu = pool_allocate;;
    memcpy(nas_sdu->msg, &reconfig->ded_info_nas_list[i].msg, reconfig->ded_info_nas_list[i].N_bytes);
    nas_sdu->N_bytes = reconfig->ded_info_nas_list[i].N_bytes;
    nas->write_pdu(lcid, nas_sdu);
  }
}

  /* Actions upon reception of RRCConnectionRelease 5.3.8.3 */
void rrc::rrc_connection_release() {
  // Save idleModeMobilityControlInfo, etc.
  state = RRC_STATE_LEAVE_CONNECTED;
  rrc_log->console("Received RRC Connection Release\n");
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
  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "BCCH DLSCH message received.");
  rrc_log->info("BCCH DLSCH message Stack latency: %ld us\n", pdu->get_latency_us());
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT dlsch_msg;
  srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes * 8);
  bit_buf.N_bits = pdu->N_bytes * 8;
  pool->deallocate(pdu);
  liblte_rrc_unpack_bcch_dlsch_msg((LIBLTE_BIT_MSG_STRUCT *) &bit_buf, &dlsch_msg);

  if (dlsch_msg.N_sibs > 0) {
    if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1 == dlsch_msg.sibs[0].sib_type && SI_ACQUIRE_SIB1 == si_acquire_state) {
      mac->bcch_stop_rx();

      // Handle SIB1
      memcpy(&current_cell->sib1, &dlsch_msg.sibs[0].sib.sib1, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT));

      rrc_log->info("SIB1 received, CellID=%d, si_window=%d, sib2_period=%d\n",
                    current_cell->sib1.cell_id & 0xfff,
                    liblte_rrc_si_window_length_num[current_cell->sib1.si_window_length],
                    liblte_rrc_si_periodicity_num[current_cell->sib1.sched_info[0].si_periodicity]);


      // Set TDD Config
      if (current_cell->sib1.tdd) {
        phy->set_config_tdd(&current_cell->sib1.tdd_cnfg);
      }

      current_cell->has_valid_sib1 = true;

      // Send PLMN and TAC to NAS
      std::stringstream ss;
      for (uint32_t i = 0; i < current_cell->sib1.N_plmn_ids; i++) {
        nas->plmn_found(current_cell->sib1.plmn_id[i].id, current_cell->sib1.tracking_area_code);
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

    } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2 == dlsch_msg.sibs[0].sib_type &&
               SI_ACQUIRE_SIB2 == si_acquire_state) {
      mac->bcch_stop_rx();

      // Handle SIB2
      memcpy(&current_cell->sib2, &dlsch_msg.sibs[0].sib.sib2, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT));
      rrc_log->info("SIB2 received\n");

      apply_sib2_configs(&current_cell->sib2);

      current_cell->has_valid_sib2 = true;

      // Jump to next state
      switch(state) {
        case RRC_STATE_CELL_SELECTING:
          si_acquire_state = SI_ACQUIRE_IDLE;
          state = RRC_STATE_CELL_SELECTED;
          break;
        default:
          si_acquire_state = SI_ACQUIRE_IDLE;
      }
    }
  }
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
      rrc_log->info("Received paging (%d/%d) for UE 0x%x\n", i + 1, pcch_msg.paging_record_list_size,
                    pcch_msg.paging_record_list[i].ue_identity.s_tmsi);
      rrc_log->console("Received paging (%d/%d) for UE 0x%x\n", i + 1, pcch_msg.paging_record_list_size,
                       pcch_msg.paging_record_list[i].ue_identity.s_tmsi);
      if (s_tmsi.mmec == s_tmsi_paged->mmec && s_tmsi.m_tmsi == s_tmsi_paged->m_tmsi) {
        rrc_log->info("S-TMSI match in paging message\n");
        rrc_log->console("S-TMSI match in paging message\n");
        mac->pcch_stop_rx();
        if (RRC_STATE_IDLE == state) {
          rrc_log->info("RRC in IDLE state - sending connection request.\n");
          state = RRC_STATE_CELL_SELECTING;
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
*
*******************************************************************************/
void rrc::write_sdu(uint32_t lcid, byte_buffer_t *sdu) {
  rrc_log->info_hex(sdu->msg, sdu->N_bytes, "RX %s SDU", get_rb_name(lcid).c_str());
  switch (state) {
    case RRC_STATE_CONNECTING:
      send_con_setup_complete(sdu);
      break;
    case RRC_STATE_CONNECTED:
      send_ul_info_transfer(lcid, sdu);
      break;
    default:
      rrc_log->error("SDU received from NAS while RRC state = %s", rrc_state_text[state]);
      break;
  }
}

void rrc::write_pdu(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->info_hex(pdu->msg, pdu->N_bytes, "TX %s PDU", get_rb_name(lcid).c_str());
  rrc_log->info("TX PDU Stack latency: %ld us\n", pdu->get_latency_us());

  switch (lcid) {
    case RB_ID_SRB0:
      parse_dl_ccch(pdu);
      break;
    case RB_ID_SRB1:
    case RB_ID_SRB2:
      parse_dl_dcch(lcid, pdu);
      break;
    default:
      rrc_log->error("TX PDU with invalid bearer id: %s", lcid);
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
      state = RRC_STATE_IDLE;
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

      // Configure PDCP for security
      usim->generate_as_keys(nas->get_ul_count(), k_rrc_enc, k_rrc_int, k_up_enc, k_up_int, cipher_algo, integ_algo);
      pdcp->config_security(lcid, k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
      send_security_mode_complete(lcid, pdu);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_RRC_CON_RECONFIG:
      transaction_id = dl_dcch_msg.msg.rrc_con_reconfig.rrc_transaction_id;
      handle_rrc_con_reconfig(lcid, &dl_dcch_msg.msg.rrc_con_reconfig, pdu);
      break;
    case LIBLTE_RRC_DL_DCCH_MSG_TYPE_UE_CAPABILITY_ENQUIRY:
      transaction_id = dl_dcch_msg.msg.ue_cap_enquiry.rrc_transaction_id;
      for (uint32_t i = 0; i < dl_dcch_msg.msg.ue_cap_enquiry.N_ue_cap_reqs; i++) {
        if (LIBLTE_RRC_RAT_TYPE_EUTRA == dl_dcch_msg.msg.ue_cap_enquiry.ue_capability_request[i]) {
          send_rrc_ue_cap_info(lcid, pdu);
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
  bool enable_ul_64 = ue_category >= 5 && current_cell->sib2.rr_config_common_sib.pusch_cnfg.enable_64_qam;
  rrc_log->info("%s 64QAM PUSCH\n", enable_ul_64 ? "Enabling" : "Disabling");
  phy->set_config_64qam_en(enable_ul_64);
}

void rrc::send_rrc_ue_cap_info(uint32_t lcid, byte_buffer_t *pdu) {
  rrc_log->debug("Preparing UE Capability Info\n");
  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;

  ul_dcch_msg.msg_type = LIBLTE_RRC_UL_DCCH_MSG_TYPE_UE_CAPABILITY_INFO;
  ul_dcch_msg.msg.ue_capability_info.rrc_transaction_id = transaction_id;

  LIBLTE_RRC_UE_CAPABILITY_INFORMATION_STRUCT *info = &ul_dcch_msg.msg.ue_capability_info;
  info->N_ue_caps = 1;
  info->ue_capability_rat[0].rat_type = LIBLTE_RRC_RAT_TYPE_EUTRA;

  LIBLTE_RRC_UE_EUTRA_CAPABILITY_STRUCT *cap = &info->ue_capability_rat[0].eutra_capability;
  cap->access_stratum_release = LIBLTE_RRC_ACCESS_STRATUM_RELEASE_REL8;
  cap->ue_category = ue_category;

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

  //TODO: Generate this from user input?
  cap->rf_params.N_supported_band_eutras = 3;
  cap->rf_params.supported_band_eutra[0].band_eutra = 3;
  cap->rf_params.supported_band_eutra[0].half_duplex = false;
  cap->rf_params.supported_band_eutra[1].band_eutra = 7;
  cap->rf_params.supported_band_eutra[1].half_duplex = false;
  cap->rf_params.supported_band_eutra[2].band_eutra = 20;
  cap->rf_params.supported_band_eutra[2].half_duplex = false;

  cap->meas_params.N_band_list_eutra = 3;
  cap->meas_params.band_list_eutra[0].N_inter_freq_need_for_gaps = 3;
  cap->meas_params.band_list_eutra[0].inter_freq_need_for_gaps[0] = true;
  cap->meas_params.band_list_eutra[0].inter_freq_need_for_gaps[1] = true;
  cap->meas_params.band_list_eutra[0].inter_freq_need_for_gaps[2] = true;
  cap->meas_params.band_list_eutra[1].N_inter_freq_need_for_gaps = 3;
  cap->meas_params.band_list_eutra[1].inter_freq_need_for_gaps[0] = true;
  cap->meas_params.band_list_eutra[1].inter_freq_need_for_gaps[1] = true;
  cap->meas_params.band_list_eutra[1].inter_freq_need_for_gaps[2] = true;
  cap->meas_params.band_list_eutra[2].N_inter_freq_need_for_gaps = 3;
  cap->meas_params.band_list_eutra[2].inter_freq_need_for_gaps[0] = true;
  cap->meas_params.band_list_eutra[2].inter_freq_need_for_gaps[1] = true;
  cap->meas_params.band_list_eutra[2].inter_freq_need_for_gaps[2] = true;

  cap->feature_group_indicator_present = true;
  cap->feature_group_indicator = 0x62001000;
  cap->inter_rat_params.utra_fdd_present = false;
  cap->inter_rat_params.utra_tdd128_present = false;
  cap->inter_rat_params.utra_tdd384_present = false;
  cap->inter_rat_params.utra_tdd768_present = false;
  cap->inter_rat_params.geran_present = false;
  cap->inter_rat_params.cdma2000_hrpd_present = false;
  cap->inter_rat_params.cdma2000_1xrtt_present = false;

  liblte_rrc_pack_ul_dcch_msg(&ul_dcch_msg, (LIBLTE_BIT_MSG_STRUCT *) &bit_buf);

  // Byte align and pack the message bits for PDCP
  if ((bit_buf.N_bits % 8) != 0) {
    for (uint32_t i = 0; i < 8 - (bit_buf.N_bits % 8); i++)
      bit_buf.msg[bit_buf.N_bits + i] = 0;
    bit_buf.N_bits += 8 - (bit_buf.N_bits % 8);
  }
  srslte_bit_pack_vector(bit_buf.msg, pdu->msg, bit_buf.N_bits);
  pdu->N_bytes = bit_buf.N_bits / 8;
  pdu->set_timestamp();

  rrc_log->info("Sending UE Capability Info\n");
  pdcp->write_sdu(lcid, pdu);
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

void rrc::apply_sib2_configs(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2) {

  // Apply RACH timeAlginmentTimer configuration
  mac_interface_rrc::mac_cfg_t cfg;
  mac->get_config(&cfg);

  cfg.main.time_alignment_timer = sib2->time_alignment_timer;
  memcpy(&cfg.rach, &sib2->rr_config_common_sib.rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT));
  cfg.prach_config_index = sib2->rr_config_common_sib.prach_cnfg.root_sequence_index;
  cfg.ul_harq_params.max_harq_msg3_tx = cfg.rach.max_harq_msg3_tx;

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
          phy_cnfg->antenna_info_explicit_value.tx_mode != LIBLTE_RRC_TRANSMISSION_MODE_2) {
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
  } else if (apply_defaults) {
    current_cfg->pdsch_cnfg_ded = LIBLTE_RRC_PDSCH_CONFIG_P_A_DB_0;
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

  // TODO: Restablish DRB1. Not done because never was suspended

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
  // TODO: setup PDCP security (using k_up_enc)

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


} // namespace srsue
