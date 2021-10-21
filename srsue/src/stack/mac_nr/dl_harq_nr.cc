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

#include "srsue/hdr/stack/mac_nr/dl_harq_nr.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/rwlock_guard.h"
#include "srsran/srslog/logger.h"
#include "srsue/hdr/stack/mac_nr/demux_nr.h"

namespace srsue {

dl_harq_entity_nr::dl_harq_entity_nr(uint8_t                  cc_idx_,
                                     mac_interface_harq_nr*   mac_,
                                     demux_interface_harq_nr* demux_unit_) :
  logger(srslog::fetch_basic_logger("MAC-NR")), cc_idx(cc_idx_), mac(mac_), demux_unit(demux_unit_), bcch_proc(this)
{
  // Init broadcast HARQ process
  bcch_proc.init(-1);
  pthread_rwlock_init(&rwlock, NULL);
}

dl_harq_entity_nr::~dl_harq_entity_nr()
{
  pthread_rwlock_destroy(&rwlock);
}

// Called from Stack thread through MAC (TODO: add shared::mutex)
int32_t dl_harq_entity_nr::set_config(const srsran::dl_harq_cfg_nr_t& cfg_)
{
  srsran::rwlock_write_guard lock(rwlock);
  if (cfg_.nof_procs < 1 || cfg_.nof_procs > SRSRAN_MAX_HARQ_PROC_DL_NR) {
    logger.error("Invalid configuration: %d HARQ processes not supported", cfg_.nof_procs);
    return SRSRAN_ERROR;
  }

  // clear old processees
  for (auto& proc : harq_procs) {
    proc = nullptr;
  }

  // Allocate and init configured HARQ processes
  for (uint32_t i = 0; i < cfg.nof_procs; i++) {
    harq_procs[i] = std::unique_ptr<dl_harq_process_nr>(new dl_harq_process_nr(this));
    if (!harq_procs.at(i)->init(i)) {
      logger.error("Error while initializing DL-HARQ process %d", i);
      return SRSRAN_ERROR;
    }
  }

  cfg = cfg_;

  logger.debug("cc_idx=%d, set number of HARQ processes for DL to %d", cc_idx, cfg.nof_procs);

  return SRSRAN_SUCCESS;
}

// Called from PHY workers
void dl_harq_entity_nr::new_grant_dl(const mac_nr_grant_dl_t& grant, mac_interface_phy_nr::tb_action_dl_t* action)
{
  srsran::rwlock_read_guard lock(rwlock);

  *action = {};

  // Fetch HARQ process
  dl_harq_process_nr* proc_ptr = nullptr;
  if (grant.rnti == SRSRAN_SIRNTI) {
    // Set BCCH PID for SI RNTI
    proc_ptr = &bcch_proc;
  } else {
    if (harq_procs.at(grant.pid) == nullptr) {
      logger.error("Grant for invalid HARQ PID=%d", grant.pid);
      return;
    }
    proc_ptr = harq_procs.at(grant.pid).get();
  }

  // Check NDI toggled state before forwarding to process
  bool ndi_toggled = (grant.ndi != harq_procs.at(grant.pid)->get_ndi());

  if (grant.rnti == mac->get_temp_crnti() && last_temporal_crnti != mac->get_temp_crnti()) {
    // Consider the NDI to have been toggled
    last_temporal_crnti = mac->get_temp_crnti();
    logger.info("Considering NDI in pid=%d to be toggled for first Temporal C-RNTI", grant.pid);
    ndi_toggled = true;
  }

  proc_ptr->new_grant_dl(std::move(grant), ndi_toggled, action);
}

/// Called from PHY workers
void dl_harq_entity_nr::tb_decoded(const mac_nr_grant_dl_t& grant, mac_interface_phy_nr::tb_action_dl_result_t result)
{
  srsran::rwlock_read_guard lock(rwlock);

  if (grant.rnti == SRSRAN_SIRNTI) {
    bcch_proc.tb_decoded(grant, std::move(result));
  } else {
    if (harq_procs.at(grant.pid) == nullptr) {
      logger.error("Decoded TB for invalid HARQ PID=%d", grant.pid);
      return;
    }
    harq_procs.at(grant.pid)->tb_decoded(grant, std::move(result));
  }
}

/// Called from MAC (Stack thread after, e.g. time alignment expire)
void dl_harq_entity_nr::reset()
{
  srsran::rwlock_write_guard lock(rwlock);
  for (const auto& proc : harq_procs) {
    if (proc != nullptr) {
      proc->reset();
    }
  }
  bcch_proc.reset();
}

dl_harq_entity_nr::dl_harq_metrics_t dl_harq_entity_nr::get_metrics()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  dl_harq_metrics_t           tmp = metrics;
  metrics                         = {};
  return tmp;
}

dl_harq_entity_nr::dl_harq_process_nr::dl_harq_process_nr(dl_harq_entity_nr* parent_) :
  harq_entity(parent_),
  softbuffer_rx(std::unique_ptr<srsran_softbuffer_rx_t>(new srsran_softbuffer_rx_t())),
  logger(srslog::fetch_basic_logger("MAC-NR"))
{}

dl_harq_entity_nr::dl_harq_process_nr::~dl_harq_process_nr()
{
  if (softbuffer_rx != nullptr) {
    srsran_softbuffer_rx_free(softbuffer_rx.get());
  }
}

bool dl_harq_entity_nr::dl_harq_process_nr::init(int pid_)
{
  if (softbuffer_rx == nullptr || srsran_softbuffer_rx_init_guru(softbuffer_rx.get(),
                                                                 SRSRAN_SCH_NR_MAX_NOF_CB_LDPC,
                                                                 SRSRAN_LDPC_MAX_LEN_ENCODED_CB) != SRSRAN_SUCCESS) {
    logger.error("Couldn't allocate and/or initialize softbuffer");
    return false;
  }

  if (pid_ < 0) {
    is_bcch = true;
    pid     = 0;
  } else {
    pid     = (uint32_t)pid_;
    is_bcch = false;
  }

  return true;
}

void dl_harq_entity_nr::dl_harq_process_nr::reset(void)
{
  current_grant = {};
  is_first_tb   = true;
  n_retx        = 0;
}

uint8_t dl_harq_entity_nr::dl_harq_process_nr::get_ndi()
{
  return current_grant.ndi;
}

void dl_harq_entity_nr::dl_harq_process_nr::new_grant_dl(const mac_nr_grant_dl_t&              grant,
                                                         const bool&                           ndi_toggled,
                                                         mac_interface_phy_nr::tb_action_dl_t* action)
{
  // Determine if it's a new transmission 5.3.2.2
  if (ndi_toggled ||                // 1st condition (NDI has changed)
      (is_bcch && grant.rv == 0) || // 2nd condition (Broadcast and 1st transmission)
      is_first_tb)                  // 3rd condition (is first tx for this tb)
  {
    // New transmission
    n_retx = 0;
    srsran_softbuffer_rx_reset_tbs(softbuffer_rx.get(), grant.tbs * 8);

    action->tb.enabled    = true;
    action->tb.softbuffer = softbuffer_rx.get();

    // reset conditions
    is_first_tb = false;
  } else {
    // This is a retransmission
    n_retx++;

    if (not acked) {
      // If data has not yet been successfully decoded, instruct the PHY to combine the received data
      action->tb.enabled    = true;
      action->tb.softbuffer = softbuffer_rx.get();
    } else {
      logger.info("DL %d: Received duplicate. Discarding and retransmitting ACK (n_retx=%d)", pid, n_retx);
    }
  }

  // store grant
  current_grant = grant;
}

void dl_harq_entity_nr::dl_harq_process_nr::tb_decoded(const mac_nr_grant_dl_t&                    grant,
                                                       mac_interface_phy_nr::tb_action_dl_result_t result)
{
  acked = result.ack;

  if (acked and result.payload != nullptr) {
    if (is_bcch) {
      logger.warning("Delivering PDU=%d bytes to Dissassemble and Demux unit (BCCH) not implemented", grant.tbs);
      reset();
    } else {
      if (grant.rnti == harq_entity->mac->get_temp_crnti()) {
        logger.debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (Temporal C-RNTI) not implemented",
                     grant.tbs);
      } else {
        logger.debug("Delivering PDU=%d bytes to Dissassemble and Demux unit", grant.tbs);
        harq_entity->demux_unit->push_pdu(std::move(result.payload), grant.tti);
      }
    }

    std::lock_guard<std::mutex> lock(harq_entity->metrics_mutex);
    harq_entity->metrics.rx_ok++;
    harq_entity->metrics.rx_brate += grant.tbs * 8;
  } else {
    std::lock_guard<std::mutex> lock(harq_entity->metrics_mutex);
    harq_entity->metrics.rx_ko++;
  }

  logger.info("DL %d:  %s tbs=%d, rv=%d, ack=%s, ndi=%d",
              pid,
              grant.rv == 0 ? "newTX" : "reTX ",
              grant.tbs,
              grant.rv,
              acked ? "OK" : "KO",
              grant.ndi);
}

} // namespace srsue
