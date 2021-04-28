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

#include "srsue/hdr/stack/mac_nr/mac_nr.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/mac/mac_rar_pdu_nr.h"
#include "srsue/hdr/stack/mac_nr/proc_ra_nr.h"

namespace srsue {

mac_nr::mac_nr(srsran::ext_task_sched_handle task_sched_) :
  task_sched(task_sched_),
  logger(srslog::fetch_basic_logger("MAC-NR")),
  proc_ra(*this, logger),
  proc_sr(logger),
  proc_bsr(logger),
  mux(*this, logger),
  demux(logger),
  pcap(nullptr)
{
  // Create PCell HARQ entities
  dl_harq.at(PCELL_CC_IDX) = dl_harq_entity_nr_ptr(new dl_harq_entity_nr(PCELL_CC_IDX, this, &demux));
  ul_harq.at(PCELL_CC_IDX) = ul_harq_entity_nr_ptr(new ul_harq_entity_nr(PCELL_CC_IDX, this, &proc_ra, &mux));
}

mac_nr::~mac_nr()
{
  stop();
}

int mac_nr::init(const mac_nr_args_t&  args_,
                 phy_interface_mac_nr* phy_,
                 rlc_interface_mac*    rlc_,
                 rrc_interface_mac*    rrc_)
{
  args = args_;
  phy  = phy_;
  rlc  = rlc_;
  rrc  = rrc_;

  // Create Stack task dispatch queue
  stack_task_dispatch_queue = task_sched.make_task_queue();

  // Init MAC sub procedures
  proc_ra.init(phy, &task_sched);
  proc_sr.init(this, phy, rrc);

  if (proc_bsr.init(&proc_sr, &mux, rlc, &task_sched) != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize BSR procedure.");
    return SRSRAN_ERROR;
  }

  if (mux.init(rlc) != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize mux unit.");
    return SRSRAN_ERROR;
  }

  if (demux.init(rlc) != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize demux unit.");
    return SRSRAN_ERROR;
  }

  // Configure PCell HARQ entities
  if (ul_harq.at(PCELL_CC_IDX)->init() != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize UL HARQ entity.");
    return SRSRAN_ERROR;
  }

  started = true;

  return SRSRAN_SUCCESS;
}

void mac_nr::start_pcap(srsran::mac_pcap* pcap_)
{
  pcap = pcap_;
}

void mac_nr::stop()
{
  if (started) {
    started = false;
  }
}

// Implement Section 5.9
void mac_nr::reset()
{
  logger.info("Resetting MAC-NR");
}

void mac_nr::run_tti(const uint32_t tti)
{
  // Early exit if MAC NR isn't used
  if (not started) {
    return;
  }

  logger.set_context(tti);

  // Step all procedures
  logger.debug("Running MAC tti=%d", tti);

  // Update state for all LCIDs/LCGs once so all procedures can use them
  update_buffer_states();

  proc_bsr.step(tti, mac_buffer_states);
  proc_sr.step(tti);

  // process received PDUs
  stack_task_dispatch_queue.push([this]() { process_pdus(); });
}

void mac_nr::update_buffer_states()
{
  // reset variables
  mac_buffer_states.reset();
  for (auto& channel : logical_channels) {
    uint32_t buffer_len = rlc->get_buffer_state(channel.lcid);
    if (buffer_len > 0) {
      mac_buffer_states.nof_lcids_with_data++;
      if (channel.lcg != mac_buffer_states.last_non_zero_lcg) {
        mac_buffer_states.nof_lcgs_with_data++;
      }
      mac_buffer_states.last_non_zero_lcg = channel.lcg;
    }
    mac_buffer_states.lcid_buffer_size[channel.lcid] += buffer_len;
    mac_buffer_states.lcg_buffer_size[channel.lcg] += buffer_len;
  }
  logger.debug("%s", mac_buffer_states.to_string());

  // Count TTI for metrics
  for (uint32_t i = 0; i < SRSRAN_MAX_CARRIERS; ++i) {
    metrics[i].nof_tti++;
  }
}

mac_interface_phy_nr::sched_rnti_t mac_nr::get_ul_sched_rnti_nr(const uint32_t tti)
{
  return {c_rnti, srsran_rnti_type_c};
}

bool mac_nr::is_si_opportunity()
{
  // TODO: ask RRC if we need SI
  return false;
}

bool mac_nr::is_paging_opportunity()
{
  return false;
}

mac_interface_phy_nr::sched_rnti_t mac_nr::get_dl_sched_rnti_nr(const uint32_t tti)
{
  // Priority: SI-RNTI, P-RNTI, RA-RNTI, Temp-RNTI, CRNTI
  if (is_si_opportunity()) {
    return {SRSRAN_SIRNTI, srsran_rnti_type_si};
  }

  if (is_paging_opportunity()) {
    return {SRSRAN_PRNTI, srsran_rnti_type_si};
  }

  if (proc_ra.is_rar_opportunity(tti)) {
    return {proc_ra.get_rar_rnti(), srsran_rnti_type_ra};
  }

  if (proc_ra.has_temp_crnti() && has_crnti() == false) {
    logger.debug("SCHED: Searching temp C-RNTI=0x%x (proc_ra)", proc_ra.get_temp_crnti());
    return {proc_ra.get_temp_crnti(), srsran_rnti_type_c};
  }

  if (has_crnti()) {
    logger.debug("SCHED: Searching C-RNTI=0x%x", get_crnti());
    return {get_crnti(), srsran_rnti_type_c};
  }

  // turn off DCI search for this TTI
  return {SRSRAN_INVALID_RNTI, srsran_rnti_type_c};
}

bool mac_nr::has_crnti()
{
  return c_rnti != SRSRAN_INVALID_RNTI;
}

uint16_t mac_nr::get_crnti()
{
  return c_rnti;
}

uint16_t mac_nr::get_temp_crnti()
{
  return proc_ra.get_temp_crnti();
}

srsran::mac_sch_subpdu_nr::lcg_bsr_t mac_nr::generate_sbsr()
{
  return proc_bsr.generate_sbsr();
}

void mac_nr::bch_decoded_ok(uint32_t tti, srsran::unique_byte_buffer_t payload)
{
  // Send MIB to RLC
  rlc->write_pdu_bcch_bch(std::move(payload));

  if (pcap) {
    // pcap->write_dl_bch(payload, len, true, tti);
  }
}

int mac_nr::sf_indication(const uint32_t tti)
{
  run_tti(tti);
  return SRSRAN_SUCCESS;
}

void mac_nr::prach_sent(const uint32_t tti,
                        const uint32_t s_id,
                        const uint32_t t_id,
                        const uint32_t f_id,
                        const uint32_t ul_carrier_id)
{
  proc_ra.prach_sent(tti, s_id, t_id, f_id, ul_carrier_id);
}

bool mac_nr::sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx)
{
  return proc_sr.sr_opportunity(tti, sr_id, meas_gap, ul_sch_tx);
}

// This function handles all PCAP writing for a decoded DL TB
void mac_nr::write_pcap(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t& tb)
{
  if (pcap) {
    if (tb.ack && tb.payload != nullptr) {
      if (proc_ra.has_rar_rnti() && grant.rnti == proc_ra.get_rar_rnti()) {
        pcap->write_dl_ra_rnti_nr(tb.payload->msg, tb.payload->N_bytes, grant.rnti, true, grant.tti);
      } else if (grant.rnti == SRSRAN_PRNTI) {
        pcap->write_dl_pch_nr(tb.payload->msg, tb.payload->N_bytes, grant.rnti, true, grant.tti);
      } else {
        pcap->write_dl_crnti_nr(tb.payload->msg, tb.payload->N_bytes, grant.rnti, true, grant.tti);
      }
    }
  }
}

/**
 * \brief Called from PHY after decoding PDCCH for DL reception
 *
 * @param cc_idx  The CC index
 * @param grant   The DL grant
 * @param action  The DL action to be filled by MAC
 */
void mac_nr::new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action)
{
  logger.debug("new_grant_dl(): cc_idx=%d, tti=%d, rnti=%d, pid=%d, tbs=%d, ndi=%d, rv=%d",
               cc_idx,
               grant.tti,
               grant.rnti,
               grant.pid,
               grant.tbs,
               grant.ndi,
               grant.rv);

  // Assert HARQ entity
  if (dl_harq.at(cc_idx) == nullptr) {
    logger.error("HARQ entity %d has not been created", cc_idx);
    return;
  }

  dl_harq.at(cc_idx)->new_grant_dl(grant, action);
}

void mac_nr::tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result)
{
  logger.debug("tb_decoded(): cc_idx=%d, tti=%d, rnti=%d, pid=%d, tbs=%d, ndi=%d, rv=%d, result=%s",
               cc_idx,
               grant.tti,
               grant.rnti,
               grant.pid,
               grant.tbs,
               grant.ndi,
               grant.rv,
               result.ack ? "OK" : "KO");

  write_pcap(cc_idx, grant, result);

  if (proc_ra.has_rar_rnti() && grant.rnti == proc_ra.get_rar_rnti()) {
    proc_ra.handle_rar_pdu(result);
  } else {
    // Assert HARQ entity
    if (dl_harq.at(cc_idx) == nullptr) {
      logger.error("HARQ entity %d has not been created", cc_idx);
      return;
    }

    dl_harq.at(cc_idx)->tb_decoded(grant, std::move(result));
  }
}

void mac_nr::new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action)
{
  logger.debug("new_grant_ul(): cc_idx=%d, tti=%d, rnti=%d, pid=%d, tbs=%d, ndi=%d, rv=%d, is_rar=%d",
               cc_idx,
               grant.tti,
               grant.rnti,
               grant.pid,
               grant.tbs,
               grant.ndi,
               grant.rv,
               grant.is_rar_grant);

  // Clear UL action
  *action = {};

  // if proc ra is in contention resolution and c_rnti == grant.c_rnti resolve contention resolution
  if (proc_ra.is_contention_resolution() && grant.rnti == c_rnti) {
    proc_ra.pdcch_to_crnti();
  }

  // Let BSR know there is a new grant, might have to send a BSR
  proc_bsr.new_grant_ul(grant.tbs);

  // Assert UL HARQ entity
  if (ul_harq.at(cc_idx) == nullptr) {
    logger.error("HARQ entity %d has not been created", cc_idx);
    return;
  }

  ul_harq.at(cc_idx)->new_grant_ul(grant, action);

  // store PCAP
  if (action->tb.enabled && pcap) {
    pcap->write_ul_crnti_nr(action->tb.payload->msg, action->tb.payload->N_bytes, grant.rnti, grant.pid, grant.tti);
  }
}

void mac_nr::timer_expired(uint32_t timer_id)
{
  // not implemented
}

int mac_nr::setup_lcid(const srsran::logical_channel_config_t& config)
{
  if (mux.setup_lcid(config) != SRSRAN_SUCCESS) {
    logger.error("Couldn't register logical channel at MUX unit.");
    return SRSRAN_ERROR;
  }

  if (proc_bsr.setup_lcid(config.lcid, config.lcg, config.priority) != SRSRAN_SUCCESS) {
    logger.error("Couldn't register logical channel at BSR procedure.");
    return SRSRAN_ERROR;
  }

  logger.info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSD=%dms, bucket_size=%d",
              config.lcid,
              config.lcg,
              config.priority,
              config.PBR,
              config.BSD,
              config.bucket_size);

  // store full config
  logical_channels.push_back(config);

  return SRSRAN_SUCCESS;
}

int mac_nr::add_tag_config(const srsran::tag_cfg_nr_t& tag_cfg)
{
  logger.info("Add tag config not supported yet");
  return SRSRAN_SUCCESS;
}

int mac_nr::remove_tag_config(const uint32_t tag_id)
{
  logger.info("Remove tag config not supported yet");
  return SRSRAN_SUCCESS;
}

int mac_nr::set_config(const srsran::phr_cfg_nr_t& phr_cfg)
{
  logger.info("Add phr config not supported yet");
  return SRSRAN_SUCCESS;
}

int mac_nr::set_config(const srsran::dl_harq_cfg_nr_t& dl_hrq_cfg)
{
  for (const auto& cc : dl_harq) {
    if (cc != nullptr) {
      if (cc->set_config(dl_hrq_cfg) != SRSRAN_SUCCESS) {
        logger.error("Couldn't configure DL HARQ entity.");
        return SRSRAN_ERROR;
      }
    }
  }
  return SRSRAN_SUCCESS;
}

int mac_nr::set_config(const srsran::bsr_cfg_nr_t& bsr_cfg)
{
  return proc_bsr.set_config(bsr_cfg);
}

int mac_nr::set_config(const srsran::sr_cfg_nr_t& sr_cfg)
{
  return proc_sr.set_config(sr_cfg);
}

void mac_nr::set_config(const srsran::rach_nr_cfg_t& rach_cfg)
{
  proc_ra.set_config(rach_cfg);
}

void mac_nr::set_contention_id(uint64_t ue_identity)
{
  contention_id = ue_identity;
}

bool mac_nr::set_crnti(const uint16_t c_rnti_)
{
  if (is_valid_crnti(c_rnti_)) {
    logger.info("Setting C-RNTI to 0x%X", c_rnti_);
    c_rnti = c_rnti_;
    return true;
  } else {
    logger.warning("Failed to set C-RNTI, 0x%X is not valid.", c_rnti_);
    return false;
  }
}

void mac_nr::start_ra_procedure()
{
  stack_task_dispatch_queue.push([this]() { proc_ra.start_by_rrc(); });
}

bool mac_nr::is_valid_crnti(const uint16_t crnti)
{
  // TS 38.321 15.3.0 Table 7.1-1
  return (crnti >= 0x0001 && crnti <= 0xFFEF);
}

void mac_nr::get_metrics(mac_metrics_t m[SRSRAN_MAX_CARRIERS])
{
  int   tx_pkts          = 0;
  int   tx_errors        = 0;
  int   tx_brate         = 0;
  int   rx_pkts          = 0;
  int   rx_errors        = 0;
  int   rx_brate         = 0;
  int   ul_buffer        = 0;
  float dl_avg_ret       = 0;
  int   dl_avg_ret_count = 0;

  // Get metrics from HARQ entities explicitly
  for (uint32_t i = 0; i < metrics.size(); ++i) {
    if (dl_harq.at(i) != nullptr) {
      dl_harq_entity_nr::dl_harq_metrics_t harq_metrics = dl_harq.at(i)->get_metrics();
      rx_pkts += (harq_metrics.rx_ok + harq_metrics.rx_ko);
      rx_errors += harq_metrics.rx_ko;
      rx_brate += harq_metrics.rx_brate;
    }
    if (ul_harq.at(i) != nullptr) {
      ul_harq_entity_nr::ul_harq_metrics_t harq_metrics = ul_harq.at(i)->get_metrics();
      tx_pkts += (harq_metrics.tx_ok + harq_metrics.tx_ko);
      tx_errors += harq_metrics.tx_ko;
      tx_brate += harq_metrics.tx_brate;
    }
  }

  // assign accumulated metrics for PCELL carrier only
  auto& pcell_cc     = metrics[PCELL_CC_IDX];
  pcell_cc.tx_pkts   = tx_pkts;
  pcell_cc.tx_errors = tx_errors;
  pcell_cc.tx_brate  = tx_brate;
  pcell_cc.rx_pkts   = rx_pkts;
  pcell_cc.rx_errors = rx_errors;
  pcell_cc.rx_brate  = rx_brate;
  pcell_cc.ul_buffer = mac_buffer_states.get_total_buffer_size();

  memcpy(m, metrics.data(), sizeof(mac_metrics_t) * SRSRAN_MAX_CARRIERS);
  metrics = {};
}

/**
 * Called from the main stack thread to process received PDUs
 */
void mac_nr::process_pdus()
{
  demux.process_pdus();
}

uint64_t mac_nr::get_contention_id()
{
  return 0xdeadbeef; // TODO when rebased on PR
}

// TODO same function as for mac_eutra
bool mac_nr::is_in_window(uint32_t tti, int* start, int* len)
{
  uint32_t st = (uint32_t)*start;
  uint32_t l  = (uint32_t)*len;

  if (srsran_tti_interval(tti, st) < l + 5) {
    if (tti > st) {
      if (tti <= st + l) {
        return true;
      } else {
        *start = 0;
        *len   = 0;
        return false;
      }
    } else {
      if (tti <= (st + l) % 10240) {
        return true;
      } else {
        *start = 0;
        *len   = 0;
        return false;
      }
    }
  }
  return false;
}

} // namespace srsue
