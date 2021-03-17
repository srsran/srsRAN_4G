/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <bitset>
#include <inttypes.h>
#include <iostream>
#include <string.h>

#include "srsenb/hdr/stack/mac/ue.h"
#include "srslte/common/string_helpers.h"
#include "srslte/interfaces/enb_phy_interfaces.h"
#include "srslte/interfaces/enb_rlc_interfaces.h"
#include "srslte/interfaces/enb_rrc_interfaces.h"

namespace srsenb {

cc_used_buffers_map::cc_used_buffers_map(srslte::pdu_queue& shared_pdu_queue_) :
  pdu_map(), shared_pdu_queue(&shared_pdu_queue_), logger(&srslog::fetch_basic_logger("MAC"))
{}

bool cc_used_buffers_map::push_pdu(tti_point tti, uint32_t len)
{
  if (not has_tti(tti)) {
    return false;
  }
  auto& pdu_pair = pdu_map[tti.to_uint()];
  if (len > 0) {
    shared_pdu_queue->push(pdu_pair.second, len);
  } else {
    shared_pdu_queue->deallocate(pdu_pair.second);
    logger->error("Error pushing PDU: null length");
  }
  // clear entry in map
  pdu_pair.first  = tti_point();
  pdu_pair.second = nullptr;
  return len > 0;
}

uint8_t* cc_used_buffers_map::request_pdu(tti_point tti, uint32_t len)
{
  if (pdu_map[tti.to_uint()].second != nullptr) {
    logger->error("UE buffers: buffer for tti=%d already allocated", tti.to_uint());
    return nullptr;
  }

  uint8_t* pdu = shared_pdu_queue->request(len);
  if (pdu == nullptr) {
    logger->error("UE buffers: Requesting buffer from pool");
    return nullptr;
  }

  pdu_map[tti.to_uint()].first  = tti;
  pdu_map[tti.to_uint()].second = pdu;
  return pdu;
}

void cc_used_buffers_map::clear_old_pdus(tti_point current_tti)
{
  static const uint32_t old_tti_threshold = SRSLTE_FDD_NOF_HARQ + 4;

  tti_point max_tti{current_tti - old_tti_threshold};
  for (auto& pdu_pair : pdu_map) {
    if (pdu_pair.second != nullptr and pdu_pair.first < max_tti) {
      logger->warning("UE buffers: Removing old buffer tti=%d, interval=%d",
                      pdu_pair.first.to_uint(),
                      current_tti - pdu_pair.first);
      remove_pdu(pdu_pair.first);
    }
  }
}

void cc_used_buffers_map::remove_pdu(tti_point tti)
{
  auto& pdu_pair = pdu_map[tti.to_uint()];
  assert(pdu_pair.second != nullptr && "cannot remove inexistent PDU");
  // return pdus back to the queue
  shared_pdu_queue->deallocate(pdu_pair.second);
  // clear entry in map
  pdu_pair.first  = tti_point();
  pdu_pair.second = nullptr;
}

bool cc_used_buffers_map::try_deallocate_pdu(tti_point tti)
{
  if (pdu_map[tti.to_uint()].second != nullptr) {
    remove_pdu(tti);
    return true;
  }
  return false;
}

void cc_used_buffers_map::clear()
{
  for (auto& pdu : pdu_map) {
    try_deallocate_pdu(pdu.first);
  }
}

uint8_t*& cc_used_buffers_map::operator[](tti_point tti)
{
  assert(has_tti(tti) && "Trying to access buffer that does not exist");
  return pdu_map[tti.to_uint()].second;
}

bool cc_used_buffers_map::has_tti(tti_point tti) const
{
  return pdu_map[tti.to_uint()].first == tti and pdu_map[tti.to_uint()].second != nullptr;
}

////////////////

cc_buffer_handler::cc_buffer_handler(srslte::pdu_queue& shared_pdu_queue_) : rx_used_buffers(shared_pdu_queue_)
{
  for (auto& harq_buffers : tx_payload_buffer) {
    for (srslte::unique_byte_buffer_t& tb_buffer : harq_buffers) {
      tb_buffer = srslte::make_byte_buffer();
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
void cc_buffer_handler::allocate_cc(uint32_t nof_prb_, uint32_t nof_rx_harq_proc_, uint32_t nof_tx_harq_proc_)
{
  assert(empty());
  nof_prb          = nof_prb_;
  nof_rx_harq_proc = nof_rx_harq_proc_;
  nof_tx_harq_proc = nof_tx_harq_proc_;

  // Create and init Rx buffers
  softbuffer_rx_list.resize(nof_rx_harq_proc);
  for (srslte_softbuffer_rx_t& buffer : softbuffer_rx_list) {
    srslte_softbuffer_rx_init(&buffer, nof_prb);
  }

  // Create and init Tx buffers
  softbuffer_tx_list.resize(nof_tx_harq_proc * SRSLTE_MAX_TB);
  for (auto& buffer : softbuffer_tx_list) {
    srslte_softbuffer_tx_init(&buffer, nof_prb);
  }
}

void cc_buffer_handler::deallocate_cc()
{
  for (auto& buffer : softbuffer_rx_list) {
    srslte_softbuffer_rx_free(&buffer);
  }
  softbuffer_rx_list.clear();

  for (auto& buffer : softbuffer_tx_list) {
    srslte_softbuffer_tx_free(&buffer);
  }
  softbuffer_tx_list.clear();
}

void cc_buffer_handler::reset()
{
  for (auto& buffer : softbuffer_rx_list) {
    srslte_softbuffer_rx_reset(&buffer);
  }
  for (auto& buffer : softbuffer_tx_list) {
    srslte_softbuffer_tx_reset(&buffer);
  }
}

ue::ue(uint16_t                 rnti_,
       uint32_t                 nof_prb_,
       sched_interface*         sched_,
       rrc_interface_mac*       rrc_,
       rlc_interface_mac*       rlc_,
       phy_interface_stack_lte* phy_,
       srslog::basic_logger&    logger_,
       uint32_t                 nof_cells_,
       uint32_t                 nof_rx_harq_proc_,
       uint32_t                 nof_tx_harq_proc_) :
  rnti(rnti_),
  nof_prb(nof_prb_),
  sched(sched_),
  rrc(rrc_),
  rlc(rlc_),
  phy(phy_),
  logger(logger_),
  mac_msg_dl(20, logger_),
  mch_mac_msg_dl(10, logger_),
  mac_msg_ul(20, logger_),
  pdus(logger_),
  nof_rx_harq_proc(nof_rx_harq_proc_),
  nof_tx_harq_proc(nof_tx_harq_proc_),
  ta_fsm(this)
{
  for (size_t i = 0; i < nof_cells_; ++i) {
    cc_buffers.emplace_back(pdus);
  }
  pdus.init(this);

  // Allocate buffer for PCell
  cc_buffers[0].allocate_cc(nof_prb, nof_rx_harq_proc, nof_tx_harq_proc);
}

ue::~ue()
{
  std::unique_lock<std::mutex> lock(rx_buffers_mutex);
  for (auto& cc : cc_buffers) {
    cc.get_rx_used_buffers().clear();
  }
}

void ue::reset()
{
  ue_metrics   = {};
  nof_failures = 0;

  for (auto& cc : cc_buffers) {
    cc.reset();
  }
}

void ue::start_pcap_net(srslte::mac_pcap_net* pcap_net_)
{
  pcap_net = pcap_net_;
}

void ue::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_;
}

srslte_softbuffer_rx_t* ue::get_rx_softbuffer(const uint32_t ue_cc_idx, const uint32_t tti)
{
  if ((size_t)ue_cc_idx >= cc_buffers.size()) {
    ERROR("UE CC Index (%d/%zd) out-of-range", ue_cc_idx, cc_buffers.size());
    return nullptr;
  }

  return &cc_buffers[ue_cc_idx].get_rx_softbuffer(tti % nof_rx_harq_proc);
}

srslte_softbuffer_tx_t*
ue::get_tx_softbuffer(const uint32_t ue_cc_idx, const uint32_t harq_process, const uint32_t tb_idx)
{
  if ((size_t)ue_cc_idx >= cc_buffers.size()) {
    ERROR("UE CC Index (%d/%zd) out-of-range", ue_cc_idx, cc_buffers.size());
    return nullptr;
  }

  return &cc_buffers[ue_cc_idx].get_tx_softbuffer(harq_process, tb_idx);
}

uint8_t* ue::request_buffer(uint32_t tti, uint32_t ue_cc_idx, const uint32_t len)
{
  std::unique_lock<std::mutex> lock(rx_buffers_mutex);
  uint8_t*                     pdu = nullptr;
  if (len > 0) {
    pdu = cc_buffers[ue_cc_idx].get_rx_used_buffers().request_pdu(tti_point(tti), len);
    logger.info("request_buffer: allocated for rnti=0x%x, tti=%d", rnti, tti);
  } else {
    logger.error("UE buffers: Requesting buffer for zero bytes");
  }
  return pdu;
}

void ue::clear_old_buffers(uint32_t tti)
{
  std::unique_lock<std::mutex> lock(rx_buffers_mutex);

  // remove old buffers
  for (auto& cc : cc_buffers) {
    cc.get_rx_used_buffers().clear_old_pdus(tti_point{tti});
  }
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
    logger.info("Added TA CMD: rnti=0x%x, ta=%d, ta_value=%d, ta_cmd=%d", rnti, ta_, ta_value, ta_cmd);
  } while (ta_value <= -31 || ta_value >= 32);
  return nof_cmd;
}

void ue::process_pdu(uint8_t* pdu, uint32_t nof_bytes, srslte::pdu_queue::channel_t channel)
{
  // Unpack ULSCH MAC PDU
  mac_msg_ul.init_rx(nof_bytes, true);
  mac_msg_ul.parse_packet(pdu);

  if (logger.info.enabled()) {
    fmt::memory_buffer str_buffer;
    mac_msg_ul.to_string(str_buffer);
    logger.info("0x%x %s", rnti, srslte::to_c_str(str_buffer));
  }

  if (pcap) {
    pcap->write_ul_crnti(pdu, nof_bytes, rnti, true, last_tti, UL_CC_IDX);
  }

  if (pcap_net) {
    pcap_net->write_ul_crnti(pdu, nof_bytes, rnti, true, last_tti, UL_CC_IDX);
  }

  pdus.deallocate(pdu);

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

      // Indicate RRC about successful activity if valid RLC message is received
      if (mac_msg_ul.get()->get_payload_size() > 64) { // do not count RLC status messages only
        rrc->set_activity_user(rnti);
        logger.debug("UL activity rnti=0x%x, n_bytes=%d", rnti, nof_bytes);
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
      bsr_received |= process_ce(mac_msg_ul.get());
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

void ue::deallocate_pdu(uint32_t tti, uint32_t ue_cc_idx)
{
  std::unique_lock<std::mutex> lock(rx_buffers_mutex);
  if (not cc_buffers[ue_cc_idx].get_rx_used_buffers().try_deallocate_pdu(tti_point(tti))) {
    logger.warning("UE buffers: Null RX PDU pointer in deallocate_pdu for rnti=0x%x tti=%d cc_idx=%d",
                   rnti,
                   tti,
                   ue_cc_idx);
  }
}

void ue::push_pdu(uint32_t tti, uint32_t ue_cc_idx, uint32_t len)
{
  std::unique_lock<std::mutex> lock(rx_buffers_mutex);
  if (not cc_buffers[ue_cc_idx].get_rx_used_buffers().push_pdu(tti_point(tti), len)) {
    logger.warning(
        "UE buffers: Failed to push RX PDU for rnti=0x%x tti=%d cc_idx=%d", rnti, tti, ue_cc_idx);
  }
}

bool ue::process_ce(srslte::sch_subh* subh)
{
  uint32_t buff_size_idx[4]   = {};
  uint32_t buff_size_bytes[4] = {};
  float    phr                = 0;
  int32_t  idx                = 0;
  uint16_t old_rnti           = 0;
  bool     is_bsr             = false;
  switch (subh->ul_sch_ce_type()) {
    case srslte::ul_sch_lcid::PHR_REPORT:
      phr = subh->get_phr();
      sched->ul_phr(rnti, (int)phr);
      metrics_phr(phr);
      break;
    case srslte::ul_sch_lcid::CRNTI:
      old_rnti = subh->get_c_rnti();
      if (sched->ue_exists(old_rnti)) {
        rrc->upd_user(rnti, old_rnti);
        rnti = old_rnti;
      } else {
        logger.error("Updating user C-RNTI: rnti=0x%x already released", old_rnti);
      }
      break;
    case srslte::ul_sch_lcid::TRUNC_BSR:
    case srslte::ul_sch_lcid::SHORT_BSR:
      idx = subh->get_bsr(buff_size_idx, buff_size_bytes);
      if (idx == -1) {
        logger.error("Invalid Index Passed to lc groups");
        break;
      }
      // Indicate BSR to scheduler
      sched->ul_bsr(rnti, idx, buff_size_bytes[idx]);
      is_bsr = true;
      break;
    case srslte::ul_sch_lcid::LONG_BSR:
      subh->get_bsr(buff_size_idx, buff_size_bytes);
      for (idx = 0; idx < sched_interface::MAX_LC_GROUP; ++idx) {
        sched->ul_bsr(rnti, idx, buff_size_bytes[idx]);
      }
      is_bsr = true;
      break;
    case srslte::ul_sch_lcid::PADDING:
      break;
    default:
      logger.error("CE:    Invalid lcid=0x%x", (int)subh->ul_sch_ce_type());
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
  const int min_sdu_len = lcid == 0 ? 1 : 2;
  int       sdu_space   = pdu->get_sdu_space();
  if (sdu_space > 0) {
    int sdu_len = SRSLTE_MIN(total_sdu_len, (uint32_t)sdu_space);
    int n       = 1;
    while (sdu_len >= min_sdu_len && n > 0) { // minimum size is a single RLC AM status PDU (2 Byte)
      if (pdu->new_subh()) {                  // there is space for a new subheader
        logger.debug("SDU:   set_sdu(), lcid=%d, sdu_len=%d, sdu_space=%d", lcid, sdu_len, sdu_space);
        n = pdu->get()->set_sdu(lcid, sdu_len, this);
        if (n > 0) { // new SDU could be added
          sdu_len -= n;
          logger.debug("SDU:   rnti=0x%x, lcid=%d, nbytes=%d, rem_len=%d", rnti, lcid, n, sdu_len);
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

void ue::allocate_ce(srslte::sch_pdu* pdu, uint32_t lcid)
{
  switch ((srslte::dl_sch_lcid)lcid) {
    case srslte::dl_sch_lcid::TA_CMD:
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
    case srslte::dl_sch_lcid::CON_RES_ID:
      if (pdu->new_subh()) {
        if (!pdu->get()->set_con_res_id(conres_id)) {
          logger.error("CE:    Setting Contention Resolution ID CE");
        }
      } else {
        logger.error("CE:    Setting Contention Resolution ID CE. No space for a subheader");
      }
      break;
    case srslte::dl_sch_lcid::SCELL_ACTIVATION:
      if (pdu->new_subh()) {
        std::array<bool, SRSLTE_MAX_CARRIERS> active_scell_list = sched->get_scell_activation_mask(rnti);
        if (pdu->get()->set_scell_activation_cmd(active_scell_list)) {
          phy->set_activation_deactivation_scell(rnti, active_scell_list);
          // Allocate and initialize Rx/Tx softbuffers for new carriers (exclude PCell)
          for (size_t i = 0; i < std::min(active_scell_list.size(), cc_buffers.size()); ++i) {
            if (active_scell_list[i] and cc_buffers[i].empty()) {
              cc_buffers[i].allocate_cc(nof_prb, nof_rx_harq_proc, nof_tx_harq_proc);
            }
          }
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

uint8_t* ue::generate_pdu(uint32_t                              ue_cc_idx,
                          uint32_t                              harq_pid,
                          uint32_t                              tb_idx,
                          const sched_interface::dl_sched_pdu_t pdu[sched_interface::MAX_RLC_PDU_LIST],
                          uint32_t                              nof_pdu_elems,
                          uint32_t                              grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret = nullptr;
  if (rlc) {
    if (ue_cc_idx < SRSLTE_MAX_CARRIERS && harq_pid < SRSLTE_FDD_NOF_HARQ && tb_idx < SRSLTE_MAX_TB) {
      srslte::byte_buffer_t* buffer = cc_buffers[ue_cc_idx].get_tx_payload_buffer(harq_pid, tb_idx);
      buffer->clear();
      mac_msg_dl.init_tx(buffer, grant_size, false);
      for (uint32_t i = 0; i < nof_pdu_elems; i++) {
        if (pdu[i].lcid <= (uint32_t)srslte::ul_sch_lcid::PHR_REPORT) {
          allocate_sdu(&mac_msg_dl, pdu[i].lcid, pdu[i].nbytes);
        } else {
          allocate_ce(&mac_msg_dl, pdu[i].lcid);
        }
      }
      ret = mac_msg_dl.write_packet(logger);
      if (logger.info.enabled()) {
        fmt::memory_buffer str_buffer;
        mac_msg_dl.to_string(str_buffer);
        logger.info("0x%x %s", rnti, srslte::to_c_str(str_buffer));
      }
    } else {
      logger.error(
          "Invalid parameters calling generate_pdu: cc_idx=%d, harq_pid=%d, tb_idx=%d", ue_cc_idx, harq_pid, tb_idx);
    }
  } else {
    std::cout << "Error ue not configured (must call config() first" << std::endl;
  }
  return ret;
}

uint8_t* ue::generate_mch_pdu(uint32_t                      harq_pid,
                              sched_interface::dl_pdu_mch_t sched_,
                              uint32_t                      nof_pdu_elems,
                              uint32_t                      grant_size)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint8_t*                    ret    = nullptr;
  srslte::byte_buffer_t*      buffer = cc_buffers[0].get_tx_payload_buffer(harq_pid, 0);
  buffer->clear();
  mch_mac_msg_dl.init_tx(buffer, grant_size);

  for (uint32_t i = 0; i < nof_pdu_elems; i++) {
    if (sched_.pdu[i].lcid == (uint32_t)srslte::mch_lcid::MCH_SCHED_INFO) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_next_mch_sched_info(sched_.mtch_sched[i].lcid, sched_.mtch_sched[i].stop);
    } else if (sched_.pdu[i].lcid == 0) {
      mch_mac_msg_dl.new_subh();
      mch_mac_msg_dl.get()->set_sdu(0, sched_.pdu[i].nbytes, sched_.mcch_payload);
    } else if (sched_.pdu[i].lcid <= (uint32_t)srslte::mch_lcid::MTCH_MAX_LCID) {
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
  ue_metrics.rnti      = rnti;
  ue_metrics.ul_buffer = sched->get_ul_buffer(rnti);
  ue_metrics.dl_buffer = sched->get_dl_buffer(rnti);

  // set PCell sector id
  std::array<int, SRSLTE_MAX_CARRIERS> cc_list = sched->get_enb_ue_cc_map(rnti);
  auto                                 it      = std::find(cc_list.begin(), cc_list.end(), 0);
  ue_metrics.cc_idx                            = std::distance(cc_list.begin(), it);

  *metrics_ = ue_metrics;

  phr_counter    = 0;
  dl_cqi_counter = 0;
  ue_metrics     = {};
}

void ue::metrics_phr(float phr)
{
  ue_metrics.phr = SRSLTE_VEC_CMA(phr, ue_metrics.phr, phr_counter);
  phr_counter++;
}

void ue::metrics_dl_ri(uint32_t dl_ri)
{
  if (ue_metrics.dl_ri == 0.0f) {
    ue_metrics.dl_ri = (float)dl_ri + 1.0f;
  } else {
    ue_metrics.dl_ri = SRSLTE_VEC_EMA((float)dl_ri + 1.0f, ue_metrics.dl_ri, 0.5f);
  }
  dl_ri_counter++;
}

void ue::metrics_dl_pmi(uint32_t dl_ri)
{
  ue_metrics.dl_pmi = SRSLTE_VEC_CMA((float)dl_ri, ue_metrics.dl_pmi, dl_pmi_counter);
  dl_pmi_counter++;
}

void ue::metrics_dl_cqi(uint32_t dl_cqi)
{
  ue_metrics.dl_cqi = SRSLTE_VEC_CMA((float)dl_cqi, ue_metrics.dl_cqi, dl_cqi_counter);
  dl_cqi_counter++;
}

void ue::metrics_rx(bool crc, uint32_t tbs)
{
  if (crc) {
    ue_metrics.rx_brate += tbs * 8;
  } else {
    ue_metrics.rx_errors++;
  }
  ue_metrics.rx_pkts++;
}

void ue::metrics_tx(bool crc, uint32_t tbs)
{
  if (crc) {
    ue_metrics.tx_brate += tbs * 8;
  } else {
    ue_metrics.tx_errors++;
  }
  ue_metrics.tx_pkts++;
}

void ue::metrics_cnt()
{
  ue_metrics.nof_tti++;
}

void ue::tic()
{
  // Check for pending TA commands
  uint32_t nof_ta_count = ta_fsm.tick();
  if (nof_ta_count) {
    sched->dl_mac_buffer_state(rnti, (uint32_t)srslte::dl_sch_lcid::TA_CMD, nof_ta_count);
  }
}

} // namespace srsenb
