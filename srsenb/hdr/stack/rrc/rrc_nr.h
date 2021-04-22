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

#ifndef SRSENB_RRC_NR_H
#define SRSENB_RRC_NR_H

#include "rrc_config_common.h"
#include "rrc_metrics.h"
#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsran/asn1/rrc_nr.h"
#include "srsran/common/block_queue.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/gnb_interfaces.h"
#include <map>
#include <queue>

namespace srsenb {

enum class rrc_nr_state_t { RRC_IDLE, RRC_INACTIVE, RRC_CONNECTED };

// TODO: Make this common to NR and LTE
struct rrc_nr_cfg_sr_t {
  uint32_t period;
  //  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t nof_prb;
  uint32_t sf_mapping[80];
  uint32_t nof_subframes;
};

struct rrc_nr_cfg_t {
  asn1::rrc_nr::mib_s                                     mib;
  asn1::rrc_nr::sib1_s                                    sib1;
  asn1::rrc_nr::sys_info_ies_s::sib_type_and_info_item_c_ sibs[ASN1_RRC_NR_MAX_SIB];
  uint32_t                                                nof_sibs;
  rrc_nr_cfg_sr_t                                         sr_cfg;
  rrc_cfg_cqi_t                                           cqi_cfg;
  srsran_cell_t                                           cell;

  std::string log_level;
  uint32_t    log_hex_limit;

  srsenb::core_less_args_t coreless;
};

class rrc_nr final : public rrc_interface_pdcp_nr,
                     public rrc_interface_mac_nr,
                     public rrc_interface_rlc_nr,
                     public rrc_interface_ngap_nr
{
public:
  explicit rrc_nr(srsran::timer_handler* timers_);

  int32_t init(const rrc_nr_cfg_t&     cfg,
               phy_interface_stack_nr* phy,
               mac_interface_rrc_nr*   mac,
               rlc_interface_rrc_nr*   rlc,
               pdcp_interface_rrc_nr*  pdcp,
               ngap_interface_rrc_nr*  ngap_,
               gtpu_interface_rrc_nr*  gtpu);

  void stop();

  void get_metrics(srsenb::rrc_metrics_t& m);

  rrc_nr_cfg_t update_default_cfg(const rrc_nr_cfg_t& rrc_cfg);
  void         add_user(uint16_t rnti);
  void         config_mac();
  int32_t      generate_sibs();
  int          read_pdu_bcch_bch(const uint32_t tti, srsran::unique_byte_buffer_t& buffer) final;
  int          read_pdu_bcch_dlsch(uint32_t sib_index, srsran::unique_byte_buffer_t& buffer) final;

  // RLC interface
  // TODO
  void        read_pdu_pcch(uint8_t* payload, uint32_t payload_size) {}
  void        max_retx_attempted(uint16_t rnti) {}
  const char* get_rb_name(uint32_t lcid) { return "invalid"; }
  // PDCP interface
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;

  class ue
  {
  public:
    ue(rrc_nr* parent_, uint16_t rnti_);

    void send_connection_setup();
    void send_dl_ccch(asn1::rrc_nr::dl_ccch_msg_s* dl_dcch_msg);

    // getters
    bool is_connected() { return state == rrc_nr_state_t::RRC_CONNECTED; }
    bool is_idle() { return state == rrc_nr_state_t::RRC_IDLE; }
    bool is_inactive() { return state == rrc_nr_state_t::RRC_INACTIVE; }

    // setters

  private:
    rrc_nr*  parent;
    uint16_t rnti;

    // state
    rrc_nr_state_t                      state          = rrc_nr_state_t::RRC_IDLE;
    uint8_t                             transaction_id = 0;
    srsran::timer_handler::unique_timer rrc_setup_periodic_timer;
  };

private:
  rrc_nr_cfg_t cfg = {};

  // interfaces
  phy_interface_stack_nr* phy  = nullptr;
  mac_interface_rrc_nr*   mac  = nullptr;
  rlc_interface_rrc_nr*   rlc  = nullptr;
  pdcp_interface_rrc_nr*  pdcp = nullptr;
  gtpu_interface_rrc_nr*  gtpu = nullptr;
  ngap_interface_rrc_nr*  ngap = nullptr;

  // args
  srsran::timer_handler* timers = nullptr;

  // derived
  uint32_t              slot_dur_ms = 0;
  srslog::basic_logger& logger;

  // vars
  std::map<uint16_t, std::unique_ptr<ue> >  users;
  bool                                      running = false;
  std::vector<srsran::unique_byte_buffer_t> sib_buffer;
  srsran::unique_byte_buffer_t              mib_buffer = nullptr;

  uint32_t nof_si_messages = 0;

  // Private Methods
  void handle_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu);

  // logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string& source, direction_t dir, const srsran::byte_buffer_t* pdu, const T& msg);
};

} // namespace srsenb

#endif // SRSENB_RRC_NR_H