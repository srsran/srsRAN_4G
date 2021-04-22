/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/test/ttcn3/hdr/ttcn3_ue.h"
#include "lte_ttcn3_phy.h"
#include "srsue/hdr/stack/ue_stack_lte.h"
#include <sstream>

extern "C" {
SRSRAN_API char* srsran_get_build_info();
SRSRAN_API char* srsran_get_build_mode();
}

ttcn3_ue::ttcn3_ue() : logger(srslog::fetch_basic_logger("UE", false)), tft_matcher(logger) {}

int ttcn3_ue::init(all_args_t args, syssim_interface_phy* syssim_, const std::string tc_name_)
{
  // Init UE log
  logger.set_level(srslog::basic_levels::info);
  logger.set_hex_dump_max_size(128);
  logger.info("Built in %s mode using %s.", srsran_get_build_mode(), srsran_get_build_info());

  // Patch args
  args.stack.nas.force_imsi_attach = true;
  args.stack.nas.eia               = "1,2,3";
  args.stack.nas.eea               = "0,1,2,3";

  // Configure default parameters
  args.stack.usim.algo = "xor";
  args.stack.usim.imei = "356092040793011";
  args.stack.usim.imsi = "001010123456789";                  // Anritsu test USIM
  args.stack.usim.k    = "000102030405060708090A0B0C0D0E0F"; // fixed as per TS 34.108 Sec. 8.2

  args.stack.rrc.feature_group       = 0xe6041000;
  args.stack.rrc.ue_category_str     = SRSRAN_UE_CATEGORY_DEFAULT;
  args.stack.rrc.ue_category         = strtol(args.stack.rrc.ue_category_str.c_str(), nullptr, 10);
  args.stack.rrc.nof_supported_bands = 1;
  args.stack.rrc.supported_bands[0]  = 7;
  args.stack.rrc.release             = 8;
  args.stack.rrc.mbms_service_id     = -1;

  args.phy.dl_earfcn = "3400";
  args.rf.type       = "none";
  args.stack.type    = "lte";
  args.phy.type      = "lte_ttcn3";

  // Instantiate layers and stack together our UE
  if (args.stack.type == "lte") {
    stack = std::unique_ptr<ue_stack_lte>(new ue_stack_lte);
    if (!stack) {
      srsran::console("Error creating LTE stack instance.\n");
      return SRSRAN_ERROR;
    }

    phy = std::unique_ptr<srsue::lte_ttcn3_phy>(new srsue::lte_ttcn3_phy);
    if (!phy) {
      srsran::console("Error creating LTE PHY instance.\n");
      return SRSRAN_ERROR;
    }
  } else {
    srsran::console("Invalid stack type %s. Supported values are [lte].\n", args.stack.type.c_str());
    return SRSRAN_ERROR;
  }

  // init layers
  if (phy->init(args.phy, stack.get(), syssim_)) {
    srsran::console("Error initializing PHY.\n");
    return SRSRAN_ERROR;
  }

  if (stack->init(args.stack, phy.get(), this)) {
    srsran::console("Error initializing stack.\n");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

ttcn3_ue::~ttcn3_ue()
{
  stop();
}

void ttcn3_ue::stop()
{
  if (stack) {
    stack->stop();
  }

  if (phy) {
    phy->stop();
  }
}

bool ttcn3_ue::switch_on()
{
  return stack->switch_on();
}

bool ttcn3_ue::switch_off()
{
  return stack->switch_off();
}

bool ttcn3_ue::enable_data()
{
  return stack->enable_data();
}

bool ttcn3_ue::disable_data()
{
  return stack->disable_data();
}

// The interface for SYSSIM
void ttcn3_ue::set_cell_map(lte_ttcn3_phy::cell_list_t phy_cell_map)
{
  phy->set_cell_map(phy_cell_map);
}

void ttcn3_ue::new_grant_ul(const srsue::mac_interface_phy_lte::mac_grant_ul_t grant)
{
  phy->new_grant_ul(grant);
}

void ttcn3_ue::new_tb(const srsue::mac_interface_phy_lte::mac_grant_dl_t mac_grant, const uint8_t* data)
{
  phy->new_tb(mac_grant, data);
}

void ttcn3_ue::set_current_tti(uint32_t tti)
{
  phy->set_current_tti(tti);
  stack->run_tti(tti, 1);
}

uint16_t ttcn3_ue::get_dl_sched_rnti(uint32_t tti)
{
  return stack->get_dl_sched_rnti(tti);
}

// GW interface
void ttcn3_ue::add_mch_port(uint32_t lcid, uint32_t port) {}
void ttcn3_ue::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  logger.debug(pdu->msg, pdu->N_bytes, "Rx PDU (%d B) on lcid=%d", pdu->N_bytes, lcid);
  switch (test_loop_mode) {
    case TEST_LOOP_INACTIVE:
      logger.warning("Test loop inactive. Dropping PDU.");
      break;
    case TEST_LOOP_MODE_A_ACTIVE:
      logger.error("Test loop mode A not implemented. Dropping PDU.");
      break;
    case TEST_LOOP_MODE_B_ACTIVE:
      // Section 5.4.4 in TS 36.509
      if (pdu_delay_timer.is_running()) {
        pdu_queue[lcid].push(std::move(pdu));
      } else {
        if (pdu_delay_timer.is_set()) {
          pdu_queue[lcid].push(std::move(pdu));
          pdu_delay_timer.run(); // timer is already set
        } else {
          loop_back_pdu_with_tft(lcid, std::move(pdu));
        }
      }
      break;
    case TEST_LOOP_MODE_C_ACTIVE:
      logger.error("Test loop mode C not implemented. Dropping PDU.");
      break;
  }
}
void ttcn3_ue::write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}
int  ttcn3_ue::setup_if_addr(uint32_t eps_bearer_id,
                            uint32_t lcid,
                            uint8_t  pdn_type,
                            uint32_t ip_addr,
                            uint8_t* ipv6_if_id,
                            char*    err_str)
{
  return 0;
}

bool ttcn3_ue::is_running()
{
  return true;
}

int ttcn3_ue::update_lcid(uint32_t eps_bearer_id, uint32_t new_lcid)
{
  return SRSRAN_SUCCESS;
}

int ttcn3_ue::apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                          const uint8_t&                                 lcid,
                                          const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  return tft_matcher.apply_traffic_flow_template(eps_bearer_id, lcid, tft);
}

void ttcn3_ue::set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms_)
{
  test_loop_mode = mode;
  switch (test_loop_mode) {
    case TEST_LOOP_INACTIVE:
      // deactivate timer
      logger.info("Deactivating Test Loop Mode");
      pdu_delay_timer.release();
      break;
    case TEST_LOOP_MODE_A_ACTIVE:
      logger.error("Test loop mode A not implemented");
      break;
    case TEST_LOOP_MODE_B_ACTIVE:
      logger.info("Activating Test loop mode B with %d ms PDU delay", ip_pdu_delay_ms_);
      pdu_delay_timer = stack->get_task_sched().get_unique_timer();
      if (ip_pdu_delay_ms_ > 0) {
        pdu_delay_timer.set(ip_pdu_delay_ms_, [this](uint32_t tid) { timer_expired(tid); });
      }
      break;
    case TEST_LOOP_MODE_C_ACTIVE:
      logger.error("Test loop mode A not implemented");
      break;
  }
}

void ttcn3_ue::timer_expired(uint32_t timeout_id)
{
  if (timeout_id == pdu_delay_timer.id()) {
    logger.info("Testmode B PDU delay timer expired");
    send_queued_data();
  }
}
void ttcn3_ue::send_queued_data()
{
  if (!stack->is_rrc_connected()) {
    logger.info("RRC not connected, requesting NAS attach");
    if (not stack->start_service_request()) {
      logger.warning("Could not reestablish the connection");
    }
    stack->get_task_sched().defer_callback(500, [&]() { send_queued_data(); });
    return;
  }

  for (auto& bearer_pdu_queue : pdu_queue) {
    logger.info("Delivering %zd buffered PDUs for LCID=%d", bearer_pdu_queue.second.size(), bearer_pdu_queue.first);
    while (not bearer_pdu_queue.second.empty()) {
      srsran::unique_byte_buffer_t pdu;
      bearer_pdu_queue.second.try_pop(&pdu);
      loop_back_pdu_with_tft(bearer_pdu_queue.first, std::move(pdu));
    }
  }
}

void ttcn3_ue::loop_back_pdu_with_tft(uint32_t input_lcid, srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Rx PDU (%d B) on lcid=%d, looping back", pdu->N_bytes, input_lcid);
  stack->write_sdu(input_lcid, std::move(pdu));
}
