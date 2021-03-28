/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/mac_nr/mac_nr.h"
#include "srsran/mac/mac_rar_pdu_nr.h"
#include "srsue/hdr/stack/mac_nr/proc_ra_nr.h"

namespace srsue {

mac_nr::mac_nr(srsran::ext_task_sched_handle task_sched_) :
  task_sched(task_sched_),
  logger(srslog::fetch_basic_logger("MAC")),
  proc_ra(logger),
  proc_sr(logger),
  mux(logger),
  pcap(nullptr)
{}

mac_nr::~mac_nr()
{
  stop();
}

int mac_nr::init(const mac_nr_args_t& args_, phy_interface_mac_nr* phy_, rlc_interface_mac* rlc_)
{
  args = args_;
  phy  = phy_;
  rlc  = rlc_;

  // Create Stack task dispatch queue
  stack_task_dispatch_queue = task_sched.make_task_queue();

  proc_ra.init(phy, this, &task_sched);

  if (mux.init() != SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize mux unit.");
    return SRSRAN_ERROR;
  }

  if (srsran_softbuffer_tx_init_guru(&softbuffer_tx, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    ERROR("Error init soft-buffer");
    return SRSRAN_ERROR;
  }

  tx_buffer = srsran::make_byte_buffer();
  if (tx_buffer == nullptr) {
    return SRSRAN_ERROR;
  }
  rlc_buffer = srsran::make_byte_buffer();
  if (rlc_buffer == nullptr) {
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

  srsran_softbuffer_tx_free(&softbuffer_tx);
}

// Implement Section 5.9
void mac_nr::reset()
{
  logger.info("Resetting MAC-NR");
}

void mac_nr::run_tti(const uint32_t tti)
{
  // Step all procedures
  logger.debug("Running MAC tti=%d", tti);
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

  if (proc_ra.has_temp_rnti() && has_crnti() == false) {
    logger.debug("SCHED: Searching temp C-RNTI=0x%x (proc_ra)", proc_ra.get_temp_rnti());
    return {proc_ra.get_temp_rnti(), srsran_rnti_type_c};
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

// This function handles all PCAP writing for a decoded DL TB
void mac_nr::write_pcap(const uint32_t cc_idx, mac_nr_grant_dl_t& grant)
{
  if (pcap) {
    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; ++i) {
      if (grant.tb[i] != nullptr) {
        if (proc_ra.has_rar_rnti() && grant.rnti == proc_ra.get_rar_rnti()) {
          pcap->write_dl_ra_rnti_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        } else if (grant.rnti == SRSRAN_PRNTI) {
          pcap->write_dl_pch_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        } else {
          pcap->write_dl_crnti_nr(grant.tb[i]->msg, grant.tb[i]->N_bytes, grant.rnti, true, grant.tti);
        }
      }
    }
  }
}

/**
 * \brief Called from PHY after decoding a TB
 *
 * The TB can directly be used
 *
 * @param cc_idx
 * @param grant structure
 */
void mac_nr::tb_decoded(const uint32_t cc_idx, mac_nr_grant_dl_t& grant)
{
  write_pcap(cc_idx, grant);
  // handle PDU
  if (proc_ra.has_rar_rnti() && grant.rnti == proc_ra.get_rar_rnti()) {
    proc_ra.handle_rar_pdu(grant);
  } else {
    // Push DL PDUs to queue for back-ground processing
    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; ++i) {
      if (grant.tb[i] != nullptr) {
        pdu_queue.push(std::move(grant.tb[i]));
      }
    }
  }

  metrics[cc_idx].rx_pkts++;
  stack_task_dispatch_queue.push([this]() { process_pdus(); });
}

void mac_nr::new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action)
{
  // if proc ra is in contention resolution and c_rnti == grant.c_rnti resolve contention resolution
  if (proc_ra.is_contention_resolution() && grant.rnti == c_rnti) {
    proc_ra.pdcch_to_crnti();
  }

  // fill TB action (goes into UL harq eventually)
  action->tb.payload    = tx_buffer.get();
  action->tb.enabled    = true;
  action->tb.rv         = 0;
  action->tb.softbuffer = &softbuffer_tx;
  srsran_softbuffer_tx_reset(&softbuffer_tx);

  // Pack MAC PDU
  get_ul_data(grant, action->tb.payload);

  metrics[cc_idx].tx_pkts++;
}

void mac_nr::get_ul_data(const mac_nr_grant_ul_t& grant, srsran::byte_buffer_t* phy_tx_pdu)
{
  // initialize MAC PDU
  phy_tx_pdu->clear();
  tx_pdu.init_tx(phy_tx_pdu, grant.tbs / 8U, true);

  if (mux.msg3_is_pending()) {
    // If message 3 is pending pack message 3 for uplink transmission
    // Use the CRNTI which is provided in the RRC reconfiguration (only for DC mode maybe other)
    tx_pdu.add_crnti_ce(c_rnti);
    srsran::mac_sch_subpdu_nr::lcg_bsr_t sbsr = {};
    sbsr.lcg_id                               = 0;
    sbsr.buffer_size                          = 1;
    tx_pdu.add_sbsr_ce(sbsr);
    logger.info("Generated msg3 with RNTI 0x%x", c_rnti);
    mux.msg3_transmitted();
  } else {
    // Pack normal UL data PDU
    while (tx_pdu.get_remaing_len() >= MIN_RLC_PDU_LEN) {
      // read RLC PDU
      rlc_buffer->clear();
      uint8_t* rd      = rlc_buffer->msg;
      int      pdu_len = 0;
      pdu_len          = rlc->read_pdu(4, rd, tx_pdu.get_remaing_len() - 2);

      // Add SDU if RLC has something to tx
      if (pdu_len > 0) {
        rlc_buffer->N_bytes = pdu_len;
        logger.info(rlc_buffer->msg, rlc_buffer->N_bytes, "Read %d B from RLC", rlc_buffer->N_bytes);

        // add to MAC PDU and pack
        if (tx_pdu.add_sdu(4, rlc_buffer->msg, rlc_buffer->N_bytes) != SRSRAN_SUCCESS) {
          logger.error("Error packing MAC PDU");
        }
      } else {
        break;
      }
    }
  }

  // Pack PDU
  tx_pdu.pack();

  logger.info(phy_tx_pdu->msg, phy_tx_pdu->N_bytes, "Generated MAC PDU (%d B)", phy_tx_pdu->N_bytes);

  if (pcap) {
    pcap->write_ul_crnti_nr(phy_tx_pdu->msg, phy_tx_pdu->N_bytes, grant.rnti, grant.pid, grant.tti);
  }
}

void mac_nr::timer_expired(uint32_t timer_id)
{
  // not implemented
}

void mac_nr::setup_lcid(const srsran::logical_channel_config_t& config)
{
  logger.info("Logical Channel Setup: LCID=%d, LCG=%d, priority=%d, PBR=%d, BSD=%dms, bucket_size=%d",
              config.lcid,
              config.lcg,
              config.priority,
              config.PBR,
              config.BSD,
              config.bucket_size);
  // mux_unit.setup_lcid(config);
  // bsr_procedure.setup_lcid(config.lcid, config.lcg, config.priority);
}

void mac_nr::set_config(const srsran::bsr_cfg_t& bsr_cfg)
{
  logger.info("BSR config periodic timer %d retx timer %d", bsr_cfg.periodic_timer, bsr_cfg.retx_timer);
  logger.warning("Not handling BSR config yet");
}

int32_t mac_nr::set_config(const srsran::sr_cfg_nr_t& sr_cfg)
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
  proc_ra.start_by_rrc();
}

bool mac_nr::is_valid_crnti(const uint16_t crnti)
{
  // TS 38.321 15.3.0 Table 7.1-1
  return (crnti >= 0x0001 && crnti <= 0xFFEF);
}

void mac_nr::get_metrics(mac_metrics_t m[SRSRAN_MAX_CARRIERS]) {}

/**
 * Called from the main stack thread to process received PDUs
 */
void mac_nr::process_pdus()
{
  while (started and not pdu_queue.empty()) {
    srsran::unique_byte_buffer_t pdu = pdu_queue.wait_pop();
    // TODO: delegate to demux class
    handle_pdu(std::move(pdu));
  }
}

void mac_nr::handle_pdu(srsran::unique_byte_buffer_t pdu)
{
  logger.info(pdu->msg, pdu->N_bytes, "Handling MAC PDU (%d B)", pdu->N_bytes);

  rx_pdu.init_rx();
  rx_pdu.unpack(pdu->msg, pdu->N_bytes);

  for (uint32_t i = 0; i < rx_pdu.get_num_subpdus(); ++i) {
    srsran::mac_sch_subpdu_nr subpdu = rx_pdu.get_subpdu(i);
    logger.info("Handling subPDU %d/%d: rnti=0x%x lcid=%d, sdu_len=%d",
                i + 1,
                rx_pdu.get_num_subpdus(),
                subpdu.get_c_rnti(),
                subpdu.get_lcid(),
                subpdu.get_sdu_length());
    if (subpdu.get_lcid() == 4) {
      rlc->write_pdu(subpdu.get_lcid(), subpdu.get_sdu(), subpdu.get_sdu_length());
    }
  }
}

uint64_t mac_nr::get_contention_id()
{
  return 0xdeadbeef; // TODO when rebased on PR
}

uint16_t mac_nr::get_c_rnti()
{
  return c_rnti;
}

void mac_nr::set_c_rnti(uint64_t c_rnti_)
{
  c_rnti = c_rnti_;
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
