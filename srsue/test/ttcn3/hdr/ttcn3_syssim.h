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

#ifndef SRSUE_TTCN3_SYSSIM_H
#define SRSUE_TTCN3_SYSSIM_H

#include "dut_utils.h"
#include "srslte/mac/pdu_queue.h"
#include "srslte/test/ue_test_interfaces.h"
#include "srslte/upper/pdcp.h"
#include "srslte/upper/rlc.h"
#include "ttcn3_common.h"
#include "ttcn3_drb_interface.h"
#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ue.h"
#include "ttcn3_ut_interface.h"
#include <functional>

#define TTCN3_CRNTI (0x1001)

class ttcn3_syssim : public syssim_interface_phy,
                     public ss_ut_interface,
                     public ss_sys_interface,
                     public ss_srb_interface,
                     public rrc_interface_rlc,
                     public rlc_interface_pdcp,
                     public rrc_interface_pdcp,
                     public srslte::pdu_queue::process_callback
{
public:
  ttcn3_syssim(srslte::logger_file* logger_file_, ttcn3_ue* ue_) :
    log{"SS  "},
    mac_msg_ul(20, ss_mac_log),
    mac_msg_dl(20, ss_mac_log),
    pdus(128),
    logger(logger_file_),
    logger_file(logger_file_),
    pool(byte_buffer_pool::get_instance()),
    ue(ue_),
    rlc(ss_rlc_log->get_service_name().c_str()),
    signal_handler(&running),
    timer_handler(create_tti_timer(), [&](uint64_t res) { new_tti_indication(res); }),
    pdcp(&stack, ss_pdcp_log->get_service_name().c_str())
  {
    if (ue->init(all_args_t{}, logger, this, "INIT_TEST") != SRSLTE_SUCCESS) {
      ue->stop();
      fprintf(stderr, "Couldn't initialize UE.\n");
    }
  }

  ~ttcn3_syssim(){};

  int init(const all_args_t& args_)
  {
    args = args_;

    // Make sure to get SS logging as well
    if (args.log.filename == "stdout") {
      logger = &logger_stdout;
    }
    srslte::logmap::set_default_logger(logger);

    // init and configure logging
    srslte::logmap::register_log(std::unique_ptr<srslte::log>{new log_filter{"SS  ", logger, true}});
    ut_log.init("UT  ", logger);
    sys_log.init("SYS ", logger);
    ip_sock_log.init("IP_S", logger);
    ip_ctrl_log.init("IP_C", logger);
    srb_log.init("SRB  ", logger);
    drb_log.init("DRB  ", logger);
    srslte::logmap::register_log(std::unique_ptr<srslte::log>{new log_filter{"SS-RLC", logger}});
    srslte::logmap::register_log(std::unique_ptr<srslte::log>{new log_filter{"SS-PDCP", logger}});

    log->set_level(args.log.all_level);
    ut_log.set_level(args.log.all_level);
    sys_log.set_level(args.log.all_level);
    ip_sock_log.set_level(args.log.all_level);
    ip_ctrl_log.set_level(args.log.all_level);
    srb_log.set_level(args.log.all_level);
    drb_log.set_level(args.log.all_level);
    ss_mac_log->set_level(args.log.all_level);
    ss_rlc_log->set_level(args.log.all_level);
    ss_pdcp_log->set_level(args.log.all_level);

    log->set_hex_limit(args.log.all_hex_limit);
    ut_log.set_hex_limit(args.log.all_hex_limit);
    sys_log.set_hex_limit(args.log.all_hex_limit);
    ip_sock_log.set_hex_limit(args.log.all_hex_limit);
    ip_ctrl_log.set_hex_limit(args.log.all_hex_limit);
    srb_log.set_hex_limit(args.log.all_hex_limit);
    drb_log.set_hex_limit(args.log.all_hex_limit);
    ss_mac_log->set_hex_limit(args.log.all_hex_limit);
    ss_rlc_log->set_hex_limit(args.log.all_hex_limit);
    ss_pdcp_log->set_hex_limit(args.log.all_hex_limit);

    // Init epoll socket and add FDs
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
      log->error("Error creating epoll\n");
      return SRSLTE_ERROR;
    }

    // add signalfd
    signal_fd = add_signalfd();
    if (add_epoll(signal_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding signalfd to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({signal_fd, &signal_handler});

    // init system interfaces to tester
    if (add_port_handler() != SRSLTE_SUCCESS) {
      log->error("Error creating port handlers\n");
      return SRSLTE_ERROR;
    }

    // Init SS layers
    pdus.init(this, log);
    rlc.init(&pdcp, this, &stack.timers, 0 /* RB_ID_SRB0 */);
    pdcp.init(&rlc, this, nullptr);

    return SRSLTE_SUCCESS;
  }

  void set_forced_lcid(int lcid) { force_lcid = lcid; }

  int add_port_handler()
  {
    // UT port
    int ut_fd = ut.init(this, &ut_log, listen_address, UT_PORT);
    if (add_epoll(ut_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding UT port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({ut_fd, &ut});
    log->console("UT handler listening on SCTP port %d\n", UT_PORT);

    // SYS port
    int sys_fd = sys.init(this, &sys_log, listen_address, SYS_PORT);
    if (add_epoll(sys_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding SYS port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({sys_fd, &sys});
    log->console("SYS handler listening on SCTP port %d\n", SYS_PORT);

    // IPsock port
    int ip_sock_fd = ip_sock.init(&ip_sock_log, listen_address, IPSOCK_PORT);
    if (add_epoll(ip_sock_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding IP sock port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({ip_sock_fd, &ip_sock});
    log->console("IPSOCK handler listening on SCTP port %d\n", IPSOCK_PORT);

    // IPctrl port
    int ip_ctrl_fd = ip_ctrl.init(&ip_ctrl_log, listen_address, IPCTRL_PORT);
    if (add_epoll(ip_ctrl_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding IP ctrl port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({ip_ctrl_fd, &ip_ctrl});
    log->console("IPCTRL handler listening on SCTP port %d\n", IPCTRL_PORT);

    // add SRB fd
    int srb_fd = srb.init(this, &srb_log, listen_address, SRB_PORT);
    if (add_epoll(srb_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding SRB port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({srb_fd, &srb});
    log->console("SRB handler listening on SCTP port %d\n", SRB_PORT);

    // add DRB fd
    int drb_fd = drb.init(this, &drb_log, listen_address, DRB_PORT);
    if (add_epoll(drb_fd, epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding DRB port to epoll\n");
      return SRSLTE_ERROR;
    }
    event_handler.insert({drb_fd, &drb});
    log->console("DRB handler listening on SCTP port %d\n", DRB_PORT);

    return SRSLTE_SUCCESS;
  }

  ///< Function called by epoll timer handler when TTI timer expires
  void new_tti_indication(uint64_t res)
  {
    tti = (tti + 1) % 10240;

    log->step(tti);
    log->debug("Start TTI\n");

    // Make sure to step SS
    step_stack();

    // inform UE about new TTI
    ue->set_current_tti(tti);

    // check scheduled actions for this TTI
    if (tti_actions.find(tti) != tti_actions.end()) {
      while (!tti_actions[tti].empty()) {
        log->debug("Running scheduled action\n");
        move_task_t task = std::move(tti_actions[tti].back());
        task();
        tti_actions[tti].pop_back();
      }
    }

    // process events, if any
    while (not event_queue.empty()) {
      ss_events_t ev = event_queue.wait_pop();
      switch (ev) {
        case UE_SWITCH_ON:
          log->console("Switching on UE ID=%d\n", run_id);
          ue->switch_on();
          break;
        case UE_SWITCH_OFF:
          log->console("Switching off UE ID=%d\n", run_id);
          ue->switch_off();
          break;
        case ENABLE_DATA:
          log->console("Enabling data for UE ID=%d\n", run_id);
          ue->enable_data();
          break;
        case DISABLE_DATA:
          log->console("Disabling data for UE ID=%d\n", run_id);
          ue->disable_data();
          break;
      }
    }

    if (pcell_idx == -1) {
      log->debug("Skipping TTI. Pcell not yet selected.\n");
      return;
    }

    // DL/UL processing if UE has selected cell
    dl_rnti = ue->get_dl_sched_rnti(tti);
    if (SRSLTE_RNTI_ISSI(dl_rnti)) {
      // deliver SIBs one after another
      mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
      dl_grant.tti                                   = tti;
      dl_grant.pid                                   = get_pid(tti);
      dl_grant.rnti                                  = dl_rnti;
      dl_grant.tb[0].tbs                             = cells[pcell_idx]->sibs[cells[pcell_idx]->sib_idx]->N_bytes;
      dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
      ue->new_tb(dl_grant, cells[pcell_idx]->sibs[cells[pcell_idx]->sib_idx]->msg);
      log->info("Delivered SIB%d for pcell_idx=%d\n", cells[pcell_idx]->sib_idx, pcell_idx);
      cells[pcell_idx]->sib_idx = (cells[pcell_idx]->sib_idx + 1) % cells[pcell_idx]->sibs.size();
    } else if (SRSLTE_RNTI_ISRAR(dl_rnti)) {
      if (prach_tti != -1) {
        rar_tti = (prach_tti + 3) % 10240;
        if (tti == rar_tti) {
          send_rar(prach_preamble_index);
        }
      }
    } else if (SRSLTE_RNTI_ISPA(dl_rnti)) {
      log->debug("Searching for paging RNTI\n");
      // PCH will be triggered from SYSSIM after receiving Paging
    } else if (SRSLTE_RNTI_ISUSER(dl_rnti)) {
      // check if this is for contention resolution after PRACH/RAR
      if (dl_rnti == crnti) {
        log->debug("Searching for C-RNTI=%d\n", crnti);

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

      if (dl_rnti != SRSLTE_INVALID_RNTI) {
        log->debug("Searching for RNTI=%d\n", dl_rnti);

        // look for DL data to be send in each bearer and provide grant accordingly
        for (int lcid = 0; lcid < SRSLTE_N_RADIO_BEARERS; lcid++) {
          uint32_t buf_state = rlc.get_buffer_state(lcid);
          // Schedule DL transmission if there is data in RLC buffer or we need to send Msg4
          if ((buf_state > 0 && bearer_follow_on_map[lcid] == false) || (msg3_tti != -1 && conres_id != 0)) {
            log->debug("LCID=%d, buffer_state=%d\n", lcid, buf_state);
            tx_payload_buffer.clear();

            const uint32_t mac_header_size = 10; // Add MAC header (10 B for all subheaders, etc)
            mac_msg_dl.init_tx(&tx_payload_buffer, buf_state + mac_header_size, false);

            // check if this is Msg4 that needs to contain the contention resolution ID CE
            if (msg3_tti != -1 && lcid == 0 && conres_id != 0) {
              if (mac_msg_dl.new_subh()) {
                if (mac_msg_dl.get()->set_con_res_id(conres_id)) {
                  log->info("CE:    Added Contention Resolution ID=0x%" PRIx64 "\n", conres_id);
                } else {
                  log->error("CE:    Setting Contention Resolution ID CE\n");
                }
                conres_id = 0; // reset CR so it's not sent twice
              } else {
                log->error("CE:    Setting Contention Resolution ID CE. No space for a subheader\n");
              }
              msg3_tti = -1;
            }

            bool has_single_sdu = false;

            // allocate SDUs
            while (buf_state > 0) { // there is pending SDU to allocate
              if (mac_msg_dl.new_subh()) {
                int n = mac_msg_dl.get()->set_sdu(lcid, buf_state, &rlc);
                if (n == -1) {
                  log->error("Error while adding SDU (%d B) to MAC PDU\n", buf_state);
                  mac_msg_dl.del_subh();
                }

                // update buffer state
                buf_state = rlc.get_buffer_state(lcid);

                if (mac_msg_dl.nof_subh() == 1) {
                  has_single_sdu = true;
                } else {
                  has_single_sdu = false;
                }
              }
            }

            // Assemble entire MAC PDU
            uint8_t* mac_pdu_ptr = mac_msg_dl.write_packet(log);
            if (mac_pdu_ptr != nullptr) {
              if (force_lcid != -1 && lcid == 0) {
                if (has_single_sdu) {
                  log->info("Patched lcid in mac header to: %d\n", force_lcid);
                  mac_pdu_ptr[0] = (mac_pdu_ptr[0] & 0xe0) | (force_lcid & 0x1f);
                } else if (mac_msg_dl.nof_subh() > 1) {
                  log->warning(
                      "Not patching lcid to %d in mac header (nof_subh == %d)\n", force_lcid, mac_msg_dl.nof_subh());
                }
              }
              log->info_hex(mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU (%d B):\n", mac_msg_dl.get_pdu_len());

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
              log->error("Error writing DL MAC PDU\n");
            }
            mac_msg_dl.reset();

          } else if (bearer_follow_on_map[lcid]) {
            log->info("Waiting for more PDUs for transmission on LCID=%d\n", lcid);
          }
        }
        // Check if we need to provide a UL grant as well
      }
    } else {
      log->debug("Not handling RNTI=%d\n", dl_rnti);
    }
  }

  void stop()
  {
    running = false;
    ue->stop();
  }

  void reset()
  {
    log->info("Resetting SS\n");
    rlc.reset();
    pdcp.reset();
    cells.clear();
    pcell_idx = -1;
    pending_bearer_config.clear();
  }

  // Called from UT before starting testcase
  void tc_start(const char* name)
  {
    // strip testsuite name
    std::string tc_name = get_tc_name(name);

    // Make a copy of the UE args for this run
    all_args_t local_args = args;

    // set up logging
    if (args.log.filename == "stdout") {
      logger = &logger_stdout;
    } else {
      logger_file->init(get_filename_with_tc_name(local_args.log.filename, run_id, tc_name).c_str(), -1);
      logger = logger_file;
    }

    log->info("Initializing UE ID=%d for TC=%s\n", run_id, tc_name.c_str());
    log->console("Initializing UE ID=%d for TC=%s\n", run_id, tc_name.c_str());

    // Patch UE config
    local_args.stack.pcap.filename     = get_filename_with_tc_name(args.stack.pcap.filename, run_id, tc_name);
    local_args.stack.pcap.nas_filename = get_filename_with_tc_name(args.stack.pcap.nas_filename, run_id, tc_name);

    // bring up UE
    if (ue->init(local_args, logger, this, tc_name)) {
      ue->stop();
      std::string err("Couldn't initialize UE.\n");
      log->error("%s\n", err.c_str());
      log->console("%s\n", err.c_str());
      return;
    }

    // create and add TTI timer to epoll
    if (add_epoll(timer_handler.get_timer_fd(), epoll_fd) != SRSLTE_SUCCESS) {
      log->error("Error while adding TTI timer to epoll\n");
    }
    event_handler.insert({timer_handler.get_timer_fd(), &timer_handler});
  }

  // Called from UT to terminate the testcase
  void tc_end()
  {
    log->info("Deinitializing UE ID=%d\n", run_id);
    log->console("Deinitializing UE ID=%d\n", run_id);
    ue->stop();

    // stop TTI timer
    del_epoll(timer_handler.get_timer_fd(), epoll_fd);

    logger_file->stop();

    run_id++;

    // Reset SS' RLC and PDCP
    reset();
  }

  void power_off_ue()
  {
    // only return after new UE instance is up and running
  }

  // Called from outside
  void switch_on_ue() { event_queue.push(UE_SWITCH_ON); }

  void switch_off_ue() { event_queue.push(UE_SWITCH_OFF); }

  void enable_data() { event_queue.push(ENABLE_DATA); }

  void disable_data() { event_queue.push(DISABLE_DATA); }

  // Called from PHY but always from the SS main thread with lock being hold
  void prach_indication(uint32_t preamble_index_, const uint32_t& cell_id)
  {
    // verify that UE intends to send PRACH on current Pcell
    if (cells[pcell_idx]->cell.id != cell_id) {
      log->error("UE is attempting to PRACH on pci=%d, current Pcell=%d\n", cell_id, cells[pcell_idx]->cell.id);
      return;
    }

    // store TTI for providing UL grant for Msg3 transmission
    prach_tti            = tti;
    prach_preamble_index = preamble_index_;
  }

  // Called from PHY
  void sr_req(uint32_t tti_tx)
  {
    log->info("Received SR from PHY\n");
    sr_tti = tti_tx;
  }

  // Called from PHY
  void tx_pdu(const uint8_t* payload, const int len, const uint32_t tx_tti)
  {
    if (payload == NULL) {
      ss_mac_log->error("Received NULL as PDU payload. Dropping.\n");
      return;
    }

    log->info_hex(payload, len, "UL MAC PDU (%d B):\n", len);

    // Parse MAC
    mac_msg_ul.init_rx(len, true);
    mac_msg_ul.parse_packet((uint8_t*)payload);

    while (mac_msg_ul.next()) {
      assert(mac_msg_ul.get());
      if (mac_msg_ul.get()->is_sdu()) {
        // Push PDU to our own RLC (needed to handle status reporting, etc. correctly
        ss_mac_log->info_hex(mac_msg_ul.get()->get_sdu_ptr(),
                             mac_msg_ul.get()->get_payload_size(),
                             "Route UL PDU to LCID=%d (%d B)\n",
                             mac_msg_ul.get()->get_sdu_lcid(),
                             mac_msg_ul.get()->get_payload_size());
        rlc.write_pdu(
            mac_msg_ul.get()->get_sdu_lcid(), mac_msg_ul.get()->get_sdu_ptr(), mac_msg_ul.get()->get_payload_size());

        // Save contention resolution if lcid == 0
        if (mac_msg_ul.get()->get_sdu_lcid() == 0) {
          int nbytes = srslte::sch_subh::MAC_CE_CONTRES_LEN;
          if (mac_msg_ul.get()->get_payload_size() >= (uint32_t)nbytes) {
            uint8_t* ue_cri_ptr = (uint8_t*)&conres_id;
            uint8_t* pkt_ptr    = mac_msg_ul.get()->get_sdu_ptr(); // Warning here: we want to include the
            for (int i = 0; i < nbytes; i++) {
              ue_cri_ptr[nbytes - i - 1] = pkt_ptr[i];
            }
            ss_mac_log->info_hex(ue_cri_ptr, nbytes, "Contention resolution ID:\n");
          } else {
            ss_mac_log->error("Received CCCH UL message of invalid size=%d bytes\n",
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
  void send_rar(uint32_t preamble_index)
  {
    log->info("Sending RAR for RAPID=%d\n", preamble_index);

    // Prepare RAR grant
    uint8_t                grant_buffer[64] = {};
    srslte_dci_rar_grant_t rar_grant        = {};
    rar_grant.tpc_pusch                     = 3;
    srslte_dci_rar_pack(&rar_grant, grant_buffer);

    // Create MAC PDU and add RAR subheader
    srslte::rar_pdu rar_pdu;
    rar_buffer.clear();

    const int rar_pdu_len = 64;
    rar_pdu.init_tx(&rar_buffer, rar_pdu_len);
    rar_pdu.set_backoff(11); // Backoff of 480ms to prevent UE from PRACHing too fast
    if (rar_pdu.new_subh()) {
      rar_pdu.get()->set_rapid(preamble_index);
      rar_pdu.get()->set_ta_cmd(0);
      rar_pdu.get()->set_temp_crnti(crnti);
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
  void send_msg3_grant()
  {
    log->info("Sending Msg3 grant for C-RNTI=%d\n", crnti);
    mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};

    ul_grant.tti_tx         = (tti + 3) % 10240;
    ul_grant.tb.tbs         = 32;
    ul_grant.tb.ndi_present = true;
    ul_grant.tb.ndi         = get_ndi_for_new_ul_tx(tti);
    ul_grant.rnti           = crnti;
    ul_grant.pid            = get_pid(tti);
    ul_grant.is_rar         = true;

    ue->new_grant_ul(ul_grant);
  }

  // Internal function called from main thread
  void send_sr_ul_grant()
  {
    // Provide new UL grant to UE
    mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};
    ul_grant.tti_tx                                = (tti + 3) % 10240;
    ul_grant.tb.tbs                                = 100; // TODO: reasonable size?
    ul_grant.tb.ndi_present                        = true;
    ul_grant.tb.ndi                                = get_ndi_for_new_ul_tx(tti);
    ul_grant.rnti                                  = crnti;
    ul_grant.pid                                   = get_pid(tti);

    ue->new_grant_ul(ul_grant);

    sr_tti = -1;
  }

  // internal function called from tx_pdu (called from main thread)
  bool process_ce(srslte::sch_subh* subh)
  {
    uint16_t rnti = dl_rnti;

    uint32_t buff_size[4] = {0, 0, 0, 0};
    float    phr          = 0;
    int32_t  idx          = 0;
    uint16_t old_rnti     = 0;
    bool     is_bsr       = false;
    switch (subh->ul_sch_ce_type()) {
      case srslte::ul_sch_lcid::PHR_REPORT:
        phr = subh->get_phr();
        ss_mac_log->info("CE:    Received PHR from rnti=0x%x, value=%.0f\n", rnti, phr);
        break;
      case srslte::ul_sch_lcid::CRNTI:
        old_rnti = subh->get_c_rnti();
        ss_mac_log->info("CE:    Received C-RNTI from temp_rnti=0x%x, rnti=0x%x\n", rnti, old_rnti);
        break;
      case srslte::ul_sch_lcid::TRUNC_BSR:
      case srslte::ul_sch_lcid::SHORT_BSR:
        idx = subh->get_bsr(buff_size);
        if (idx == -1) {
          ss_mac_log->error("Invalid Index Passed to lc groups\n");
          break;
        }
        ss_mac_log->info("CE:    Received %s BSR rnti=0x%x, lcg=%d, value=%d\n",
                         subh->ul_sch_ce_type() == srslte::ul_sch_lcid::SHORT_BSR ? "Short" : "Trunc",
                         rnti,
                         idx,
                         buff_size[idx]);
        is_bsr = true;
        break;
      case srslte::ul_sch_lcid::LONG_BSR:
        subh->get_bsr(buff_size);
        is_bsr = true;
        ss_mac_log->info("CE:    Received Long BSR rnti=0x%x, value=%d,%d,%d,%d\n",
                         rnti,
                         buff_size[0],
                         buff_size[1],
                         buff_size[2],
                         buff_size[3]);
        break;
      case srslte::ul_sch_lcid::PADDING:
        ss_mac_log->debug("CE:    Received padding for rnti=0x%x\n", rnti);
        break;
      default:
        ss_mac_log->error("CE:    Invalid lcid=0x%x\n", subh->lcid_value());
        break;
    }
    return is_bsr;
  }

  uint32_t get_pid(const uint32_t tti_) { return tti_ % SRSLTE_FDD_NOF_HARQ; }

  bool get_ndi_for_new_ul_tx(const uint32_t tti_)
  {
    // toggle NDI to always create new Tx
    const uint32_t pid = get_pid(tti_);
    last_ul_ndi[pid]   = !last_ul_ndi[pid];
    log->info("UL-PID=%d NDI=%s\n", pid, last_ul_ndi[pid] ? "1" : "0");
    return last_ul_ndi[pid];
  }

  bool get_ndi_for_new_dl_tx(const uint32_t tti_)
  {
    // toggle NDI to always create new Tx
    const uint32_t pid = get_pid(tti_);
    last_dl_ndi[pid]   = !last_dl_ndi[pid];
    log->info("DL-PID=%d NDI=%s\n", pid, last_dl_ndi[pid] ? "1" : "0");
    return last_dl_ndi[pid];
  }

  int run()
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
        break;
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
    return SRSLTE_SUCCESS;
  }

  uint32_t get_tti() { return tti; }

  void process_pdu(uint8_t* buff, uint32_t len, pdu_queue::channel_t channel) {}

  void set_cell_config(const ttcn3_helpers::timing_info_t timing,
                       const std::string                  cell_name,
                       const uint32_t                     earfcn,
                       const srslte_cell_t                cell,
                       const float                        power)
  {
    if (timing.now) {
      set_cell_config_impl(cell_name, earfcn, cell, power);
    } else {
      log->debug("Scheduling Cell configuration of %s for TTI=%d\n", cell_name.c_str(), timing.tti);
      tti_actions[timing.tti].push_back(
          [this, cell_name, earfcn, cell, power]() { set_cell_config_impl(cell_name, earfcn, cell, power); });
    }
  }

  void set_cell_config_impl(const std::string   cell_name_,
                            const uint32_t      earfcn_,
                            const srslte_cell_t cell_,
                            const float         power_)
  {
    // check if cell already exists
    if (not syssim_has_cell(cell_name_)) {
      // insert new cell
      log->info("Adding cell %s with cellId=%d and power=%.2f dBm\n", cell_name_.c_str(), cell_.id, power_);
      unique_syssim_cell_t cell = unique_syssim_cell_t(new syssim_cell_t);
      cell->name                = cell_name_;
      cell->cell                = cell_;
      cell->initial_power       = power_;
      cell->earfcn              = earfcn_;
      cells.push_back(std::move(cell));
    } else {
      // cell is already there
      log->info("Cell already there, reconfigure\n");
    }

    update_cell_map();
  }

  // internal function
  bool syssim_has_cell(std::string cell_name)
  {
    for (auto& cell : cells) {
      if (cell->name == cell_name) {
        return true;
      }
    }
    return false;
  }

  void set_cell_attenuation(const ttcn3_helpers::timing_info_t timing, const std::string cell_name, const float value)
  {
    if (timing.now) {
      set_cell_attenuation_impl(cell_name, value);
    } else {
      log->debug("Scheduling Cell attenuation reconfiguration of %s for TTI=%d\n", cell_name.c_str(), timing.tti);
      tti_actions[timing.tti].push_back([this, cell_name, value]() { set_cell_attenuation_impl(cell_name, value); });
    }
  }

  void set_cell_attenuation_impl(const std::string cell_name, const float value)
  {
    if (not syssim_has_cell(cell_name)) {
      log->error("Can't set cell power. Cell not found.\n");
    }

    // update cell's power
    for (auto& cell : cells) {
      if (cell->name == cell_name) {
        cell->attenuation = value;
        break;
      }
    }

    update_cell_map();
  }

  // Internal function
  void update_cell_map()
  {
    // convert syssim cell list to phy cell list
    {
      lte_ttcn3_phy::cell_list_t phy_cells;
      for (auto& ss_cell : cells) {
        lte_ttcn3_phy::cell_t phy_cell = {};
        phy_cell.info                  = ss_cell->cell;
        phy_cell.power                 = ss_cell->initial_power - ss_cell->attenuation;
        phy_cell.earfcn                = ss_cell->earfcn;
        log->info("Configuring cell with PCI=%d with TxPower=%.2f\n", phy_cell.info.id, phy_cell.power);
        phy_cells.push_back(phy_cell);
      }

      // SYSSIM defines what cells the UE can connect to
      ue->set_cell_map(phy_cells);
    }
  }

  bool have_valid_pcell() { return (pcell_idx >= 0 && pcell_idx < static_cast<int>(cells.size())); }

  void add_bcch_dlsch_pdu(const string cell_name, unique_byte_buffer_t pdu)
  {
    if (not syssim_has_cell(cell_name)) {
      log->error("Can't add BCCH to cell. Cell not found.\n");
    }

    // add SIB
    for (auto& cell : cells) {
      if (cell->name == cell_name) {
        cell->sibs.push_back(std::move(pdu));
        break;
      }
    }
  }

  void add_ccch_pdu(const ttcn3_helpers::timing_info_t timing, unique_byte_buffer_t pdu)
  {
    if (timing.now) {
      // Add to SRB0 Tx queue
      rlc.write_sdu(0, std::move(pdu));
    } else {
      log->debug("Scheduling CCCH PDU for TTI=%d\n", timing.tti);
      auto task = [this](srslte::unique_byte_buffer_t& pdu) { rlc.write_sdu(0, std::move(pdu)); };
      tti_actions[timing.tti].push_back(std::bind(task, std::move(pdu)));
    }
  }

  void
  add_dcch_pdu(const ttcn3_helpers::timing_info_t timing, uint32_t lcid, unique_byte_buffer_t pdu, bool follow_on_flag)
  {
    if (timing.now) {
      add_dcch_pdu_impl(lcid, std::move(pdu), follow_on_flag);
    } else {
      log->debug("Scheduling DCCH PDU for TTI=%d\n", timing.tti);
      auto task = [this](uint32_t lcid, srslte::unique_byte_buffer_t& pdu, bool follow_on_flag) {
        add_dcch_pdu_impl(lcid, std::move(pdu), follow_on_flag);
      };
      tti_actions[timing.tti].push_back(std::bind(task, lcid, std::move(pdu), follow_on_flag));
    }
  }

  void add_dcch_pdu_impl(uint32_t lcid, unique_byte_buffer_t pdu, bool follow_on_flag)
  {
    // push to PDCP and create DL grant for it
    log->info("Writing PDU (%d B) to LCID=%d\n", pdu->N_bytes, lcid);
    pdcp.write_sdu(lcid, std::move(pdu), true);
    bearer_follow_on_map[lcid] = follow_on_flag;
  }

  void add_pch_pdu(unique_byte_buffer_t pdu)
  {
    log->info("Received PCH PDU (%d B)\n", pdu->N_bytes);

    // Prepare MAC grant for PCH
    mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
    dl_grant.tti                                   = tti;
    dl_grant.pid                                   = get_pid(tti);
    dl_grant.rnti                                  = SRSLTE_PRNTI;
    dl_grant.tb[0].tbs                             = pdu->N_bytes;
    dl_grant.tb[0].ndi_present                     = true;
    dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
    ue->new_tb(dl_grant, (const uint8_t*)pdu->msg);
  }

  void step_stack() { stack.run_tti(); }

  void add_srb(const ttcn3_helpers::timing_info_t timing, const uint32_t lcid, const pdcp_config_t pdcp_config)
  {
    if (timing.now) {
      add_srb_impl(lcid, pdcp_config);
    } else {
      log->debug("Scheduling SRB%d addition for TTI=%d\n", lcid, timing.tti);
      tti_actions[timing.tti].push_back([this, lcid, pdcp_config]() { add_srb_impl(lcid, pdcp_config); });
    }
  }

  void add_srb_impl(const uint32_t lcid, const pdcp_config_t pdcp_config)
  {
    log->info("Adding SRB%d\n", lcid);
    pdcp.add_bearer(lcid, pdcp_config);
    rlc.add_bearer(lcid, srslte::rlc_config_t::srb_config(lcid));
  }

  void reestablish_bearer(uint32_t lcid)
  {
    log->info("Reestablishing LCID=%d\n", lcid);
    pdcp.reestablish(lcid);
    rlc.reestablish(lcid);
  }

  void del_srb(const ttcn3_helpers::timing_info_t timing, const uint32_t lcid)
  {
    if (timing.now) {
      del_srb_impl(lcid);
    } else {
      log->debug("Scheduling SRB%d deletion for TTI=%d\n", lcid, timing.tti);
      tti_actions[timing.tti].push_back([this, lcid]() { del_srb_impl(lcid); });
    }
  }

  void del_srb_impl(uint32_t lcid)
  {
    log->info("Deleting SRB%d\n", lcid);
    // Only delete SRB1/2
    if (lcid > 0) {
      pdcp.del_bearer(lcid);
      rlc.del_bearer(lcid);
    }
  }

  // RRC interface for PDCP, PDCP calls RRC to push RRC SDU
  void write_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
  {
    log->info_hex(pdu->msg,
                  pdu->N_bytes,
                  "RRC SDU received for LCID=%d cell_id=%d (%d B)\n",
                  lcid,
                  cells[pcell_idx]->cell.id,
                  pdu->N_bytes);

    // check cell ID
    if (cells[pcell_idx]->cell.id > 256) {
      log->error("Cell ID too large to fit in single byte.\n");
      return;
    }

    // We don't handle RRC, prepend LCID
    pdu->msg--;
    *pdu->msg = lcid;
    pdu->N_bytes++;

    // prepend pcell PCID
    pdu->msg--;
    *pdu->msg = static_cast<uint8_t>(cells[pcell_idx]->cell.id);
    pdu->N_bytes++;

    // push content to Titan
    srb.tx(std::move(pdu));
  }

  // Not supported right now
  void write_pdu_bcch_bch(unique_byte_buffer_t pdu) { log->error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_bcch_dlsch(unique_byte_buffer_t pdu) { log->error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_pcch(unique_byte_buffer_t pdu) { log->error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_mch(uint32_t lcid, unique_byte_buffer_t pdu) { log->error("%s not implemented.\n", __FUNCTION__); }
  void max_retx_attempted() { log->error("%s not implemented.\n", __FUNCTION__); }

  std::string get_rb_name(uint32_t lcid)
  {
    if (lcid < rb_id_vec.size()) {
      return rb_id_vec.at(lcid);
    }
    return std::string("RB");
  };

  void write_sdu(uint32_t lcid, unique_byte_buffer_t sdu, bool blocking = true)
  {
    log->info_hex(sdu->msg, sdu->N_bytes, "Received SDU on LCID=%d\n", lcid);

    uint8_t* mac_pdu_ptr;
    mac_pdu_ptr = mac_msg_dl.write_packet(log);
    log->info_hex(mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU:\n");

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

  void discard_sdu(uint32_t lcid, uint32_t sn) {}

  bool rb_is_um(uint32_t lcid) { return false; }

  void set_as_security(const ttcn3_helpers::timing_info_t        timing,
                       std::array<uint8_t, 32>                   k_rrc_enc_,
                       std::array<uint8_t, 32>                   k_rrc_int_,
                       std::array<uint8_t, 32>                   k_up_enc_,
                       const srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                       const srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                       const ttcn3_helpers::pdcp_count_map_t     bearers_)
  {
    if (timing.now) {
      set_as_security_impl(k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_);
    } else {
      log->debug("Scheduling AS security configuration for TTI=%d\n", timing.tti);
      tti_actions[timing.tti].push_back(
          [this, k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_]() {
            set_as_security_impl(k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_, bearers_);
          });
    }
  }

  void set_as_security_impl(std::array<uint8_t, 32>                   k_rrc_enc_,
                            std::array<uint8_t, 32>                   k_rrc_int_,
                            std::array<uint8_t, 32>                   k_up_enc_,
                            const srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                            const srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_,
                            const ttcn3_helpers::pdcp_count_map_t     bearers)
  {
    // store security config for later use (i.e. new bearer added)
    sec_cfg = {.k_rrc_int   = k_rrc_int_,
               .k_rrc_enc   = k_rrc_enc_,
               .k_up_int    = {},
               .k_up_enc    = k_up_enc_,
               .integ_algo  = integ_algo_,
               .cipher_algo = cipher_algo_};

    for (auto& lcid : bearers) {
      pdcp.config_security(lcid.rb_id, sec_cfg);

      log->info("Setting AS security for LCID=%d in DL direction from SN=%d onwards\n", lcid.rb_id, lcid.dl_value);
      pdcp.enable_security_timed(lcid.rb_id, DIRECTION_TX, lcid.dl_value);

      log->info("Setting AS security for LCID=%d in UL direction from SN=%d onwards\n", lcid.rb_id, lcid.ul_value);
      pdcp.enable_security_timed(lcid.rb_id, DIRECTION_RX, lcid.ul_value);
    }
  }

  void release_as_security(const ttcn3_helpers::timing_info_t timing)
  {
    if (timing.now) {
      release_as_security_impl();
    } else {
      log->debug("Scheduling Release of AS security for TTI=%d\n", timing.tti);
      tti_actions[timing.tti].push_back([this]() { release_as_security_impl(); });
    }
  }

  void release_as_security_impl()
  {
    log->info("Releasing AS security\n");
    pending_bearer_config.clear();
  }

  void select_cell(srslte_cell_t phy_cell)
  {
    // find matching cell in SS cell list
    for (uint32_t i = 0; i < cells.size(); ++i) {
      if (cells[i]->cell.id == phy_cell.id) {
        pcell_idx = i;
        log->info("New PCell: PCI=%d\n", cells[pcell_idx]->cell.id);
        return;
      }
    }
  }

  ttcn3_helpers::pdcp_count_map_t get_pdcp_count()
  {
    // prepare response to SS
    std::vector<ttcn3_helpers::pdcp_count_t> bearers;
    for (uint32_t i = 0; i < rb_id_vec.size(); i++) {
      if (pdcp.is_lcid_enabled(i)) {
        ttcn3_helpers::pdcp_count_t bearer;
        uint16_t                    tmp; // not handling HFN
        pdcp.get_bearer_status(i, &bearer.dl_value, &tmp, &bearer.ul_value, &tmp);
        bearer.rb_is_srb = i <= 2;
        bearer.rb_id     = i;
        log->debug("PDCP count lcid=%d, dl=%d, ul=%d\n", bearer.rb_id, bearer.dl_value, bearer.ul_value);
        bearers.push_back(bearer);
      }
    }
    return bearers;
  }

private:
  // SYS interface
  ttcn3_ut_interface      ut;
  ttcn3_sys_interface     sys;
  ttcn3_ip_sock_interface ip_sock;
  ttcn3_ip_ctrl_interface ip_ctrl;
  ttcn3_srb_interface     srb;
  ttcn3_drb_interface     drb;

  // Epoll
  int                                epoll_fd  = -1;
  int                                signal_fd = -1; ///< FD for signals
  std::map<uint32_t, epoll_handler*> event_handler;  ///< Lookup table for handler
  epoll_timer_handler                timer_handler;
  epoll_signal_handler               signal_handler;

  // Logging stuff
  srslte::logger_stdout logger_stdout;
  srslte::logger_file*  logger_file = nullptr;
  srslte::logger*       logger      = nullptr;
  srslte::log_ref       log;
  srslte::log_filter    ut_log;
  srslte::log_filter    sys_log;
  srslte::log_filter    ip_sock_log;
  srslte::log_filter    ip_ctrl_log;
  srslte::log_filter    srb_log;
  srslte::log_filter    drb_log;
  srslte::log_ref       ss_mac_log{"SS-MAC"};
  srslte::log_ref       ss_rlc_log{"SS-RLC"};
  srslte::log_ref       ss_pdcp_log{"SS-PDCP"};

  all_args_t args = {};

  srslte::byte_buffer_pool* pool = nullptr;

  // Simulator vars
  ttcn3_ue* ue      = nullptr;
  bool      running = false;

  typedef enum { UE_SWITCH_ON = 0, UE_SWITCH_OFF, ENABLE_DATA, DISABLE_DATA } ss_events_t;
  block_queue<ss_events_t> event_queue;

  uint32_t run_id = 0;

  int32_t                 tti                  = 0;
  int32_t                 prach_tti            = -1;
  int32_t                 rar_tti              = -1;
  int32_t                 msg3_tti             = -1;
  int32_t                 sr_tti               = -1;
  uint32_t                prach_preamble_index = 0;
  uint16_t                dl_rnti              = 0;
  uint16_t                crnti                = TTCN3_CRNTI;
  int                     force_lcid           = -1;
  srsue::stack_test_dummy stack;
  bool                    last_dl_ndi[SRSLTE_FDD_NOF_HARQ] = {};
  bool                    last_ul_ndi[SRSLTE_FDD_NOF_HARQ] = {};

  // For events/actions that need to be carried out in a specific TTI
  typedef std::vector<move_task_t>              task_list_t;
  typedef std::map<const uint32_t, task_list_t> tti_action_map_t;
  tti_action_map_t                              tti_actions;

  // Map between the cellId (name) used by 3GPP test suite and srsLTE cell struct
  typedef struct {
    std::string                       name;
    srslte_cell_t                     cell          = {};
    float                             initial_power = 0.0;
    float                             attenuation   = 0.0;
    uint32_t                          earfcn        = 0;
    std::vector<unique_byte_buffer_t> sibs;
    int                               sib_idx = 0; ///< Index of SIB scheduled for next transmission
  } syssim_cell_t;
  typedef std::unique_ptr<syssim_cell_t> unique_syssim_cell_t;
  std::vector<unique_syssim_cell_t>      cells;
  int32_t                                pcell_idx = -1;

  srslte::pdu_queue pdus;
  srslte::sch_pdu   mac_msg_dl, mac_msg_ul;

  // buffer for DL transmissions
  srslte::byte_buffer_t rar_buffer;
  srslte::byte_buffer_t tx_payload_buffer; // Used to buffer final MAC PDU

  uint64_t conres_id = 0;

  // Simulator objects
  srslte::rlc              rlc;
  srslte::pdcp             pdcp;
  std::map<uint32_t, bool> bearer_follow_on_map; ///< Indicates if for a given LCID the follow_on_flag is set or not

  // security config
  ttcn3_helpers::pdcp_count_map_t pending_bearer_config; ///< List of bearers with pending security configuration
  srslte::as_security_config_t    sec_cfg;

  std::vector<std::string> rb_id_vec =
      {"SRB0", "SRB1", "SRB2", "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8"};

  // port constants
  const std::string listen_address = "0.0.0.0";
  const uint32_t    UT_PORT        = 2222;
  const uint32_t    SYS_PORT       = 2223;
  const uint32_t    IPSOCK_PORT    = 2224;
  const uint32_t    IPCTRL_PORT    = 2225;
  const uint32_t    SRB_PORT       = 2226;
  const uint32_t    DRB_PORT       = 2227;
};

#endif // SRSUE_TTCN3_SYSSIM_H
