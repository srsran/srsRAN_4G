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
#include <queue>

typedef struct {
  uint32_t                      ue_category;
  uint32_t                      feature_group;
  uint8_t                       supported_bands[LIBLTE_RRC_BAND_N_ITEMS];
  uint32_t                      nof_supported_bands;
}rrc_args_t;

using srslte::byte_buffer_t;

namespace srsue {


class cell_t
{
 public:
  bool is_valid() {
    return earfcn != 0 && srslte_cell_isvalid(&phy_cell);
  }
  bool equals(cell_t *x) {
    return equals(x->earfcn, x->phy_cell.id);
  }
  bool equals(uint32_t earfcn, uint32_t pci) {
    return earfcn == this->earfcn && pci == phy_cell.id;
  }
  bool greater(cell_t *x) {
    return x->rsrp > rsrp;
  }
  bool plmn_equals(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {
    for (uint32_t i = 0; i < sib1.N_plmn_ids; i++) {
      if (plmn_id.mcc == sib1.plmn_id[i].id.mcc && plmn_id.mnc == sib1.plmn_id[i].id.mnc) {
        return true;
      }
    }
    return false;
  }
  cell_t() {
    srslte_cell_t tmp = {};
    cell_t(tmp, 0, 0);
  }
  cell_t(srslte_cell_t phy_cell, uint32_t earfcn, float rsrp) {
    this->has_valid_sib1 = false;
    this->has_valid_sib2 = false;
    this->has_valid_sib3 = false;
    this->has_valid_sib13 = false;
    this->phy_cell = phy_cell;
    this->rsrp = rsrp;
    this->earfcn = earfcn;
    in_sync = false;
    bzero(&sib1, sizeof(sib1));
    bzero(&sib2, sizeof(sib2));
    bzero(&sib3, sizeof(sib3));
    bzero(&sib13, sizeof(sib13));
  }

  uint32_t earfcn;
  srslte_cell_t phy_cell;
  float    rsrp;
  bool     has_valid_sib1;
  bool     has_valid_sib2;
  bool     has_valid_sib3;
  bool     has_valid_sib13;
  bool     in_sync;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT  sib1;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT  sib2;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT  sib3;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT sib13;
};

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
  ~rrc();

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

  void set_args(rrc_args_t *args);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id);

  void liblte_rrc_log(char *str);


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
  void new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn, int pci);

  // MAC interface
  void ho_ra_completed(bool ra_successful);
  void release_pucch_srs();
  void run_tti(uint32_t tti);

  void ra_problem();

  // GW interface
  bool is_connected();

  bool have_drb();

  // PDCP interface
  void write_pdu(uint32_t lcid, byte_buffer_t *pdu);

  void write_pdu_bcch_bch(byte_buffer_t *pdu);

  void write_pdu_bcch_dlsch(byte_buffer_t *pdu);

  void write_pdu_pcch(byte_buffer_t *pdu);


private:
  srslte::byte_buffer_pool *pool;
  srslte::log *rrc_log;
  phy_interface_rrc *phy;
  mac_interface_rrc *mac;
  rlc_interface_rrc *rlc;
  pdcp_interface_rrc *pdcp;
  nas_interface_rrc *nas;
  usim_interface_rrc *usim;

  LIBLTE_RRC_UL_DCCH_MSG_STRUCT ul_dcch_msg;
  LIBLTE_RRC_UL_CCCH_MSG_STRUCT ul_ccch_msg;
  LIBLTE_RRC_DL_CCCH_MSG_STRUCT dl_ccch_msg;
  LIBLTE_RRC_DL_DCCH_MSG_STRUCT dl_dcch_msg;

  byte_buffer_t* byte_align_and_pack(byte_buffer_t *pdu = NULL);
  void send_ul_ccch_msg(byte_buffer_t *pdu = NULL);
  void send_ul_dcch_msg(byte_buffer_t *pdu = NULL);
  srslte::bit_buffer_t          bit_buf;

  pthread_mutex_t mutex;

  rrc_state_t state;
  uint8_t transaction_id;
  bool drb_up;

  rrc_args_t args;
  bool first_stimsi_attempt;

  uint16_t ho_src_rnti;
  cell_t   ho_src_cell;
  phy_interface_rrc::phy_cfg_t ho_src_phy_cfg;
  mac_interface_rrc::mac_cfg_t ho_src_mac_cfg;
  bool pending_mob_reconf;
  LIBLTE_RRC_CONNECTION_RECONFIGURATION_STRUCT mob_reconf;

  // timeouts in ms

  uint32_t connecting_timeout;
  static const uint32_t RRC_CONNECTING_TIMEOUT = 5000;

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

  // RRC constants and timers
  srslte::mac_interface_timers *mac_timers;
  uint32_t n310_cnt, N310;
  uint32_t n311_cnt, N311;
  uint32_t t301, t310, t311, t304;

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

  static const std::string rb_id_str[];

  std::string get_rb_name(uint32_t lcid)
  {
    if (lcid < RB_ID_MAX) {
      return rb_id_str[lcid];
    } else {
      return "INVALID_RB";
    }
  }

  // List of strongest neighbour cell
  const static int NOF_NEIGHBOUR_CELLS = 8;
  std::vector<cell_t*> neighbour_cells;
  cell_t *serving_cell;
  void set_serving_cell(uint32_t cell_idx);
  void set_serving_cell(uint32_t earfcn, uint32_t pci);

  int  find_neighbour_cell(uint32_t earfcn, uint32_t pci);
  bool add_neighbour_cell(uint32_t earfcn, uint32_t pci, float rsrp);
  bool add_neighbour_cell(uint32_t earfcn, srslte_cell_t phy_cell, float rsrp);
  bool add_neighbour_cell(cell_t *cell);
  void sort_neighbour_cells();

  typedef enum {
    SI_ACQUIRE_IDLE = 0,
    SI_ACQUIRE_SIB1,
    SI_ACQUIRE_SIB2
  } si_acquire_state_t;

  si_acquire_state_t si_acquire_state;
  void               run_si_acquisition_procedure();
  uint32_t           sib_start_tti(uint32_t tti, uint32_t period, uint32_t offset, uint32_t sf);
  uint32_t           nof_sib1_trials;
  uint16_t           sysinfo_index;
  uint32_t           last_win_start;

  void select_next_cell_in_plmn();
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT selected_plmn_id;

  bool thread_running;
  void run_thread();

  // Measurements sub-class
  class rrc_meas {
  public:
    void init(rrc *parent);
    void reset();
    void parse_meas_config(LIBLTE_RRC_MEAS_CONFIG_STRUCT *meas_config);
    void new_phy_meas(uint32_t earfcn, uint32_t pci, float rsrp, float rsrq, uint32_t tti);
    void run_tti(uint32_t tti);
    bool timer_expired(uint32_t timer_id);
    void ho_finish();
  private:

    const static int NOF_MEASUREMENTS = 3;

    typedef enum {RSRP = 0, RSRQ = 1, BOTH = 2} quantity_t;

    typedef struct {
      uint32_t pci;
      float    q_offset;
    } meas_cell_t;

    typedef struct {
      uint32_t                        earfcn;
      float                           q_offset;
      std::map<uint32_t, meas_cell_t> cells;
    } meas_obj_t;

    typedef struct {
      uint32_t   interval;
      uint32_t   max_cell;
      uint32_t   amount;
      quantity_t trigger_quantity;
      quantity_t report_quantity;
      LIBLTE_RRC_EVENT_EUTRA_STRUCT event;
      enum {EVENT, PERIODIC} trigger_type;
    } report_cfg_t;

    typedef struct {
      float    ms[NOF_MEASUREMENTS];
      bool     triggered;
      bool     timer_enter_triggered;
      bool     timer_exit_triggered;
      uint32_t enter_tti;
      uint32_t exit_tti;
    } meas_value_t;

    typedef struct {
      uint32_t nof_reports_sent;
      uint32_t report_id;
      uint32_t object_id;
      bool     triggered;
      uint32_t periodic_timer;
      std::map<uint32_t, meas_value_t> cell_values; // Value for each PCI in this object
    } meas_t;

    std::map<uint32_t, meas_obj_t>    objects;
    std::map<uint32_t, report_cfg_t>  reports_cfg;
    std::map<uint32_t, meas_t>        active;

    rrc               *parent;
    srslte::log       *log_h;
    phy_interface_rrc *phy;
    srslte::mac_interface_timers *mac_timers;

    uint32_t filter_k_rsrp, filter_k_rsrq;
    float    filter_a[NOF_MEASUREMENTS];

    meas_value_t pcell_measurement;

    bool  s_measure_enabled;
    float s_measure_value;

    void stop_reports(meas_t *m);
    void stop_reports_object(uint32_t object_id);
    void remove_meas_object(uint32_t object_id);
    void remove_meas_report(uint32_t report_id);
    void remove_meas_id(uint32_t measId);
    void remove_meas_id(std::map<uint32_t, meas_t>::iterator it);
    void calculate_triggers(uint32_t tti);
    void update_phy();
    void L3_filter(meas_value_t *value, float rsrp[NOF_MEASUREMENTS]);
    bool find_earfcn_cell(uint32_t earfcn, uint32_t pci, meas_obj_t **object, int *cell_idx);
    float   range_to_value(quantity_t quant, uint8_t range);
    uint8_t value_to_range(quantity_t quant, float value);
    bool process_event(LIBLTE_RRC_EVENT_EUTRA_STRUCT *event, uint32_t tti,
                       bool enter_condition, bool exit_condition,
                       meas_t *m, meas_value_t *cell);

    void generate_report(uint32_t meas_id);
  };

  rrc_meas measurements;


  // Cell selection/reselection functions/variables
  typedef struct {
    float Qrxlevmin;
    float Qrxlevminoffset;
    float Qqualmin;
    float Qqualminoffset;
    float s_intrasearchP;
    float q_hyst;
    float threshservinglow;

  } cell_resel_cfg_t;

  cell_resel_cfg_t cell_resel_cfg;

  float         get_srxlev(float Qrxlevmeas);
  float         get_squal(float Qqualmeas);
  void          cell_reselection_eval(float rsrp, float rsrq);
  bool          cell_selection_eval(float rsrp, float rsrq = 0);

  bool connection_requested;
  void plmn_select_rrc(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id);

  // RLC interface
  void          max_retx_attempted();

  // Senders
  void          send_con_request();
  void          send_con_restablish_request(LIBLTE_RRC_CON_REEST_REQ_CAUSE_ENUM cause, uint16_t crnti);
  void          send_con_restablish_complete();
  void          send_con_setup_complete(byte_buffer_t *nas_msg);
  void          send_ul_info_transfer(uint32_t lcid, byte_buffer_t *sdu);
  void          send_security_mode_complete(uint32_t lcid, byte_buffer_t *pdu);
  void          send_rrc_con_reconfig_complete(byte_buffer_t *pdu);
  void          send_rrc_ue_cap_info(byte_buffer_t *pdu);

  // Parsers
  void          parse_dl_ccch(byte_buffer_t *pdu);
  void          parse_dl_dcch(uint32_t lcid, byte_buffer_t *pdu);
  void          parse_dl_info_transfer(uint32_t lcid, byte_buffer_t *pdu);

  // Helpers
  void          ho_failed();
  bool          ho_prepare();
  void          rrc_connection_release();
  void          con_restablish_cell_reselected();
  void          radio_link_failure();
  void          leave_connected();

  static void*  start_sib_thread(void *rrc_);
  void          sib_search();
  void          apply_rr_config_common_dl(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config);
  void          apply_rr_config_common_ul(LIBLTE_RRC_RR_CONFIG_COMMON_STRUCT *config);
  void          handle_sib1();
  void          handle_sib2();
  void          handle_sib3();
  void          handle_sib13();

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
};

} // namespace srsue


#endif // RRC_H
