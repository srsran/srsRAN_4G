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

#include "srsue/hdr/stack/mac/proc_ra.h"
#include "srslte/common/log_helper.h"
#include "srsue/hdr/stack/mac/mux.h"
#include <inttypes.h> // for printing uint64_t
#include <stdint.h>
#include <stdlib.h>

/* Random access procedure as specified in Section 5.1 of 36.321 */

namespace srsue {

const char* state_str[] = {"RA:    INIT:   ",
                           "RA:    INIT:   ",
                           "RA:    PDCCH:  ",
                           "RA:    Rx:     ",
                           "RA:    Backoff: ",
                           "RA:    ConRes: ",
                           "RA:    WaitComplt: ",
                           "RA:    Complt: "};

#define rError(fmt, ...) Error("%s" fmt, state_str[state], ##__VA_ARGS__)
#define rInfo(fmt, ...) Info("%s" fmt, state_str[state], ##__VA_ARGS__)
#define rDebug(fmt, ...) Debug("%s" fmt, state_str[state], ##__VA_ARGS__)

// Table 7.2-1. Backoff Parameter values
uint32_t backoff_table[16] = {0, 10, 20, 30, 40, 60, 80, 120, 160, 240, 320, 480, 960, 960, 960, 960};

// Table 7.6-1: DELTA_PREAMBLE values.
int delta_preamble_db_table[5] = {0, 0, -3, -3, 8};

// Initializes memory and pointers to other objects
void ra_proc::init(phy_interface_mac_lte*               phy_h_,
                   rrc_interface_mac*                   rrc_,
                   srslte::log_ref                      log_h_,
                   mac_interface_rrc::ue_rnti_t*        rntis_,
                   srslte::timer_handler::unique_timer* time_alignment_timer_,
                   mux*                                 mux_unit_,
                   srslte::task_handler_interface*      stack_)
{
  phy_h    = phy_h_;
  log_h    = log_h_;
  rntis    = rntis_;
  mux_unit = mux_unit_;
  rrc      = rrc_;
  stack    = stack_;

  time_alignment_timer        = time_alignment_timer_;
  contention_resolution_timer = stack->get_unique_timer();

  srslte_softbuffer_rx_init(&softbuffer_rar, 10);

  reset();
}

ra_proc::~ra_proc()
{
  srslte_softbuffer_rx_free(&softbuffer_rar);
}

void ra_proc::reset()
{
  state            = IDLE;
  started_by_pdcch = false;
  contention_resolution_timer.stop();
}

void ra_proc::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_;
}

/* Sets a new configuration. The configuration is applied by initialization() function */
void ra_proc::set_config(srslte::rach_cfg_t& rach_cfg_)
{
  std::unique_lock<std::mutex> ul(mutex);
  new_cfg = rach_cfg_;
}

/* Reads the configuration and configures internal variables */
void ra_proc::read_params()
{
  mutex.lock();
  rach_cfg = new_cfg;
  mutex.unlock();

  // Read initialization parameters
  if (noncontention_enabled) {
    preambleIndex         = next_preamble_idx;
    maskIndex             = next_prach_mask;
    noncontention_enabled = false;
  } else {
    preambleIndex = 0; // pass when called from higher layers for non-contention based RA
    maskIndex     = 0; // same
  }

  if (rach_cfg.nof_groupA_preambles == 0) {
    rach_cfg.nof_groupA_preambles = rach_cfg.nof_preambles;
  }

  phy_interface_mac_lte::prach_info_t prach_info = phy_h->prach_get_info();
  delta_preamble_db                              = delta_preamble_db_table[prach_info.preamble_format % 5];

  if (rach_cfg.contentionResolutionTimer > 0) {
    contention_resolution_timer.set(rach_cfg.contentionResolutionTimer, [this](uint32_t tid) { timer_expired(tid); });
  }
}

/* Function called by MAC every TTI. Runs a state function until it changes to a different state
 */
void ra_proc::step(uint32_t tti_)
{
  switch (state) {
    case IDLE:
      break;
    case PDCCH_SETUP:
      state_pdcch_setup();
      break;
    case RESPONSE_RECEPTION:
      state_response_reception(tti_);
      break;
    case BACKOFF_WAIT:
      state_backoff_wait(tti_);
      break;
    case CONTENTION_RESOLUTION:
      state_contention_resolution();
      break;
    case START_WAIT_COMPLETION:
      state_completition();
      break;
    case WAITING_PHY_CONFIG:
    case WAITING_COMPLETION:
      // do nothing, bc we are waiting for the phy to finish
      break;
  }
}

/* Waits for PRACH to be transmitted by PHY. Once it's transmitted, configure RA-RNTI and wait for RAR reception
 */
void ra_proc::state_pdcch_setup()
{
  phy_interface_mac_lte::prach_info_t info = phy_h->prach_get_info();
  if (info.is_transmitted) {
    ra_tti  = info.tti_ra;
    ra_rnti = 1 + (ra_tti % 10) + info.f_id;
    rInfo("seq=%d, ra-rnti=0x%x, ra-tti=%d, f_id=%d\n", sel_preamble, ra_rnti, info.tti_ra, info.f_id);
    log_h->console("Random Access Transmission: seq=%d, ra-rnti=0x%x\n", sel_preamble, ra_rnti);
    rar_window_st   = ra_tti + 3;
    rntis->rar_rnti = ra_rnti;
    state           = RESPONSE_RECEPTION;
  } else {
    rDebug("preamble not yet transmitted\n");
  }
}

/* Waits for RAR reception. rar_received variable will be set by tb_decoded_ok() function which is called when a DL
 * TB assigned to RA-RNTI is received
 */
void ra_proc::state_response_reception(uint32_t tti)
{
  // do nothing. Processing done in tb_decoded_ok()
  if (!rar_received) {
    uint32_t interval = srslte_tti_interval(tti, ra_tti + 3 + rach_cfg.responseWindowSize - 1);
    if (interval > 0 && interval < 100) {
      Error("RA response not received within the response window\n");
      response_error();
    }
  }
}

/* Waits for given backoff interval to expire
 */
void ra_proc::state_backoff_wait(uint32_t tti)
{
  if (backoff_interval > 0) {
    // Backoff_interval = 0 is handled before entering here
    // When we arrive to this state, there is already 1 TTI delay
    if (backoff_interval == 1) {
      resource_selection();
    } else {
      // If it's the first time, save TTI
      if (backoff_interval_start == -1) {
        backoff_interval_start = tti;
        backoff_interval--;
      }
      if (srslte_tti_interval(tti, backoff_interval_start) >= backoff_interval) {
        backoff_interval = 0;
        resource_selection();
      }
    }
  }
}

/* Actions during contention resolution state as defined in 5.1.5
 * Resolution of the Contention is made by contention_resolution_id_received() and pdcch_to_crnti()
 */
void ra_proc::state_contention_resolution()
{
  // Once Msg3 is transmitted, start contention resolution timer
  if (mux_unit->msg3_is_transmitted() && !contention_resolution_timer.is_running()) {
    // Start contention resolution timer
    rInfo("Starting ContentionResolutionTimer=%d ms\n", contention_resolution_timer.duration());
    contention_resolution_timer.run();
  }
}

/* This step just configures the PHY to generate the C-RNTI. It is called from a state because it takes a long time to
 * compute
 */
void ra_proc::state_completition()
{
  state            = WAITING_COMPLETION;
  uint16_t rnti    = rntis->crnti;
  uint32_t task_id = current_task_id;
  stack->enqueue_background_task([this, rnti, task_id](uint32_t worker_id) {
    phy_h->set_crnti(rnti);
    // signal MAC RA proc to go back to idle
    notify_ra_completed(task_id);
  });
}

void ra_proc::notify_phy_config_completed(uint32_t task_id)
{
  if (current_task_id == task_id) {
    if (state != WAITING_PHY_CONFIG) {
      rError("Received unexpected notification of PHY configuration completed\n");
    } else {
      rDebug("RA waiting PHY configuration completed\n");
    }
    // Jump directly to Resource selection
    resource_selection();
  } else {
    rError("Received old notification of PHY configuration (old task_id=%d, current_task_id=%d)\n",
           task_id,
           current_task_id);
  }
}

void ra_proc::notify_ra_completed(uint32_t task_id)
{
  if (current_task_id == task_id) {
    if (state != WAITING_COMPLETION) {
      rError("Received unexpected notification of RA completion\n");
    } else {
      rInfo("RA waiting procedure completed\n");
    }
    state = IDLE;
  } else {
    rError("Received old notification of RA completition (old task_id=%d, current_task_id=%d)\n",
           task_id,
           current_task_id);
  }
}

/* RA procedure initialization as defined in 5.1.1 */
void ra_proc::initialization()
{
  read_params();
  current_task_id++;
  transmitted_contention_id   = 0;
  preambleTransmissionCounter = 1;
  mux_unit->msg3_flush();
  backoff_param_ms = 0;

  // Instruct phy to configure PRACH
  state            = WAITING_PHY_CONFIG;
  uint32_t task_id = current_task_id;
  stack->enqueue_background_task([this, task_id](uint32_t worker_id) {
    phy_h->configure_prach_params();
    // notify back MAC
    stack->notify_background_task_result([this, task_id]() { notify_phy_config_completed(task_id); });
  });
}

/* Resource selection as defined in 5.1.2 */
void ra_proc::resource_selection()
{
  ra_group_t sel_group;

  uint32_t nof_groupB_preambles = 0;
  if (rach_cfg.nof_groupA_preambles > 0) {
    nof_groupB_preambles = rach_cfg.nof_preambles - rach_cfg.nof_groupA_preambles;
  }

  if (preambleIndex > 0) {
    // Preamble is chosen by Higher layers (ie Network)
    sel_maskIndex = maskIndex;
    sel_preamble  = (uint32_t)preambleIndex;
  } else {
    // Preamble is chosen by MAC UE
    if (!mux_unit->msg3_is_transmitted()) {
      if (nof_groupB_preambles &&
          new_ra_msg_len > rach_cfg.messageSizeGroupA) { // Check also pathloss (Pcmax,deltaPreamble and powerOffset)
        sel_group = RA_GROUP_B;
      } else {
        sel_group = RA_GROUP_A;
      }
      last_msg3_group = sel_group;
    } else {
      sel_group = last_msg3_group;
    }
    if (sel_group == RA_GROUP_A) {
      if (rach_cfg.nof_groupA_preambles) {
        // randomly choose preamble from [0 nof_groupA_preambles)
        sel_preamble = rand() % rach_cfg.nof_groupA_preambles;
      } else {
        rError("Selected group preamble A but nof_groupA_preambles=0\n");
        state = IDLE;
        return;
      }
    } else {
      if (nof_groupB_preambles) {
        // randomly choose preamble from [nof_groupA_preambles nof_groupB_preambles)
        sel_preamble = rach_cfg.nof_groupA_preambles + rand() % nof_groupB_preambles;
      } else {
        rError("Selected group preamble B but nof_groupA_preambles=0\n");
        state = IDLE;
        return;
      }
    }
    sel_maskIndex = 0;
  }

  rDebug("Selected preambleIndex=%d maskIndex=%d GroupA=%d, GroupB=%d\n",
         sel_preamble,
         sel_maskIndex,
         rach_cfg.nof_groupA_preambles,
         nof_groupB_preambles);

  // Jump directly to transmission
  preamble_transmission();
}

/* Preamble transmission as defined in 5.1.3 */
void ra_proc::preamble_transmission()
{

  received_target_power_dbm = rach_cfg.iniReceivedTargetPower + delta_preamble_db +
                              (preambleTransmissionCounter - 1) * rach_cfg.powerRampingStep;

  phy_h->prach_send(sel_preamble, sel_maskIndex - 1, received_target_power_dbm);
  rntis->rar_rnti        = 0;
  ra_tti                 = 0;
  rar_received           = false;
  backoff_interval_start = -1;

  state = PDCCH_SETUP;
}

// Process Timing Advance Command as defined in Section 5.2
void ra_proc::process_timeadv_cmd(uint32_t ta)
{
  if (preambleIndex == 0) {
    // Preamble not selected by UE MAC
    phy_h->set_timeadv_rar(ta);
    // Only if timer is running reset the timer
    if (time_alignment_timer->is_running()) {
      time_alignment_timer->run();
    }
    Debug("Applying RAR TA CMD %d\n", ta);
  } else {
    // Preamble selected by UE MAC
    if (!time_alignment_timer->is_running()) {
      phy_h->set_timeadv_rar(ta);
      time_alignment_timer->run();
      Debug("Applying RAR TA CMD %d\n", ta);
    } else {
      // Ignore TA CMD
      Warning("Ignoring RAR TA CMD because timeAlignmentTimer still running\n");
    }
  }
}

/* Called upon the reception of a DL grant for RA-RNTI
 * Configures the action and softbuffer for the reception of the associated TB
 */
void ra_proc::new_grant_dl(mac_interface_phy_lte::mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action)
{
  bzero(action, sizeof(mac_interface_phy_lte::tb_action_dl_t));

  if (grant.tb[0].tbs < MAX_RAR_PDU_LEN) {
    rDebug("DL dci found RA-RNTI=%d\n", ra_rnti);
    action->tb[0].enabled       = true;
    action->tb[0].payload       = rar_pdu_buffer;
    action->tb[0].rv            = grant.tb[0].rv;
    action->tb[0].softbuffer.rx = &softbuffer_rar;
    rar_grant_nbytes            = grant.tb[0].tbs;
    if (action->tb[0].rv == 0) {
      srslte_softbuffer_rx_reset(&softbuffer_rar);
    }
  } else {
    rError("Received RAR dci exceeds buffer length (%d>%d)\n", grant.tb[0].tbs, MAX_RAR_PDU_LEN);
  }
}

/* Called upon the successful decoding of a TB addressed to RA-RNTI.
 * Processes the reception of a RAR as defined in 5.1.4
 */
void ra_proc::tb_decoded_ok(const uint8_t cc_idx, const uint32_t tti)
{
  if (pcap) {
    pcap->write_dl_ranti(rar_pdu_buffer, rar_grant_nbytes, ra_rnti, true, tti, cc_idx);
  }

  rDebug("RAR decoded successfully TBS=%d\n", rar_grant_nbytes);

  rar_pdu_msg.init_rx(rar_grant_nbytes);
  rar_pdu_msg.parse_packet(rar_pdu_buffer);

  // Set Backoff parameter
  if (rar_pdu_msg.has_backoff()) {
    backoff_param_ms = backoff_table[rar_pdu_msg.get_backoff() % 16];
  } else {
    backoff_param_ms = 0;
  }

  current_ta = 0;

  while (rar_pdu_msg.next()) {
    if (rar_pdu_msg.get()->has_rapid() && rar_pdu_msg.get()->get_rapid() == sel_preamble) {

      rar_received = true;
      process_timeadv_cmd(rar_pdu_msg.get()->get_ta_cmd());

      // TODO: Indicate received target power
      // phy_h->set_target_power_rar(iniReceivedTargetPower, (preambleTransmissionCounter-1)*powerRampingStep);

      uint8_t grant[srslte::rar_subh::RAR_GRANT_LEN];
      rar_pdu_msg.get()->get_sched_grant(grant);

      rntis->rar_rnti = 0;
      phy_h->set_rar_grant(grant, rar_pdu_msg.get()->get_temp_crnti());

      current_ta = rar_pdu_msg.get()->get_ta_cmd();

      rInfo("RAPID=%d, TA=%d, T-CRNTI=0x%x\n",
            sel_preamble,
            rar_pdu_msg.get()->get_ta_cmd(),
            rar_pdu_msg.get()->get_temp_crnti());

      if (preambleIndex > 0) {
        // Preamble selected by Network
        complete();
      } else {
        // Preamble selected by UE MAC
        mux_unit->msg3_prepare();
        rntis->temp_rnti = rar_pdu_msg.get()->get_temp_crnti();

        // If this is the first successfully received RAR within this procedure, Msg3 is empty
        if (mux_unit->msg3_is_empty()) {

          // Save transmitted C-RNTI (if any)
          transmitted_crnti = rntis->crnti;

          // If we have a C-RNTI, tell Mux unit to append C-RNTI CE if no CCCH SDU transmission
          if (transmitted_crnti) {
            rInfo("Appending C-RNTI MAC CE 0x%x in next transmission\n", transmitted_crnti);
            mux_unit->append_crnti_ce_next_tx(transmitted_crnti);
          }
        }

        // Save transmitted UE contention id, as defined by higher layers
        transmitted_contention_id = rntis->contention_id;

        rDebug("Waiting for Contention Resolution\n");
        state = CONTENTION_RESOLUTION;
      }
    } else {
      if (rar_pdu_msg.get()->has_rapid()) {
        rInfo("Found RAR for preamble %d\n", rar_pdu_msg.get()->get_rapid());
      }
    }
  }
}

/* Called after RA response window expiration without a valid RAPID or after a reception of an invalid
 * Contention Resolution ID
 */
void ra_proc::response_error()
{
  rntis->temp_rnti = 0;
  preambleTransmissionCounter++;
  if (preambleTransmissionCounter >= rach_cfg.preambleTransMax + 1) {
    rError("Maximum number of transmissions reached (%d)\n", rach_cfg.preambleTransMax);
    rrc->ra_problem();
    state = IDLE;
    if (ra_is_ho) {
      rrc->ho_ra_completed(false);
    }
  } else {
    backoff_interval_start = -1;
    if (backoff_param_ms) {
      backoff_interval = rand() % backoff_param_ms;
    } else {
      backoff_interval = 0;
    }
    if (backoff_interval) {
      rDebug("Backoff wait interval %d\n", backoff_interval);
      state = BACKOFF_WAIT;
    } else {
      rDebug("Transmitting immediately (%d/%d)\n", preambleTransmissionCounter, rach_cfg.preambleTransMax);
      resource_selection();
    }
  }
}

bool ra_proc::is_contention_resolution()
{
  return state == CONTENTION_RESOLUTION;
}

/* Perform the actions upon completition of the RA procedure as defined in 5.1.6 */
void ra_proc::complete()
{
  // Start looking for PDCCH CRNTI
  if (!transmitted_crnti) {
    rntis->crnti = rntis->temp_rnti;
  }
  rntis->temp_rnti = 0;

  mux_unit->msg3_flush();

  if (ra_is_ho) {
    rrc->ho_ra_completed(true);
  }
  log_h->console("Random Access Complete.     c-rnti=0x%x, ta=%d\n", rntis->crnti, current_ta);
  rInfo("Random Access Complete.     c-rnti=0x%x, ta=%d\n", rntis->crnti, current_ta);

  state = START_WAIT_COMPLETION;
}

void ra_proc::start_noncont(uint32_t preamble_index, uint32_t prach_mask)
{
  next_preamble_idx     = preamble_index;
  next_prach_mask       = prach_mask;
  noncontention_enabled = true;
  start_mac_order(56, true);
}

void ra_proc::start_mac_order(uint32_t msg_len_bits, bool is_ho)
{
  if (state == IDLE) {
    ra_is_ho         = is_ho;
    started_by_pdcch = false;
    new_ra_msg_len   = msg_len_bits;
    rInfo("Starting PRACH by MAC order\n");
    initialization();
  } else {
    Warning("Trying to start PRACH by MAC order in invalid state (%s)\n", state_str[state]);
  }
}

void ra_proc::start_pdcch_order()
{
  if (state == IDLE) {
    started_by_pdcch = true;
    rInfo("Starting PRACH by PDCCH order\n");
    initialization();
  } else {
    Warning("Trying to start PRACH by MAC order in invalid state (%s)\n", state_str[state]);
  }
}

// Contention Resolution Timer is expired (Section 5.1.5)
void ra_proc::timer_expired(uint32_t timer_id)
{
  rInfo("Contention Resolution Timer expired. Stopping PDCCH Search and going to Response Error\n");
  response_error();
}

/* Function called by MAC when a Contention Resolution ID CE is received.
 * Performs the actions defined in 5.1.5 for Temporal C-RNTI Contention Resolution
 */
bool ra_proc::contention_resolution_id_received(uint64_t rx_contention_id)
{
  bool uecri_successful = false;

  rDebug("MAC PDU Contains Contention Resolution ID CE\n");

  if (state != CONTENTION_RESOLUTION) {
    rError("Received contention resolution in wrong state. Aborting.\n");
    response_error();
  }

  // MAC PDU successfully decoded and contains MAC CE contention Id
  contention_resolution_timer.stop();

  if (transmitted_contention_id == rx_contention_id) {
    // UE Contention Resolution ID included in MAC CE matches the CCCH SDU transmitted in Msg3
    uecri_successful = true;
    complete();
  } else {
    rInfo("Transmitted UE Contention Id differs from received Contention ID (0x%" PRIx64 " != 0x%" PRIx64 ")\n",
          transmitted_contention_id,
          rx_contention_id);

    // Discard MAC PDU
    uecri_successful = false;

    // Contention Resolution not successfully is like RAR not successful
    response_error();
  }

  return uecri_successful;
}

void ra_proc::pdcch_to_crnti(bool is_new_uplink_transmission)
{
  rDebug("PDCCH to C-RNTI received %s new UL transmission\n", is_new_uplink_transmission ? "with" : "without");
  if ((!started_by_pdcch && is_new_uplink_transmission) || started_by_pdcch) {
    rDebug("PDCCH for C-RNTI received\n");
    contention_resolution_timer.stop();
    complete();
  }
}

void ra_proc::update_rar_window(int& rar_window_start, int& rar_window_length)
{
  if (state != RESPONSE_RECEPTION) {
    // reset RAR window params to default values to disable RAR search
    rar_window_start  = -1;
    rar_window_length = -1;
  } else {
    rar_window_length = rach_cfg.responseWindowSize;
    rar_window_start  = rar_window_st;
  }
  rDebug("rar_window_start=%d, rar_window_length=%d\n", rar_window_start, rar_window_length);
}

// Restart timer at each Msg3 HARQ retransmission (5.1.5)
void ra_proc::harq_retx()
{
  rInfo("Restarting ContentionResolutionTimer=%d ms\n", contention_resolution_timer.duration());
  contention_resolution_timer.run();
}

void ra_proc::harq_max_retx()
{
  Warning("Contention Resolution is considered not successful. Stopping PDCCH Search and going to Response Error\n");
  response_error();
}
} // namespace srsue
