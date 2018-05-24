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

#ifndef SRSUE_DL_HARQ_H
#define SRSUE_DL_HARQ_H

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#include "srslte/common/log.h"
#include "srslte/common/timers.h"
#include "demux.h"
#include "dl_sps.h"
#include "srslte/common/mac_pcap.h"

#include "srslte/interfaces/ue_interfaces.h"

/* Downlink HARQ entity as defined in 5.3.2 of 36.321 */


namespace srsue {

template <std::size_t N, typename Tgrant, typename Taction, typename Tphygrant>
class dl_harq_entity
{
public:

  const static uint32_t HARQ_BCCH_PID = N;
  
  dl_harq_entity() : proc(N+1)
  {
    pcap = NULL;
  }
    
  bool init(srslte::log *log_h_, srslte::timers::timer *timer_aligment_timer_, demux *demux_unit_)
  {
    timer_aligment_timer  = timer_aligment_timer_;
    demux_unit = demux_unit_; 
    si_window_start = 0; 
    log_h = log_h_; 
    for (uint32_t i=0;i<N+1;i++) {
      if (!proc[i].init(i, this)) {
        return false; 
      }
    }
    return true;
  }

  /***************** PHY->MAC interface for DL processes **************************/
  void new_grant_dl(Tgrant grant, Taction *action)
  {
    if (grant.rnti_type != SRSLTE_RNTI_SPS) {
      uint32_t harq_pid; 
      // Set BCCH PID for SI RNTI 
      if (grant.rnti_type == SRSLTE_RNTI_SI) {
        harq_pid = HARQ_BCCH_PID; 
      } else {
        harq_pid = grant.pid%N;
      }
      if (grant.rnti_type == SRSLTE_RNTI_TEMP && last_temporal_crnti != grant.rnti) {
        grant.ndi[0] = true;
        Info("Set NDI=1 for Temp-RNTI DL grant\n");
        last_temporal_crnti = grant.rnti;
      }
      if (grant.rnti_type == SRSLTE_RNTI_USER && proc[harq_pid].is_sps()) {
        grant.ndi[0] = true;
        Info("Set NDI=1 for C-RNTI DL grant\n");
      }
      proc[harq_pid].new_grant_dl(grant, action);
    } else {
      /* This is for SPS scheduling */
      uint32_t harq_pid = get_harq_sps_pid(grant.tti)%N;
      if (grant.ndi[0]) {
        grant.ndi[0] = false;
        proc[harq_pid].new_grant_dl(grant, action);
      } else {
        if (grant.is_sps_release) {
          dl_sps_assig.clear();
          if (timer_aligment_timer->is_running()) {
            //phy_h->send_sps_ack();
            Warning("PHY Send SPS ACK not implemented\n");
          }
        } else {
          Error("SPS not implemented\n");
          //dl_sps_assig.reset(grant.tti, grant);
          //grant.ndi = true;
          //procs[harq_pid].save_grant();
        }
      }
    }
  }


  void tb_decoded(bool ack, uint32_t tb_idx, srslte_rnti_type_t rnti_type, uint32_t harq_pid)
  {
    if (rnti_type == SRSLTE_RNTI_SI) {
      proc[N].tb_decoded(ack, 0);
    } else {
      proc[harq_pid%N].tb_decoded(ack, tb_idx);
    }
  }


  void reset()
  {
    for (uint32_t i=0;i<N+1;i++) {
      proc[i].reset();
    }
    dl_sps_assig.clear();
  }

  void start_pcap(srslte::mac_pcap* pcap_) { pcap = pcap_; }

  int  get_current_tbs(uint32_t harq_pid, uint32_t tb_idx) { return proc[harq_pid%N].get_current_tbs(tb_idx); }

  void set_si_window_start(int si_window_start_) { si_window_start = si_window_start_; }

  float get_average_retx() { return average_retx; }

private:
  class dl_harq_process {
  public:
    dl_harq_process() : subproc(SRSLTE_MAX_TB) {

    }

    bool init(uint32_t pid_, dl_harq_entity *parent) {
      bool ret = true;

      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        ret &= subproc[tb].init(pid_, parent, tb);
      }
      return ret;
    }

    void reset(void) {
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        subproc[tb].reset();
      }
    }

    void new_grant_dl(Tgrant grant, Taction *action) {
      /* Fill action structure */
      bzero(action, sizeof(Taction));
      action->generate_ack = true;
      action->rnti = grant.rnti;

      /* For each subprocess... */
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        action->default_ack[tb] = false;
        action->decode_enabled[tb] = false;
        action->phy_grant.dl.tb_en[tb] = grant.tb_en[tb];
        if (grant.tb_en[tb]) {
          subproc[tb].new_grant_dl(grant, action);
        }
      }
    }

    int get_current_tbs(uint32_t tb_idx) { return subproc[tb_idx].get_current_tbs(); }

    bool is_sps() { return false; }

    void tb_decoded(bool ack_, uint32_t tb_idx) {
      subproc[tb_idx].tb_decoded(ack_);
    }

  private:

    const static int RESET_DUPLICATE_TIMEOUT = 8*6;

    class dl_tb_process {
    public:
      dl_tb_process(void) {
        is_initiated = false;
        ack = false;
        bzero(&cur_grant, sizeof(Tgrant));
        payload_buffer_ptr = NULL; 
        pthread_mutex_init(&mutex, NULL);
      }

      ~dl_tb_process() {
        if (is_initiated) {
          srslte_softbuffer_rx_free(&softbuffer);
        }
      }

      bool init(uint32_t pid_, dl_harq_entity *parent, uint32_t tb_idx) {
        tid = tb_idx;
        if (srslte_softbuffer_rx_init(&softbuffer, 110)) {
          Error("Error initiating soft buffer\n");
          return false;
        } else {
          pid = pid_;
          is_first_tb = true;
          is_initiated = true;
          harq_entity = parent;
          log_h = harq_entity->log_h;
          return true;
        }
      }

      void reset(bool lock = true) {
        if (lock) {
          pthread_mutex_lock(&mutex);
        }
        is_first_tb = true;
        ack = false;
        n_retx = 0;
        if (payload_buffer_ptr) {
          if (pid != HARQ_BCCH_PID) {
            harq_entity->demux_unit->deallocate(payload_buffer_ptr);
          }
          payload_buffer_ptr = NULL;
        }
        bzero(&cur_grant, sizeof(Tgrant));
        if (is_initiated && lock) {
          srslte_softbuffer_rx_reset(&softbuffer);
        }
        if (lock) {
          pthread_mutex_unlock(&mutex);
        }
      }

      void new_grant_dl(Tgrant grant, Taction *action) {

        pthread_mutex_lock(&mutex);

        // Compute RV for BCCH when not specified in PDCCH format
        if (pid == HARQ_BCCH_PID && grant.rv[tid] == -1) {
          uint32_t k;
          if ((grant.tti / 10) % 2 == 0 && grant.tti % 10 == 5) { // This is SIB1, k is different
            k = (grant.tti / 20) % 4;
            grant.rv[tid] = ((uint32_t) ceilf((float) 1.5 * k)) % 4;
          } else if (grant.rv[tid] == -1) {
            k = (grant.tti - harq_entity->si_window_start) % 4;
            grant.rv[tid] = ((uint32_t) ceilf((float) 1.5 * k)) % 4;
          }
        }
        calc_is_new_transmission(grant);
        // If this is a new transmission or the size of the TB has changed
        if (is_new_transmission || (cur_grant.n_bytes[tid] != grant.n_bytes[tid])) {
          if (!is_new_transmission) {
            Warning("DL PID %d: Size of grant changed during a retransmission %d!=%d\n", pid,
                    cur_grant.n_bytes[tid], grant.n_bytes[tid]);
          }
          ack = false;
          srslte_softbuffer_rx_reset_tbs(&softbuffer, cur_grant.n_bytes[tid] * 8);
          n_retx = 0;
        }

        // If data has not yet been successfully decoded
        if (!ack) {

          // Save grant
          grant.last_ndi[tid] = cur_grant.ndi[tid];
          grant.last_tti = cur_grant.tti;
          memcpy(&cur_grant, &grant, sizeof(Tgrant));

          if (payload_buffer_ptr) {
            Warning("DL PID %d: Allocating buffer already allocated. Deallocating.\n", pid);
            if (pid != HARQ_BCCH_PID) {
              harq_entity->demux_unit->deallocate(payload_buffer_ptr);
            }
          }

          // Instruct the PHY To combine the received data and attempt to decode it
          if (pid == HARQ_BCCH_PID) {
            payload_buffer_ptr = harq_entity->demux_unit->request_buffer_bcch(cur_grant.n_bytes[tid]);
          } else {
            payload_buffer_ptr = harq_entity->demux_unit->request_buffer(cur_grant.n_bytes[tid]);
          }
          action->payload_ptr[tid] = payload_buffer_ptr;
          if (!action->payload_ptr[tid]) {
            action->decode_enabled[tid] = false;
            Error("Can't get a buffer for TBS=%d\n", cur_grant.n_bytes[tid]);
            pthread_mutex_unlock(&mutex);
            return;
          }
          action->decode_enabled[tid] = true;
          action->rv[tid] = cur_grant.rv[tid];
          action->softbuffers[tid] = &softbuffer;
          memcpy(&action->phy_grant, &cur_grant.phy_grant, sizeof(Tphygrant));
          n_retx++;

        } else {
          action->default_ack[tid] = true;
          uint32_t interval = srslte_tti_interval(grant.tti, cur_grant.tti);
          Warning("DL PID %d: Received duplicate TB. Discarting and retransmitting ACK (grant_tti=%d, ndi=%d, sz=%d, reset=%s)\n",
                  pid, cur_grant.tti, cur_grant.ndi[tid], cur_grant.n_bytes[tid], interval>RESET_DUPLICATE_TIMEOUT?"yes":"no");
          if (interval > RESET_DUPLICATE_TIMEOUT) {
            reset(false);
          }
        }

        if (pid == HARQ_BCCH_PID || harq_entity->timer_aligment_timer->is_expired()) {
          // Do not generate ACK
          Debug("Not generating ACK\n");
          action->generate_ack = false;
        } else {
          if (cur_grant.rnti_type == SRSLTE_RNTI_TEMP && !ack) {
            // Postpone ACK after contention resolution is resolved
            action->generate_ack_callback = harq_entity->generate_ack_callback;
            action->generate_ack_callback_arg = harq_entity->demux_unit;
            Debug("ACK pending contention resolution\n");
          } else {
            Debug("Generating ACK\n");
          }
        }

        if (!action->decode_enabled[tid]) {
          pthread_mutex_unlock(&mutex);
        }

      }

      void tb_decoded(bool ack_) {
        ack = ack_;
        if (ack) {
          if (pid == HARQ_BCCH_PID) {
            if (harq_entity->pcap) {
              harq_entity->pcap->write_dl_sirnti(payload_buffer_ptr, cur_grant.n_bytes[tid], ack, cur_grant.tti);
            }
            Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (BCCH)\n", cur_grant.n_bytes[tid]);
            harq_entity->demux_unit->push_pdu_bcch(payload_buffer_ptr, cur_grant.n_bytes[tid], cur_grant.tti);
          } else {
            if (harq_entity->pcap) {
              harq_entity->pcap->write_dl_crnti(payload_buffer_ptr, cur_grant.n_bytes[tid], cur_grant.rnti, ack,
                                                cur_grant.tti);
            }
            if (cur_grant.rnti_type == SRSLTE_RNTI_TEMP) {
              Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit (Temporal C-RNTI)\n",
                    cur_grant.n_bytes[tid]);
              harq_entity->demux_unit->push_pdu_temp_crnti(payload_buffer_ptr, cur_grant.n_bytes[tid]);
            } else {
              Debug("Delivering PDU=%d bytes to Dissassemble and Demux unit\n", cur_grant.n_bytes[tid]);
              harq_entity->demux_unit->push_pdu(payload_buffer_ptr, cur_grant.n_bytes[tid], cur_grant.tti);

              // Compute average number of retransmissions per packet
              harq_entity->average_retx = SRSLTE_VEC_CMA((float) n_retx, harq_entity->average_retx,
                                                         harq_entity->nof_pkts++);
            }
          }
        } else if (pid != HARQ_BCCH_PID) {
          harq_entity->demux_unit->deallocate(payload_buffer_ptr);
        }

        payload_buffer_ptr = NULL;

        Info("DL %d (TB %d):  %s tbs=%d, rv=%d, ack=%s, ndi=%d (%d), tti=%d (%d)\n",
             pid, tid, is_new_transmission ? "newTX" : "reTX ",
             cur_grant.n_bytes[tid], cur_grant.rv[tid], ack ? "OK" : "KO",
             cur_grant.ndi[tid], cur_grant.last_ndi[tid], cur_grant.tti, cur_grant.last_tti);

        pthread_mutex_unlock(&mutex);

        if (ack && pid == HARQ_BCCH_PID) {
          reset();
        }
      }

      int get_current_tbs(void) { return cur_grant.n_bytes[tid] * 8; }

    private:
      // Determine if it's a new transmission 5.3.2.2
      bool calc_is_new_transmission(Tgrant grant) {

        if (grant.phy_grant.dl.mcs[tid].idx <= 28 &&          // mcs 29,30,31 always retx regardless of rest
            ((grant.ndi[tid] != cur_grant.ndi[tid])       || // 1st condition (NDI has changed)
             (pid == HARQ_BCCH_PID && grant.rv[tid] == 0) || // 2nd condition (Broadcast and 1st transmission)
             is_first_tb))
        {
          is_first_tb         = false;
          is_new_transmission = true;
          Debug("Set HARQ for new transmission\n");
        } else {
          is_new_transmission = false;
          Debug("Set HARQ for retransmission\n");
        }

        return is_new_transmission;
      }

      pthread_mutex_t mutex;

      bool is_initiated;
      dl_harq_entity *harq_entity;
      srslte::log *log_h;

      bool is_first_tb;
      bool is_new_transmission;

      uint32_t pid;     /* HARQ Proccess ID   */
      uint32_t tid;     /* Transport block ID */
      uint8_t *payload_buffer_ptr;
      bool ack;

      uint32_t n_retx;

      Tgrant cur_grant;
      srslte_softbuffer_rx_t softbuffer;
    };

    /* Transport blocks */
    std::vector<dl_tb_process> subproc;
  };
  // Private members of dl_harq_entity
  
  static bool      generate_ack_callback(void *arg)
  {
    demux *demux_unit = (demux*) arg;
    return demux_unit->get_uecrid_successful();
  }

  uint32_t         get_harq_sps_pid(uint32_t tti) { return 0; }
  
  dl_sps           dl_sps_assig;
  

  std::vector<dl_harq_process> proc;
  srslte::timers::timer   *timer_aligment_timer;
  demux           *demux_unit; 
  srslte::log     *log_h;
  srslte::mac_pcap *pcap; 
  uint16_t         last_temporal_crnti;
  int              si_window_start;

  float            average_retx;   
  uint64_t         nof_pkts; 
};

} // namespace srsue

#endif // SRSUE_DL_HARQ_H
