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

#ifndef RRC_H
#define RRC_H

#include "pthread.h"

#include "rrc_common.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/security.h"
#include "srslte/common/threads.h"

#include <map>

using srslte::byte_buffer_t;

namespace srsue {

static std::string rb_id_str[] = {"SRB0", "SRB1", "SRB2",
                                    "DRB1","DRB2","DRB3",
                                    "DRB4","DRB5","DRB6",
                                    "DRB7","DRB8"};

class rrc
  :public rrc_interface_nas
  ,public rrc_interface_phy
  ,public rrc_interface_mac
  ,public rrc_interface_pdcp
  ,public rrc_interface_rlc
  ,public srslte::timer_callback
  ,public thread
{
public:
  rrc();

  void init(phy_interface_rrc *phy_,
            mac_interface_rrc *mac_,
            rlc_interface_rrc *rlc_,
            pdcp_interface_rrc *pdcp_,
            nas_interface_rrc *nas_,
            usim_interface_rrc *usim_,
            srslte::mac_interface_timers *mac_timers_,
            srslte::log *rrc_log_);

  void stop();

  rrc_state_t get_state();

  void set_ue_category(int category);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id);

  void liblte_rrc_log(char *str);

private:
  srslte::byte_buffer_pool *pool;
  srslte::log *rrc_log;
  phy_interface_rrc *phy;
  mac_interface_rrc *mac;
  rlc_interface_rrc *rlc;
  pdcp_interface_rrc *pdcp;
  nas_interface_rrc *nas;
  usim_interface_rrc *usim;

  srslte::bit_buffer_t bit_buf;

  pthread_mutex_t mutex;

  rrc_state_t state;
  uint8_t transaction_id;
  bool drb_up;

  // timeouts in ms

  uint32_t connecting_timeout;
  static const uint32_t RRC_CONNECTING_TIMEOUT = 1000;

  uint32_t plmn_select_timeout;
  static const uint32_t RRC_PLMN_SELECT_TIMEOUT = 10000;

  uint32_t select_cell_timeout;
  static const uint32_t RRC_SELECT_CELL_TIMEOUT = 2000;

  uint8_t k_rrc_enc[32];
  uint8_t k_rrc_int[32];
  uint8_t k_up_enc[32];
  uint8_t k_up_int[32];   // Not used: only for relay nodes (3GPP 33.401 Annex A.7)

  srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo;

  std::map<uint32_t, LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT> srbs;
  std::map<uint32_t, LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT> drbs;

  LIBLTE_RRC_DL_CCCH_MSG_STRUCT dl_ccch_msg;
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;

  // RRC constants and timers
  srslte::mac_interface_timers *mac_timers;
  uint32_t n310_cnt, N310;
  uint32_t n311_cnt, N311;
  uint32_t t301, t310, t311;
  int ue_category;

  typedef struct {
    uint32_t earfcn;
    srslte_cell_t phy_cell;
    float    rsrp;
    bool     has_valid_sib1;
    bool     has_valid_sib2;
    bool     in_sync;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT sib1;
    LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT sib2;
  } cell_t;

  std::vector<cell_t> known_cells;
  cell_t *current_cell;


  typedef enum {
    SI_ACQUIRE_IDLE = 0,
    SI_ACQUIRE_SIB1,
    SI_ACQUIRE_SIB2
  } si_acquire_state_t;

  si_acquire_state_t si_acquire_state;
  void               run_si_acquisition_procedure();
  uint32_t           sib_start_tti(uint32_t tti, uint32_t period, uint32_t x);
  uint32_t           nof_sib1_trials;
  uint32_t           last_win_start;

  void select_next_cell_in_plmn();
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT selected_plmn_id;
  int last_selected_cell;

  bool thread_running;
  void run_thread();

  // NAS interface
  void write_sdu(uint32_t lcid, byte_buffer_t *sdu);

  uint16_t get_mcc();

  uint16_t get_mnc();

  void enable_capabilities();
  void plmn_search();
  void plmn_select(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id);

  // PHY interface
  void in_sync();
  void out_of_sync();
  void earfcn_end();
  void cell_found(uint32_t earfcn, srslte_cell_t phy_cell, float rsrp);

  // MAC interface
  void release_pucch_srs();

  void ra_problem();

  // GW interface
  bool is_connected();

  bool have_drb();

  // PDCP interface
  void write_pdu(uint32_t lcid, byte_buffer_t *pdu);

  void write_pdu_bcch_bch(byte_buffer_t *pdu);

  void write_pdu_bcch_dlsch(byte_buffer_t *pdu);

  void write_pdu_pcch(byte_buffer_t *pdu);

  // Radio bearers
  typedef enum{
    RB_ID_SRB0 = 0,
    RB_ID_SRB1,
    RB_ID_SRB2,
    RB_ID_DRB1,
    RB_ID_DRB2,
    RB_ID_DRB3,
    RB_ID_DRB4,
    RB_ID_DRB5,
    RB_ID_DRB6,
    RB_ID_DRB7,
    RB_ID_DRB8,
    RB_ID_MAX
  } rb_id_t;

  std::string get_rb_name(uint32_t lcid) {
    if (lcid < RB_ID_MAX) {
      return rb_id_str[lcid];
    } else {
      return std::string("INVALID_RB");
    }
  }

  // RLC interface
  void          max_retx_attempted();

  // Senders
  void          send_con_request();
  void          send_con_restablish_request();
  void          send_con_restablish_complete();
  void          send_con_setup_complete(byte_buffer_t *nas_msg);
  void          send_ul_info_transfer(uint32_t lcid, byte_buffer_t *sdu);
  void          send_security_mode_complete(uint32_t lcid, byte_buffer_t *pdu);
  void          send_rrc_con_reconfig_complete(uint32_t lcid, byte_buffer_t *pdu);
  void          send_rrc_ue_cap_info(uint32_t lcid, byte_buffer_t *pdu);

  // Parsers
  void          parse_dl_ccch(byte_buffer_t *pdu);
  void          parse_dl_dcch(uint32_t lcid, byte_buffer_t *pdu);
  void          parse_dl_info_transfer(uint32_t lcid, byte_buffer_t *pdu);

  // Helpers
  void          rrc_connection_release();
  void          radio_link_failure(); 
  static void*  start_sib_thread(void *rrc_);
  void          sib_search();
  void          apply_sib2_configs(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2);
  void          handle_con_setup(LIBLTE_RRC_CONNECTION_SETUP_STRUCT *setup);
  void          handle_con_reest(LIBLTE_RRC_CONNECTION_REESTABLISHMENT_STRUCT *setup);
  void          handle_rrc_con_reconfig(uint32_t lcid, LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT *reconfig, byte_buffer_t *pdu);
  void          add_srb(LIBLTE_RRC_SRB_TO_ADD_MOD_STRUCT *srb_cnfg);
  void          add_drb(LIBLTE_RRC_DRB_TO_ADD_MOD_STRUCT *drb_cnfg);
  void          release_drb(uint8_t lcid);
  void          apply_rr_config_dedicated(LIBLTE_RRC_RR_CONFIG_DEDICATED_STRUCT *cnfg);
  void          apply_phy_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *phy_cnfg, bool apply_defaults); 
  void          apply_mac_config_dedicated(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *mac_cfg, bool apply_defaults); 
  
  // Helpers for setting default values 
  void          set_phy_default_pucch_srs();
  void          set_phy_default();
  void          set_mac_default();
  void          set_rrc_default(); 
  void          set_bearers();
  
};

} // namespace srsue


#endif // RRC_H
