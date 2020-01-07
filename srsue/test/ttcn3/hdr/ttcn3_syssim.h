/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include "srslte/common/netsource_handler.h"
#include "srslte/common/pdu_queue.h"
#include "srslte/common/threads.h"
#include "srslte/upper/pdcp.h"
#include "srslte/upper/rlc.h"
#include "ttcn3_ip_ctrl_interface.h"
#include "ttcn3_ip_sock_interface.h"
#include "ttcn3_srb_interface.h"
#include "ttcn3_sys_interface.h"
#include "ttcn3_ue.h"
#include "ttcn3_ut_interface.h"

#include <srslte/interfaces/ue_interfaces.h>

#define TTCN3_CRNTI (0x1001)

class ttcn3_syssim : public thread,
                     public syssim_interface_phy,
                     public ss_ut_interface,
                     public ss_sys_interface,
                     public ss_srb_interface,
                     public rrc_interface_rlc,
                     public rlc_interface_pdcp,
                     public rrc_interface_pdcp,
                     public srslte::pdu_queue::process_callback
{
public:
  ttcn3_syssim(srslte::logger_file* logger_file_) :
    mac_msg_ul(20, &ss_mac_log),
    mac_msg_dl(20, &ss_mac_log),
    timers(8),
    pdus(128),
    logger(logger_file_),
    logger_file(logger_file_),
    pool(byte_buffer_pool::get_instance()),
    thread("TTCN3_SYSSIM"),
    rlc(&ss_rlc_log),
    pdcp(&timers, &ss_pdcp_log){};

  ~ttcn3_syssim(){};

  void init(const all_args_t& args_)
  {
    std::lock_guard<std::mutex> lock(mutex);

    args = args_;

    // Make sure to get SS logging as well
    if (args.log.filename == "stdout") {
      logger = &logger_stdout;
    }

    // init and configure logging
    log.init("SS  ", logger);
    ut_log.init("UT  ", logger);
    sys_log.init("SYS ", logger);
    ip_sock_log.init("IP_S", logger);
    ip_ctrl_log.init("IP_C", logger);
    srb_log.init("SRB  ", logger);
    ss_mac_log.init("SS-MAC", logger);
    ss_rlc_log.init("SS-RLC", logger);
    ss_pdcp_log.init("SS-PDCP", logger);

    log.set_level(args.log.all_level);
    ut_log.set_level(args.log.all_level);
    sys_log.set_level(args.log.all_level);
    ip_sock_log.set_level(args.log.all_level);
    ip_ctrl_log.set_level(args.log.all_level);
    srb_log.set_level(args.log.all_level);
    ss_mac_log.set_level(args.log.all_level);
    ss_rlc_log.set_level(args.log.all_level);
    ss_pdcp_log.set_level(args.log.all_level);

    log.set_hex_limit(args.log.all_hex_limit);
    ut_log.set_hex_limit(args.log.all_hex_limit);
    sys_log.set_hex_limit(args.log.all_hex_limit);
    ip_sock_log.set_hex_limit(args.log.all_hex_limit);
    ip_ctrl_log.set_hex_limit(args.log.all_hex_limit);
    srb_log.set_hex_limit(args.log.all_hex_limit);
    ss_mac_log.set_hex_limit(args.log.all_hex_limit);
    ss_rlc_log.set_hex_limit(args.log.all_hex_limit);
    ss_pdcp_log.set_hex_limit(args.log.all_hex_limit);

    // init system interfaces to tester
    ut.init(this, &ut_log, "0.0.0.0", 2222);
    sys.init(this, &sys_log, "0.0.0.0", 2223);
    ip_sock.init(&ip_sock_log, "0.0.0.0", 2224);
    ip_ctrl.init(&ip_ctrl_log, "0.0.0.0", 2225);
    srb.init(this, &srb_log, "0.0.0.0", 2226);

    ut.start(-2);
    sys.start(-2);
    ip_sock.start(-2);
    ip_ctrl.start(-2);
    srb.start(-2);

    pdus.init(this, &log);
    rlc.init(&pdcp, this, &timers, 0 /* RB_ID_SRB0 */);
    pdcp.init(&rlc, this, nullptr);
  }

  void stop()
  {
    std::lock_guard<std::mutex> lock(mutex);

    running = false;

    if (ue != NULL) {
      ue->stop();
    }

    // Stopping system interface
    ut.stop();
    sys.stop();
    ip_sock.stop();
    ip_ctrl.stop();
    srb.stop();
  }

  // Internal function called with acquired lock
  void reset()
  {
    rlc.reset();
    pdcp.reset();
    cells.clear();
    pcell_idx = -1;
  }

  // Called from UT before starting testcase
  void tc_start(const char* name)
  {
    std::lock_guard<std::mutex> lock(mutex);

    if (ue == nullptr) {
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

      log.info("Initializing UE ID=%d for TC=%s\n", run_id, tc_name.c_str());
      log.console("Initializing UE ID=%d for TC=%s\n", run_id, tc_name.c_str());

      // Patch UE config
      local_args.stack.pcap.filename     = get_filename_with_tc_name(args.stack.pcap.filename, run_id, tc_name);
      local_args.stack.pcap.nas_filename = get_filename_with_tc_name(args.stack.pcap.nas_filename, run_id, tc_name);

      // bring up UE
      ue = std::unique_ptr<ttcn3_ue>(new ttcn3_ue());
      if (ue->init(local_args, logger, this, tc_name)) {
        ue->stop();
        ue.reset(nullptr);
        std::string err("Couldn't initialize UE.\n");
        log.error("%s\n", err.c_str());
        log.console("%s\n", err.c_str());
        return;
      }

      // Start simulator thread
      running = true;
      start();
    } else {
      log.error("UE hasn't been deallocated properly because TC didn't finish correctly.\n");
      log.console("UE hasn't been deallocated properly because TC didn't finish correctly.\n");
    }
  }

  // Called from UT to terminate the testcase
  void tc_end()
  {
    // ask periodic thread to stop before locking mutex
    running = false;

    std::lock_guard<std::mutex> lock(mutex);

    if (ue != nullptr) {
      log.info("Deinitializing UE ID=%d\n", run_id);
      log.console("Deinitializing UE ID=%d\n", run_id);
      ue->stop();

      // wait until SS main thread has terminated before resetting UE
      wait_thread_finish();

      ue.reset();

      // Reset SS' RLC and PDCP
      reset();

      logger_file->stop();

      run_id++;
    } else {
      log.error("UE is not allocated. Nothing needs to be done.\n");
      log.console("UE is not allocated. Nothing needs to be done.\n");
    }
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
      log.error(
          "UE is attempting to PRACH on pci=%d while current Pcell is pci=%d\n", cell_id, cells[pcell_idx]->cell.id);
      return;
    }

    // store TTI for providing UL grant for Msg3 transmission
    prach_tti            = tti;
    prach_preamble_index = preamble_index_;
  }

  // Called from PHY but always from the SS main thread with lock being hold
  void sr_req(uint32_t tti_tx)
  {
    log.info("Received SR from PHY\n");
    sr_tti = tti_tx;
  }

  // Called from PHY but always from the SS main thread with lock being hold
  void tx_pdu(const uint8_t* payload, const int len, const uint32_t tx_tti)
  {
    if (payload == NULL) {
      ss_mac_log.error("Received NULL as PDU payload. Dropping.\n");
      return;
    }

    // Parse MAC
    mac_msg_ul.init_rx(len, true);
    mac_msg_ul.parse_packet((uint8_t*)payload);

    while (mac_msg_ul.next()) {
      assert(mac_msg_ul.get());
      if (mac_msg_ul.get()->is_sdu()) {
        // Route logical channel
        ss_mac_log.info_hex(mac_msg_ul.get()->get_sdu_ptr(),
                            mac_msg_ul.get()->get_payload_size(),
                            "PDU:   rnti=0x%x, lcid=%d, %d bytes\n",
                            0xdead,
                            mac_msg_ul.get()->get_sdu_lcid(),
                            mac_msg_ul.get()->get_payload_size());

        // Push PDU to our own RLC (needed to handle status reporting, etc. correctly
        ss_mac_log.info_hex(mac_msg_ul.get()->get_sdu_ptr(),
                            mac_msg_ul.get()->get_payload_size(),
                            "Route PDU to LCID=%d (%d B)\n",
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
            ss_mac_log.info_hex(ue_cri_ptr, nbytes, "Contention resolution ID:\n");
          } else {
            ss_mac_log.error("Received CCCH UL message of invalid size=%d bytes\n",
                             mac_msg_ul.get()->get_payload_size());
          }
        }
      }
    }
    mac_msg_ul.reset();

    /* Process CE after all SDUs because we need to update BSR after */
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
    log.info("Sending RAR for RAPID=%d\n", preamble_index);

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
    log.info("Sending Msg3 grant for C-RNTI=%d\n", crnti);
    mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};

    ul_grant.tb.tbs         = 32;
    ul_grant.tb.ndi_present = true;
    ul_grant.tb.ndi         = get_ndi_for_new_ul_tx(tti);
    ul_grant.rnti           = crnti;
    ul_grant.pid            = get_pid(tti);

    ue->new_grant_ul(ul_grant);
  }

  // Internal function called from main thread
  void send_sr_ul_grant()
  {
    // Provide new UL grant to UE
    mac_interface_phy_lte::mac_grant_ul_t ul_grant = {};
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
    switch (subh->ce_type()) {
      case srslte::sch_subh::PHR_REPORT:
        phr = subh->get_phr();
        ss_mac_log.info("CE:    Received PHR from rnti=0x%x, value=%.0f\n", rnti, phr);
        break;
      case srslte::sch_subh::CRNTI:
        old_rnti = subh->get_c_rnti();
        ss_mac_log.info("CE:    Received C-RNTI from temp_rnti=0x%x, rnti=0x%x\n", rnti, old_rnti);
        break;
      case srslte::sch_subh::TRUNC_BSR:
      case srslte::sch_subh::SHORT_BSR:
        idx = subh->get_bsr(buff_size);
        if (idx == -1) {
          ss_mac_log.error("Invalid Index Passed to lc groups\n");
          break;
        }
        ss_mac_log.info("CE:    Received %s BSR rnti=0x%x, lcg=%d, value=%d\n",
                        subh->ce_type() == srslte::sch_subh::SHORT_BSR ? "Short" : "Trunc",
                        rnti,
                        idx,
                        buff_size[idx]);
        is_bsr = true;
        break;
      case srslte::sch_subh::LONG_BSR:
        subh->get_bsr(buff_size);
        is_bsr = true;
        ss_mac_log.info("CE:    Received Long BSR rnti=0x%x, value=%d,%d,%d,%d\n",
                        rnti,
                        buff_size[0],
                        buff_size[1],
                        buff_size[2],
                        buff_size[3]);
        break;
      case srslte::sch_subh::PADDING:
        ss_mac_log.debug("CE:    Received padding for rnti=0x%x\n", rnti);
        break;
      default:
        ss_mac_log.error("CE:    Invalid lcid=0x%x\n", subh->ce_type());
        break;
    }
    return is_bsr;
  }

  uint32_t get_pid(const uint32_t tti_) { return tti_ % (2 * FDD_HARQ_DELAY_MS); }

  bool get_ndi_for_new_ul_tx(const uint32_t tti_)
  {
    // toggle NDI to always create new Tx
    const uint32_t pid = get_pid(tti_);
    last_ul_ndi[pid]   = !last_ul_ndi[pid];
    log.info("UL-PID=%d NDI=%s\n", pid, last_ul_ndi[pid] ? "1" : "0");
    return last_ul_ndi[pid];
  }

  bool get_ndi_for_new_dl_tx(const uint32_t tti_)
  {
    // toggle NDI to always create new Tx
    const uint32_t pid = get_pid(tti_);
    last_dl_ndi[pid]   = !last_dl_ndi[pid];
    log.info("DL-PID=%d NDI=%s\n", pid, last_dl_ndi[pid] ? "1" : "0");
    return last_dl_ndi[pid];
  }

  void run_thread()
  {
    uint32_t sib_idx = 0;

    while (running) {
      {
        std::lock_guard<std::mutex> lock(mutex);

        tti = (tti + 1) % 10240;

        log.debug("SYSSIM-TTI=%d\n", tti);
        ue->set_current_tti(tti);

        // process events, if any
        while (not event_queue.empty()) {
          ss_events_t ev = event_queue.wait_pop();
          switch (ev) {
            case UE_SWITCH_ON:
              log.console("Switching on UE ID=%d\n", run_id);
              ue->switch_on();
              break;
            case UE_SWITCH_OFF:
              log.console("Switching off UE ID=%d\n", run_id);
              ue->switch_off();
              break;
            case ENABLE_DATA:
              log.console("Enabling data for UE ID=%d\n", run_id);
              ue->enable_data();
              break;
            case DISABLE_DATA:
              log.console("Disabling data for UE ID=%d\n", run_id);
              ue->disable_data();
              break;
          }
        }

        dl_rnti = ue->get_dl_sched_rnti(tti);

        if (SRSLTE_RNTI_ISSI(dl_rnti)) {
          // deliver SIBs one after another
          mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
          dl_grant.pid                                   = get_pid(tti);
          dl_grant.rnti                                  = dl_rnti;
          dl_grant.tb[0].tbs                             = cells[pcell_idx]->sibs[sib_idx]->N_bytes;
          dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
          ue->new_tb(dl_grant, cells[pcell_idx]->sibs[sib_idx]->msg);
          log.info("Delivered SIB%d for pcell_idx=%d\n", sib_idx, pcell_idx);
          sib_idx = (sib_idx + 1) % cells[pcell_idx]->sibs.size();
        } else if (SRSLTE_RNTI_ISRAR(dl_rnti)) {
          if (prach_tti != -1) {
            rar_tti = (prach_tti + 3) % 10240;
            if (tti == rar_tti) {
              send_rar(prach_preamble_index);
            }
          }
        } else if (SRSLTE_RNTI_ISPA(dl_rnti)) {
          log.debug("Searching for paging RNTI\n");
          // PCH will be triggered from SYSSIM after receiving Paging
        } else if (SRSLTE_RNTI_ISUSER(dl_rnti)) {
          // check if this is for contention resolution after PRACH/RAR
          if (dl_rnti == crnti) {
            log.debug("Searching for C-RNTI=%d\n", crnti);

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

          if (dl_rnti != 0) {
            log.debug("Searching for RNTI=%d\n", dl_rnti);

            // look for DL data to be send in each bearer and provide grant accordingly
            for (int lcid = 0; lcid < SRSLTE_N_RADIO_BEARERS; lcid++) {
              uint32_t buf_state = rlc.get_buffer_state(lcid);
              if (buf_state > 0) {
                log.debug("LCID=%d, buffer_state=%d\n", lcid, buf_state);
                const uint32_t mac_header_size = 10; // Add MAC header (10 B for all subheaders, etc)
                if (tmp_rlc_buffer.get_tailroom() > (buf_state + mac_header_size)) {
                  uint32_t pdu_size = rlc.read_pdu(lcid, tmp_rlc_buffer.msg, buf_state);
                  tx_payload_buffer.clear();
                  mac_msg_dl.init_tx(&tx_payload_buffer, pdu_size + mac_header_size, false);

                  // check if this is Msg4 that needs to contain the contention resolution ID CE
                  if (msg3_tti != -1) {
                    if (lcid == 0) {
                      if (mac_msg_dl.new_subh()) {
                        if (mac_msg_dl.get()->set_con_res_id(conres_id)) {
                          log.info("CE:    Added Contention Resolution ID=0x%" PRIx64 "\n", conres_id);
                        } else {
                          log.error("CE:    Setting Contention Resolution ID CE\n");
                        }
                      } else {
                        log.error("CE:    Setting Contention Resolution ID CE. No space for a subheader\n");
                      }
                      msg3_tti = -1;
                    }
                  }

                  // Add payload
                  if (mac_msg_dl.new_subh()) {
                    int n = mac_msg_dl.get()->set_sdu(lcid, pdu_size, tmp_rlc_buffer.msg);
                    if (n == -1) {
                      log.error("Error while adding SDU (%d B) to MAC PDU\n", pdu_size);
                      mac_msg_dl.del_subh();
                    }
                  }

                  uint8_t* mac_pdu_ptr = mac_msg_dl.write_packet(&log);
                  if (mac_pdu_ptr != nullptr) {
                    log.info_hex(
                        mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU (%d B):\n", mac_msg_dl.get_pdu_len());

                    // Prepare MAC grant for CCCH
                    mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
                    dl_grant.pid                                   = get_pid(tti);
                    dl_grant.rnti                                  = dl_rnti;
                    dl_grant.tb[0].tbs                             = mac_msg_dl.get_pdu_len();
                    dl_grant.tb[0].ndi_present                     = true;
                    dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);

                    ue->new_tb(dl_grant, (const uint8_t*)mac_pdu_ptr);
                  } else {
                    log.error("Error writing DL MAC PDU\n");
                  }
                  mac_msg_dl.reset();
                } else {
                  log.error("Can't fit RLC PDU into buffer (%d > %d)\n", buf_state, tmp_rlc_buffer.get_tailroom());
                }
              }
            }
            // Check if we need to provide a UL grant as well
          }
        } else {
          log.debug("Not handling RNTI=%d\n", dl_rnti);
        }
      }
      usleep(1000);
    }

    log.info("Leaving main thread.\n");
    log.console("Leaving main thread.\n");
  }

  uint32_t get_tti() { return tti; }

  void process_pdu(uint8_t* buff, uint32_t len, pdu_queue::channel_t channel) {}

  void set_cell_config(std::string name, uint32_t earfcn_, srslte_cell_t cell_, const float power)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // check if cell already exists
    if (not syssim_has_cell(name)) {
      // insert new cell
      log.info("Adding cell %s with cellId=%d and power=%.2f dBm\n", name.c_str(), cell_.id, power);
      unique_syssim_cell_t cell = unique_syssim_cell_t(new syssim_cell_t);
      cell->name                = name;
      cell->cell                = cell_;
      cell->initial_power       = power;
      cell->earfcn              = earfcn_;
      cells.push_back(std::move(cell));
    } else {
      // cell is already there
      log.info("Cell already there, reconfigure\n");
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

  void set_cell_attenuation(std::string cell_name, const float value)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not syssim_has_cell(cell_name)) {
      log.error("Can't set cell power. Cell not found.\n");
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
    // Find cell with highest power and select as serving cell
    if (not ue) {
      log.error("Can't configure cell. UE not initialized.\n");
    }

    // convert syssim cell list to phy cell list
    {
      lte_ttcn3_phy::cell_list_t phy_cells;
      for (auto& ss_cell : cells) {
        lte_ttcn3_phy::cell_t phy_cell = {};
        phy_cell.info                  = ss_cell->cell;
        phy_cell.power                 = ss_cell->initial_power - ss_cell->attenuation;
        phy_cell.earfcn                = ss_cell->earfcn;
        log.info("Configuring cell with PCI=%d with TxPower=%.2f\n", phy_cell.info.id, phy_cell.power);
        phy_cells.push_back(phy_cell);
      }

      // SYSSIM defines what cells the UE can connect to
      ue->set_cell_map(phy_cells);
    }

    // reselect SS Pcell
    float max_power = -145;
    for (uint32_t i = 0; i < cells.size(); ++i) {
      float actual_power = cells[i]->initial_power - cells[i]->attenuation;
      if (actual_power > max_power) {
        max_power = actual_power;
        pcell_idx = i;
        log.info("Selecting PCI=%d with TxPower=%.2f as Pcell\n", cells[pcell_idx]->cell.id, max_power);
      }
    }
  }

  bool have_valid_pcell() { return (pcell_idx >= 0 && pcell_idx < static_cast<int>(cells.size())); }

  void add_bcch_dlsch_pdu(const string cell_name, unique_byte_buffer_t pdu)
  {
    std::lock_guard<std::mutex> lock(mutex);

    if (not syssim_has_cell(cell_name)) {
      log.error("Can't add BCCH to cell. Cell not found.\n");
    }

    // add SIB
    for (auto& cell : cells) {
      if (cell->name == cell_name) {
        cell->sibs.push_back(std::move(pdu));
        break;
      }
    }
  }

  void add_ccch_pdu(unique_byte_buffer_t pdu)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // Add to SRB0 Tx queue
    rlc.write_sdu(0, std::move(pdu));
  }

  void add_dcch_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
  {
    std::lock_guard<std::mutex> lock(mutex);

    // push to PDCP and create DL grant for it
    log.info("Writing PDU (%d B) to LCID=%d\n", pdu->N_bytes, lcid);
    pdcp.write_sdu(lcid, std::move(pdu), true);
  }

  void add_pch_pdu(unique_byte_buffer_t pdu)
  {
    std::lock_guard<std::mutex> lock(mutex);
    log.info("Received PCH PDU (%d B)\n", pdu->N_bytes);

    // Prepare MAC grant for PCH
    mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
    dl_grant.pid                                   = get_pid(tti);
    dl_grant.rnti                                  = SRSLTE_PRNTI;
    dl_grant.tb[0].tbs                             = pdu->N_bytes;
    dl_grant.tb[0].ndi_present                     = true;
    dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);
    ue->new_tb(dl_grant, (const uint8_t*)pdu->msg);
  }

  void step_timer() { timers.step_all(); }

  void add_srb(uint32_t lcid, pdcp_config_t pdcp_config)
  {
    std::lock_guard<std::mutex> lock(mutex);
    pdcp.add_bearer(lcid, pdcp_config);
    rlc.add_bearer(lcid, srslte::rlc_config_t::srb_config(lcid));
  }

  void reestablish_bearer(uint32_t lcid)
  {
    std::lock_guard<std::mutex> lock(mutex);
    log.info("Reestablishing LCID=%d\n", lcid);
    pdcp.reestablish(lcid);
    rlc.reestablish(lcid);
  }

  void del_srb(uint32_t lcid)
  {
    std::lock_guard<std::mutex> lock(mutex);
    // Only delete SRB1/2
    if (lcid > 0) {
      pdcp.del_bearer(lcid);
      rlc.del_bearer(lcid);
    }

    // Reset HARQ to generate new transmissions
    if (lcid == 0) {
      log.info("Resetting UL/DL NDI counters\n");
      memset(last_dl_ndi, 0, sizeof(last_dl_ndi));
      memset(last_ul_ndi, 0, sizeof(last_ul_ndi));
    }
  }

  // RRC interface for PDCP, PDCP calls RRC to push RRC SDU
  void write_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
  {
    log.info_hex(pdu->msg, pdu->N_bytes, "RRC SDU received for LCID=%d (%d B)\n", lcid, pdu->N_bytes);

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
  void write_pdu_bcch_bch(unique_byte_buffer_t pdu) { log.error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_bcch_dlsch(unique_byte_buffer_t pdu) { log.error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_pcch(unique_byte_buffer_t pdu) { log.error("%s not implemented.\n", __FUNCTION__); }
  void write_pdu_mch(uint32_t lcid, unique_byte_buffer_t pdu) { log.error("%s not implemented.\n", __FUNCTION__); }
  void max_retx_attempted() { log.error("%s not implemented.\n", __FUNCTION__); }

  std::string get_rb_name(uint32_t lcid)
  {
    if (lcid < rb_id_vec.size()) {
      return rb_id_vec.at(lcid);
    }
    return std::string("RB");
  };

  void write_sdu(uint32_t lcid, unique_byte_buffer_t sdu, bool blocking = true)
  {
    log.info_hex(sdu->msg, sdu->N_bytes, "Received SDU on LCID=%d\n", lcid);

    uint8_t* mac_pdu_ptr;
    mac_pdu_ptr = mac_msg_dl.write_packet(&log);
    log.info_hex(mac_pdu_ptr, mac_msg_dl.get_pdu_len(), "DL MAC PDU:\n");

    // Prepare MAC grant for CCCH
    mac_interface_phy_lte::mac_grant_dl_t dl_grant = {};
    dl_grant.pid                                   = get_pid(tti);
    dl_grant.rnti                                  = dl_rnti;
    dl_grant.tb[0].tbs                             = mac_msg_dl.get_pdu_len();
    dl_grant.tb[0].ndi_present                     = true;
    dl_grant.tb[0].ndi                             = get_ndi_for_new_dl_tx(tti);

    ue->new_tb(dl_grant, (const uint8_t*)mac_pdu_ptr);
  }

  void discard_sdu(uint32_t lcid, uint32_t sn) {}

  bool rb_is_um(uint32_t lcid) { return false; }

  int set_as_security(const uint32_t                            lcid,
                      std::array<uint8_t, 32>                   k_rrc_enc,
                      std::array<uint8_t, 32>                   k_rrc_int,
                      std::array<uint8_t, 32>                   k_up_enc,
                      const srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                      const srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
  {
    log.info("Setting AS security for LCID=%d\n", lcid);
    pdcp.config_security(lcid, k_rrc_enc.data(), k_rrc_int.data(), k_up_enc.data(), cipher_algo, integ_algo);
    pdcp.enable_integrity(lcid);
    pdcp.enable_encryption(lcid);
    return 0;
  }

private:
  // SYS interface
  ttcn3_ut_interface      ut;
  ttcn3_sys_interface     sys;
  ttcn3_ip_sock_interface ip_sock;
  ttcn3_ip_ctrl_interface ip_ctrl;
  ttcn3_srb_interface     srb;

  // Logging stuff
  srslte::logger_stdout logger_stdout;
  srslte::logger_file*  logger_file = nullptr;
  srslte::logger*       logger      = nullptr;
  srslte::log_filter    log;
  srslte::log_filter    ut_log;
  srslte::log_filter    sys_log;
  srslte::log_filter    ip_sock_log;
  srslte::log_filter    ip_ctrl_log;
  srslte::log_filter    srb_log;
  srslte::log_filter    ss_mac_log;
  srslte::log_filter    ss_rlc_log;
  srslte::log_filter    ss_pdcp_log;

  all_args_t args = {};

  srslte::byte_buffer_pool* pool = nullptr;

  // Simulator vars
  unique_ptr<ttcn3_ue> ue = nullptr;
  std::mutex           mutex;
  bool                 running = false;

  typedef enum { UE_SWITCH_ON = 0, UE_SWITCH_OFF, ENABLE_DATA, DISABLE_DATA } ss_events_t;
  block_queue<ss_events_t> event_queue;

  uint32_t run_id = 0;

  int32_t               tti                  = 0;
  int32_t               prach_tti            = -1;
  int32_t               rar_tti              = -1;
  int32_t               msg3_tti             = -1;
  int32_t               sr_tti               = -1;
  uint32_t              prach_preamble_index = 0;
  uint16_t              dl_rnti              = 0;
  uint16_t              crnti                = TTCN3_CRNTI;
  srslte::timer_handler timers;
  bool                  last_dl_ndi[2 * FDD_HARQ_DELAY_MS] = {};
  bool                  last_ul_ndi[2 * FDD_HARQ_DELAY_MS] = {};

  // Map between the cellId (name) used by 3GPP test suite and srsLTE cell struct
  typedef struct {
    std::string                       name;
    srslte_cell_t                     cell          = {};
    float                             initial_power = 0.0;
    float                             attenuation   = 0.0;
    uint32_t                          earfcn        = 0;
    std::vector<unique_byte_buffer_t> sibs;
  } syssim_cell_t;
  typedef std::unique_ptr<syssim_cell_t> unique_syssim_cell_t;
  std::vector<unique_syssim_cell_t>      cells;
  int32_t                                pcell_idx = -1;

  srslte::pdu_queue pdus;
  srslte::sch_pdu   mac_msg_dl, mac_msg_ul;

  // buffer for DL transmissions
  srslte::byte_buffer_t rar_buffer;
  srslte::byte_buffer_t tmp_rlc_buffer;    // Used to buffer RLC PDU
  srslte::byte_buffer_t tx_payload_buffer; // Used to buffer final MAC PDU

  uint64_t conres_id = 0;

  // Simulator objects
  srslte::rlc  rlc;
  srslte::pdcp pdcp;

  std::vector<std::string> rb_id_vec =
      {"SRB0", "SRB1", "SRB2", "DRB1", "DRB2", "DRB3", "DRB4", "DRB5", "DRB6", "DRB7", "DRB8"};
};

#endif // SRSUE_TTCN3_SYSSIM_H
