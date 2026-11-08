// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "ttcn3_ue.h"
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
  args.phy.type      = "lte_ttcn3";

  // Instantiate layers and stack together our UE
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
  tft_matcher.reset(); // empty all TFTs
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
      loop_back_pdu_mode_a(lcid, std::move(pdu));
      break;
    case TEST_LOOP_MODE_B_ACTIVE:
      // Section 5.4.4 in TS 36.509
      if (pdu_delay_timer.is_running()) {
        pdu_queue.push(std::move(pdu));
      } else {
        if (pdu_delay_timer.is_set()) {
          pdu_queue.push(std::move(pdu));
          pdu_delay_timer.run(); // timer is already set
        } else {
          loop_back_pdu_with_tft(std::move(pdu));
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
                            uint8_t  pdn_type,
                            uint32_t ip_addr,
                            uint8_t* ipv6_if_id,
                            char*    err_str)
{
  default_eps_bearer_id = static_cast<int32_t>(eps_bearer_id);
  return 0;
}

bool ttcn3_ue::is_running()
{
  return true;
}

int ttcn3_ue::deactivate_eps_bearer(const uint32_t eps_bearer_id)
{
  if (default_eps_bearer_id == static_cast<int32_t>(eps_bearer_id)) {
    default_eps_bearer_id = -1;
  }
  return SRSRAN_SUCCESS;
}

int ttcn3_ue::apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                          const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
{
  return tft_matcher.apply_traffic_flow_template(eps_bearer_id, tft);
}

void ttcn3_ue::set_test_loop_mode(const test_loop_mode_state_t       mode,
                                  const uint32_t                     ip_pdu_delay_ms_,
                                  const std::vector<lb_setup_drb_t>& lb_setup_)
{
  test_loop_mode = mode;
  switch (test_loop_mode) {
    case TEST_LOOP_INACTIVE:
      // deactivate timer
      logger.info("Deactivating Test Loop Mode");
      pdu_delay_timer.release();
      break;
    case TEST_LOOP_MODE_A_ACTIVE:
      logger.info("Activating Test loop mode A with %zd LB entities", lb_setup_.size());
      lb_setup = lb_setup_;
      break;
    case TEST_LOOP_MODE_B_ACTIVE:
      logger.info("Activating Test loop mode B with %d ms PDU delay", ip_pdu_delay_ms_);
      pdu_delay_timer = stack->get_task_sched().get_unique_timer();
      if (ip_pdu_delay_ms_ > 0) {
        pdu_delay_timer.set(ip_pdu_delay_ms_, [this](uint32_t tid) { timer_expired(tid); });
      }
      break;
    case TEST_LOOP_MODE_C_ACTIVE:
      logger.error("Test loop mode C not implemented");
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

  logger.info("Delivering %zd buffered UL PDUs", pdu_queue.size());
  while (not pdu_queue.empty()) {
    srsran::unique_byte_buffer_t pdu;
    pdu_queue.try_pop(&pdu);
    loop_back_pdu_with_tft(std::move(pdu));
  }
}

void ttcn3_ue::loop_back_pdu_with_tft(srsran::unique_byte_buffer_t pdu)
{
  if (default_eps_bearer_id == -1) {
    logger.error("No default EPS bearer activated. Dropping PDU.");
  }

  uint8_t target_eps_bearer_id = default_eps_bearer_id;
  if (tft_matcher.check_tft_filter_match(pdu, target_eps_bearer_id) == SRSRAN_SUCCESS) {
    logger.debug("Updated EPS bearer");
  }

  logger.info(
      pdu->msg, pdu->N_bytes, "Rx PDU (%d B) looping back on eps_bearer_id=%d", pdu->N_bytes, target_eps_bearer_id);

  stack->write_sdu(target_eps_bearer_id, std::move(pdu));
}

// Section 5.4.3 in TS 36.509, return the PDCP SDU on the same DRB, scaled to the UL PDCP SDU size of the LB setup
void ttcn3_ue::loop_back_pdu_mode_a(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  uint32_t drb_id       = lcid - 2; // LCID 3..10 map to DRB 1..8
  uint32_t nof_ul_bytes = pdu->N_bytes;
  if (not lb_setup.empty()) {
    auto it = std::find_if(
        lb_setup.begin(), lb_setup.end(), [drb_id](const lb_setup_drb_t& lb) { return lb.drb_id == drb_id; });
    if (it == lb_setup.end()) {
      logger.warning("No LB setup for DRB%d. Dropping PDU.", drb_id);
      return;
    }
    nof_ul_bytes = (it->ul_pdcp_sdu_size + 7) / 8;
  }

  if (nof_ul_bytes == 0 or pdu->N_bytes == 0) {
    logger.debug("UL PDCP SDU size is zero for DRB%d. Dropping PDU.", drb_id);
    return;
  }

  // Truncate or repeat the received data to fill the UL PDCP SDU
  if (nof_ul_bytes > pdu->N_bytes) {
    if (nof_ul_bytes > pdu->get_tailroom() + pdu->N_bytes) {
      logger.error("UL PDCP SDU size of %d B exceeds buffer size. Dropping PDU.", nof_ul_bytes);
      return;
    }
    for (uint32_t i = pdu->N_bytes; i < nof_ul_bytes; ++i) {
      pdu->msg[i] = pdu->msg[i % pdu->N_bytes];
    }
  }
  pdu->N_bytes = nof_ul_bytes;

  srsue::ue_bearer_manager::radio_bearer_t bearer = stack->get_lcid_bearer(lcid);
  if (not bearer.is_valid()) {
    logger.error("No EPS bearer for lcid=%d. Dropping PDU.", lcid);
    return;
  }
  logger.info(pdu->msg, pdu->N_bytes, "Looping back PDU (%d B) on DRB%d", pdu->N_bytes, drb_id);
  stack->write_sdu(bearer.eps_bearer_id, std::move(pdu));
}
