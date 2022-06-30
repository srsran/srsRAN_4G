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

#include "srsue/hdr/stack/mac_nr/proc_ra_nr.h"
#include "srsran/common/standard_streams.h"
#include "srsran/mac/mac_rar_pdu_nr.h"
#include "srsue/hdr/stack/mac_nr/mac_nr.h"

namespace srsue {

const char* state_str_nr[] = {"RA:    IDLE:   ",
                              "RA:    PDCCH_SETUP:  ",
                              "RA:    WAITING_FOR_PRACH_SENT:     ",
                              "RA:    WAITING_FOR_RESPONSE_RECEPTION: ",
                              "RA:    WAITING_FOR_COMPLETION: ",
                              "RA:    MAX_RA_STATES: "};

// Table 7.2-1. Backoff Parameter values
uint32_t backoff_table_nr[16] = {0, 10, 20, 30, 40, 60, 80, 120, 160, 240, 320, 480, 960, 1920, 1920, 1920};

// Table 7.6-1: DELTA_PREAMBLE values long
int delta_preamble_db_table_nr[5] = {0, -3, -6, 0};

proc_ra_nr::proc_ra_nr(mac_interface_proc_ra_nr& mac_, srslog::basic_logger& logger_) : mac(mac_), logger(logger_) {}

void proc_ra_nr::init(phy_interface_mac_nr* phy_, srsran::ext_task_sched_handle* task_sched_)
{
  phy                         = phy_;
  task_sched                  = task_sched_;
  task_queue                  = task_sched->make_task_queue();
  prach_send_timer            = task_sched->get_unique_timer();
  rar_timeout_timer           = task_sched->get_unique_timer();
  contention_resolution_timer = task_sched->get_unique_timer();
  backoff_timer               = task_sched->get_unique_timer();
}

/* Sets a new configuration. The configuration is applied by initialization() function */
void proc_ra_nr::set_config(const srsran::rach_cfg_nr_t& rach_cfg_)
{
  if (state != IDLE) {
    logger.warning("Wrong state for ra reponse reception %s (expected state %s)",
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state),
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, IDLE));
    return;
  }
  rach_cfg   = rach_cfg_;
  configured = true;
  logger.info("Set RACH common config (Config Index %d, preambleTransMax %d, Response Window %d)",
              rach_cfg.prach_ConfigurationIndex,
              rach_cfg.preambleTransMax,
              rach_cfg.ra_responseWindow);
}

void proc_ra_nr::start_by_rrc()
{
  if (state != IDLE || configured == false) {
    logger.warning("Trying to start PRACH by RRC order in invalid state (%s)",
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state));
    return;
  }
  started_by = initiators_t::RRC;
  logger.info("Starting PRACH by RRC order");
  ra_procedure_initialization();
}

void proc_ra_nr::start_by_mac()
{
  if (state != IDLE || configured == false) {
    logger.warning("Trying to start PRACH by MAC order in invalid state (%s)",
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state));
    return;
  }
  started_by = initiators_t::MAC;
  logger.info("Starting PRACH by MAC order");
  ra_procedure_initialization();
}

bool proc_ra_nr::is_rar_opportunity(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);
  // TODO replace second "&&"" by rar_timeout_timer.running if timer thread safe and delayed starting (tti+3)
  if (state == WAITING_FOR_RESPONSE_RECEPTION && ra_window_start > 0 && ra_window_length > 0 &&
      mac_nr::is_in_window(tti, &ra_window_start, &ra_window_length)) {
    logger.debug("SCHED: Searching RAR-RNTI=0x%x, tti=%d", rar_rnti, tti);
    return true;
  }
  return false;
}

uint16_t proc_ra_nr::get_rar_rnti()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (rar_rnti == SRSRAN_INVALID_RNTI || state != WAITING_FOR_RESPONSE_RECEPTION) {
    logger.error("Requested ra rnti is invalid. Anyway we return an invalid ra rnti");
    return SRSRAN_INVALID_RNTI;
  }
  return rar_rnti;
}

bool proc_ra_nr::has_rar_rnti()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (rar_rnti != SRSRAN_INVALID_RNTI) {
    return true;
  }
  return false;
}

void proc_ra_nr::received_contention_resolution(bool is_successful)
{
  std::lock_guard<std::mutex> lock(mutex);
  ra_contention_resolution(is_successful);
}

void proc_ra_nr::timer_expired(uint32_t timer_id)
{
  if (prach_send_timer.id() == timer_id) {
    logger.warning("PRACH Send timer expired. PRACH was not transmitted within %d ttis by phy. (TODO)",
                   prach_send_timer.duration());
    ra_error();
  } else if (rar_timeout_timer.id() == timer_id) {
    logger.warning("RAR Timer expired. RA response not received within the response window");
    ra_error();
  } else if (contention_resolution_timer.id() == timer_id) {
    logger.warning("Contention Resolution Timer expired. Stopping PDCCH Search and going to Response Error (TODO)");
    ra_error();
  } else if (backoff_timer.id() == timer_id) {
    logger.info("Transmitting new preamble (%d/%d)", preamble_transmission_counter, rach_cfg.preambleTransMax);
    ra_resource_selection();
  } else {
    logger.error("Timer not implemented");
  }
}

// 5.1.2 Random Access Resource selection
void proc_ra_nr::ra_procedure_initialization()
{
  mac.msg3_flush();
  preamble_transmission_counter = 1;
  preamble_power_ramping_step   = rach_cfg.powerRampingStep;
  scaling_factor_bi             = 1;
  preamble_backoff              = 0;
  preambleTransMax              = rach_cfg.preambleTransMax;
  ra_resource_selection();
}

// 5.1.2 Random Access Resource selection (TODO)
void proc_ra_nr::ra_resource_selection()
{
  ra_preamble_transmission();
}

// 5.1.3 Random Access Preamble transmission
void proc_ra_nr::ra_preamble_transmission()
{
  uint32_t delta_preamble        = 0; // TODO calulate the delta preamble based on delta_preamble_db_table_nr
  preamble_received_target_power = rach_cfg.PreambleReceivedTargetPower + delta_preamble +
                                   (preamble_transmission_counter - 1) * rach_cfg.powerRampingStep +
                                   power_offset_2step_ra;
  preamble_index = 0;
  prach_occasion = 0;
  // instruct the physical layer to transmit the Random Access Preamble using the selected PRACH occasion, corresponding
  // RA-RNTI (if available), PREAMBLE_INDEX, and PREAMBLE_RECEIVED_TARGET_POWER.
  phy->send_prach(prach_occasion, preamble_index, preamble_received_target_power);
  prach_send_timer.set(PRACH_SEND_CALLBACK_TIMEOUT, [this](uint32_t tid) { timer_expired(tid); });
  prach_send_timer.run();
  state = WAITING_FOR_PRACH_SENT;
}

// 5.1.4 Random Access Preamble transmission
void proc_ra_nr::ra_response_reception(const mac_interface_phy_nr::tb_action_dl_result_t& tb)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (state != WAITING_FOR_RESPONSE_RECEPTION) {
    logger.warning(
        "Wrong state for ra reponse reception %s (expected state %s)",
        srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state),
        srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, WAITING_FOR_RESPONSE_RECEPTION));
    return;
  }

  // Stop rar timer
  rar_timeout_timer.stop();
  if (tb.ack && tb.payload != nullptr) {
    srsran::mac_rar_pdu_nr pdu;
    if (!pdu.unpack(tb.payload->msg, tb.payload->N_bytes)) {
      logger.warning("Error unpacking RAR PDU");
      return;
    }

    fmt::memory_buffer buff;
    pdu.to_string(buff);
    logger.info("%s", srsran::to_c_str(buff));

    for (auto& subpdu : pdu.get_subpdus()) {
      if (subpdu.has_rapid() && subpdu.get_rapid() == preamble_index) {
        logger.debug("PROC RA NR: Setting UL grant and prepare Msg3");
        mac.set_temp_crnti(subpdu.get_temp_crnti());

        // Set Temporary-C-RNTI if provided, otherwise C-RNTI is ok
        phy->set_rar_grant(tb.rx_slot_idx, subpdu.get_ul_grant(), subpdu.get_temp_crnti(), srsran_rnti_type_ra);

        // Apply TA CMD
        current_ta = subpdu.get_ta();
        phy->set_timeadv_rar(tb.rx_slot_idx, current_ta);

        // reset all parameters that are used before rar
        rar_rnti = SRSRAN_INVALID_RNTI;
        mac.msg3_prepare();

        // Set Backoff parameter
        if (subpdu.has_backoff()) {
          preamble_backoff = backoff_table_nr[subpdu.get_backoff() % 16]; // TODO multiplied with SCALING_FACTOR_BI.
        } else {
          preamble_backoff = 0;
        }
      }
    }
  }
  contention_resolution_timer.set(rach_cfg.ra_ContentionResolutionTimer, [this](uint32_t tid) { timer_expired(tid); });
  contention_resolution_timer.run();
  logger.debug("Waiting for Contention Resolution");
  state = WAITING_FOR_CONTENTION_RESOLUTION;
}

// TS 38.321 Section 5.1.5 2 ways to resolve contention resolution
// if the C-RNTI MAC CE was included in Msg3: (only this one is implemented)
void proc_ra_nr::ra_contention_resolution(bool received_con_res_matches_ue_id)
{
  if (state != WAITING_FOR_CONTENTION_RESOLUTION) {
    logger.warning(
        "Wrong state for ra contention resolution by phy %s (expected state %s)",
        srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state),
        srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, WAITING_FOR_CONTENTION_RESOLUTION));
    return;
  }
  if (started_by == initiators_t::RRC || started_by == initiators_t::MAC || received_con_res_matches_ue_id) {
    if (received_con_res_matches_ue_id) {
      logger.info("Received CONRES ID matches transmitted UE ID");
    } else {
      logger.info("PDCCH to C-RNTI received with a new UL grant of transmission");
    }
    contention_resolution_timer.stop();
    state = WAITING_FOR_COMPLETION;
    ra_completion();
  } else {
    logger.error("Not started by the correct initiator MAC or RRC");
  }
}

void proc_ra_nr::ra_completion()
{
  if (state != WAITING_FOR_COMPLETION) {
    logger.warning("Wrong state for ra completion by phy %s (expected state %s)",
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state),
                   srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, WAITING_FOR_COMPLETION));
    return;
  }

  // Start looking for PDCCH CRNTI
  if (!mac.get_crnti()) {
    // promote temp RNTI to new C-RNTI
    mac.set_crnti_to_temp();
    mac.set_temp_crnti(SRSRAN_INVALID_RNTI);
  }

  srsran::console("Random Access Complete.     c-rnti=0x%x, ta=%d\n", mac.get_crnti(), current_ta);
  logger.info("Random Access Complete.     c-rnti=0x%x, ta=%d", mac.get_crnti(), current_ta);

  mac.rrc_ra_completed();
  reset();
}

void proc_ra_nr::ra_error()
{
  std::lock_guard<std::mutex> lock(mutex);
  preamble_transmission_counter++;
  contention_resolution_timer.stop();
  mac.set_temp_crnti(SRSRAN_INVALID_RNTI);
  uint32_t backoff_wait;
  bool     ra_procedure_completed = false; // true = (unsuccessfully) completed, false = uncompleted

  if (preamble_transmission_counter >= rach_cfg.preambleTransMax + 1) {
    logger.warning("Maximum number of transmissions reached (%d)", rach_cfg.preambleTransMax);
    // if the Random Access Preamble is transmitted on the SpCell assumption (TODO)
    mac.rrc_ra_problem();                  //  indicate a Random Access problem to upper layers;
    if (started_by == initiators_t::MAC) { // if this Random Access procedure was triggered for SI request
      ra_procedure_completed = true;       // consider the Random Access procedure unsuccessfully completed.
      reset();
    }
  } else {
    // try again, if RA failed
    if (preamble_backoff) {
      backoff_wait = rand() % preamble_backoff;
    } else {
      backoff_wait = 0;
    }
    logger.debug("Backoff wait interval %d", backoff_wait);

    if (backoff_wait > 0) {
      backoff_timer.set(backoff_wait, [this](uint32_t tid) { timer_expired(tid); });
      backoff_timer.run();
    } else {
      timer_expired(backoff_timer.id());
    }
  }
}

// Is called by PHY once it has transmitted the prach transmitted, than configure RA-RNTI and wait for RAR reception
void proc_ra_nr::prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id)
{
  task_queue.push([this, tti, s_id, t_id, f_id, ul_carrier_id]() {
    if (state != WAITING_FOR_PRACH_SENT) {
      logger.warning("Wrong state for prach sent notification by phy %s (expected state %s)",
                     srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, state),
                     srsran::enum_to_text(state_str_nr, (uint32_t)ra_state_t::MAX_RA_STATES, WAITING_FOR_PRACH_SENT));
      return;
    }
    prach_send_timer.stop();
    rar_rnti = 1 + s_id + 14 * t_id + 14 * 80 * f_id + 14 * 80 * 8 * ul_carrier_id;
    logger.info(
        "prach_occasion=%d, preamble_index=%d, ra-rnti=0x%x, ra-tti=%d, s_id=%d, t_id=%d, f_id=%d, ul_carrier_id=%d",
        prach_occasion,
        preamble_index,
        rar_rnti,
        tti,
        s_id,
        t_id,
        f_id,
        ul_carrier_id);
    srsran::console("Random Access Transmission: prach_occasion=%d, preamble_index=%d, ra-rnti=0x%x, tti=%d\n",
                    prach_occasion,
                    preamble_index,
                    rar_rnti,
                    tti);
    uint32_t rar_window_st = TTI_ADD(tti, 3);
    // TODO check ra_response window (delayed start)? // last 3 check if needed when we have a delayed start
    rar_timeout_timer.set(rach_cfg.ra_responseWindow + 3 + 10, [this](uint32_t tid) { timer_expired(tid); });
    rar_timeout_timer.run();
    // Wait for RAR reception
    ra_window_length = rach_cfg.ra_responseWindow;
    ra_window_start  = TTI_ADD(tti, 3);
    logger.debug("Calculated ra_window_start=%d, ra_window_length=%d", ra_window_start, ra_window_length);
    state = WAITING_FOR_RESPONSE_RECEPTION;
  });
}

// Called by PHY thread through MAC parent
void proc_ra_nr::handle_rar_pdu(mac_interface_phy_nr::tb_action_dl_result_t& result)
{
  // Defer the handling of the grant to main stack thread in ra_response_reception
  auto task_handler = [this](const mac_interface_phy_nr::tb_action_dl_result_t& t) {
    ra_response_reception(std::move(t));
  };
  task_queue.push(std::bind(task_handler, std::move(result)));
}

// Called from PHY thread, defer actions therefore.
void proc_ra_nr::pdcch_to_crnti()
{
  task_queue.push([this]() { ra_contention_resolution(false); });
}

bool proc_ra_nr::is_contention_resolution()
{
  return state == WAITING_FOR_CONTENTION_RESOLUTION;
}

void proc_ra_nr::reset()
{
  state      = IDLE;
  started_by = initiators_t::initiators_t_NULLTYPE;
  mac.set_temp_crnti(SRSRAN_INVALID_RNTI);
  prach_send_timer.stop();
  rar_timeout_timer.stop();
  contention_resolution_timer.stop();
}

} // namespace srsue
