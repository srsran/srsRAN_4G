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

#include <bitset>
#include <inttypes.h>
#include <iostream>
#include <string.h>

#include "srsenb/hdr/stack/mac/ue.h"
#include "srslte/common/log_helper.h"
#include "srslte/interfaces/enb_interfaces.h"

namespace srsenb {

ue::ue(uint16_t                 rnti_,
       uint32_t                 nof_prb_,
       sched_interface*         sched_,
       rrc_interface_mac*       rrc_,
       rlc_interface_mac*       rlc_,
       phy_interface_stack_lte* phy_,
       srslte::log_ref          log_,
       uint32_t                 nof_cells_,
       uint32_t                 nof_rx_harq_proc_,
       uint32_t                 nof_tx_harq_proc_) :
  rnti(rnti_),
  nof_prb(nof_prb_),
  sched(sched_),
  rrc(rrc_),
  rlc(rlc_),
  phy(phy_),
  log_h(log_),
  mac_msg_dl(20, log_),
  mch_mac_msg_dl(10, log_),
  mac_msg_ul(20, log_),
  pdus(128),
  nof_rx_harq_proc(nof_rx_harq_proc_),
  nof_tx_harq_proc(nof_tx_harq_proc_),
  ta_fsm(this)
{
  srslte::byte_buffer_pool* pool = srslte::byte_buffer_pool::get_instance();
  tx_payload_buffer.resize(nof_cells_);
  for (auto& carrier_buffers : tx_payload_buffer) {
    for (auto& harq_buffers : carrier_buffers) {
      for (srslte::unique_byte_buffer_t& tb_buffer : harq_buffers) {
        tb_buffer = srslte::allocate_unique_buffer(*pool);
      }
    }
  }

  pdus.init(this, log_h);

  // Allocate buffer for PCell
  allocate_cc_buffers();

  // Set LCID group for SRB0 and SRB1
  set_lcg(0, 0);
  set_lcg(1, 0);
}

ue::~ue()
{
  // Free up all softbuffers for all CCs
  for (auto cc : softbuffer_rx) {
    for (auto buffer : cc) {
      srslte_softbuffer_rx_free(&buffer);
    }
  }

  for (auto cc : softbuffer_tx) {
    for (auto buffer : cc) {
      srslte_softbuffer_tx_free(&buffer);
    }
  }
}

void ue::reset()
{
  metrics      = {};
  nof_failures = 0;

  for (auto cc : softbuffer_rx) {
    for (auto buffer : cc) {
      srslte_softbuffer_rx_reset(&buffer);
    }
  }

  for (auto cc : softbuffer_tx) {
    for (auto buffer : cc) {
      srslte_softbuffer_tx_reset(&buffer);
    }
  }
}

/**
 * Allocate and initialize softbuffers for Tx and Rx and
 * append to current list of CC buffers. It uses the configured
 * number of HARQ processes and cell width.
 *
 * @param num_cc Number of carriers to add buffers for (default 1)
 * @return number of carriers
 */
uint32_t ue::allocate_cc_buffers(const uint32_t num_cc)
{
  for (uint32_t i = 0; i < num_cc; ++i) {
    // create and init Rx buffers for Pcell
    softbuffer_rx.emplace_back();
    softbuffer_rx.back().resize(nof_rx_harq_proc);
    for (auto& buffer : softbuffer_rx.back()) {
      srslte_softbuffer_rx_init(&buffer, nof_prb);
    }

    pending_buffers.emplace_back();
    pending_buffers.back().resize(nof_rx_harq_proc);
    for (auto& buffer : pending_buffers.back()) {
      buffer = nullptr;
    }

    // Create and init Tx buffers for Pcell
    softbuffer_tx.emplace_back();
    softbuffer_tx.back().resize(nof_tx_harq_proc);
    for (auto& buffer : softbuffer_tx.back()) {
      srslte_softbuffer_tx_init(&buffer, nof_prb);
    }
    // don't need to reset because just initiated the buffers
  }
  return softbuffer_tx.size();
}

void ue::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_;
}

uint32_t ue::rl_failure()
{
  nof_failures++;
  return nof_failures;
}

void ue::rl_failure_reset()
{
  nof_failures = 0;
}

void ue::set_lcg(uint32_t lcid, uint32_t lcg)
{
  // find and remove if already exists
  for (int i = 0; i < 4; i++) {
    lc_groups[lcg].erase(std::remove(lc_groups[lcg].begin(), lc_groups[lcg].end(), lcid), lc_groups[lcg].end());
  }
  lc_groups[lcg].push_back(lcid);
}

srslte_softbuffer_rx_t* ue::get_rx_softbuffer(const uint32_t ue_cc_idx, const uint32_t tti)
{
  return &softbuffer_rx.at(ue_cc_idx).at(tti % nof_rx_harq_proc);
}

srslte_softbuffer_tx_t*
ue::get_tx_softbuffer(const uint32_t ue_cc_idx, const uint32_t harq_process, const uint32_t tb_idx)
{
  return &softbuffer_tx.at(ue_cc_idx).at((harq_process * SRSLTE_MAX_TB + tb_idx) % nof_tx_harq_proc);
}

uint8_t* ue::request_buffer(const uint32_t ue_cc_idx, const uint32_t tti, const uint32_t len)
{
  uint8_t* ret = nullptr;
  if (len > 0) {
    if (!pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc)) {
      ret                                                      = pdus.request(len);
      pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc) = ret;
    } else {
      log_h->error("Requesting buffer for pid %d, not pushed yet\n", tti % nof_rx_harq_proc);
    }
  } else {
    log_h->warning("Requesting buffer for zero bytes\n");
  }
  return ret;
}

bool ue::process_pdus()
{
  return pdus.process_pdus();
}

void ue::set_tti(uint32_t tti)
{
  last_tti = tti;
}

uint32_t ue::set_ta(int ta_)
{
  int      ta       = ta_;
  uint32_t nof_cmd  = 0;
  int      ta_value = 0;
  do {
    ta_value = SRSLTE_MAX(-31, SRSLTE_MIN(32, ta));
    ta -= ta_value;
    uint32_t ta_cmd = (uint32_t)(ta_value + 31);
    pending_ta_commands.try_push(ta_cmd);
    nof_cmd++;
    Info("Added TA CMD: rnti=0x%x, ta=%d, ta_value=%d, ta_cmd=%d\n", rnti, ta_, ta_value, ta_cmd);
  } while (ta_value <= -31 || ta_value >= 32);
  return nof_cmd;
}

#include <assert.h>

void ue::process_pdu(uint8_t* pdu, uint32_t nof_bytes, srslte::pdu_queue::channel_t channel)
{
  // Unpack ULSCH MAC PDU
  mac_msg_ul.init_rx(nof_bytes, true);
  mac_msg_ul.parse_packet(pdu);

  if (pcap) {
    pcap->write_ul_crnti(pdu, nof_bytes, rnti, true, last_tti, UL_CC_IDX);
  }

  pdus.deallocate(pdu);

  uint32_t lcid_most_data = 0;
  int      most_data      = -99;

  while (mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (mac_msg_ul.get()->is_sdu()) {
      // Route logical channel
      log_h->debug_hex(mac_msg_ul.get()->get_sdu_ptr(),
                       mac_msg_ul.get()->get_payload_size(),
                       "PDU:   rnti=0x%x, lcid=%d, %d bytes\n",
                       rnti,
                       mac_msg_ul.get()->get_sdu_lcid(),
                       mac_msg_ul.get()->get_payload_size());

      /* In some cases, an uplink transmission with only CQI has all zeros and gets routed to RRC
       * Compute the checksum if lcid=0 and avoid routing in that case
       */
      bool route_pdu = true;
      if (mac_msg_ul.get()->get_sdu_lcid() == 0) {
        uint8_t* x   = mac_msg_ul.get()->get_sdu_ptr();
        uint32_t sum = 0;
        for (uint32_t i = 0; i < mac_msg_ul.get()->get_payload_size(); i++) {
          sum += x[i];
        }
        if (sum == 0) {
          route_pdu = false;
          Warning("Received all zero PDU\n");
        }
      }

      if (route_pdu) {
        rlc->write_pdu(rnti,
                       mac_msg_ul.get()->get_sdu_lcid(),
                       mac_msg_ul.get()->get_sdu_ptr(),
                       mac_msg_ul.get()->get_payload_size());
      }

      // Indicate scheduler to update BSR counters
      // sched->ul_recv_len(rnti, mac_msg_ul.get()->get_sdu_lcid(), mac_msg_ul.get()->get_payload_size());

      // Indicate RRC about successful activity if valid RLC message is received
      if (mac_msg_ul.get()->get_payload_size() > 64) { // do not count RLC status messages only
        rrc->set_activity_user(rnti);
        log_h->debug("UL activity rnti=0x%x, n_bytes=%d\n", rnti, nof_bytes);
      }

      if ((int)mac_msg_ul.get()->get_payload_size() > most_data) {
        most_data      = (int)mac_msg_ul.get()->get_payload_size();
        lcid_most_data = mac_msg_ul.get()->get_sdu_lcid();
      }

      // Save contention resolution if lcid == 0
      if (mac_msg_ul.get()->get_sdu_lcid() == 0 && route_pdu) {
        int nbytes = srslte::sch_subh::MAC_CE_CONTRES_LEN;
        if (mac_msg_ul.get()->get_payload_size() >= (uint32_t)nbytes) {
          uint8_t* ue_cri_ptr = (uint8_t*)&conres_id;
          uint8_t* pkt_ptr    = mac_msg_ul.get()->get_sdu_ptr(); // Warning here: we want to include the
          for (int i = 0; i < nbytes; i++) {
            ue_cri_ptr[nbytes - i - 1] = pkt_ptr[i];
          }
        } else {
          Error("Received CCCH UL message of invalid size=%d bytes\n", mac_msg_ul.get()->get_payload_size());
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

  // If BSR is not received means that new data has arrived and there is no space for BSR transmission
  if (!bsr_received && lcid_most_data > 2) {
    // Add BSR to the LCID for which most data was received
    sched->ul_bsr(rnti, lcid_most_data, 256, false); // false adds BSR instead of setting
    Debug("BSR not received. Giving extra dci\n");
  }

  Debug("MAC PDU processed\n");
}

void ue::deallocate_pdu(const uint32_t ue_cc_idx, const uint32_t tti)
{
  if (pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc)) {
    pdus.deallocate(pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc));
    pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc) = nullptr;
  } else {
    log_h->console(
        "Error deallocating buffer for ue_cc_idx=%d, pid=%d. Not requested\n", ue_cc_idx, tti % nof_rx_harq_proc);
  }
}

void ue::push_pdu(const uint32_t ue_cc_idx, const uint32_t tti, uint32_t len)
{
  if (pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc)) {
    pdus.push(pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc), len);
    pending_buffers.at(ue_cc_idx).at(tti % nof_rx_harq_proc) = nullptr;
  } else {
    log_h->console("Error pushing buffer for ue_cc_idx=%d, pid=%d. Not requested\n", ue_cc_idx, tti % nof_rx_harq_proc);
  }
}

bool ue::process_ce(srslte::sch_subh* subh)
{
  uint32_t buff_size[4] = {0, 0, 0, 0};
  float    phr          = 0;
  int32_t  idx          = 0;
  uint16_t old_rnti     = 0;
  bool     is_bsr       = false;
  switch (subh->ul_sch_ce_type()) {
    case srslte::ul_sch_lcid::PHR_REPORT:
      phr = subh->get_phr();
      Info("CE:    Received PHR from rnti=0x%x, value=%.0f\n", rnti, phr);
      sched->ul_phr(rnti, (int)phr);
      metrics_phr(phr);
      break;
    case srslte::ul_sch_lcid::CRNTI:
      old_rnti = subh->get_c_rnti();
      Info("CE:    Received C-RNTI from temp_rnti=0x%x, rnti=0x%x\n", rnti, old_rnti);
      if (sched->ue_exists(old_rnti)) {
        rrc->upd_user(rnti, old_rnti);
        rnti = old_rnti;
      } else {
        Error("Updating user C-RNTI: rnti=0x%x already released\n", old_rnti);
      }
      break;
    case srslte::ul_sch_lcid::TRUNC_BSR:
    case srslte::ul_sch_lcid::SHORT_BSR:
      idx = subh->get_bsr(buff_size);
      if (idx == -1) {
        Error("Invalid Index Passed to lc groups\n");
        break;
      }
      for (uint32_t i = 0; i < lc_groups[idx].size(); i++) {
        // Indicate BSR to scheduler
        sched->ul_bsr(rnti, lc_groups[idx][i], buff_size[idx]);
      }
      Info("CE:    Received %s BSR rnti=0x%x, lcg=%d, value=%d\n",
           subh->ul_sch_ce_type() == srslte::ul_sch_lcid::SHORT_BSR ? "Short" : "Trunc",
           rnti,
           idx,
           buff_size[idx]);
      is_bsr = true;
      break;
    case srslte::ul_sch_lcid::LONG_BSR:
      subh->get_bsr(buff_size);
      for (idx = 0; idx < 4; idx++) {
        for (uint32_t i = 0; i < lc_groups[idx].size(); i++) {
          sched->ul_bsr(rnti, lc_groups[idx][i], buff_size[idx]);
        }
      }
      is_bsr = true;
      Info("CE:    Received Long BSR rnti=0x%x, value=%d,%d,%d,%d\n",
           rnti,
           buff_size[0],
           buff_size[1],
           buff_size[2],
           buff_size[3]);
      break;
    case srslte::ul_sch_lcid::PADDING:
      Debug("CE:    Received padding for rnti=0x%x\n", rnti);
      break;
    default:
      Error("CE:    Invalid lcid=0x%x\n", (int)subh->ul_sch_ce_type());
      break;
  }
  return is_bsr;
}

int ue::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes)
{
  return rlc->read_pdu(rnti, lcid, payload, requested_bytes);
}

void ue::allocate_sdu(srslte::sch_pdu* pdu, uint32_t lcid, uint32_t total_sdu_len)
{
  int sdu_space = pdu->get_sdu_space();
  if (sdu_space > 0) {
    int sdu_len = SRSLTE_MIN(total_sdu_len, (uint32_t)sdu_space);
    int n       = 1;
    while (sdu_len >= 2 && n > 0) { // minimum size is a single RLC AM status PDU (2 Byte)
      if (pdu->new_subh()) {        // there is space for a new subheader
        log_h->debug("SDU:   set_sdu(), lcid=%d, sdu_len=%d, sdu_space=%d\n", lcid, sdu_len, sdu_space);
        n = pdu->get()->set_sdu(lcid, sdu_len, this);
        if (n > 0) { // new SDU could be added
          sdu_len -= n;
          log_h->debug("SDU:   rnti=0x%x, lcid=%d, nbytes=%d, rem_len=%d\n", rnti, lcid, n, sdu_len);
        } else {
          Debug("Could not add SDU lcid=%d nbytes=%d, space=%d\n", lcid, sdu_len, sdu_space);
          pdu->del_subh();
        }
      } else {
        n = 0;
      }
    }
  }
}

void ue::allocate_ce(srslte::sch_pdu* pdu, uint32_t lcid)
{
  switch ((srslte::dl_sch_lcid)lcid) {
    case srslte::dl_sch_lcid::TA_CMD:
      if (pdu->new_subh()) {
        uint32_t ta_cmd = 31;
        pending_ta_commands.try_pop(&ta_cmd);
        if (pdu->get()->set_ta_cmd(ta_cmd)) {
          Info("CE:    Added TA CMD=%d\n", ta_cmd);
        } else {
          Error("CE:    Setting TA CMD CE\n");
        }
      } else {
        Error("CE:    Setting TA CMD CE. No space for a subheader\n");
      }
      break;
    case srslte::dl_sch_lcid::CON_RES_ID:
      if (pdu->new_subh()) {
        if (pdu->get()->set_con_res_id(conres_id)) {
          Info("CE:    Added Contention Resolution ID=0x%" PRIx64 "\n", conres_id);
        } else {
          Error("CE:    Setting Contention Resolution ID CE\n");
        }
      } else {
        Error("CE:    Setting Contention Resolution ID CE. No space for a subheader\n");
      }
      break;
    case srslte::dl_sch_lcid::SCELL_ACTIVATION:
      if (pdu->new_subh()) {
        std::array<int, SRSLTE_MAX_CARRIERS>  enb_ue_cc_map     = sched->get_enb_ue_cc_map(rnti);
        std::array<bool, SRSLTE_MAX_CARRIERS> active_scell_list = {};
        size_t                                enb_cc_idx        = 0;
        for (; enb_cc_idx < enb_ue_cc_map.size(); ++enb_cc_idx) {
          if (enb_ue_cc_map[enb_cc_idx] >= 8) {
            break;
          }
          if (enb_ue_cc_map[enb_cc_idx] <= 0) {
            // inactive or PCell
            continue;
          }
          active_scell_list[enb_ue_cc_map[enb_cc_idx]] = true;
        }
        if (enb_cc_idx == enb_ue_cc_map.size() and pdu->get()->set_scell_activation_cmd(active_scell_list)) {
          phy->set_activation_deactivation_scell(rnti, active_scell_list);
          Info("CE:    Added SCell Activation CE.\n");
          // Allocate and initialize Rx/Tx softbuffers for new carriers (exclude PCell)
          allocate_cc_buffers(active_scell_list.size() - 1);
        } else {
          Error("CE:    Setting SCell Activation CE\n");
        }
      } else {
        Error("CE:    Setting SCell Activation CE. No space for a subheader\n");
      }
      break;
    default:
      Error("CE:    Allocating CE=0x%x. Not supported\n", lcid);
      break;
  }
}

uint8_t* ue::generate_pdu(uint32_t                        ue_cc_idx,
                          uint32_t                        harq_pid,
                          uint32_t                        tb_idx,
                          sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                          uint32_t                        nof_pdu_elems,
                          uint32_t                        grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret = nullptr;
  if (rlc) {
    if (ue_cc_idx < SRSLTE_MAX_CARRIERS && harq_pid < SRSLTE_FDD_NOF_HARQ && tb_idx < SRSLTE_MAX_TB) {
      tx_payload_buffer[ue_cc_idx][harq_pid][tb_idx]->clear();
      mac_msg_dl.init_tx(tx_payload_buffer[ue_cc_idx][harq_pid][tb_idx].get(), grant_size, false);
      for (uint32_t i = 0; i < nof_pdu_elems; i++) {
        if (pdu[i].lcid <= (uint32_t)srslte::ul_sch_lcid::PHR_REPORT) {
          allocate_sdu(&mac_msg_dl, pdu[i].lcid, pdu[i].nbytes);
        } else {
          allocate_ce(&mac_msg_dl, pdu[i].lcid);
        }
      }
      ret = mac_msg_dl.write_packet(log_h);
    } else {
      log_h->error(
          "Invalid parameters calling generate_pdu: cc_idx=%d, harq_pid=%d, tb_idx=%d\n", ue_cc_idx, harq_pid, tb_idx);
    }
  } else {
    std::cout << "Error ue not configured (must call config() first" << std::endl;
  }
  return ret;
}

uint8_t* ue::generate_mch_pdu(uint32_t                      harq_pid,
                              sched_interface::dl_pdu_mch_t sched,
                              uint32_t                      nof_pdu_elems,
                              uint32_t                      grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret = nullptr;
  tx_payload_buffer[0][harq_pid][0]->clear();
  mch_mac_msg_dl.init_tx(tx_payload_buffer[0][harq_pid][0].get(), grant_size);

  for (uint32_t i = 0; i < nof_pdu_elems; i++) {
    if (sched.pdu[i].lcid == (uint32_t)srslte::mch_lcid::MCH_SCHED_INFO) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_next_mch_sched_info(sched.mtch_sched[i].lcid, sched.mtch_sched[i].stop);
    } else if (sched.pdu[i].lcid == 0) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_sdu(0, sched.pdu[i].nbytes, sched.mcch_payload);
    } else if (sched.pdu[i].lcid <= (uint32_t)srslte::mch_lcid::MTCH_MAX_LCID) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_sdu(sched.pdu[i].lcid, sched.pdu[i].nbytes, sched.mtch_sched[i].mtch_payload);
    }
  }

  ret = mch_mac_msg_dl.write_packet(log_h);
  return ret;
}

/******* METRICS interface ***************/
void ue::metrics_read(mac_metrics_t* metrics_)
{
  metrics.rnti      = rnti;
  metrics.ul_buffer = sched->get_ul_buffer(rnti);
  metrics.dl_buffer = sched->get_dl_buffer(rnti);

  memcpy(metrics_, &metrics, sizeof(mac_metrics_t));

  phr_counter    = 0;
  dl_cqi_counter = 0;
  metrics        = {};
}

void ue::metrics_phr(float phr)
{
  metrics.phr = SRSLTE_VEC_CMA(phr, metrics.phr, phr_counter);
  phr_counter++;
}

void ue::metrics_dl_ri(uint32_t dl_ri)
{
  if (metrics.dl_ri == 0.0f) {
    metrics.dl_ri = (float)dl_ri + 1.0f;
  } else {
    metrics.dl_ri = SRSLTE_VEC_EMA((float)dl_ri + 1.0f, metrics.dl_ri, 0.5f);
  }
  dl_ri_counter++;
}

void ue::metrics_dl_pmi(uint32_t dl_ri)
{
  metrics.dl_pmi = SRSLTE_VEC_CMA((float)dl_ri, metrics.dl_pmi, dl_pmi_counter);
  dl_pmi_counter++;
}

void ue::metrics_dl_cqi(uint32_t dl_cqi)
{
  metrics.dl_cqi = SRSLTE_VEC_CMA((float)dl_cqi, metrics.dl_cqi, dl_cqi_counter);
  dl_cqi_counter++;
}

void ue::metrics_rx(bool crc, uint32_t tbs)
{
  if (crc) {
    metrics.rx_brate += tbs * 8;
  } else {
    metrics.rx_errors++;
  }
  metrics.rx_pkts++;
}

void ue::metrics_tx(bool crc, uint32_t tbs)
{
  if (crc) {
    metrics.tx_brate += tbs * 8;
  } else {
    metrics.tx_errors++;
  }
  metrics.tx_pkts++;
}

void ue::metrics_cnt()
{
  metrics.nof_tti++;
}

} // namespace srsenb
