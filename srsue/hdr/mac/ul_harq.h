/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSUE_UL_HARQ_H
#define SRSUE_UL_HARQ_H

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/log.h"
#include "mux.h"
#include "ul_sps.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/timers.h"
#include "srslte/common/interfaces_common.h"

/* Uplink HARQ entity as defined in 5.4.2 of 36.321 */
namespace srsue {

template <std::size_t N, typename Tgrant, typename Taction, typename Tphygrant>
class ul_harq_entity {
public:
  static uint32_t pidof(uint32_t tti) {
    return (uint32_t) tti % N;
  }

  ul_harq_entity() : proc(N) {
    contention_timer = NULL;

    pcap = NULL;
    mux_unit = NULL;
    log_h = NULL;
    params = NULL;
    rntis = NULL;
    average_retx = 0;
    nof_pkts = 0;
  }

  bool init(srslte::log *log_h_,
            mac_interface_rrc_common::ue_rnti_t *rntis_,
            mac_interface_rrc_common::ul_harq_params_t *params_,
            srslte::timers::timer *contention_timer_,
            mux *mux_unit_) {
    log_h = log_h_;
    mux_unit = mux_unit_;
    params = params_;
    rntis = rntis_;
    contention_timer = contention_timer_;
    for (uint32_t i = 0; i < N; i++) {
      if (!proc[i].init(i, this)) {
        return false;
      }
    }
    return true;
  }

  void reset() {
    for (uint32_t i = 0; i < N; i++) {
      proc[i].reset();
    }
    ul_sps_assig.clear();
  }

  void reset_ndi() {
    for (uint32_t i = 0; i < N; i++) {
      proc[i].reset_ndi();
    }
  }

  void start_pcap(srslte::mac_pcap *pcap_) {
    pcap = pcap_;
  }


  /***************** PHY->MAC interface for UL processes **************************/
  void new_grant_ul(Tgrant grant, Taction *action) {
    new_grant_ul_ack(grant, NULL, action);
  }
  void new_grant_ul_ack(Tgrant grant, bool *ack, Taction *action)
  {
    if (grant.rnti_type == SRSLTE_RNTI_USER ||
      grant.rnti_type == SRSLTE_RNTI_TEMP ||
      grant.rnti_type == SRSLTE_RNTI_RAR)
    {
      if (grant.rnti_type == SRSLTE_RNTI_USER && proc[pidof(grant.tti)].is_sps()) {
        grant.ndi[0] = true;
      }
      run_tti(grant.tti, &grant, ack, action);
    } else if (grant.rnti_type == SRSLTE_RNTI_SPS) {
      if (grant.ndi[0]) {
        grant.ndi[0] = proc[pidof(grant.tti)].get_ndi();
        run_tti(grant.tti, &grant, ack, action);
      } else {
        Info("Not implemented\n");
      }
    }
  }

  void harq_recv(uint32_t tti, bool ack, Taction *action)
  {
    run_tti(tti, NULL, &ack, action);
  }

  int get_current_tbs(uint32_t tti)
  {
    int tti_harq = (int) tti-4;
    if (tti_harq < 0) {
      tti_harq += 10240;
    }
    uint32_t pid_harq = pidof(tti_harq);
    return proc[pid_harq].get_current_tbs();
  }
  
  float get_average_retx()
  {
    return average_retx;
  }
    
private:
  class ul_harq_process {
  public:
    ul_harq_process() {
      pid = 0;
      harq_feedback = false;
      log_h = NULL;
      bzero(&softbuffer, sizeof(srslte_softbuffer_tx_t));
      is_msg3 = false;
      pdu_ptr = NULL;
      current_tx_nb = 0;
      current_irv = 0;
      is_initiated = false;
      is_grant_configured = false;
      tti_last_tx = 0;
      payload_buffer = NULL;
      bzero(&cur_grant, sizeof(Tgrant));
    }

    ~ul_harq_process()
    {
      if (is_initiated) {
        if (payload_buffer) {
          free(payload_buffer);
        }
        srslte_softbuffer_tx_free(&softbuffer);
      }
    }

    bool init(uint32_t pid_, ul_harq_entity *parent)
    {
      if (srslte_softbuffer_tx_init(&softbuffer, 110)) {
        fprintf(stderr, "Error initiating soft buffer\n");
        return false;
      } else {
        is_initiated = true;
        harq_entity = parent;
        log_h = harq_entity->log_h;
        pid = pid_;
        payload_buffer = (uint8_t*) srslte_vec_malloc(payload_buffer_len*sizeof(uint8_t));
        if (!payload_buffer) {
          Error("Allocating memory\n");
          return false;
        }
        pdu_ptr = payload_buffer;
        return true;
      }
    }

    void reset()
    {
      current_tx_nb = 0;
      current_irv = 0;
      tti_last_tx = 0;
      is_grant_configured = false;
      bzero(&cur_grant, sizeof(Tgrant));
    }

    void reset_ndi() { cur_grant.ndi[0] = false; }

    void run_tti(uint32_t tti_tx, Tgrant *grant, bool *ack, Taction* action)
    {
      if (ack) {
        if (grant) {
          if (grant->ndi[0] == get_ndi() && grant->phy_grant.ul.mcs.tbs != 0) {
            *ack = false;
          }
        }
        harq_feedback = *ack;
      }

      // Reset HARQ process if TB has changed
      if (harq_feedback && has_grant() && grant) {
        if (grant->n_bytes[0] != cur_grant.n_bytes[0] && cur_grant.n_bytes[0] > 0 && grant->n_bytes[0] > 0) {
          Debug("UL %d: Reset due to change of grant size last_grant=%d, new_grant=%d\n",
               pid, cur_grant.n_bytes[0], grant->n_bytes[0]);
          reset();
        }
      }

      // Receive and route HARQ feedbacks
      if (grant) {
        if (grant->has_cqi_request && grant->phy_grant.ul.mcs.tbs == 0) {
          /* Only CQI reporting (without SCH) */
          memcpy(&action->phy_grant.ul, &grant->phy_grant.ul, sizeof(srslte_ra_ul_grant_t));
          //memcpy(&cur_grant, grant, sizeof(Tgrant));
          action->tx_enabled = true;
          action->rnti = grant->rnti;
        } else if ((!(grant->rnti_type == SRSLTE_RNTI_TEMP) && grant->ndi[0] != get_ndi() && harq_feedback) ||
            (grant->rnti_type == SRSLTE_RNTI_USER && !has_grant())                  ||
             grant->is_from_rar)
        {
          // New transmission
          reset();

          // Uplink grant in a RAR and there is a PDU in the Msg3 buffer
          if (grant->is_from_rar && harq_entity->mux_unit->msg3_is_pending()) {
            Debug("Getting Msg3 buffer payload, grant size=%d bytes\n", grant->n_bytes[0]);
            pdu_ptr  = harq_entity->mux_unit->msg3_get(payload_buffer, grant->n_bytes[0]);
            if (pdu_ptr) {
              generate_new_tx(tti_tx, true, grant, action);
            } else {
              Warning("UL RAR grant available but no Msg3 on buffer\n");
            }

            // Normal UL grant
          } else {
            if (grant->is_from_rar) {
              grant->rnti = harq_entity->rntis->crnti;
            }
            // Request a MAC PDU from the Multiplexing & Assemble Unit
            pdu_ptr = harq_entity->mux_unit->pdu_get(payload_buffer, grant->n_bytes[0], tti_tx, pid);
            if (pdu_ptr) {
              generate_new_tx(tti_tx, false, grant, action);
            } else {
              Warning("Uplink grant but no MAC PDU in Multiplex Unit buffer\n");
            }
          }
        } else if (has_grant()) {
          // Adaptive Re-TX
          generate_retx(tti_tx, grant, action);
        } else {
          Warning("UL %d: Received retransmission but no previous grant available for this PID.\n", pid);
        }
      } else if (has_grant()) {
        // Non-Adaptive Re-Tx
        generate_retx(tti_tx, action);
      }
      if (harq_entity->pcap && grant) {
        if (grant->is_from_rar && harq_entity->rntis->temp_rnti) {
          grant->rnti = harq_entity->rntis->temp_rnti;
        }
        harq_entity->pcap->write_ul_crnti(pdu_ptr, grant->n_bytes[0], grant->rnti, get_nof_retx(), tti_tx);
      }
    }

    uint32_t get_rv()
    {
      int rv_of_irv[4] = {0, 2, 3, 1};
      return rv_of_irv[current_irv%4];
    }

    bool has_grant() { return is_grant_configured; }
    bool get_ndi() { return cur_grant.ndi[0]; }
    bool is_sps() { return false; }
    uint32_t get_nof_retx() { return current_tx_nb; }
    int get_current_tbs() { return cur_grant.n_bytes[0]*8; }
   
  private: 
    Tgrant                      cur_grant;
    
    uint32_t                    pid;
    uint32_t                    current_tx_nb;
    uint32_t                    current_irv; 
    bool                        harq_feedback; 
    srslte::log                 *log_h;
    ul_harq_entity              *harq_entity; 
    bool                        is_grant_configured; 
    srslte_softbuffer_tx_t      softbuffer; 
    bool                        is_msg3;
    bool                        is_initiated;    
    uint32_t                    tti_last_tx;

    
    const static int payload_buffer_len = 128*1024; 
    uint8_t *payload_buffer;
    uint8_t *pdu_ptr; 
    
    void generate_retx(uint32_t tti_tx, Taction *action)
    {
      generate_retx(tti_tx, NULL, action);
    }

    // Retransmission with or w/o grant (Section 5.4.2.2)
    void generate_retx(uint32_t tti_tx, Tgrant *grant,
                                        Taction *action)
    {
      uint32_t max_retx;
      if (is_msg3) {
        max_retx = harq_entity->params->max_harq_msg3_tx;
      } else {
        max_retx = harq_entity->params->max_harq_tx;
      }

      if (current_tx_nb >= max_retx) {
        Info("UL %d:  Maximum number of ReTX reached (%d). Discarting TB.\n", pid, max_retx);
        reset();
        action->expect_ack = false;
        return;
      }

      int irv_of_rv[4] = {0, 3, 1, 2};

      // HARQ entity requests an adaptive transmission
      if (grant) {
        if (grant->rv[0]) {
          current_irv = irv_of_rv[grant->rv[0]%4];
        }

        Info("UL %d:  Adaptive retx=%d, RV=%d, TBS=%d, HI=%s, ndi=%d, prev_ndi=%d\n",
             pid, current_tx_nb, get_rv(), grant->n_bytes[0], harq_feedback?"ACK":"NACK", grant->ndi[0], cur_grant.ndi[0]);

        memcpy(&cur_grant, grant, sizeof(Tgrant));
        harq_feedback = false;

        generate_tx(tti_tx, action);

      // HARQ entity requests a non-adaptive transmission
      } else if (!harq_feedback) {
        // Non-adaptive retx are only sent if HI=NACK. If HI=ACK but no grant was received do not reset PID
        Info("UL %d:  Non-Adaptive retx=%d, RV=%d, TBS=%d, HI=%s\n",
             pid, current_tx_nb, get_rv(), cur_grant.n_bytes[0], harq_feedback?"ACK":"NACK");

        generate_tx(tti_tx, action);
      }

      // On every Msg3 retransmission, restart mac-ContentionResolutionTimer as defined in Section 5.1.5
      if (is_msg3) {
        harq_entity->contention_timer->reset();
      }

      harq_entity->mux_unit->pusch_retx(tti_tx, pid);
    }

    // New transmission (Section 5.4.2.2)
    void generate_new_tx(uint32_t tti_tx, bool is_msg3_, Tgrant *grant, Taction *action)
    {
      if (grant) {
        // Compute average number of retransmissions per packet considering previous packet
        harq_entity->average_retx = SRSLTE_VEC_CMA((float) current_tx_nb, harq_entity->average_retx, harq_entity->nof_pkts++);
        memcpy(&cur_grant, grant, sizeof(Tgrant));
        harq_feedback = false;
        is_grant_configured = true;
        current_tx_nb = 0;
        current_irv = 0;
        is_msg3 = is_msg3_;

        Info("UL %d:  New TX%s, RV=%d, TBS=%d, RNTI=%d\n",
             pid, is_msg3?" for Msg3":"", get_rv(), cur_grant.n_bytes[0],
             is_msg3?harq_entity->rntis->temp_rnti:cur_grant.rnti);
        generate_tx(tti_tx, action);
      }
    }

    // Transmission of pending frame (Section 5.4.2.2)
    void generate_tx(uint32_t tti_tx, Taction *action)
    {
      action->current_tx_nb = current_tx_nb;
      current_tx_nb++;
      action->expect_ack = true;
      action->rnti = is_msg3?harq_entity->rntis->temp_rnti:cur_grant.rnti;
      action->rv[0] = cur_grant.rv[0]>0?cur_grant.rv[0]:get_rv();
      action->softbuffers = &softbuffer;
      action->tx_enabled = true;
      action->payload_ptr[0] = pdu_ptr;
      memcpy(&action->phy_grant, &cur_grant.phy_grant, sizeof(Tphygrant));

      current_irv = (current_irv+1)%4;
      tti_last_tx = tti_tx;
    }
  };

  // Implements Section 5.4.2.1
  // Called with UL grant
  void run_tti(uint32_t tti, Tgrant *grant, bool *ack, Taction* action)
  {
    uint32_t tti_tx = (tti+action->tti_offset)%10240;
    proc[pidof(tti_tx)].run_tti(tti_tx, grant, ack, action);
  }

  ul_sps           ul_sps_assig;

  srslte::timers::timer  *contention_timer;
  mux             *mux_unit;
  std::vector<ul_harq_process> proc;
  srslte::log     *log_h;
  srslte::mac_pcap *pcap; 

  mac_interface_rrc_common::ue_rnti_t *rntis;
  mac_interface_rrc_common::ul_harq_params_t *params;
  
  float            average_retx;   
  uint64_t         nof_pkts; 
};

} // namespace srsue

#endif // SRSUE_UL_HARQ_H
