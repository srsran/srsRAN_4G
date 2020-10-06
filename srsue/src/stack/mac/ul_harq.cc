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

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

#include "srsue/hdr/stack/mac/ul_harq.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/timers.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

ul_harq_entity::ul_harq_entity(const uint8_t cc_idx_) : proc(SRSLTE_MAX_HARQ_PROC), cc_idx(cc_idx_) {}

bool ul_harq_entity::init(srslte::log_ref                      log_h_,
                          mac_interface_rrc_common::ue_rnti_t* rntis_,
                          ra_proc*                             ra_procedure_,
                          mux*                                 mux_unit_)
{
  log_h        = log_h_;
  mux_unit     = mux_unit_;
  ra_procedure = ra_procedure_;
  rntis        = rntis_;

  for (uint32_t i = 0; i < SRSLTE_MAX_HARQ_PROC; i++) {
    if (!proc[i].init(i, this)) {
      return false;
    }
  }
  return true;
}

void ul_harq_entity::reset()
{
  for (uint32_t i = 0; i < SRSLTE_MAX_HARQ_PROC; i++) {
    proc[i].reset();
  }
  ul_sps_assig.clear();
}

void ul_harq_entity::reset_ndi()
{
  for (uint32_t i = 0; i < SRSLTE_MAX_HARQ_PROC; i++) {
    proc[i].reset_ndi();
  }
}

void ul_harq_entity::set_config(srslte::ul_harq_cfg_t& harq_cfg_)
{
  harq_cfg = harq_cfg_;
}

void ul_harq_entity::start_pcap(srslte::mac_pcap* pcap_)
{
  pcap = pcap_;
}

/***************** PHY->MAC interface for UL processes **************************/
void ul_harq_entity::new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t  grant,
                                  mac_interface_phy_lte::tb_action_ul_t* action)
{
  bzero(action, sizeof(mac_interface_phy_lte::tb_action_ul_t));

  if (grant.pid >= SRSLTE_MAX_HARQ_PROC) {
    Error("Invalid PID: %d\n", grant.pid);
    return;
  }
  if (grant.rnti == rntis->crnti || grant.rnti == rntis->temp_rnti || SRSLTE_RNTI_ISRAR(grant.rnti)) {
    if (grant.rnti == rntis->crnti && proc[grant.pid].is_sps()) {
      grant.tb.ndi = true;
    }
    proc[grant.pid].new_grant_ul(grant, action);
  } else if (grant.rnti == rntis->sps_rnti) {
    if (grant.tb.ndi) {
      grant.tb.ndi = proc[grant.pid].get_ndi();
      proc[grant.pid].new_grant_ul(grant, action);
    } else {
      Info("Not implemented\n");
    }
  } else {
    Info("Received grant for unknown rnti=0x%x\n", grant.rnti);
  }
}

int ul_harq_entity::get_current_tbs(uint32_t pid)
{
  if (pid >= SRSLTE_MAX_HARQ_PROC) {
    Error("Invalid PID: %d\n", pid);
    return 0;
  }
  return proc[pid].get_current_tbs();
}

float ul_harq_entity::get_average_retx()
{
  return average_retx;
}

ul_harq_entity::ul_harq_process::ul_harq_process()
{
  pdu_ptr        = NULL;
  payload_buffer = NULL;

  bzero(&cur_grant, sizeof(mac_interface_phy_lte::mac_grant_ul_t));

  harq_feedback       = false;
  is_initiated        = false;
  is_grant_configured = false;

  pid           = 0;
  current_tx_nb = 0;
  current_irv   = 0;
}

ul_harq_entity::ul_harq_process::~ul_harq_process()
{
  if (is_initiated) {
    srslte_softbuffer_tx_free(&softbuffer);
  }
}

bool ul_harq_entity::ul_harq_process::init(uint32_t pid_, ul_harq_entity* parent)
{
  if (srslte_softbuffer_tx_init(&softbuffer, 110)) {
    ERROR("Error initiating soft buffer\n");
    return false;
  }

  harq_entity  = parent;
  log_h        = harq_entity->log_h;
  is_initiated = true;
  pid          = pid_;

  payload_buffer = std::unique_ptr<byte_buffer_t>(new byte_buffer_t);
  if (!payload_buffer) {
    Error("Allocating memory\n");
    return false;
  }
  pdu_ptr = payload_buffer->msg;
  return true;
}

void ul_harq_entity::ul_harq_process::reset()
{
  current_tx_nb       = 0;
  current_irv         = 0;
  is_grant_configured = false;
  bzero(&cur_grant, sizeof(mac_interface_phy_lte::mac_grant_ul_t));
}

void ul_harq_entity::ul_harq_process::reset_ndi()
{
  cur_grant.tb.ndi = false;
}

void ul_harq_entity::ul_harq_process::new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t  grant,
                                                   mac_interface_phy_lte::tb_action_ul_t* action)
{
  if (grant.phich_available) {
    if (grant.tb.ndi_present && (grant.tb.ndi == get_ndi()) && (grant.tb.tbs != 0)) {
      harq_feedback = false;
    } else {
      harq_feedback = grant.hi_value;
    }

    // Get maximum retransmissions
    uint32_t max_retx;
    if (grant.rnti == harq_entity->rntis->temp_rnti) {
      max_retx = harq_entity->harq_cfg.max_harq_msg3_tx;
    } else {
      max_retx = harq_entity->harq_cfg.max_harq_tx;
    }

    // Check maximum retransmissions, do not consider last retx ACK
    if (current_tx_nb >= max_retx && !grant.hi_value) {
      Info("UL %d:  Maximum number of ReTX reached (%d). Discarding TB.\n", pid, max_retx);
      if (grant.rnti == harq_entity->rntis->temp_rnti) {
        harq_entity->ra_procedure->harq_max_retx();
      }
      reset();
    } else if (grant.rnti == harq_entity->rntis->temp_rnti && current_tx_nb) {
      harq_entity->ra_procedure->harq_retx();
    }
  }

  // Reset HARQ process if TB has changed
  if (harq_feedback && has_grant() && grant.tb.ndi_present) {
    if (grant.tb.tbs != cur_grant.tb.tbs && cur_grant.tb.tbs > 0 && grant.tb.tbs > 0) {
      Debug(
          "UL %d: Reset due to change of dci size last_grant=%d, new_grant=%d\n", pid, cur_grant.tb.tbs, grant.tb.tbs);
      reset();
    }
  }
  // Receive and route HARQ feedbacks
  if (grant.tb.ndi_present) {
    /* If TBS != 0, it's a CQI request, don't read PDU from RLC */
    if (grant.tb.tbs == 0) {
      action->tb.enabled = true;

      // Decide if adaptive retx or new tx. 3 checks in 5.4.2.1
    } else if ((grant.rnti != harq_entity->rntis->temp_rnti &&
                grant.tb.ndi != get_ndi()) || // If not addressed to T-CRNTI and NDI toggled
               (grant.rnti == harq_entity->rntis->crnti &&
                !has_grant()) || // If addressed to C-RNTI and buffer is empty
               (grant.is_rar))   // Grant received in a RAR
    {
      // New transmission
      reset();

      // Check buffer size
      if (grant.tb.tbs > payload_buffer_len) {
        Error("Grant size exceeds payload buffer size (%d > %d)\n", grant.tb.tbs, payload_buffer_len);
        return;
      }

      // Uplink dci in a RAR and there is a PDU in the Msg3 buffer
      if (grant.is_rar) {
        if (harq_entity->mux_unit->msg3_is_pending()) {
          Debug("Getting Msg3 buffer payload, grant size=%d bytes\n", grant.tb.tbs);
          pdu_ptr = harq_entity->mux_unit->msg3_get(payload_buffer.get(), grant.tb.tbs);
          if (pdu_ptr) {
            generate_new_tx(grant, action);
          } else {
            Warning("UL RAR dci available but no Msg3 on buffer\n");
          }
        } else {
          Warning("UL RAR available but no Msg3 pending on buffer\n");
        }
        // Normal UL dci
      } else {
        // Request a MAC PDU from the Multiplexing & Assemble Unit
        pdu_ptr = harq_entity->mux_unit->pdu_get(payload_buffer.get(), grant.tb.tbs);
        if (pdu_ptr) {
          generate_new_tx(grant, action);
        } else {
          Warning("Uplink dci but no MAC PDU in Multiplex Unit buffer\n");
        }
      }

      if (grant.rnti == harq_entity->rntis->crnti && harq_entity->ra_procedure->is_contention_resolution()) {
        harq_entity->ra_procedure->pdcch_to_crnti(true);
      }
    } else if (has_grant()) {
      // Adaptive Re-TX
      generate_retx(grant, action);
    } else {
      Warning("UL %d: Received retransmission but no previous dci available for this PID.\n", pid);
    }
    if (harq_entity->pcap) {
      uint16_t rnti;
      if (grant.rnti == harq_entity->rntis->temp_rnti && harq_entity->rntis->temp_rnti) {
        rnti = harq_entity->rntis->temp_rnti;
      } else {
        rnti = harq_entity->rntis->crnti;
      }
      harq_entity->pcap->write_ul_crnti(pdu_ptr, grant.tb.tbs, rnti, get_nof_retx(), grant.tti_tx, harq_entity->cc_idx);
    }
  } else if (has_grant()) {
    // Non-Adaptive Re-Tx
    generate_retx(grant, action);
  }
}

uint32_t ul_harq_entity::ul_harq_process::get_rv()
{
  int rv_of_irv[4] = {0, 2, 3, 1};
  return rv_of_irv[current_irv % 4];
}

bool ul_harq_entity::ul_harq_process::has_grant()
{
  return is_grant_configured;
}

bool ul_harq_entity::ul_harq_process::get_ndi()
{
  return cur_grant.tb.ndi;
}

bool ul_harq_entity::ul_harq_process::is_sps()
{
  return false;
}

uint32_t ul_harq_entity::ul_harq_process::get_nof_retx()
{
  return current_tx_nb;
}

int ul_harq_entity::ul_harq_process::get_current_tbs()
{
  return cur_grant.tb.tbs;
}

// Retransmission with or w/o dci (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_retx(mac_interface_phy_lte::mac_grant_ul_t  grant,
                                                    mac_interface_phy_lte::tb_action_ul_t* action)
{
  int irv_of_rv[4] = {0, 3, 1, 2};

  // HARQ entity requests an adaptive transmission
  if (grant.tb.ndi_present) {
    if (grant.tb.rv) {
      current_irv = irv_of_rv[grant.tb.rv % 4];
    }

    Info("UL %d:  Adaptive retx=%d, RV=%d, TBS=%d, HI=%s, ndi=%d, prev_ndi=%d\n",
         pid,
         current_tx_nb,
         get_rv(),
         grant.tb.tbs,
         harq_feedback ? "ACK" : "NACK",
         grant.tb.ndi,
         cur_grant.tb.ndi);

    cur_grant     = grant;
    harq_feedback = false;

    generate_tx(action);

    // HARQ entity requests a non-adaptive transmission
  } else if (!harq_feedback) {
    // Non-adaptive retx are only sent if HI=NACK. If HI=ACK but no dci was received do not reset PID
    Info("UL %d:  Non-Adaptive retx=%d, RV=%d, TBS=%d, HI=%s\n",
         pid,
         current_tx_nb,
         get_rv(),
         cur_grant.tb.tbs,
         harq_feedback ? "ACK" : "NACK");

    generate_tx(action);
  }
}

// New transmission (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_new_tx(mac_interface_phy_lte::mac_grant_ul_t  grant,
                                                      mac_interface_phy_lte::tb_action_ul_t* action)
{
  // Compute average number of retransmissions per packet considering previous packet
  harq_entity->average_retx = SRSLTE_VEC_CMA((float)current_tx_nb, harq_entity->average_retx, harq_entity->nof_pkts++);
  cur_grant                 = grant;
  harq_feedback             = false;
  is_grant_configured       = true;
  current_tx_nb             = 0;
  current_irv               = 0;

  action->is_rar = grant.is_rar || (grant.rnti == harq_entity->rntis->temp_rnti);

  Info("UL %d:  New TX%s, RV=%d, TBS=%d\n",
       pid,
       grant.rnti == harq_entity->rntis->temp_rnti ? " for Msg3" : "",
       get_rv(),
       cur_grant.tb.tbs);
  generate_tx(action);
}

// Transmission of pending frame (Section 5.4.2.2)
void ul_harq_entity::ul_harq_process::generate_tx(mac_interface_phy_lte::tb_action_ul_t* action)
{
  current_tx_nb++;

  action->current_tx_nb = current_tx_nb;
  action->expect_ack    = true;

  action->tb.rv            = cur_grant.tb.rv > 0 ? cur_grant.tb.rv : get_rv();
  action->tb.enabled       = true;
  action->tb.payload       = pdu_ptr;
  action->tb.softbuffer.tx = &softbuffer;

  current_irv = (current_irv + 1) % 4;
}

} // namespace srsue
