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

#include "ttcn3_syssim.h"
#include "dut_utils.h"
#include "srsran/mac/pdu_queue.h"
#include "srsran/rlc/rlc.h"
#include "srsran/srslog/srslog.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsran/upper/pdcp.h"
#include "swappable_sink.h"
#include "ttcn3_common.h"
#include "ttcn3_drb_interface.h"
#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ue.h"
#include "ttcn3_ut_interface.h"
#include <functional>

ttcn3_syssim::ttcn3_syssim(ttcn3_ue* ue_) :
  logger(srslog::fetch_basic_logger("SS")),
  ut_logger(srslog::fetch_basic_logger("UT", false)),
  sys_logger(srslog::fetch_basic_logger("SYS", false)),
  ip_sock_logger(srslog::fetch_basic_logger("IP_S", false)),
  ip_ctrl_logger(srslog::fetch_basic_logger("IP_C", false)),
  srb_logger(srslog::fetch_basic_logger("SRB", false)),
  drb_logger(srslog::fetch_basic_logger("DRB", false)),
  ss_mac_logger(srslog::fetch_basic_logger("SS-MAC", false)),
  ss_rlc_logger(srslog::fetch_basic_logger("SS-RLC", false)),
  ss_pdcp_logger(srslog::fetch_basic_logger("SS-PDCP", false)),
  ut(ut_logger),
  sys(sys_logger),
  ip_sock(ip_sock_logger),
  ip_ctrl(ip_ctrl_logger),
  srb(srb_logger),
  drb(drb_logger),
  mac_msg_ul(20, ss_mac_logger),
  mac_msg_dl(20, ss_mac_logger),
  pdus(logger),
  ue(ue_),
  signal_handler(running),
  timer_handler(create_tti_timer(), [&](uint64_t res) { new_tti_indication(res); })
{
  all_args_t args            = {};
  args.stack.sync_queue_size = MULTIQUEUE_DEFAULT_CAPACITY;
  if (ue->init(args, this, "INIT_TEST") != SRSRAN_SUCCESS) {
    ue->stop();
    fprintf(stderr, "Couldn't initialize UE.\n");
  }
}

ttcn3_syssim::~ttcn3_syssim(){};

int ttcn3_syssim::init(const all_args_t& args_)
{
  args = args_;

  // Make sure to get SS logging as well
  if (args.log.filename == "stdout") {
    auto* swp_sink = srslog::find_sink(swappable_sink::name());
    if (!swp_sink) {
      logger.error("Unable to find the swappable sink");
      srsran::console("Unable to find the swappable sink\n");
      return SRSRAN_ERROR;
    }
    static_cast<swappable_sink*>(swp_sink)->swap_to_stdout();
  }

  // init and configure logging
  auto logger_lvl = srslog::str_to_basic_level(args.log.all_level);
  logger.set_level(logger_lvl);
  ut_logger.set_level(logger_lvl);
  sys_logger.set_level(logger_lvl);
  ip_sock_logger.set_level(logger_lvl);
  ip_ctrl_logger.set_level(logger_lvl);
  srb_logger.set_level(logger_lvl);
  drb_logger.set_level(logger_lvl);
  ss_mac_logger.set_level(logger_lvl);
  ss_rlc_logger.set_level(logger_lvl);
  ss_pdcp_logger.set_level(logger_lvl);

  logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ut_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  sys_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ip_sock_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ip_ctrl_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  srb_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  drb_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ss_mac_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ss_rlc_logger.set_hex_dump_max_size(args.log.all_hex_limit);
  ss_pdcp_logger.set_hex_dump_max_size(args.log.all_hex_limit);

  // Init epoll socket and add FDs
  epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    logger.error("Error creating epoll");
    return SRSRAN_ERROR;
  }

  // add signalfd
  signal_fd = add_signalfd();
  if (add_epoll(signal_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding signalfd to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({signal_fd, &signal_handler});

  // init system interfaces to tester
  if (add_port_handler() != SRSRAN_SUCCESS) {
    logger.error("Error creating port handlers");
    return SRSRAN_ERROR;
  }

  // Init common SS layers
  pdus.init(this);

  return SRSRAN_SUCCESS;
}

void ttcn3_syssim::set_forced_lcid(int lcid)
{
  force_lcid = lcid;
}

int ttcn3_syssim::add_port_handler()
{
  // UT port
  int ut_fd = ut.init(this, listen_address, UT_PORT);
  if (add_epoll(ut_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding UT port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({ut_fd, &ut});
  srsran::console("UT handler listening on SCTP port %d\n", UT_PORT);

  // SYS port
  int sys_fd = sys.init(this, listen_address, SYS_PORT);
  if (add_epoll(sys_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding SYS port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({sys_fd, &sys});
  srsran::console("SYS handler listening on SCTP port %d\n", SYS_PORT);

  // IPsock port
  int ip_sock_fd = ip_sock.init(listen_address, IPSOCK_PORT);
  if (add_epoll(ip_sock_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding IP sock port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({ip_sock_fd, &ip_sock});
  srsran::console("IPSOCK handler listening on SCTP port %d\n", IPSOCK_PORT);

  // IPctrl port
  int ip_ctrl_fd = ip_ctrl.init(listen_address, IPCTRL_PORT);
  if (add_epoll(ip_ctrl_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding IP ctrl port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({ip_ctrl_fd, &ip_ctrl});
  srsran::console("IPCTRL handler listening on SCTP port %d\n", IPCTRL_PORT);

  // add SRB fd
  int srb_fd = srb.init(this, listen_address, SRB_PORT);
  if (add_epoll(srb_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding SRB port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({srb_fd, &srb});
  srsran::console("SRB handler listening on SCTP port %d\n", SRB_PORT);

  // add DRB fd
  int drb_fd = drb.init(this, listen_address, DRB_PORT);
  if (add_epoll(drb_fd, epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding DRB port to epoll");
    return SRSRAN_ERROR;
  }
  event_handler.insert({drb_fd, &drb});
  srsran::console("DRB handler listening on SCTP port %d\n", DRB_PORT);

  return SRSRAN_SUCCESS;
}

///< Function called by epoll timer handler when TTI timer expires
void ttcn3_syssim::new_tti_indication(uint64_t res)
{
  std::lock_guard<std::mutex> lock(syssim_mutex);

  tti = (tti + 1) % 10240;

  logger.set_context(tti);
  logger.debug("Start TTI");

  // Make sure to step SS
  step_stack();

  // inform UE about new TTI
  ue->set_current_tti(tti);

  // check scheduled actions for this TTI
  if (tti_actions.find(tti) != tti_actions.end()) {
    while (!tti_actions[tti].empty()) {
      logger.debug("Running scheduled action");
      move_task_t task = std::move(tti_actions[tti].front());
      task();
      tti_actions[tti].pop();
    }
  }

  // process events, if any
  while (not event_queue.empty()) {
    ss_events_t ev = event_queue.wait_pop();
    switch (ev) {
      case UE_SWITCH_ON:
        srsran::console("Switching on UE ID=%d\n", run_id);
        ue->switch_on();
        break;
      case UE_SWITCH_OFF:
        srsran::console("Switching off UE ID=%d\n", run_id);
        ue->switch_off();
        break;
      case ENABLE_DATA:
        srsran::console("Enabling data for UE ID=%d\n", run_id);
        ue->enable_data();
        break;
      case DISABLE_DATA:
        srsran::console("Disabling data for UE ID=%d\n", run_id);
        ue->disable_data();
        break;
    }
  }

  if (pcell_idx == -1) {
    logger.debug("Skipping TTI. Pcell not yet selected.");
    return;
  }

  // DL/UL processing if UE has selected cell
  dl_rnti = ue->get_dl_sched_rnti(tti);
  if (SRSRAN_RNTI_ISSI(dl_rnti) && (tti % 2 == 0)) {
    // deliver SIBs one after another in every other TTI
    mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
    dl_grant.tti                                   = tti;
    dl_grant.pid                                   = get_pid(tti);
    dl_grant.rnti                                  = dl_rnti;
    dl_grant.tb[0].tbs                             = cells[pcell_idx]->sibs[cells[pcell_idx]->sib_idx]->N_bytes;
    dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
    ue->new_tb(dl_grant, cells[pcell_idx]->sibs[cells[pcell_idx]->sib_idx]->msg);
    logger.info("Delivered SIB%d for pcell_idx=%d", cells[pcell_idx]->sib_idx + 1, pcell_idx);
    cells[pcell_idx]->sib_idx = (cells[pcell_idx]->sib_idx + 1) % cells[pcell_idx]->sibs.size();
  } else if (SRSRAN_RNTI_ISRAR(dl_rnti)) {
    if (prach_tti != -1) {
      rar_tti = (prach_tti + 3) % 10240;
      if (tti == rar_tti) {
        send_rar(prach_preamble_index);
      }
    }
  } else if (SRSRAN_RNTI_ISPA(dl_rnti)) {
    logger.debug("Searching for paging RNTI");
    // PCH will be triggered from SYSSIM after receiving Paging
  } else if (SRSRAN_RNTI_ISUSER(dl_rnti)) {
    // check if this is for contention resolution after PRACH/RAR
    if (dl_rnti == cells[pcell_idx]->config.crnti) {
      logger.debug("Searching for C-RNTI=0x%x", dl_rnti);

      if (rar_tti != -1) {
        msg3_tti = (rar_tti + 3) % 10240;
        if (tti == msg3_tti) {
          send_msg3_grant();
          rar_tti = -1;
        }
      }
    }

    // check for SR
    if (sr_tti != -1) {
      send_sr_ul_grant();
    }

    if (dl_rnti != SRSRAN_INVALID_RNTI) {
      logger.debug("Searching for RNTI=0x%x", dl_rnti);

      // look for DL data to be send in each bearer and provide grant accordingly
      for (int lcid = 0; lcid < SRSRAN_N_RADIO_BEARERS; lcid++) {
        uint32_t buf_state = cells[pcell_idx]->rlc.get_buffer_state(lcid);
        // Schedule DL transmission if there is data in RLC buffer or we need to send Msg4
        if ((buf_state > 0 && cells[pcell_idx]->bearer_follow_on_map[lcid] == false) ||
            (msg3_tti != -1 && conres_id != 0)) {
          logger.debug("LCID=%d, buffer_state=%d", lcid, buf_state);
          tx_payload_buffer.clear();

          const uint32_t mac_header_size = 10; // Add MAC header (10 B for all subheaders, etc)
          mac_msg_dl.init_tx(&tx_payload_buffer, buf_state + mac_header_size, false);

          // check if this is Msg4 that needs to contain the contention resolution ID CE
          if (msg3_tti != -1 && lcid == 0 && conres_id != 0) {
            if (mac_msg_dl.new_subh()) {
              if (mac_msg_dl.get()->set_con_res_id(conres_id)) {
                logger.info("CE:    Added Contention Resolution ID=0x%" PRIx64 "", conres_id);
              } else {
                logger.error("CE:    Setting Contention Resolution ID CE");
              }
              conres_id = 0; // reset CR so it's not sent twice
            } else {
              logger.error("CE:    Setting Contention Resolution ID CE. No space for a subheader");
            }
            msg3_tti = -1;
          }

          bool has_single_sdu = false;

          // allocate SDUs
          while (buf_state > 0) { // there is pending SDU to allocate
            if (mac_msg_dl.new_subh()) {
              int n = mac_msg_dl.get()->set_sdu(lcid, buf_state, &cells[pcell_idx]->rlc);
              if (n == -1) {
                logger.error("Error while adding SDU (%d B) to MAC PDU", buf_state);
                mac_msg_dl.del_subh();
              }

              // update buffer state
              buf_state = cells[pcell_idx]->rlc.get_buffer_state(lcid);

              if (mac_msg_dl.nof_subh() == 1) {
                has_single_sdu = true;
              } else {
                has_single_sdu = false;
              }
            }
          }

          // Assemble entire MAC PDU
          uint8_t* mac_pdu_ptr = mac_msg_dl.write_packet(logger);
          if (mac_pdu_ptr != nullptr) {
            if (force_lcid != -1 && lcid == 0) {
              if (has_single_sdu) {
                logger.info("Patched lcid in mac header to: %d", force_lcid);
                mac_pdu_ptr[0] = (mac_pdu_ptr[0] & 0xe0) | (force_lcid & 0x1f);
              } else if (mac_msg_dl.nof_subh() > 1) {
                logger.warning(
                    "Not patching lcid to %d in mac header (nof_subh == %d)", force_lcid, mac_msg_dl.nof_subh());
              }
            }
            logger.info(mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU (%d B):", mac_msg_dl.get_pdu_len());

            // Prepare MAC grant for CCCH
            mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
            dl_grant.tti                                   = tti;
            dl_grant.pid                                   = get_pid(tti);
            dl_grant.rnti                                  = dl_rnti;
            dl_grant.tb[0].tbs                             = mac_msg_dl.get_pdu_len();
            dl_grant.tb[0].ndi_present                     = true;
            dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);

            ue->new_tb(dl_grant, (const uint8_t*)mac_pdu_ptr);
          } else {
            logger.error("Error writing DL MAC PDU");
          }
          mac_msg_dl.reset();

        } else if (cells[pcell_idx]->bearer_follow_on_map[lcid]) {
          logger.info("Waiting for more PDUs for transmission on LCID=%d", lcid);
        }
      }
      // Check if we need to provide a UL grant as well
    }
  } else {
    logger.debug("Not handling RNTI=%d", dl_rnti);
  }
}

void ttcn3_syssim::stop()
{
  running = false;
  ue->stop();
}

void ttcn3_syssim::reset()
{
  std::lock_guard<std::mutex> lock(syssim_mutex);
  logger.info("Resetting SS");
  cells.clear();
  pcell_idx = -1;
}

// Called from UT before starting testcase
void ttcn3_syssim::tc_start(const char* name)
{
  // strip testsuite name
  std::string tc_name = get_tc_name(name);

  // Make a copy of the UE args for this run
  all_args_t local_args = args;

  auto* swp_sink = srslog::find_sink(swappable_sink::name());
  if (!swp_sink) {
    logger.error("Unable to find the swappable sink");
    srsran::console("Unable to find the swappable sink\n");
    return;
  }

  // set up logging
  if (args.log.filename == "stdout") {
    static_cast<swappable_sink*>(swp_sink)->swap_to_stdout();
  } else {
    const std::string& file_tc_name = get_filename_with_tc_name(local_args.log.filename, run_id, tc_name);
    static_cast<swappable_sink*>(swp_sink)->swap_sink(file_tc_name);
  }

  logger.info("Initializing UE ID=%d for TC=%s", run_id, tc_name.c_str());
  srsran::console("Initializing UE ID=%d for TC=%s\n", run_id, tc_name.c_str());

  // Patch UE config
  local_args.stack.pkt_trace.mac_pcap.filename =
      get_filename_with_tc_name(args.stack.pkt_trace.mac_pcap.filename, run_id, tc_name);
  local_args.stack.pkt_trace.nas_pcap.filename =
      get_filename_with_tc_name(args.stack.pkt_trace.nas_pcap.filename, run_id, tc_name);

  // bring up UE
  if (ue->init(local_args, this, tc_name)) {
    ue->stop();
    std::string err("Couldn't initialize UE.");
    logger.error("%s", err.c_str());
    srsran::console("%s\n", err.c_str());
    return;
  }

  // create and add TTI timer to epoll
  if (add_epoll(timer_handler.get_timer_fd(), epoll_fd) != SRSRAN_SUCCESS) {
    logger.error("Error while adding TTI timer to epoll");
  }
  event_handler.insert({timer_handler.get_timer_fd(), &timer_handler});
}

// Called from UT to terminate the testcase
void ttcn3_syssim::tc_end()
{
  logger.info("Deinitializing UE ID=%d", run_id);
  srsran::console("Deinitializing UE ID=%d\n", run_id);
  ue->stop();

  // stop TTI timer
  del_epoll(timer_handler.get_timer_fd(), epoll_fd);

  run_id++;

  // Reset SS
  reset();
}

void ttcn3_syssim::power_off_ue()
{
  // only return after new UE instance is up and running
}

// Called from outside
void ttcn3_syssim::switch_on_ue()
{
  event_queue.push(UE_SWITCH_ON);
}

void ttcn3_syssim::switch_off_ue()
{
  event_queue.push(UE_SWITCH_OFF);
}

void ttcn3_syssim::enable_data()
{
  event_queue.push(ENABLE_DATA);
}

void ttcn3_syssim::disable_data()
{
  event_queue.push(DISABLE_DATA);
}

// Called from PHY through RA procedure running on Stack thread
void ttcn3_syssim::prach_indication(uint32_t preamble_index_, const uint32_t& cell_id)
{
  std::lock_guard<std::mutex> lock(syssim_mutex);

  // verify that UE intends to send PRACH on current Pcell
  if (cells[pcell_idx]->config.phy_cell.id != cell_id) {
    logger.error(
        "UE is attempting to PRACH on pci=%d, current Pcell=%d", cell_id, cells[pcell_idx]->config.phy_cell.id);
    return;
  }

  // store TTI for providing UL grant for Msg3 transmission
  prach_tti            = tti;
  prach_preamble_index = preamble_index_;
}

// Called from PHY
void ttcn3_syssim::sr_req(uint32_t tti_tx)
{
  logger.info("Received SR from PHY");
  sr_tti = tti_tx;
}

// Called from PHY
void ttcn3_syssim::tx_pdu(const uint8_t* payload, const int len, const uint32_t tx_tti)
{
  if (payload == NULL) {
    ss_mac_logger.error("Received NULL as PDU payload. Dropping.");
    return;
  }

  if (pcell_idx == -1) {
    logger.debug("Skipping TTI. Pcell not yet selected.");
    return;
  }

  logger.info(payload, len, "UL MAC PDU (%d B):", len);

  // Parse MAC
  mac_msg_ul.init_rx(len, true);
  mac_msg_ul.parse_packet((uint8_t*)payload);

  while (mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (mac_msg_ul.get()->is_sdu()) {
      // Push PDU to our own RLC (needed to handle status reporting, etc. correctly
      ss_mac_logger.info(mac_msg_ul.get()->get_sdu_ptr(),
                         mac_msg_ul.get()->get_payload_size(),
                         "Route UL PDU to LCID=%d (%d B)",
                         mac_msg_ul.get()->get_sdu_lcid(),
                         mac_msg_ul.get()->get_payload_size());
      cells[pcell_idx]->rlc.write_pdu(
          mac_msg_ul.get()->get_sdu_lcid(), mac_msg_ul.get()->get_sdu_ptr(), mac_msg_ul.get()->get_payload_size());

      // Save contention resolution if lcid == 0
      if (mac_msg_ul.get()->get_sdu_lcid() == 0) {
        int nbytes = srsran::sch_subh::MAC_CE_CONTRES_LEN;
        if (mac_msg_ul.get()->get_payload_size() >= (uint32_t)nbytes) {
          uint8_t* ue_cri_ptr = (uint8_t*)&conres_id;
          uint8_t* pkt_ptr    = mac_msg_ul.get()->get_sdu_ptr(); // Warning here: we want to include the
          for (int i = 0; i < nbytes; i++) {
            ue_cri_ptr[nbytes - i - 1] = pkt_ptr[i];
          }
          ss_mac_logger.info(ue_cri_ptr, nbytes, "Contention resolution ID:");
        } else {
          ss_mac_logger.error("Received CCCH UL message of invalid size=%d bytes",
                              mac_msg_ul.get()->get_payload_size());
        }
      }
    }
  }
  mac_msg_ul.reset();

  // Process CE after all SDUs because we need to update BSR after
  bool bsr_received = false;
  while (mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (!mac_msg_ul.get()->is_sdu()) {
      // Process MAC Control Element
      bsr_received |= process_ce(mac_msg_ul.get());
    }
  }
}

// Internal function called from main thread
void ttcn3_syssim::send_rar(uint32_t preamble_index)
{
  logger.info("Sending RAR for RAPID=%d", preamble_index);

  // Prepare RAR grant
  uint8_t                grant_buffer[64] = {};
  srsran_dci_rar_grant_t rar_grant        = {};
  rar_grant.tpc_pusch                     = 3;
  srsran_dci_rar_pack(&rar_grant, grant_buffer);

  // Create MAC PDU and add RAR subheader
  srsran::rar_pdu rar_pdu;
  rar_buffer.clear();

  const int rar_pdu_len = 64;
  rar_pdu.init_tx(&rar_buffer, rar_pdu_len);
  rar_pdu.set_backoff(11); // Backoff of 480ms to prevent UE from PRACHing too fast
  if (rar_pdu.new_subh()) {
    rar_pdu.get()->set_rapid(preamble_index);
    rar_pdu.get()->set_ta_cmd(0);
    rar_pdu.get()->set_temp_crnti(cells[pcell_idx]->config.temp_crnti);
    rar_pdu.get()->set_sched_grant(grant_buffer);
  }
  rar_pdu.write_packet(rar_buffer.msg);
  rar_buffer.N_bytes = rar_pdu_len;

  // Prepare grant and pass all to MAC
  mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
  dl_grant.tti                                   = tti;
  dl_grant.pid                                   = get_pid(tti);
  dl_grant.rnti                                  = 0x1; // must be a valid RAR-RNTI
  dl_grant.tb[0].tbs                             = rar_buffer.N_bytes;
  dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);

  // send grant and pass payload to TB data (grant contains length)
  ue->new_tb(dl_grant, rar_buffer.msg);

  // reset last PRACH transmission tti
  prach_tti = -1;
}

// Internal function called from main thread
void ttcn3_syssim::send_msg3_grant()
{
  logger.info("Sending Msg3 grant for C-RNTI=0x%x", cells[pcell_idx]->config.crnti);
  mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};

  ul_grant.tti_tx         = (tti + 3) % 10240;
  ul_grant.tb.tbs         = 32;
  ul_grant.tb.ndi_present = true;
  ul_grant.tb.ndi         = get_ndi_for_new_ul_tx(tti);
  ul_grant.rnti           = cells[pcell_idx]->config.crnti;
  ul_grant.pid            = get_pid(tti);
  ul_grant.is_rar         = true;

  ue->new_grant_ul(ul_grant);
}

// Internal function called from main thread
void ttcn3_syssim::send_sr_ul_grant()
{
  // Provide new UL grant to UE
  mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};
  ul_grant.tti_tx                                = (tti + 3) % 10240;
  ul_grant.tb.tbs                                = 100; // TODO: reasonable size?
  ul_grant.tb.ndi_present                        = true;
  ul_grant.tb.ndi                                = get_ndi_for_new_ul_tx(tti);
  ul_grant.rnti                                  = cells[pcell_idx]->config.crnti;
  ul_grant.pid                                   = get_pid(tti);

  ue->new_grant_ul(ul_grant);

  sr_tti = -1;
}

// internal function called from tx_pdu (called from main thread)
bool ttcn3_syssim::process_ce(srsran::sch_subh* subh)
{
  uint16_t rnti = dl_rnti;

  uint32_t buff_size_idx[4]   = {};
  uint32_t buff_size_bytes[4] = {};
  float    phr                = 0;
  int32_t  idx                = 0;
  uint16_t old_rnti           = 0;
  bool     is_bsr             = false;
  switch (subh->ul_sch_ce_type()) {
    case srsran::ul_sch_lcid::PHR_REPORT:
      phr = subh->get_phr();
      ss_mac_logger.info("CE:    Received PHR from rnti=0x%x, value=%.0f", rnti, phr);
      break;
    case srsran::ul_sch_lcid::CRNTI:
      old_rnti = subh->get_c_rnti();
      ss_mac_logger.info("CE:    Received C-RNTI from temp_rnti=0x%x, rnti=0x%x", rnti, old_rnti);
      break;
    case srsran::ul_sch_lcid::TRUNC_BSR:
    case srsran::ul_sch_lcid::SHORT_BSR:
      idx = subh->get_bsr(buff_size_idx, buff_size_bytes);
      if (idx == -1) {
        ss_mac_logger.error("Invalid Index Passed to lc groups");
        break;
      }
      ss_mac_logger.info("CE:    Received %s BSR rnti=0x%x, lcg=%d, value=%d",
                         subh->ul_sch_ce_type() == srsran::ul_sch_lcid::SHORT_BSR ? "Short" : "Trunc",
                         rnti,
                         idx,
                         buff_size_idx[idx]);
      is_bsr = true;
      break;
    case srsran::ul_sch_lcid::LONG_BSR:
      subh->get_bsr(buff_size_idx, buff_size_bytes);
      is_bsr = true;
      ss_mac_logger.info("CE:    Received Long BSR rnti=0x%x, value=%d,%d,%d,%d",
                         rnti,
                         buff_size_idx[0],
                         buff_size_idx[1],
                         buff_size_idx[2],
                         buff_size_idx[3]);
      break;
    case srsran::ul_sch_lcid::PADDING:
      ss_mac_logger.debug("CE:    Received padding for rnti=0x%x", rnti);
      break;
    default:
      ss_mac_logger.error("CE:    Invalid lcid=0x%x", subh->lcid_value());
      break;
  }
  return is_bsr;
}

uint32_t ttcn3_syssim::get_pid(const uint32_t tti_)
{
  return tti_ % SRSRAN_FDD_NOF_HARQ;
}

bool ttcn3_syssim::get_ndi_for_new_ul_tx(const uint32_t tti_)
{
  // toggle NDI to always create new Tx
  const uint32_t pid = get_pid(tti_);
  last_ul_ndi[pid]   = !last_ul_ndi[pid];
  logger.info("UL-PID=%d NDI=%s", pid, last_ul_ndi[pid] ? "1" : "0");
  return last_ul_ndi[pid];
}

bool ttcn3_syssim::get_ndi_for_new_dl_tx(const uint32_t tti_)
{
  // toggle NDI to always create new Tx
  const uint32_t pid = get_pid(tti_);
  last_dl_ndi[pid]   = !last_dl_ndi[pid];
  logger.info("DL-PID=%d NDI=%s", pid, last_dl_ndi[pid] ? "1" : "0");
  return last_dl_ndi[pid];
}

int ttcn3_syssim::run()
{
  running = true;

  while (running) {
    // wait for event
    const int32_t      epoll_timeout_ms   = -1;
    const uint32_t     MAX_EVENTS         = 1;
    struct epoll_event events[MAX_EVENTS] = {};
    int                nof_events         = epoll_wait(epoll_fd, events, MAX_EVENTS, epoll_timeout_ms);

    // handle event
    if (nof_events == -1) {
      perror("epoll_wait() error");
      continue;
    }
    if (nof_events == 0) {
      printf("time out %f sec expired\n", epoll_timeout_ms / 1000.0);
      continue;
    }

    for (int i = 0; i < nof_events; ++i) {
      if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
        ///< An error has occured on this fd, or the socket is not ready for reading
        fprintf(stderr, "epoll error\n");
        close(events[i].data.fd);
        continue;
      }

      int fd = events[i].data.fd;
      if (event_handler.find(fd) != event_handler.end()) {
        event_handler[fd]->handle_event(fd, events[i], epoll_fd);
      }
    }
  }
  return SRSRAN_SUCCESS;
}

uint32_t ttcn3_syssim::get_tti()
{
  return tti;
}

void ttcn3_syssim::process_pdu(uint8_t* buff, uint32_t len, pdu_queue::channel_t channel, int ul_nof_prbs) {}

void ttcn3_syssim::set_cell_config(const ttcn3_helpers::timing_info_t timing, const cell_config_t cell)
{
  if (timing.now) {
    set_cell_config_impl(cell);
  } else {
    logger.debug("Scheduling Cell configuration of %s for TTI=%d", cell.name.c_str(), timing.tti);
    tti_actions[timing.tti].push([this, cell]() { set_cell_config_impl(cell); });
  }
}

void ttcn3_syssim::set_cell_config_impl(const cell_config_t config)
{
  // check if cell already exists
  if (not syssim_has_cell(config.name)) {
    // insert new cell
    logger.info("Adding cell %s with cellId=%d and initial_power=%.2f dBm",
                config.name.c_str(),
                config.phy_cell.id,
                config.initial_power);
    unique_syssim_cell_t cell = unique_syssim_cell_t(new syssim_cell_t(this));
    cell->config              = config;

    // init RLC and PDCP
    cell->rlc.init(&cell->pdcp, this, stack.task_sched.get_timer_handler(), 0 /* RB_ID_SRB0 */);
    cell->pdcp.init(&cell->rlc, this, this);

    cells.push_back(std::move(cell));
  } else {
    // cell is already there
    logger.info("Cell already there, reconfigure");
    // We only support (Temp-)CRNTI reconfiguration
    syssim_cell_t* ss_cell = get_cell(config.name);
    if (config.crnti > 0) {
      ss_cell->config.crnti = config.crnti;
    }
    if (config.temp_crnti > 0) {
      ss_cell->config.temp_crnti = config.temp_crnti;
    }
  }

  update_cell_map();
}

// internal function
bool ttcn3_syssim::syssim_has_cell(const std::string cell_name)
{
  for (auto& cell : cells) {
    if (cell->config.name == cell_name) {
      return true;
    }
  }
  return false;
}

ttcn3_syssim::syssim_cell_t* ttcn3_syssim::get_cell(const std::string cell_name)
{
  for (auto& cell : cells) {
    if (cell->config.name == cell_name) {
      return cell.get();
    }
  }
  return nullptr;
}

void ttcn3_syssim::set_cell_attenuation(const ttcn3_helpers::timing_info_t timing,
                                        const std::string                  cell_name,
                                        const float                        value)
{
  if (timing.now) {
    set_cell_attenuation_impl(cell_name, value);
  } else {
    logger.debug("Scheduling Cell attenuation reconfiguration of %s for TTI=%d", cell_name.c_str(), timing.tti);
    tti_actions[timing.tti].push([this, cell_name, value]() { set_cell_attenuation_impl(cell_name, value); });
  }
}

void ttcn3_syssim::set_cell_attenuation_impl(const std::string cell_name, const float value)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't set cell power. Cell not found.");
    return;
  }

  // update cell's power
  auto cell                = get_cell(cell_name);
  cell->config.attenuation = value;

  update_cell_map();
}

// Internal function
void ttcn3_syssim::update_cell_map()
{
  // convert syssim cell list to phy cell list
  lte_ttcn3_phy::cell_list_t phy_cells;
  for (auto& ss_cell : cells) {
    lte_ttcn3_phy::cell_t phy_cell = {};
    phy_cell.info                  = ss_cell->config.phy_cell;
    phy_cell.power                 = ss_cell->config.initial_power - ss_cell->config.attenuation;
    phy_cell.earfcn                = ss_cell->config.earfcn;
    logger.info("Configuring cell with PCI=%d with TxPower=%.2f", phy_cell.info.id, phy_cell.power);
    phy_cells.push_back(phy_cell);
  }

  // SYSSIM defines what cells the UE can connect to
  ue->set_cell_map(phy_cells);
}

bool ttcn3_syssim::have_valid_pcell()
{
  return (pcell_idx >= 0 && pcell_idx < static_cast<int>(cells.size()));
}

void ttcn3_syssim::add_bcch_dlsch_pdu(const string cell_name, unique_byte_buffer_t pdu)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't add BCCH to cell. Cell not found.");
    return;
  }

  auto cell = get_cell(cell_name);
  cell->sibs.push_back(std::move(pdu));
}

void ttcn3_syssim::add_ccch_pdu(const ttcn3_helpers::timing_info_t timing,
                                const std::string                  cell_name,
                                unique_byte_buffer_t               pdu)
{
  if (timing.now) {
    // Add to SRB0 Tx queue
    add_ccch_pdu_impl(cell_name, std::move(pdu));
  } else {
    logger.debug("Scheduling CCCH PDU for TTI=%d", timing.tti);
    auto task = [this, cell_name](srsran::unique_byte_buffer_t& pdu) { add_ccch_pdu_impl(cell_name, std::move(pdu)); };
    tti_actions[timing.tti].push(std::bind(task, std::move(pdu)));
  }
}

void ttcn3_syssim::add_ccch_pdu_impl(const std::string cell_name, unique_byte_buffer_t pdu)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't add CCCH to cell. Cell not found.");
    return;
  }

  auto cell = get_cell(cell_name);
  // Add to SRB0 Tx queue
  cell->rlc.write_sdu(0, std::move(pdu));
}

void ttcn3_syssim::add_dcch_pdu(const ttcn3_helpers::timing_info_t timing,
                                const std::string                  cell_name,
                                uint32_t                           lcid,
                                unique_byte_buffer_t               pdu,
                                bool                               follow_on_flag)
{
  if (timing.now) {
    add_dcch_pdu_impl(cell_name, lcid, std::move(pdu), follow_on_flag);
  } else {
    logger.debug("Scheduling DCCH PDU for TTI=%d", timing.tti);
    auto task = [this, cell_name](uint32_t lcid, srsran::unique_byte_buffer_t& pdu, bool follow_on_flag) {
      add_dcch_pdu_impl(cell_name, lcid, std::move(pdu), follow_on_flag);
    };
    tti_actions[timing.tti].push(std::bind(task, lcid, std::move(pdu), follow_on_flag));
  }
}

void ttcn3_syssim::add_dcch_pdu_impl(const std::string    cell_name,
                                     uint32_t             lcid,
                                     unique_byte_buffer_t pdu,
                                     bool                 follow_on_flag)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't add CCCH to cell. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  // push to PDCP and create DL grant for it
  logger.info("Writing PDU (%d B) to LCID=%d", pdu->N_bytes, lcid);
  cell->pdcp.write_sdu(lcid, std::move(pdu));
  cell->bearer_follow_on_map[lcid] = follow_on_flag;
}

void ttcn3_syssim::add_pch_pdu(unique_byte_buffer_t pdu)
{
  logger.info("Received PCH PDU (%d B)", pdu->N_bytes);

  // Prepare MAC grant for PCH
  mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
  dl_grant.tti                                   = tti;
  dl_grant.pid                                   = get_pid(tti);
  dl_grant.rnti                                  = SRSRAN_PRNTI;
  dl_grant.tb[0].tbs                             = pdu->N_bytes;
  dl_grant.tb[0].ndi_present                     = true;
  dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
  ue->new_tb(dl_grant, (const uint8_t*)pdu->msg);
}

void ttcn3_syssim::step_stack()
{
  stack.run_tti();
}

void ttcn3_syssim::add_srb(const ttcn3_helpers::timing_info_t timing,
                           const std::string                  cell_name,
                           const uint32_t                     lcid,
                           const pdcp_config_t                pdcp_config)
{
  if (timing.now) {
    add_srb_impl(cell_name, lcid, pdcp_config);
  } else {
    logger.debug("Scheduling SRB%d addition for TTI=%d", lcid, timing.tti);
    tti_actions[timing.tti].push(
        [this, cell_name, lcid, pdcp_config]() { add_srb_impl(cell_name, lcid, pdcp_config); });
  }
}

void ttcn3_syssim::add_srb_impl(const std::string cell_name, const uint32_t lcid, const pdcp_config_t pdcp_config)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't add SRB to cell. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  logger.info("Adding SRB%d", lcid);
  cell->pdcp.add_bearer(lcid, pdcp_config);
  cell->rlc.add_bearer(lcid, srsran::rlc_config_t::srb_config(lcid));
}

void ttcn3_syssim::reestablish_bearer(const std::string cell_name, const uint32_t lcid)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't reestablish bearer. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  logger.info("Reestablishing LCID=%d", lcid);
  cell->pdcp.reestablish(lcid);
  cell->rlc.reestablish(lcid);
}

void ttcn3_syssim::del_srb(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const uint32_t lcid)
{
  if (timing.now) {
    del_srb_impl(cell_name, lcid);
  } else {
    logger.debug("Scheduling SRB%d deletion for TTI=%d", lcid, timing.tti);
    tti_actions[timing.tti].push([this, cell_name, lcid]() { del_srb_impl(cell_name, lcid); });
  }
}

void ttcn3_syssim::del_srb_impl(const std::string cell_name, const uint32_t lcid)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't delete bearer. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  logger.info("Deleting SRB%d", lcid);
  // Only delete SRB1/2
  if (lcid > 0) {
    cell->pdcp.del_bearer(lcid);
    cell->rlc.del_bearer(lcid);
  }
}

void ttcn3_syssim::add_drb(const ttcn3_helpers::timing_info_t timing,
                           const std::string                  cell_name,
                           const uint32_t                     lcid,
                           const srsran::pdcp_config_t        pdcp_config)
{
  if (timing.now) {
    add_drb_impl(cell_name, lcid, pdcp_config);
  } else {
    logger.debug("Scheduling DRB%d addition for TTI=%d", lcid - 2, timing.tti);
    tti_actions[timing.tti].push(
        [this, cell_name, lcid, pdcp_config]() { add_drb_impl(cell_name, lcid, pdcp_config); });
  }
}

void ttcn3_syssim::add_drb_impl(const std::string cell_name, const uint32_t lcid, const pdcp_config_t pdcp_config)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't add DRB. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  if (lcid > 2) {
    logger.info("Adding DRB%d", lcid - 2);
    cell->pdcp.add_bearer(lcid, pdcp_config);
    cell->rlc.add_bearer(lcid, srsran::rlc_config_t::default_rlc_am_config());
  }
}

void ttcn3_syssim::del_drb(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const uint32_t lcid)
{
  if (timing.now) {
    del_drb_impl(cell_name, lcid);
  } else {
    logger.debug("Scheduling DRB%d deletion for TTI=%d", lcid - 2, timing.tti);
    tti_actions[timing.tti].push([this, cell_name, lcid]() { del_drb_impl(cell_name, lcid); });
  }
}

void ttcn3_syssim::del_drb_impl(const std::string cell_name, const uint32_t lcid)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't delete DRB. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  // Only delete DRB
  if (lcid > 2) {
    logger.info("Deleting DRB%d", lcid - 2);
    cell->pdcp.del_bearer(lcid);
    cell->rlc.del_bearer(lcid);
  }
}

// RRC interface for PDCP, PDCP calls RRC to push RRC SDU
void ttcn3_syssim::write_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg,
              pdu->N_bytes,
              "RRC SDU received for LCID=%d cell_id=%d (%d B)",
              lcid,
              cells[pcell_idx]->config.phy_cell.id,
              pdu->N_bytes);

  // push content to Titan
  if (lcid <= 2) {
    std::string out = ttcn3_helpers::get_rrc_pdu_ind_for_pdu(tti, lcid, cells[pcell_idx]->config.name, std::move(pdu));
    srb.tx(reinterpret_cast<const uint8_t*>(out.c_str()), out.length());
  } else {
    std::string out =
        ttcn3_helpers::get_drb_common_ind_for_pdu(tti, lcid, cells[pcell_idx]->config.name, std::move(pdu));
    drb.tx(reinterpret_cast<const uint8_t*>(out.c_str()), out.length());
  }
}

// Not supported right now
void ttcn3_syssim::write_pdu_bcch_bch(unique_byte_buffer_t pdu)
{
  logger.error("%s not implemented.", __FUNCTION__);
}
void ttcn3_syssim::write_pdu_bcch_dlsch(unique_byte_buffer_t pdu)
{
  logger.error("%s not implemented.", __FUNCTION__);
}
void ttcn3_syssim::write_pdu_pcch(unique_byte_buffer_t pdu)
{
  logger.error("%s not implemented.", __FUNCTION__);
}
void ttcn3_syssim::write_pdu_mch(uint32_t lcid, unique_byte_buffer_t pdu)
{
  logger.error("%s not implemented.", __FUNCTION__);
}
void ttcn3_syssim::max_retx_attempted()
{
  logger.error("%s not implemented.", __FUNCTION__);
}
void ttcn3_syssim::protocol_failure()
{
  logger.error("%s not implemented.", __FUNCTION__);
}

void ttcn3_syssim::notify_pdcp_integrity_error(uint32_t lcid)
{
  logger.error("%s not implemented.", __FUNCTION__);
}

const char* ttcn3_syssim::get_rb_name(uint32_t lcid)
{
  if (lcid < rb_id_vec.size()) {
    return rb_id_vec.at(lcid).c_str();
  }
  return "RB";
};

void ttcn3_syssim::write_sdu(uint32_t lcid, unique_byte_buffer_t sdu)
{
  logger.info(sdu->msg, sdu->N_bytes, "Received SDU on LCID=%d", lcid);

  uint8_t* mac_pdu_ptr = mac_msg_dl.write_packet(logger);
  logger.info(mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU:");

  // Prepare MAC grant for CCCH
  mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
  dl_grant.pid                                   = get_pid(tti);
  dl_grant.tti                                   = tti;
  dl_grant.rnti                                  = dl_rnti;
  dl_grant.tb[0].tbs                             = mac_msg_dl.get_pdu_len();
  dl_grant.tb[0].ndi_present                     = true;
  dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);

  // Pass to UE
  ue->new_tb(dl_grant, (const uint8_t*)mac_pdu_ptr);
}

void ttcn3_syssim::discard_sdu(uint32_t lcid, uint32_t sn) {}

bool ttcn3_syssim::rb_is_um(uint32_t lcid)
{
  return false;
}

bool ttcn3_syssim::sdu_queue_is_full(uint32_t lcid)
{
  return false;
}

bool ttcn3_syssim::is_suspended(uint32_t lcid)
{
  return false;
}

void ttcn3_syssim::set_as_security(const ttcn3_helpers::timing_info_t        timing,
                                   const std::string                         cell_name,
                                   std::array<uint8_t, 32>                   k_rrc_enc_,
                                   std::array<uint8_t, 32>                   k_rrc_int_,
                                   std::array<uint8_t, 32>                   k_up_enc_,
                                   const srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                   const srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                                   const ttcn3_helpers::pdcp_count_map_t     bearers_)
{
  if (timing.now) {
    set_as_security_impl(cell_name, k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_);
  } else {
    logger.debug("Scheduling AS security configuration for TTI=%d", timing.tti);
    tti_actions[timing.tti].push(
        [this, cell_name, k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_]() {
          set_as_security_impl(cell_name, k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_);
        });
  }
}

void ttcn3_syssim::set_as_security_impl(const std::string                         cell_name,
                                        std::array<uint8_t, 32>                   k_rrc_enc_,
                                        std::array<uint8_t, 32>                   k_rrc_int_,
                                        std::array<uint8_t, 32>                   k_up_enc_,
                                        const srsran::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                        const srsran::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                                        const ttcn3_helpers::pdcp_count_map_t     bearers)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't configure AS security. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  // store security config for later use (i.e. new bearer added)
  cell->sec_cfg = {.k_rrc_int   = k_rrc_int_,
                   .k_rrc_enc   = k_rrc_enc_,
                   .k_up_int    = {},
                   .k_up_enc    = k_up_enc_,
                   .integ_algo  = integ_algo_,
                   .cipher_algo = cipher_algo_};

  for (auto& lcid : bearers) {
    cell->pdcp.config_security(lcid.rb_id, cell->sec_cfg);

    logger.info("Setting AS security for LCID=%d in DL direction from SN=%d onwards", lcid.rb_id, lcid.dl_value);
    cell->pdcp.enable_security_timed(lcid.rb_id, DIRECTION_TX, lcid.dl_value);

    logger.info("Setting AS security for LCID=%d in UL direction from SN=%d onwards", lcid.rb_id, lcid.ul_value);
    cell->pdcp.enable_security_timed(lcid.rb_id, DIRECTION_RX, lcid.ul_value);
  }
}

void ttcn3_syssim::release_as_security(const ttcn3_helpers::timing_info_t timing, const std::string cell_name)
{
  if (timing.now) {
    release_as_security_impl(cell_name);
  } else {
    logger.debug("Scheduling Release of AS security for TTI=%d", timing.tti);
    tti_actions[timing.tti].push([this, cell_name]() { release_as_security_impl(cell_name); });
  }
}

void ttcn3_syssim::release_as_security_impl(const std::string cell_name)
{
  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't release AS security. Cell %s not found.", cell_name.c_str());
    return;
  }

  auto cell = get_cell(cell_name);

  logger.info("Releasing AS security");
  cell->pending_bearer_config.clear();
}

// Called from PHY in Stack context
void ttcn3_syssim::select_cell(srsran_cell_t phy_cell)
{
  std::lock_guard<std::mutex> lock(syssim_mutex);
  // find matching cell in SS cell list
  for (uint32_t i = 0; i < cells.size(); ++i) {
    if (cells[i]->config.phy_cell.id == phy_cell.id) {
      pcell_idx = i;
      logger.info("New PCell: PCI=%d", cells[pcell_idx]->config.phy_cell.id);
      return;
    }
  }
}

ttcn3_helpers::pdcp_count_map_t ttcn3_syssim::get_pdcp_count(const std::string cell_name)
{
  // prepare response to SS
  std::vector<ttcn3_helpers::pdcp_count_t> bearers;

  if (not syssim_has_cell(cell_name)) {
    logger.error("Can't obtain PDCP count. Cell %s not found.", cell_name.c_str());
    return bearers;
  }

  auto cell = get_cell(cell_name);

  for (uint32_t i = 0; i < rb_id_vec.size(); i++) {
    if (cell->pdcp.is_lcid_enabled(i)) {
      ttcn3_helpers::pdcp_count_t bearer;
      uint16_t                    tmp; // not handling HFN
      srsran::pdcp_lte_state_t    pdcp_state;
      cell->pdcp.get_bearer_state(i, &pdcp_state);
      bearer.rb_is_srb = i <= 2;
      bearer.rb_id     = i;
      bearer.dl_value  = pdcp_state.next_pdcp_tx_sn;
      bearer.ul_value  = pdcp_state.next_pdcp_rx_sn;
      logger.debug("PDCP count lcid=%d, dl=%d, ul=%d", bearer.rb_id, bearer.dl_value, bearer.ul_value);
      bearers.push_back(bearer);
    }
  }
  return bearers;
}
