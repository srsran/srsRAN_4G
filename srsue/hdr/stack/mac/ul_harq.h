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

#ifndef SRSUE_UL_HARQ_H
#define SRSUE_UL_HARQ_H

#include "mux.h"
#include "proc_ra.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/mac_pcap.h"
#include "srslte/common/timers.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "ul_sps.h"

using namespace srslte;

namespace srsue {

class ul_harq_entity
{
public:
  ul_harq_entity(const uint8_t cc_idx_);

  bool init(srslte::log_ref log_h_, mac_interface_rrc_common::ue_rnti_t* rntis_, ra_proc* ra_proc_h_, mux* mux_unit_);

  void reset();
  void reset_ndi();
  void set_config(srslte::ul_harq_cfg_t& harq_cfg);

  void start_pcap(srslte::mac_pcap* pcap_);

  /***************** PHY->MAC interface for UL processes **************************/
  void new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t grant, mac_interface_phy_lte::tb_action_ul_t* action);

  int   get_current_tbs(uint32_t pid);
  float get_average_retx();

private:
  class ul_harq_process
  {
  public:
    ul_harq_process();
    ~ul_harq_process();

    bool init(uint32_t pid_, ul_harq_entity* parent);
    void reset();
    void reset_ndi();

    uint32_t get_rv();
    bool     has_grant();
    bool     get_ndi();
    bool     is_sps();

    uint32_t get_nof_retx();
    int      get_current_tbs();

    // Implements Section 5.4.2.1
    void new_grant_ul(mac_interface_phy_lte::mac_grant_ul_t grant, mac_interface_phy_lte::tb_action_ul_t* action);

  private:
    mac_interface_phy_lte::mac_grant_ul_t cur_grant;

    uint32_t pid;
    uint32_t current_tx_nb;
    uint32_t current_irv;
    bool     harq_feedback;
    bool     is_grant_configured;
    bool     is_initiated;

    srslte::log_ref        log_h;
    ul_harq_entity*        harq_entity;
    srslte_softbuffer_tx_t softbuffer;

    const static int               payload_buffer_len = 128 * 1024;
    std::unique_ptr<byte_buffer_t> payload_buffer     = nullptr;
    uint8_t*                       pdu_ptr;

    void generate_tx(mac_interface_phy_lte::tb_action_ul_t* action);
    void generate_retx(mac_interface_phy_lte::mac_grant_ul_t grant, mac_interface_phy_lte::tb_action_ul_t* action);
    void generate_new_tx(mac_interface_phy_lte::mac_grant_ul_t grant, mac_interface_phy_lte::tb_action_ul_t* action);
  };

  ul_sps ul_sps_assig;

  std::vector<ul_harq_process> proc;

  mux*              mux_unit = nullptr;
  srslte::mac_pcap* pcap     = nullptr;
  srslte::log_ref   log_h;

  mac_interface_rrc_common::ue_rnti_t* rntis    = nullptr;
  srslte::ul_harq_cfg_t                harq_cfg = {};

  float    average_retx = 0.0;
  uint64_t nof_pkts     = 0;
  ra_proc* ra_procedure = nullptr;

  uint8_t cc_idx = 0;
};

typedef std::unique_ptr<ul_harq_entity> ul_harq_entity_ptr;
typedef std::vector<ul_harq_entity_ptr> ul_harq_entity_vector;

} // namespace srsue

#endif // SRSUE_UL_HARQ_H
