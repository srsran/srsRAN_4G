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

#include "srsue/hdr/stack/mac_nr/ul_harq_nr.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/common/timers.h"

namespace srsue {

ul_harq_entity_nr::ul_harq_entity_nr(const uint8_t          cc_idx_,
                                     mac_interface_harq_nr* mac_,
                                     proc_ra_nr*            ra_proc_,
                                     mux_nr*                mux_) :
  mac(mac_), mux(mux_), logger(srslog::fetch_basic_logger("MAC"))
{}

int ul_harq_entity_nr::init()
{
  uint32_t proc_count = 0;
  for (auto& proc : harq_procs) {
    if (not proc.init(proc_count++, this)) {
      logger.error("Couldn't initialize HARQ procedure");
      return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

void ul_harq_entity_nr::reset()
{
  for (auto& proc : harq_procs) {
    proc.reset();
  }
}

void ul_harq_entity_nr::reset_ndi()
{
  for (auto& proc : harq_procs) {
    proc.reset_ndi();
  }
}

void ul_harq_entity_nr::set_config(srsran::ul_harq_cfg_t& harq_cfg_)
{
  harq_cfg = harq_cfg_;
}

/***************** PHY->MAC interface for UL processes **************************/
void ul_harq_entity_nr::new_grant_ul(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                                     mac_interface_phy_nr::tb_action_ul_t*          action)
{
  if (grant.pid >= harq_procs.size()) {
    logger.error("UL grant for invalid HARQ PID=%d", grant.pid);
    return;
  }

  // Determine whether the NDI has been toggled for this process
  bool ndi_toggled = (grant.ndi != harq_procs.at(grant.pid).get_ndi());

  // 1> if UL MAC entity's C-RNTI or Temporary C-RNTI; or Received in RAR;
  if (mac->get_crnti() == grant.rnti || mac->get_temp_crnti() == grant.rnti || grant.is_rar_grant) {
    // 2> if UL grant for C-RNTI and if the previous grant delivered to the HARQ entity
    // for the same HARQ process was either an uplink grant received for the MAC entity's CS-RNTI or a
    // configured uplink grant:
    if (mac->get_crnti() == grant.rnti && harq_procs.at(grant.pid).has_grant()) {
      // 3> consider the NDI to have been toggled regardless of the value of the NDI.
      ndi_toggled = true;
    }

    // 2> if the UL grant is for MAC entity's C-RNTI, and the HARQ process is configured for a configured uplink grant:
    if (mac->get_crnti() == grant.rnti && harq_procs.at(grant.pid).has_grant()) {
      // TODO: add handling for configuredGrantTimer
    }

    // 2> deliver the uplink grant and the associated HARQ information to the HARQ entity
    harq_procs.at(grant.pid).new_grant_ul(grant, ndi_toggled, action);
  } else if (mac->get_csrnti() == grant.rnti) {
    // SPS not supported
    logger.warning("Ignoring grant for CS-RNTI=0x%x", grant.rnti);
  } else {
    logger.warning("Received grant for unknown rnti=0x%x", grant.rnti);
  }

  srsran_expect(action->tb.enabled ? action->tb.payload != nullptr : true,
                "UL action enabled but no HARQ buffer provided");
}

int ul_harq_entity_nr::get_current_tbs(uint32_t pid)
{
  if (pid >= harq_procs.size()) {
    logger.error("Invalid HARQ PID=%d", pid);
    return 0;
  }
  return harq_procs.at(pid).get_current_tbs();
}

ul_harq_entity_nr::ul_harq_metrics_t ul_harq_entity_nr::get_metrics()
{
  std::lock_guard<std::mutex>          lock(metrics_mutex);
  ul_harq_entity_nr::ul_harq_metrics_t tmp = metrics;
  metrics                                  = {};
  return tmp;
}

ul_harq_entity_nr::ul_harq_process_nr::ul_harq_process_nr() :
  logger(srslog::fetch_basic_logger("MAC")), softbuffer({0, 0, nullptr})
{}

ul_harq_entity_nr::ul_harq_process_nr::~ul_harq_process_nr()
{
  if (is_initiated) {
    srsran_softbuffer_tx_free(&softbuffer);
  }
}

bool ul_harq_entity_nr::ul_harq_process_nr::init(uint32_t pid_, ul_harq_entity_nr* entity_)
{
  if (srsran_softbuffer_tx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
      SRSRAN_SUCCESS) {
    logger.error("Couldn't initialize softbuffer");
    return false;
  }

  pid         = pid_;
  harq_entity = entity_;

  is_initiated = true;

  return true;
}

void ul_harq_entity_nr::ul_harq_process_nr::reset()
{
  nof_retx      = 0;
  harq_buffer   = nullptr;
  current_grant = {};
  reset_ndi();
}

void ul_harq_entity_nr::ul_harq_process_nr::reset_ndi()
{
  current_grant.ndi = NDI_NOT_SET;
}

uint8_t ul_harq_entity_nr::ul_harq_process_nr::get_ndi()
{
  return current_grant.ndi;
}

bool ul_harq_entity_nr::ul_harq_process_nr::has_grant()
{
  return grant_configured;
}

// Basic handling of new grant
void ul_harq_entity_nr::ul_harq_process_nr::new_grant_ul(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                                                         const bool&                                    ndi_toggled,
                                                         mac_interface_phy_nr::tb_action_ul_t*          action)
{
  // Get maximum retransmissions
  uint32_t max_retx = harq_entity->harq_cfg.max_harq_tx;

  // Check maximum retransmissions, do not consider last retx ACK
  if (nof_retx >= max_retx) {
    logger.info("UL %d:  Maximum number of ReTX reached (%d). Discarding TB.", pid, max_retx);
    if (grant.rnti == harq_entity->mac->get_temp_crnti()) {
      // TODO: signal maxRetx to RA?
      // harq_entity->ra_proc->harq_max_retx();
    }
    reset();
  }

  // Checks in 5.4.2.1
  if ((grant.rnti != harq_entity->mac->get_temp_crnti() &&
       ndi_toggled) || // If not addressed to T-CRNTI and NDI toggled
      (grant.rnti == harq_entity->mac->get_crnti() &&
       harq_buffer == nullptr) || // If addressed to C-RNTI and buffer is empty
      (grant.is_rar_grant)        // Grant received in a RAR
  ) {
    // new transmission

    // generate new PDU (Msg3 or normal UL)
    harq_buffer = harq_entity->mux->get_pdu(grant.tbs);

    // 3> if a MAC PDU to transmit has been obtained
    if (harq_buffer != nullptr) {
      // 4> deliver the MAC PDU
      // 4> instruct the identified HARQ process to trigger a new transmission;
      generate_new_tx(grant, action);

      // TODO: add handling of configuredGrantTimer
    } else {
      // HARQ buffer is automatically flushed
    }
  } else {
    // retransmission
    if (harq_buffer == nullptr) {
      // ignore the UL grant
      logger.info("UL %d:  HARQ buffer empty. Ignoring grant.", pid);
      return;
    }

    // 4> instruct the identified HARQ process to trigger a retransmission;
    generate_retx(grant, action);

    // TODO: add handling of configuredGrantTimer
  }
}

uint32_t ul_harq_entity_nr::ul_harq_process_nr::get_nof_retx()
{
  return nof_retx;
}

int ul_harq_entity_nr::ul_harq_process_nr::get_current_tbs()
{
  return current_grant.tbs;
}

void ul_harq_entity_nr::ul_harq_process_nr::save_grant(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant)
{
  current_grant = grant;
  // When determining if NDI has been toggled compared to the value in the previous transmission the MAC entity shall
  // ignore NDI received in all uplink grants on PDCCH for its Temporary C-RNTI.
  if (grant.is_rar_grant || grant.rnti == harq_entity->mac->get_temp_crnti()) {
    reset_ndi();
  }
}

// New transmission (Section 5.4.2.2)
void ul_harq_entity_nr::ul_harq_process_nr::generate_new_tx(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                                                            mac_interface_phy_nr::tb_action_ul_t*          action)
{
  save_grant(grant);
  nof_retx      = 0;

  logger.info("UL %d:  New TX%s, rv=%d, tbs=%d",
              pid,
              grant.rnti == harq_entity->mac->get_temp_crnti() ? " for Msg3" : "",
              grant.rv,
              grant.tbs);

  generate_tx(action);

  std::lock_guard<std::mutex> lock(harq_entity->metrics_mutex);
  harq_entity->metrics.tx_ok++;
}

// Retransmission (Section 5.4.2.2)
void ul_harq_entity_nr::ul_harq_process_nr::generate_retx(const mac_interface_phy_nr::mac_nr_grant_ul_t& grant,
                                                          mac_interface_phy_nr::tb_action_ul_t*          action)
{
  logger.info("UL %d:  Retx=%d, rv=%d, tbs=%d", pid, nof_retx, grant.rv, grant.tbs);

  // overwrite original grant
  save_grant(grant);

  generate_tx(action);

  // increment Tx error count
  std::lock_guard<std::mutex> lock(harq_entity->metrics_mutex);
  harq_entity->metrics.tx_ko++;
}

// Transmission of pending frame (Section 5.4.2.2)
void ul_harq_entity_nr::ul_harq_process_nr::generate_tx(mac_interface_phy_nr::tb_action_ul_t* action)
{
  nof_retx++;

  {
    std::lock_guard<std::mutex> lock(harq_entity->metrics_mutex);
    harq_entity->metrics.tx_brate += current_grant.tbs * 8;
  }

  action->tb.rv         = current_grant.rv;
  action->tb.enabled    = true;
  action->tb.payload    = harq_buffer.get();
  action->tb.softbuffer = &softbuffer;

  srsran_softbuffer_tx_reset(&softbuffer);
}

} // namespace srsue
