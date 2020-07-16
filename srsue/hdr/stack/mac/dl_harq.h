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

#ifndef SRSUE_DL_HARQ_H
#define SRSUE_DL_HARQ_H

#include "demux.h"
#include "dl_sps.h"
#include "srslte/common/log.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/timers.h"

#include "srslte/interfaces/ue_interfaces.h"

/* Downlink HARQ entity as defined in 5.3.2 of 36.321 */

namespace srsue {

class dl_harq_entity
{
public:
  dl_harq_entity(uint8_t cc_idx_);

  bool init(srslte::log_ref log_h, mac_interface_rrc::ue_rnti_t* rntis, demux* demux_unit);
  void reset();
  void start_pcap(srslte::mac_pcap* pcap_);

  /***************** PHY->MAC interface for DL processes **************************/
  void new_grant_dl(mac_interface_phy_lte::mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action);
  void tb_decoded(mac_interface_phy_lte::mac_grant_dl_t grant, bool ack[SRSLTE_MAX_CODEWORDS]);

  void set_si_window_start(int si_window_start);

  float get_average_retx();

private:
  class dl_harq_process
  {
  public:
    dl_harq_process();
    bool init(int pid, dl_harq_entity* parent);
    void reset(void);
    void reset_ndi();

    void new_grant_dl(mac_interface_phy_lte::mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action);
    void tb_decoded(mac_interface_phy_lte::mac_grant_dl_t grant, bool ack[SRSLTE_MAX_CODEWORDS]);

    bool is_sps();

  private:
    const static int RESET_DUPLICATE_TIMEOUT = 6;

    class dl_tb_process
    {
    public:
      dl_tb_process(void);
      ~dl_tb_process();

      bool init(int pid, dl_harq_entity* parent, uint32_t tb_idx);
      void reset(bool lock = true);
      void reset_ndi();

      void new_grant_dl(mac_interface_phy_lte::mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action);
      void tb_decoded(mac_interface_phy_lte::mac_grant_dl_t grant, bool* ack_ptr);

    private:
      // Determine if it's a new transmission 5.3.2.2
      bool calc_is_new_transmission(mac_interface_phy_lte::mac_grant_dl_t grant);

      std::mutex mutex;

      bool            is_initiated;
      dl_harq_entity* harq_entity;
      srslte::log_ref log_h;

      bool is_first_tb;
      bool is_new_transmission;

      bool     is_bcch;
      uint32_t pid; /* HARQ Proccess ID   */
      uint32_t tid; /* Transport block ID */
      uint8_t* payload_buffer_ptr;
      bool     ack;

      uint32_t n_retx;

      mac_interface_phy_lte::mac_grant_dl_t cur_grant;
      srslte_softbuffer_rx_t                softbuffer;
    };

    /* Transport blocks */
    std::vector<dl_tb_process> subproc;
  };

  // Private members of dl_harq_entity

  uint32_t get_harq_sps_pid(uint32_t tti);

  dl_sps dl_sps_assig;

  std::vector<dl_harq_process>  proc;
  dl_harq_process               bcch_proc;
  demux*                        demux_unit = nullptr;
  srslte::log_ref               log_h;
  srslte::mac_pcap*             pcap                = nullptr;
  mac_interface_rrc::ue_rnti_t* rntis               = nullptr;
  uint16_t                      last_temporal_crnti = 0;
  int                           si_window_start     = 0;

  float    average_retx = 0.0;
  uint64_t nof_pkts     = 0;
  uint8_t  cc_idx       = 0;
};

typedef std::unique_ptr<dl_harq_entity> dl_harq_entity_ptr;
typedef std::array<dl_harq_entity_ptr, SRSLTE_MAX_CARRIERS> dl_harq_entity_vector;

} // namespace srsue

#endif // SRSUE_DL_HARQ_H
