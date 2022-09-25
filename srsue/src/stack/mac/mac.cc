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

#define Error(fmt, ...) logger.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) logger.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug(fmt, ##__VA_ARGS__)

#include <string.h>
#include <strings.h>

#include "srsran/common/pcap.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsue/hdr/stack/mac/mac.h"

namespace srsue {

mac::mac(const char* logname, ext_task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger(logname)),
  mch_msg(10, logger),
  mux_unit(logger),
  demux_unit(logger),
  ra_procedure(logger),
  sr_procedure(logger),
  bsr_procedure(logger),
  phr_procedure(logger),
  pcap(nullptr),
  task_sched(task_sched_)
{
  // Create PCell HARQ entities
  ul_harq.at(PCELL_CC_IDX) = ul_harq_entity_ptr(new ul_harq_entity(PCELL_CC_IDX));
  dl_harq.at(PCELL_CC_IDX) = dl_harq_entity_ptr(new dl_harq_entity(PCELL_CC_IDX));

  srsran_softbuffer_rx_init(&pch_softbuffer, 100);

  // Keep initialising members
  bzero(&metrics, sizeof(mac_metrics_t));
  clear_rntis();
}

mac::~mac()
{
  stop();

  srsran_softbuffer_rx_free(&pch_softbuffer);
}

bool mac::init(phy_interface_mac_lte* phy, rlc_interface_mac* rlc, rrc_interface_mac* rrc)
{
  phy_h = phy;
  rlc_h = rlc;
  rrc_h = rrc;

  timer_alignment = task_sched.get_unique_timer();

  // Create Stack task dispatch queue
  stack_task_dispatch_queue = task_sched.make_task_queue();

  bsr_procedure.init(&sr_procedure, rlc_h, &task_sched);
  phr_procedure.init(phy_h, &task_sched);
  mux_unit.init(rlc_h, &bsr_procedure, &phr_procedure);
  demux_unit.init(phy_h, rlc_h, this, &timer_alignment);
  ra_procedure.init(phy_h, rrc, &uernti, &timer_alignment, &mux_unit, &task_sched);
  sr_procedure.init(&ra_procedure, phy_h, rrc);

  // Create UL/DL unique HARQ pointers
  ul_harq.at(PCELL_CC_IDX)->init(&uernti, &ra_procedure, &mux_unit);
  dl_harq.at(PCELL_CC_IDX)->init(&uernti, &demux_unit);

  reset();

  // Set default MAC parameters
  mac_cfg_t default_cfg = {};
  set_config(default_cfg);

  initialized = true;

  return true;
}

void mac::stop()
{
  if (initialized) {
    run_tti(0); // make sure it's not locked after last TTI
    initialized = false;
  }
}

void mac::start_pcap(srsran::mac_pcap* pcap_)
{
  pcap = pcap_;
  for (auto& r : dl_harq) {
    if (r != nullptr) {
      r->start_pcap(pcap);
    }
  }
  for (auto& r : ul_harq) {
    if (r != nullptr) {
      r->start_pcap(pcap);
    }
  }
  ra_procedure.start_pcap(pcap);
}

// TODO: Change the function name and implement reconfiguration as in specs
// Implement Section 5.8
void mac::reconfiguration(const uint32_t& cc_idx, const bool& enable)
{
  if (cc_idx < SRSRAN_MAX_CARRIERS) {
    if (enable and ul_harq.at(cc_idx) == nullptr) {
      ul_harq_entity_ptr ul = ul_harq_entity_ptr(new ul_harq_entity(cc_idx));
      ul->init(&uernti, &ra_procedure, &mux_unit);
      ul->set_config(ul_harq_cfg);

      if (pcap != nullptr) {
        ul->start_pcap(pcap);
      }

      ul_harq.at(cc_idx) = std::move(ul);
    }

    if (enable and dl_harq.at(cc_idx) == nullptr) {
      dl_harq_entity_ptr dl = dl_harq_entity_ptr(new dl_harq_entity(cc_idx));
      dl->init(&uernti, &demux_unit);

      if (pcap != nullptr) {
        dl->start_pcap(pcap);
      }

      dl_harq.at(cc_idx) = std::move(dl);
    }
  }
}

// Implement Section 5.9
void mac::reset()
{
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    bzero(&metrics, sizeof(mac_metrics_t));
  }

  Info("Resetting MAC");

  timer_alignment.stop();

  // Releases UL resources and Resets HARQ processes
  timer_alignment_expire();

  mux_unit.msg3_flush();
  mux_unit.reset();

  demux_unit.reset();

  ra_procedure.reset();
  sr_procedure.reset();
  bsr_procedure.reset();
  phr_procedure.reset();

  // Setup default LCID 0 with highest priority
  logical_channel_config_t config = {};
  config.lcid                     = 0;
  config.lcg                      = 0;
  config.PBR                      = -1;
  config.BSD                      = 50;
  config.priority                 = 0;
  setup_lcid(config);

  // and LCID 1 with lower priority
  config.lcid     = 1;
  config.priority = 1;
  setup_lcid(config);

  mux_unit.print_logical_channel_state("After MAC reset:");
  is_first_ul_grant = true;

  clear_rntis();
}

void mac::run_tti(const uint32_t tti)
{
  logger.set_context(tti);

  /* Warning: Here order of invocation of procedures is important!! */

  // Step all procedures (must follow this order)
  Debug("Running MAC tti=%d", tti);
  mux_unit.step();
  bsr_procedure.step(tti);
  sr_procedure.step(tti);
  phr_procedure.step();
  ra_procedure.step(tti);
  ra_procedure.update_rar_window(ra_window);

  // Count TTI for metrics
  std::lock_guard<std::mutex> lock(metrics_mutex);
  for (uint32_t i = 0; i < SRSRAN_MAX_CARRIERS; i++) {
    metrics[i].nof_tti++;
  }
}

void mac::bcch_start_rx(int si_window_start_, int si_window_length_)
{
  if (si_window_length_ >= 0 && si_window_start_ >= 0) {
    dl_harq.at(0)->set_si_window_start(si_window_start_);
    si_window.set(si_window_length_, si_window_start_);
  } else {
    si_window.reset();
  }
  Info("SCHED: Searching for DL dci for SI-RNTI window_st=%d, window_len=%d",
       si_window.get_start(),
       si_window.get_length());
}

void mac::bcch_stop_rx()
{
  bcch_start_rx(-1, -1);
}

void mac::pcch_start_rx()
{
  this->p_window.set(0, 1);
}

void mac::clear_rntis()
{
  p_window.reset();
  si_window.reset();
  ra_window.reset();
  uernti.reset();
}

uint16_t mac::get_crnti()
{
  return uernti.get_crnti();
}

void mac::set_ho_rnti(uint16_t crnti, uint16_t target_pci)
{
  uernti.set_crnti(crnti);
  if (pcap) {
    pcap->set_ue_id(target_pci);
  }
}

uint16_t mac::get_ul_sched_rnti(uint32_t tti)
{
  if (uernti.get_temp_rnti() && !uernti.get_crnti()) {
    return uernti.get_temp_rnti();
  }
  if (uernti.get_crnti()) {
    return uernti.get_crnti();
  }
  return SRSRAN_INVALID_RNTI;
}

uint16_t mac::get_dl_sched_rnti(uint32_t tti)
{
  // Priority: SI-RNTI, P-RNTI, RA-RNTI, Temp-RNTI, CRNTI
  if (si_window.is_in_window(tti)) {
    // TODO: This scheduling decision belongs to RRC
    if (si_window.get_length() > 1) {               // This is not a SIB1
      if ((tti / 10) % 2 == 0 && (tti % 10) == 5) { // Skip subframe #5 for which SFN mod 2 = 0
        return SRSRAN_INVALID_RNTI;
      }
    }
    Debug("SCHED: Searching SI-RNTI, tti=%d, window start=%d, length=%d",
          tti,
          si_window.get_start(),
          si_window.get_length());
    return SRSRAN_SIRNTI;
  }
  if (uernti.get_rar_rnti() && ra_window.is_in_window(tti)) {
    Debug("SCHED: Searching RAR-RNTI=0x%x, tti=%d", uernti.get_rar_rnti(), tti);
    return uernti.get_rar_rnti();
  }
  if (uernti.get_temp_rnti() && !uernti.get_crnti()) {
    Debug("SCHED: Searching Temp-RNTI=0x%x", uernti.get_temp_rnti());
    return uernti.get_temp_rnti();
  }
  if (uernti.get_crnti()) {
    Debug("SCHED: Searching C-RNTI=0x%x", uernti.get_crnti());
    return uernti.get_crnti();
  }
  if (p_window.is_set()) {
    Debug("SCHED: Searching P-RNTI");
    return SRSRAN_PRNTI;
  }

  // turn off DCI search for this TTI
  return SRSRAN_INVALID_RNTI;
}

void mac::bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len)
{
  // Send MIB to RLC
  unique_byte_buffer_t buf = make_byte_buffer();
  if (buf != nullptr) {
    memcpy(buf->msg, payload, len);
    buf->N_bytes = len;
    buf->set_timestamp();
    auto p = stack_task_dispatch_queue.try_push(std::bind(
        [this](srsran::unique_byte_buffer_t& buf) { rlc_h->write_pdu_bcch_bch(std::move(buf)); }, std::move(buf)));
    if (p.is_error()) {
      Warning("Failed to dispatch rlc::write_pdu_bcch_bch task to stack");
    }
  } else {
    logger.error("Fatal error: Out of buffers from the pool in write_pdu_bcch_bch()");
  }

  if (pcap) {
    pcap->write_dl_bch(payload, len, true, phy_h->get_current_tti(), cc_idx);
  }
}

void mac::mch_decoded(uint32_t len, bool crc, uint8_t* payload)
{
  // Parse MAC header
  if (crc) {
    mch_msg.init_rx(len);
    mch_msg.parse_packet(payload);
    while (mch_msg.next()) {
      for (uint32_t i = 0; i < phy_mbsfn_cfg.nof_mbsfn_services; i++) {
        if (srsran::mch_lcid::MCH_SCHED_INFO == mch_msg.get()->mch_ce_type()) {
          uint16_t stop;
          uint8_t  lcid;
          if (mch_msg.get()->get_next_mch_sched_info(&lcid, &stop)) {
            phy_h->set_mch_period_stop(stop);
            Info("MCH Sched Info: LCID: %d, Stop: %d, tti is %d ", lcid, stop, phy_h->get_current_tti());
          }
        }
      }
    }

    demux_unit.push_pdu_mch(payload, len);
    process_pdus();

    if (pcap) {
      pcap->write_dl_mch(payload, len, true, phy_h->get_current_tti(), 0);
    }

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics[0].rx_brate += len * 8;
  } else {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics[0].rx_errors++;
  }
  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics[0].rx_pkts++;
}

void mac::tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSRAN_MAX_CODEWORDS])
{
  if (grant.is_pdcch_order) {
    ra_procedure.set_config_ded(grant.preamble_idx, grant.prach_mask_idx);
    ra_procedure.start_pdcch_order();
  } else if (SRSRAN_RNTI_ISRAR(grant.rnti)) {
    if (ack[0]) {
      ra_procedure.tb_decoded_ok(cc_idx, grant.tti);
    }
  } else if (grant.rnti == SRSRAN_PRNTI) {
    // Send PCH payload to RLC
    unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu != nullptr) {
      memcpy(pdu->msg, pch_payload_buffer, grant.tb[0].tbs);
      pdu->N_bytes = grant.tb[0].tbs;
      pdu->set_timestamp();

      auto ret = stack_task_dispatch_queue.try_push(std::bind(
          [this](srsran::unique_byte_buffer_t& pdu) { rlc_h->write_pdu_pcch(std::move(pdu)); }, std::move(pdu)));
      if (ret.is_error()) {
        Warning("Failed to dispatch rlc::write_pdu_pcch task to stack");
      }
    } else {
      logger.error("Fatal error: Out of buffers from the pool in write_pdu_pcch()");
    }

    if (pcap) {
      pcap->write_dl_pch(pch_payload_buffer, grant.tb[0].tbs, true, grant.tti, cc_idx);
    }
  } else {
    // Assert DL HARQ entity
    if (dl_harq.at(cc_idx) == nullptr) {
      Error("HARQ entity %d has not been created", cc_idx);
      return;
    }

    dl_harq.at(cc_idx)->tb_decoded(grant, ack);
    process_pdus();

    {
      std::lock_guard<std::mutex> lock(metrics_mutex);
      for (uint32_t tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
        if (grant.tb[tb].tbs) {
          if (ack[tb]) {
            metrics[cc_idx].rx_brate += grant.tb[tb].tbs * 8;
          } else {
            metrics[cc_idx].rx_errors++;
          }
          metrics[cc_idx].rx_pkts++;
        }
      }
    }
  }
}

void mac::new_grant_dl(uint32_t                               cc_idx,
                       mac_interface_phy_lte::mac_grant_dl_t  grant,
                       mac_interface_phy_lte::tb_action_dl_t* action)
{
  if (SRSRAN_RNTI_ISRAR(grant.rnti)) {
    ra_procedure.new_grant_dl(grant, action);
  } else if (grant.rnti == SRSRAN_PRNTI) {
    bzero(action, sizeof(mac_interface_phy_lte::tb_action_dl_t));
    if (grant.tb[0].tbs > pch_payload_buffer_sz) {
      Error("Received dci for PCH (%d bytes) exceeds buffer (%d bytes)", grant.tb[0].tbs, int(pch_payload_buffer_sz));
      action->tb[0].enabled = false;
    } else {
      action->tb[0].enabled       = true;
      action->tb[0].payload       = pch_payload_buffer;
      action->tb[0].softbuffer.rx = &pch_softbuffer;
      action->tb[0].rv            = grant.tb[0].rv;
      srsran_softbuffer_rx_reset_cb(&pch_softbuffer, 1);
    }
  } else if (grant.is_pdcch_order) {
    // if the grant is a PDCCH order then there is no associated PDSCH
    action->tb[0].enabled = false;
  } else if (!(grant.rnti == SRSRAN_SIRNTI && cc_idx != 0)) {
    // If PDCCH for C-RNTI and RA procedure in Contention Resolution, notify it
    if (grant.rnti == uernti.get_crnti() && ra_procedure.is_contention_resolution()) {
      ra_procedure.pdcch_to_crnti(false);
    }
    // Assert DL HARQ entity
    if (dl_harq.at(cc_idx) == nullptr) {
      Error("HARQ entity %d has not been created", cc_idx);
      return;
    }

    dl_harq.at(cc_idx)->new_grant_dl(grant, action);
  } else {
    /* Discard */
    Info("Discarding dci in CC %d, RNTI=0x%x", cc_idx, grant.rnti);
  }
}

void mac::process_pdus()
{
  // dispatch work to stack thread
  auto ret = stack_task_dispatch_queue.try_push([this]() {
    bool have_data = true;
    while (initialized.load(std::memory_order_relaxed) and have_data) {
      have_data = demux_unit.process_pdus();
    }
  });
  if (ret.is_error() && initialized.load(std::memory_order_relaxed)) {
    Warning("Failed to dispatch mac::%s task to stack thread", __func__);
  }
}

uint32_t mac::get_current_tti()
{
  return phy_h->get_current_tti();
}

void mac::reset_harq(uint32_t cc_idx)
{
  if (ul_harq.at(cc_idx) != nullptr) {
    ul_harq.at(cc_idx)->reset();
  }

  if (dl_harq.at(cc_idx) != nullptr) {
    dl_harq.at(cc_idx)->reset();
  }
}

bool mac::contention_resolution_id_rcv(uint64_t id)
{
  return ra_procedure.contention_resolution_id_received(id);
}

void mac::new_grant_ul(uint32_t                               cc_idx,
                       mac_interface_phy_lte::mac_grant_ul_t  grant,
                       mac_interface_phy_lte::tb_action_ul_t* action)
{
  // Start PHR Periodic timer on first UL DCI (See TS 36.321 Sec 5.4.6)
  if (is_first_ul_grant) {
    is_first_ul_grant = false;
    phr_procedure.start_periodic_timer();
  }

  // Assert UL HARQ entity
  if (ul_harq.at(cc_idx) == nullptr) {
    Error("HARQ entity %d has not been created", cc_idx);
    return;
  }

  ul_harq.at(cc_idx)->new_grant_ul(grant, action);

  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics[cc_idx].tx_pkts++;

    if (grant.phich_available) {
      if (!grant.hi_value) {
        metrics[cc_idx].tx_errors++;
      } else {
        metrics[cc_idx].tx_brate += ul_harq.at(cc_idx)->get_current_tbs(grant.pid) * 8;
      }
    }
  } // end of holding metrics mutex
}


void mac::setup_timers(int time_alignment_timer)
{
  // stop currently running time alignment timer
  if (timer_alignment.is_running()) {
    timer_alignment.stop();
  }

  if (time_alignment_timer > 0) {
    timer_alignment.set(time_alignment_timer, [this](uint32_t tid) { timer_expired(tid); });
  }
}

void mac::timer_expired(uint32_t timer_id)
{
  if (timer_id == timer_alignment.id()) {
    Info("Time Alignment Timer expired");
    timer_alignment_expire();
  } else {
    Warning("Received callback from unknown timer_id=%d", timer_id);
  }
}

/* Function called on expiry of TimeAlignmentTimer */
void mac::timer_alignment_expire()
{
  rrc_h->release_pucch_srs();
  for (auto& r : dl_harq) {
    if (r != nullptr) {
      r->reset();
    }
  }
  for (auto& r : ul_harq) {
    if (r != nullptr) {
      r->reset();
    }
  }
}

void mac::set_contention_id(uint64_t uecri)
{
  uernti.set_contention_id(uecri);
}

void mac::set_rach_ded_cfg(uint32_t preamble_index, uint32_t prach_mask)
{
  ra_procedure.set_config_ded(preamble_index, prach_mask);
}

void mac::set_mbsfn_config(uint32_t nof_mbsfn_services)
{
  // ul_cfg->nof_mbsfn_services = config.mbsfn.mcch.pmch_infolist_r9[0].mbms_sessioninfolist_r9_size;
  phy_mbsfn_cfg.nof_mbsfn_services = nof_mbsfn_services;
}

// Only reset SR config
void mac::set_config(sr_cfg_t& sr_cfg)
{
  Info("Setting SR configuration");
  sr_procedure.set_config(sr_cfg);
}

void mac::set_config(mac_cfg_t& mac_cfg)
{
  Info("Setting configuration");
  // Set configuration for each module in MAC
  bsr_procedure.set_config(mac_cfg.bsr_cfg);
  phr_procedure.set_config(mac_cfg.phr_cfg);
  sr_procedure.set_config(mac_cfg.sr_cfg);
  ra_procedure.set_config(mac_cfg.rach_cfg);
  ul_harq_cfg = mac_cfg.harq_cfg;
  for (auto& i : ul_harq) {
    if (i != nullptr) {
      i->set_config(ul_harq_cfg);
    }
  }
  setup_timers(mac_cfg.time_alignment_timer);
}

void mac::setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD)
{
  logical_channel_config_t config = {};
  config.lcid                     = lcid;
  config.lcg                      = lcg;
  config.priority                 = priority;
  config.PBR                      = PBR_x_tti;
  config.BSD                      = BSD;
  config.bucket_size              = config.PBR * config.BSD;
  setup_lcid(config);
}

void mac::setup_lcid(const logical_channel_config_t& config)
{
  Info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSD=%dms, bucket_size=%d",
       config.lcid,
       config.lcg,
       config.priority,
       config.PBR,
       config.BSD,
       config.bucket_size);
  mux_unit.setup_lcid(config);
  bsr_procedure.setup_lcid(config.lcid, config.lcg, config.priority);
}

void mac::mch_start_rx(uint32_t lcid)
{
  demux_unit.mch_start_rx(lcid);
}

void mac::get_metrics(mac_metrics_t m[SRSRAN_MAX_CARRIERS])
{
  std::lock_guard<std::mutex> lock(metrics_mutex);

  int   tx_pkts          = 0;
  int   tx_errors        = 0;
  int   tx_brate         = 0;
  int   rx_pkts          = 0;
  int   rx_errors        = 0;
  int   rx_brate         = 0;
  int   ul_buffer        = 0;
  float dl_avg_ret       = 0;
  int   dl_avg_ret_count = 0;

  for (uint32_t r = 0; r < dl_harq.size(); r++) {
    tx_pkts += metrics[r].tx_pkts;
    tx_errors += metrics[r].tx_errors;
    tx_brate += metrics[r].tx_brate;
    rx_pkts += metrics[r].rx_pkts;
    rx_errors += metrics[r].rx_errors;
    rx_brate += metrics[r].rx_brate;
    ul_buffer += metrics[r].ul_buffer;

    if (metrics[r].rx_pkts) {
      dl_avg_ret += dl_harq.at(r)->get_average_retx();
      dl_avg_ret_count++;
    }
  }

  if (dl_avg_ret_count) {
    dl_avg_ret /= dl_avg_ret_count;
  }

  Debug("DL retx: %.2f \%%, perpkt: %.2f, UL retx: %.2f \%% perpkt: %.2f",
        rx_pkts ? ((float)100 * rx_errors / rx_pkts) : 0.0f,
        dl_avg_ret,
        tx_pkts ? ((float)100 * tx_errors / tx_pkts) : 0.0f,
        ul_harq.at(PCELL_CC_IDX)->get_average_retx());

  metrics[PCELL_CC_IDX].ul_buffer = (int)bsr_procedure.get_buffer_state();
  memcpy(m, metrics, sizeof(mac_metrics_t) * SRSRAN_MAX_CARRIERS);
  m = metrics;
  bzero(&metrics, sizeof(mac_metrics_t) * SRSRAN_MAX_CARRIERS);
}

} // namespace srsue
