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

#include <bitset>
#include <inttypes.h>
#include <iostream>
#include <string.h>

#include "srsenb/hdr/stack/mac/ue.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/enb_phy_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_rrc_interface_mac.h"

namespace srsenb {

ue_cc_softbuffers::ue_cc_softbuffers(uint32_t nof_prb, uint32_t nof_tx_harq_proc_, uint32_t nof_rx_harq_proc_) :
  nof_tx_harq_proc(nof_tx_harq_proc_), nof_rx_harq_proc(nof_rx_harq_proc_)
{
  // Create and init Rx buffers
  softbuffer_rx_list.resize(nof_rx_harq_proc);
  for (srsran_softbuffer_rx_t& buffer : softbuffer_rx_list) {
    srsran_softbuffer_rx_init(&buffer, nof_prb);
  }

  // Create and init Tx buffers
  softbuffer_tx_list.resize(nof_tx_harq_proc * SRSRAN_MAX_TB);
  for (auto& buffer : softbuffer_tx_list) {
    srsran_softbuffer_tx_init(&buffer, nof_prb);
  }
}

ue_cc_softbuffers::~ue_cc_softbuffers()
{
  for (auto& buffer : softbuffer_rx_list) {
    srsran_softbuffer_rx_free(&buffer);
  }
  softbuffer_rx_list.clear();

  for (auto& buffer : softbuffer_tx_list) {
    srsran_softbuffer_tx_free(&buffer);
  }
  softbuffer_tx_list.clear();
}

void ue_cc_softbuffers::clear()
{
  for (auto& buffer : softbuffer_rx_list) {
    srsran_softbuffer_rx_reset(&buffer);
  }
  for (auto& buffer : softbuffer_tx_list) {
    srsran_softbuffer_tx_reset(&buffer);
  }
}

cc_used_buffers_map::cc_used_buffers_map() : logger(&srslog::fetch_basic_logger("MAC")) {}

cc_used_buffers_map::~cc_used_buffers_map()
{
  clear();
}

srsran::unique_byte_buffer_t cc_used_buffers_map::release_pdu(tti_point tti)
{
  std::unique_lock<std::mutex> lock(mutex);
  if (not has_tti(tti)) {
    return nullptr;
  }

  // Extract PDU from PDU map
  srsran::unique_byte_buffer_t pdu = std::move(pdu_map[tti.to_uint()]);

  // clear entry in map
  pdu_map.erase(tti.to_uint());
  return pdu;
}

uint8_t* cc_used_buffers_map::request_pdu(tti_point tti, uint32_t len)
{
  std::unique_lock<std::mutex> lock(mutex);
  if (not pdu_map.has_space(tti.to_uint())) {
    logger->error("UE buffers: could not allocate buffer for tti=%d", tti.to_uint());
    return nullptr;
  }

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger->error("UE buffers: Requesting buffer from byte buffer pool");
    return nullptr;
  }
  srsran_assert(len < pdu->get_tailroom(), "Requested UL pdu doesn't fit in byte_buffer");
  pdu->N_bytes = len;

  auto inserted_elem = pdu_map.insert(tti.to_uint(), std::move(pdu));
  srsran_assert(inserted_elem.has_value(), "Failure to allocate new buffer in mac::ue");
  return inserted_elem.value()->second->data();
}

void cc_used_buffers_map::clear_old_pdus(tti_point current_tti)
{
  std::unique_lock<std::mutex> lock(mutex);
  static const uint32_t        old_tti_threshold = SRSRAN_FDD_NOF_HARQ + 4;

  tti_point max_tti{current_tti - old_tti_threshold};
  for (auto& pdu_pair : pdu_map) {
    tti_point t(pdu_pair.first);
    if (t < max_tti) {
      logger->warning("UE buffers: Removing old buffer tti=%d, interval=%d", t.to_uint(), current_tti - t);
      pdu_map.erase(t.to_uint());
    }
  }
}

uint8_t*& cc_used_buffers_map::operator[](tti_point tti)
{
  return pdu_map[tti.to_uint()]->msg;
}

bool cc_used_buffers_map::has_tti(tti_point tti) const
{
  return pdu_map.contains(tti.to_uint()) and pdu_map[tti.to_uint()] != nullptr;
}

////////////////

cc_buffer_handler::cc_buffer_handler()
{
  for (auto& harq_buffers : tx_payload_buffer) {
    for (srsran::unique_byte_buffer_t& tb_buffer : harq_buffers) {
      tb_buffer = srsran::make_byte_buffer();
      if (tb_buffer == nullptr) {
        srslog::fetch_basic_logger("MAC").error("Failed to allocate HARQ buffers for UE");
        return;
      }
    }
  }
}

cc_buffer_handler::~cc_buffer_handler()
{
  deallocate_cc();
}

/**
 * Allocate and initialize softbuffers for Tx and Rx. It uses the configured
 * number of HARQ processes and cell width.
 *
 * @param num_cc Number of carriers to add buffers for (default 1)
 * @return number of carriers
 */
void cc_buffer_handler::allocate_cc(srsran::unique_pool_ptr<ue_cc_softbuffers> cc_softbuffers_)
{
  srsran_assert(empty(), "Cannot allocate softbuffers in CC that is already initialized");
  cc_softbuffers = std::move(cc_softbuffers_);
}

void cc_buffer_handler::deallocate_cc()
{
  cc_softbuffers.reset();
}

void cc_buffer_handler::reset()
{
  if (not empty()) {
    cc_softbuffers->clear();
  }
}

ue::ue(uint16_t                                 rnti_,
       uint32_t                                 enb_cc_idx,
       sched_interface*                         sched_,
       rrc_interface_mac*                       rrc_,
       rlc_interface_mac*                       rlc_,
       phy_interface_stack_lte*                 phy_,
       srslog::basic_logger&                    logger_,
       uint32_t                                 nof_cells_,
       srsran::obj_pool_itf<ue_cc_softbuffers>* softbuffer_pool_) :
  rnti(rnti_),
  sched(sched_),
  rrc(rrc_),
  rlc(rlc_),
  phy(phy_),
  logger(logger_),
  mac_msg_dl(20, logger_),
  mch_mac_msg_dl(10, logger_),
  mac_msg_ul(20, logger_),
  ta_fsm(this),
  softbuffer_pool(softbuffer_pool_),
  cc_buffers(nof_cells_)
{
  // Allocate buffer for PCell
  cc_buffers[enb_cc_idx].allocate_cc(softbuffer_pool->make());
}

ue::~ue() {}

void ue::reset()
{
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    ue_metrics = {};
  }
  nof_failures = 0;

  for (auto& cc : cc_buffers) {
    cc.reset();
  }
}

void ue::start_pcap_net(srsran::mac_pcap_net* pcap_net_)
{
  pcap_net = pcap_net_;
}

void ue::start_pcap(srsran::mac_pcap* pcap_)
{
  pcap = pcap_;
}

void ue::ue_cfg(const sched_interface::ue_cfg_t& ue_cfg)
{
  for (const auto& ue_cc : ue_cfg.supported_cc_list) {
    // Allocate and initialize Rx/Tx softbuffers for new carriers (exclude PCell)
    if (ue_cc.active and cc_buffers[ue_cc.enb_cc_idx].empty()) {
      cc_buffers[ue_cc.enb_cc_idx].allocate_cc(softbuffer_pool->make());
    }
  }
}

srsran_softbuffer_rx_t* ue::get_rx_softbuffer(uint32_t enb_cc_idx, uint32_t tti)
{
  if ((size_t)enb_cc_idx >= cc_buffers.size() or cc_buffers[enb_cc_idx].empty()) {
    ERROR("eNB CC Index (%d/%zd) out-of-range", enb_cc_idx, cc_buffers.size());
    return nullptr;
  }

  return &cc_buffers[enb_cc_idx].get_rx_softbuffer(tti);
}

srsran_softbuffer_tx_t* ue::get_tx_softbuffer(uint32_t enb_cc_idx, uint32_t harq_process, uint32_t tb_idx)
{
  if ((size_t)enb_cc_idx >= cc_buffers.size() or cc_buffers[enb_cc_idx].empty()) {
    ERROR("eNB CC Index (%d/%zd) out-of-range", enb_cc_idx, cc_buffers.size());
    return nullptr;
  }

  return &cc_buffers[enb_cc_idx].get_tx_softbuffer(harq_process, tb_idx);
}

uint8_t* ue::request_buffer(uint32_t tti, uint32_t enb_cc_idx, uint32_t len)
{
  srsran_assert(len > 0, "UE buffers: Requesting buffer for zero bytes");
  return cc_buffers[enb_cc_idx].get_rx_used_buffers().request_pdu(tti_point(tti), len);
}

void ue::clear_old_buffers(uint32_t tti)
{
  // remove old buffers
  for (auto& cc : cc_buffers) {
    cc.get_rx_used_buffers().clear_old_pdus(tti_point{tti});
  }
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
    ta_value = SRSRAN_MAX(-31, SRSRAN_MIN(32, ta));
    ta -= ta_value;
    uint32_t ta_cmd = (uint32_t)(ta_value + 31);
    pending_ta_commands.try_push(ta_cmd);
    nof_cmd++;
    logger.info("Added TA CMD: rnti=0x%x, ta=%d, ta_value=%d, ta_cmd=%d", rnti, ta_, ta_value, ta_cmd);
  } while (ta_value <= -31 || ta_value >= 32);
  return nof_cmd;
}

void ue::process_pdu(srsran::unique_byte_buffer_t pdu, uint32_t ue_cc_idx, uint32_t grant_nof_prbs)
{
  // Unpack ULSCH MAC PDU
  mac_msg_ul.init_rx(pdu->size(), true);
  mac_msg_ul.parse_packet(pdu->data());

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    mac_msg_ul.to_string(str_buffer);
    logger.info("0x%x %s", rnti, srsran::to_c_str(str_buffer));
  }

  if (pcap != nullptr) {
    pcap->write_ul_crnti(pdu->data(), pdu->size(), rnti, true, last_tti, ue_cc_idx);
  }

  if (pcap_net != nullptr) {
    pcap_net->write_ul_crnti(pdu->data(), pdu->size(), rnti, true, last_tti, ue_cc_idx);
  }

  uint32_t lcid_most_data = 0;
  int      most_data      = -99;

  while (mac_msg_ul.next()) {
    assert(mac_msg_ul.get());
    if (mac_msg_ul.get()->is_sdu()) {
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
          logger.debug("Received all zero PDU");
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

      // Indicate DRB activity in UL to RRC
      if (mac_msg_ul.get()->get_sdu_lcid() > 2) {
        rrc->set_activity_user(rnti);
        logger.debug("UL activity rnti=0x%x, n_bytes=%d", rnti, pdu->size());
      }

      if ((int)mac_msg_ul.get()->get_payload_size() > most_data) {
        most_data      = (int)mac_msg_ul.get()->get_payload_size();
        lcid_most_data = mac_msg_ul.get()->get_sdu_lcid();
      }

      // Save contention resolution if lcid == 0
      if (mac_msg_ul.get()->get_sdu_lcid() == 0 && route_pdu) {
        int nbytes = srsran::sch_subh::MAC_CE_CONTRES_LEN;
        if (mac_msg_ul.get()->get_payload_size() >= (uint32_t)nbytes) {
          uint8_t* ue_cri_ptr = (uint8_t*)&conres_id;
          uint8_t* pkt_ptr    = mac_msg_ul.get()->get_sdu_ptr(); // Warning here: we want to include the
          for (int i = 0; i < nbytes; i++) {
            ue_cri_ptr[nbytes - i - 1] = pkt_ptr[i];
          }
        } else {
          logger.error("Received CCCH UL message of invalid size=%d bytes", mac_msg_ul.get()->get_payload_size());
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
      bsr_received |= process_ce(mac_msg_ul.get(), grant_nof_prbs);
    }
  }

  // If BSR is not received means that new data has arrived and there is no space for BSR transmission
  if (!bsr_received && lcid_most_data > 2) {
    // Add BSR to the LCID for which most data was received
    sched->ul_buffer_add(rnti, lcid_most_data, 256);
    logger.debug("BSR not received. Giving extra dci");
  }

  logger.debug("MAC PDU processed");
}

srsran::unique_byte_buffer_t ue::release_pdu(uint32_t tti, uint32_t enb_cc_idx)
{
  return cc_buffers[enb_cc_idx].get_rx_used_buffers().release_pdu(tti_point(tti));
}

bool ue::process_ce(srsran::sch_subh* subh, uint32_t grant_nof_prbs)
{
  uint32_t buff_size_idx[4]   = {};
  uint32_t buff_size_bytes[4] = {};
  float    phr                = 0;
  int32_t  idx                = 0;
  uint16_t old_rnti           = 0;
  bool     is_bsr             = false;
  switch (subh->ul_sch_ce_type()) {
    case srsran::ul_sch_lcid::PHR_REPORT:
      phr = subh->get_phr();
      srsran_assert(grant_nof_prbs > 0, "Invalid nof prbs=%d provided for PHR handling", grant_nof_prbs);
      sched->ul_phr(rnti, (int)phr, grant_nof_prbs);
      metrics_phr(phr);
      break;
    case srsran::ul_sch_lcid::CRNTI:
      old_rnti = subh->get_c_rnti();
      if (sched->ue_exists(old_rnti)) {
        rrc->upd_user(rnti, old_rnti);
        rnti = old_rnti;
      } else {
        logger.warning("Updating user C-RNTI: rnti=0x%x already released.", old_rnti);
        // Disable scheduling for all bearers. The new rnti will be removed on msg3 timer expiry in the RRC
        for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; ++lcid) {
          sched->bearer_ue_rem(rnti, lcid);
        }
      }
      break;
    case srsran::ul_sch_lcid::TRUNC_BSR:
    case srsran::ul_sch_lcid::SHORT_BSR:
      idx = subh->get_bsr(buff_size_idx, buff_size_bytes);
      if (idx == -1) {
        logger.error("Invalid Index Passed to lc groups");
        break;
      }
      // Indicate BSR to scheduler
      sched->ul_bsr(rnti, idx, buff_size_bytes[idx]);
      is_bsr = true;
      break;
    case srsran::ul_sch_lcid::LONG_BSR:
      subh->get_bsr(buff_size_idx, buff_size_bytes);
      for (idx = 0; idx < sched_interface::MAX_LC_GROUP; ++idx) {
        sched->ul_bsr(rnti, idx, buff_size_bytes[idx]);
      }
      is_bsr = true;
      break;
    case srsran::ul_sch_lcid::PADDING:
      break;
    default:
      logger.error("CE:    Invalid lcid=0x%x", (int)subh->ul_sch_ce_type());
      break;
  }
  return is_bsr;
}

uint32_t ue::read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes)
{
  return rlc->read_pdu(rnti, lcid, payload, requested_bytes);
}

void ue::allocate_sdu(srsran::sch_pdu* pdu, uint32_t lcid, uint32_t total_sdu_len)
{
  const int min_sdu_len = lcid == 0 ? 1 : 2;
  int       sdu_space   = pdu->get_sdu_space();
  if (sdu_space > 0) {
    int sdu_len = SRSRAN_MIN(total_sdu_len, (uint32_t)sdu_space);
    int n       = 1;
    while (sdu_len >= min_sdu_len && n > 0) { // minimum size is a single RLC AM status PDU (2 Byte)
      if (pdu->new_subh()) {                  // there is space for a new subheader
        logger.debug("SDU:   set_sdu(), lcid=%d, sdu_len=%d, sdu_space=%d", lcid, sdu_len, sdu_space);
        n = pdu->get()->set_sdu(lcid, sdu_len, this);
        if (n > 0) { // new SDU could be added
          sdu_len -= n;
          logger.debug("SDU:   rnti=0x%x, lcid=%d, nbytes=%d, rem_len=%d", rnti, lcid, n, sdu_len);

          // Indicate DRB activity in DL to RRC
          if (lcid > 2) {
            rrc->set_activity_user(rnti);
            logger.debug("DL activity rnti=0x%x, n_bytes=%d", rnti, sdu_len);
          }

        } else {
          logger.debug("Could not add SDU lcid=%d nbytes=%d, space=%d", lcid, sdu_len, sdu_space);
          pdu->del_subh();
        }
      } else {
        n = 0;
      }
    }
  }
}

void ue::allocate_ce(srsran::sch_pdu* pdu, uint32_t lcid)
{
  switch ((srsran::dl_sch_lcid)lcid) {
    case srsran::dl_sch_lcid::TA_CMD:
      if (pdu->new_subh()) {
        uint32_t ta_cmd = 31;
        pending_ta_commands.try_pop(&ta_cmd);
        if (!pdu->get()->set_ta_cmd(ta_cmd)) {
          logger.error("CE:    Setting TA CMD CE");
        }
      } else {
        logger.error("CE:    Setting TA CMD CE. No space for a subheader");
      }
      break;
    case srsran::dl_sch_lcid::CON_RES_ID:
      if (pdu->new_subh()) {
        if (!pdu->get()->set_con_res_id(conres_id)) {
          logger.error("CE:    Setting Contention Resolution ID CE");
        }
      } else {
        logger.error("CE:    Setting Contention Resolution ID CE. No space for a subheader");
      }
      break;
    case srsran::dl_sch_lcid::SCELL_ACTIVATION:
      if (pdu->new_subh()) {
        std::array<int, SRSRAN_MAX_CARRIERS>  active_ccs = sched->get_enb_ue_activ_cc_map(rnti);
        std::array<bool, SRSRAN_MAX_CARRIERS> active_scell_list{};
        for (int ue_cc_idx : active_ccs) {
          if (ue_cc_idx > 0) {
            active_scell_list[ue_cc_idx] = true;
          }
        }
        if (pdu->get()->set_scell_activation_cmd(active_scell_list)) {
          phy->set_activation_deactivation_scell(rnti, active_scell_list);
        } else {
          logger.error("CE:    Setting SCell Activation CE");
        }
      } else {
        logger.error("CE:    Setting SCell Activation CE. No space for a subheader");
      }
      break;
    default:
      logger.error("CE:    Allocating CE=0x%x. Not supported", lcid);
      break;
  }
}

uint8_t* ue::generate_pdu(uint32_t                              enb_cc_idx,
                          uint32_t                              harq_pid,
                          uint32_t                              tb_idx,
                          const sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                          uint32_t                              nof_pdu_elems,
                          uint32_t                              grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret = nullptr;
  if (enb_cc_idx < SRSRAN_MAX_CARRIERS && harq_pid < SRSRAN_FDD_NOF_HARQ && tb_idx < SRSRAN_MAX_TB) {
    srsran::byte_buffer_t* buffer = cc_buffers[enb_cc_idx].get_tx_payload_buffer(harq_pid, tb_idx);
    buffer->clear();
    mac_msg_dl.init_tx(buffer, grant_size, false);
    for (uint32_t i = 0; i < nof_pdu_elems; i++) {
      if (pdu[i].lcid <= (uint32_t)srsran::ul_sch_lcid::PHR_REPORT) {
        allocate_sdu(&mac_msg_dl, pdu[i].lcid, pdu[i].nbytes);
      } else {
        allocate_ce(&mac_msg_dl, pdu[i].lcid);
      }
    }
    ret = mac_msg_dl.write_packet(logger);
    if (logger.info.enabled()) {
      fmt::memory_buffer str_buffer;
      mac_msg_dl.to_string(str_buffer);
      logger.info("0x%x %s", rnti, srsran::to_c_str(str_buffer));
    }
  } else {
    logger.error(
        "Invalid parameters calling generate_pdu: cc_idx=%d, harq_pid=%d, tb_idx=%d", enb_cc_idx, harq_pid, tb_idx);
  }
  return ret;
}

uint8_t* ue::generate_mch_pdu(uint32_t                             harq_pid,
                              const sched_interface::dl_pdu_mch_t& sched_,
                              uint32_t                             nof_pdu_elems,
                              uint32_t                             grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret    = nullptr;
  srsran::byte_buffer_t*      buffer = cc_buffers[0].get_tx_payload_buffer(harq_pid, 0);
  buffer->clear();
  mch_mac_msg_dl.init_tx(buffer, grant_size);

  for (uint32_t i = 0; i < nof_pdu_elems; i++) {
    if (sched_.pdu[i].lcid == (uint32_t)srsran::mch_lcid::MCH_SCHED_INFO) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_next_mch_sched_info(sched_.mtch_sched[i].lcid, sched_.mtch_sched[i].stop);
    } else if (sched_.pdu[i].lcid == 0) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_sdu(0, sched_.pdu[i].nbytes, sched_.mcch_payload);
    } else if (sched_.pdu[i].lcid <= (uint32_t)srsran::mch_lcid::MTCH_MAX_LCID) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_sdu(sched_.pdu[i].lcid, sched_.pdu[i].nbytes, sched_.mtch_sched[i].mtch_payload);
    }
  }

  ret = mch_mac_msg_dl.write_packet(logger);
  return ret;
}

/******* METRICS interface ***************/
void ue::metrics_read(mac_ue_metrics_t* metrics_)
{
  uint32_t ul_buffer = sched->get_ul_buffer(rnti);
  uint32_t dl_buffer = sched->get_dl_buffer(rnti);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.rnti      = rnti;
  ue_metrics.ul_buffer = ul_buffer;
  ue_metrics.dl_buffer = dl_buffer;

  // set PCell sector id
  std::array<int, SRSRAN_MAX_CARRIERS> cc_list = sched->get_enb_ue_cc_map(rnti);
  auto                                 it      = std::find(cc_list.begin(), cc_list.end(), 0);
  ue_metrics.cc_idx                            = std::distance(cc_list.begin(), it);

  *metrics_ = ue_metrics;

  phr_counter    = 0;
  dl_cqi_counter = 0;
  ue_metrics     = {};
}

void ue::metrics_phr(float phr)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.phr = SRSRAN_VEC_CMA(phr, ue_metrics.phr, phr_counter);
  phr_counter++;
}

void ue::metrics_dl_ri(uint32_t dl_ri)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  if (ue_metrics.dl_ri == 0.0f) {
    ue_metrics.dl_ri = (float)dl_ri + 1.0f;
  } else {
    ue_metrics.dl_ri = SRSRAN_VEC_EMA((float)dl_ri + 1.0f, ue_metrics.dl_ri, 0.5f);
  }
  dl_ri_counter++;
}

void ue::metrics_dl_pmi(uint32_t dl_ri)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.dl_pmi = SRSRAN_VEC_CMA((float)dl_ri, ue_metrics.dl_pmi, dl_pmi_counter);
  dl_pmi_counter++;
}

void ue::metrics_dl_cqi(uint32_t dl_cqi)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.dl_cqi = SRSRAN_VEC_CMA((float)dl_cqi, ue_metrics.dl_cqi, dl_cqi_counter);
  dl_cqi_counter++;
}

void ue::metrics_rx(bool crc, uint32_t tbs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  if (crc) {
    ue_metrics.rx_brate += tbs * 8;
  } else {
    ue_metrics.rx_errors++;
  }
  ue_metrics.rx_pkts++;
}

void ue::metrics_tx(bool crc, uint32_t tbs)
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  if (crc) {
    ue_metrics.tx_brate += tbs * 8;
  } else {
    ue_metrics.tx_errors++;
  }
  ue_metrics.tx_pkts++;
}

void ue::metrics_cnt()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  ue_metrics.nof_tti++;
}

void ue::tic()
{
  // Check for pending TA commands
  uint32_t nof_ta_count = ta_fsm.tick();
  if (nof_ta_count) {
    sched->dl_mac_buffer_state(rnti, (uint32_t)srsran::dl_sch_lcid::TA_CMD, nof_ta_count);
  }
}

void ue::trigger_padding(int lcid)
{
  sched->ul_bsr(rnti, lcid, 20e6);
}

} // namespace srsenb
