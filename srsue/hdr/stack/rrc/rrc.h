/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSUE_RRC_H
#define SRSUE_RRC_H

#include "pthread.h"

#include "rrc_common.h"
#include "rrc_metrics.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/common/threads.h"
#include "srslte/interfaces/ue_interfaces.h"

#include <map>
#include <math.h>
#include <queue>

#define SRSLTE_RRC_N_BANDS 43
typedef struct {
  std::string ue_category_str;
  uint32_t    ue_category;
  int         ue_category_ul;
  int         ue_category_dl;
  uint32_t    release;
  uint32_t    feature_group;
  uint8_t     supported_bands[SRSLTE_RRC_N_BANDS];
  uint32_t    nof_supported_bands;
  bool        support_ca;
  int         mbms_service_id;
  uint32_t    mbms_service_port;
} rrc_args_t;

#define SRSLTE_UE_CATEGORY_DEFAULT "4"
#define SRSLTE_UE_CATEGORY_MIN 1
#define SRSLTE_UE_CATEGORY_MAX 21
#define SRSLTE_RELEASE_DEFAULT 8
#define SRSLTE_RELEASE_MIN 8
#define SRSLTE_RELEASE_MAX 15

using srslte::byte_buffer_t;

namespace srsue {

class cell_t
{
 public:
  bool is_valid() {
    return phy_cell.earfcn != 0 && srslte_cell_isvalid(&phy_cell.cell);
  }
  bool equals(cell_t *x) {
    return equals(x->phy_cell.earfcn, x->phy_cell.cell.id);
  }
  bool equals(uint32_t earfcn, uint32_t pci) {
    return earfcn == this->phy_cell.earfcn && pci == phy_cell.cell.id;
  }
  // NaN means an RSRP value has not yet been obtained. Keep then in the list and clean them if never updated
  bool greater(cell_t *x) {
    return rsrp > x->rsrp || std::isnan(rsrp);
  }
  bool plmn_equals(asn1::rrc::plmn_id_s plmn_id)
  {
    if (has_valid_sib1) {
      for (uint32_t i = 0; i < sib1.cell_access_related_info.plmn_id_list.size(); i++) {
        if (plmn_id.mcc == sib1.cell_access_related_info.plmn_id_list[i].plmn_id.mcc &&
            plmn_id.mnc == sib1.cell_access_related_info.plmn_id_list[i].plmn_id.mnc) {
          return true;
        }
      }
    }
    return false;
  }

  uint32_t nof_plmns() {
    if (has_valid_sib1) {
      return sib1.cell_access_related_info.plmn_id_list.size();
    } else {
      return 0;
    }
  }

  srslte::plmn_id_t get_plmn(uint32_t idx)
  {
    if (idx < sib1.cell_access_related_info.plmn_id_list.size() && has_valid_sib1) {
      return srslte::make_plmn_id_t(sib1.cell_access_related_info.plmn_id_list[idx].plmn_id);
    } else {
      return {};
    }
  }

  uint16_t get_tac() {
    if (has_valid_sib1) {
      return (uint16_t)sib1.cell_access_related_info.tac.to_number();
    } else {
      return 0;
    }
  }

  cell_t() {
    phy_interface_rrc_lte::phy_cell_t tmp;
    ZERO_OBJECT(tmp);
    ZERO_OBJECT(phy_cell);
    cell_t(tmp, 0);
  }
  cell_t(phy_interface_rrc_lte::phy_cell_t phy_cell, float rsrp)
  {
    gettimeofday(&last_update, NULL);
    this->has_valid_sib1 = false;
    this->has_valid_sib2 = false;
    this->has_valid_sib3 = false;
    this->has_valid_sib13 = false;
    this->phy_cell = phy_cell;
    this->rsrp = rsrp;
    in_sync = true;
    bzero(&sib1, sizeof(sib1));
    bzero(&sib2, sizeof(sib2));
    bzero(&sib3, sizeof(sib3));
    bzero(&sib13, sizeof(sib13));
  }

  uint32_t get_earfcn() {
    return phy_cell.earfcn;
  }

  uint32_t get_pci() {
    return phy_cell.cell.id;
  }

  void set_rsrp(float rsrp) {
    if (!std::isnan(rsrp)) {
      this->rsrp = rsrp;
    }
    in_sync = true;
    gettimeofday(&last_update, NULL);
  }

  float get_rsrp() {
    return rsrp;
  }

  void set_sib1(asn1::rrc::sib_type1_s* sib1_)
  {
    sib1           = *sib1_;
    has_valid_sib1 = true;
  }
  void set_sib2(asn1::rrc::sib_type2_s* sib2_)
  {
    sib2           = *sib2_;
    has_valid_sib2 = true;
  }
  void set_sib3(asn1::rrc::sib_type3_s* sib3_)
  {
    sib3           = *sib3_;
    has_valid_sib3 = true;
  }
  void set_sib13(asn1::rrc::sib_type13_r9_s* sib13_)
  {
    sib13           = *sib13_;
    has_valid_sib13 = true;
  }

  // TODO: replace with TTI count
  uint32_t timeout_secs(struct timeval now) {
    struct timeval t[3];
    memcpy(&t[2], &now, sizeof(struct timeval));
    memcpy(&t[1], &last_update, sizeof(struct timeval));
    get_time_interval(t);
    return t[0].tv_sec;
  }

  asn1::rrc::sib_type1_s*     sib1ptr() { return &sib1; }
  asn1::rrc::sib_type2_s*     sib2ptr() { return &sib2; }
  asn1::rrc::sib_type3_s*     sib3ptr() { return &sib3; }
  asn1::rrc::sib_type13_r9_s* sib13ptr() { return &sib13; }

  uint32_t get_cell_id() { return (uint32_t)sib1.cell_access_related_info.cell_id.to_number(); }

  bool has_sib1() {
    return has_valid_sib1;
  }
  bool has_sib2() {
    return has_valid_sib2;
  }
  bool has_sib3() {
    return has_valid_sib3;
  }
  bool has_sib13() {
    return has_valid_sib13;
  }

  bool has_sib(uint32_t index) {
    switch(index) {
      case 0:
        return has_sib1();
      case 1:
        return has_sib2();
      case 2:
        return has_sib3();
      case 12:
        return has_sib13();
    }
    return false;
  }

  void reset_sibs() {
    has_valid_sib1 = false;
    has_valid_sib2 = false;
    has_valid_sib3 = false;
    has_valid_sib13 = false;
  }

  uint16_t get_mcc() {
    uint16_t mcc;
    if (has_valid_sib1) {
      if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
        if (srslte::bytes_to_mcc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mcc[0], &mcc)) {
          return mcc;
        }
      }
    }
    return 0;
  }

  uint16_t get_mnc() {
    uint16_t mnc;
    if (has_valid_sib1) {
      if (sib1.cell_access_related_info.plmn_id_list.size() > 0) {
        if (srslte::bytes_to_mnc(&sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc[0], &mnc,
                                 sib1.cell_access_related_info.plmn_id_list[0].plmn_id.mnc.size())) {
          return mnc;
        }
      }
    }
    return 0;
  }

  phy_interface_rrc_lte::phy_cell_t phy_cell;
  bool                          in_sync;
  bool                          has_mcch;
  asn1::rrc::sib_type1_s        sib1;
  asn1::rrc::sib_type2_s        sib2;
  asn1::rrc::sib_type3_s        sib3;
  asn1::rrc::sib_type13_r9_s    sib13;
  asn1::rrc::mcch_msg_s         mcch;

private:
  float    rsrp;
  
  struct timeval last_update;

  bool     has_valid_sib1;
  bool     has_valid_sib2;
  bool     has_valid_sib3;
  bool     has_valid_sib13;
};

class rrc : public rrc_interface_nas,
            public rrc_interface_phy_lte,
            public rrc_interface_mac,
            public rrc_interface_pdcp,
            public rrc_interface_rlc,
            public srslte::timer_callback
{
public:
  rrc(srslte::log* rrc_log_);
  ~rrc();

  void init(phy_interface_rrc_lte* phy_,
            mac_interface_rrc*     mac_,
            rlc_interface_rrc*     rlc_,
            pdcp_interface_rrc*    pdcp_,
            nas_interface_rrc*     nas_,
            usim_interface_rrc*    usim_,
            gw_interface_rrc*      gw_,
            srslte::timers*        timers_,
            stack_interface_rrc*   stack_,
            const rrc_args_t&      args_);

  void stop();

  void get_metrics(rrc_metrics_t& m);

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id);
  void srslte_rrc_log(const char* str);

  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string source, const direction_t dir, const srslte::byte_buffer_t* pdu, const T& msg);

  std::string print_mbms();
  bool mbms_service_start(uint32_t serv, uint32_t port);

  // NAS interface
  void     write_sdu(srslte::unique_byte_buffer_t sdu);
  void enable_capabilities();
  uint16_t get_mcc();
  uint16_t get_mnc();
  bool     plmn_search() final;
  void     plmn_select(srslte::plmn_id_t plmn_id);
  bool     connection_request(srslte::establishment_cause_t cause, srslte::unique_byte_buffer_t dedicated_info_nas);
  void     set_ue_identity(srslte::s_tmsi_t s_tmsi);
  void     paging_completed(bool outcome) final;

  // PHY interface
  void in_sync();
  void out_of_sync();
  void new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn, int pci);

  // MAC interface
  void ho_ra_completed(bool ra_successful);
  void release_pucch_srs();
  void run_tti(uint32_t tti);
  void ra_problem();

  // GW interface
  bool is_connected(); // this is also NAS interface
  bool have_drb();

  // PDCP interface
  void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_pcch(srslte::unique_byte_buffer_t pdu);
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  // STACK interface
  void cell_search_completed(const phy_interface_rrc_lte::cell_search_ret_t& cs_ret,
                             const phy_interface_rrc_lte::phy_cell_t&        found_cell);

private:
  typedef struct {
    enum { PDU, PCCH, PDU_MCH, RLF, PDU_BCCH_DLSCH, STOP } command;
    srslte::unique_byte_buffer_t pdu;
    uint16_t lcid;
  } cmd_msg_t;

  bool                           running = false;
  srslte::block_queue<cmd_msg_t> cmd_q;

  void process_pcch(srslte::unique_byte_buffer_t pdu);

  srslte::byte_buffer_pool* pool    = nullptr;
  srslte::log*              rrc_log = nullptr;
  phy_interface_rrc_lte*    phy     = nullptr;
  mac_interface_rrc*        mac     = nullptr;
  rlc_interface_rrc*        rlc     = nullptr;
  pdcp_interface_rrc*       pdcp    = nullptr;
  nas_interface_rrc*        nas     = nullptr;
  usim_interface_rrc*       usim    = nullptr;
  gw_interface_rrc*         gw      = nullptr;
  stack_interface_rrc*      stack   = nullptr;

  srslte::unique_byte_buffer_t dedicated_info_nas;

  void send_ul_ccch_msg(const asn1::rrc::ul_ccch_msg_s& msg);
  void send_ul_dcch_msg(uint32_t lcid, const asn1::rrc::ul_dcch_msg_s& msg);

  srslte::bit_buffer_t          bit_buf;

  rrc_state_t         state, last_state = RRC_STATE_IDLE;
  uint8_t             transaction_id = 0;
  srslte::s_tmsi_t    ue_identity;
  bool                ue_identity_configured = false;

  bool drb_up = false;

  rrc_args_t args = {};

  uint32_t cell_clean_cnt = 0;

  uint16_t                         ho_src_rnti = 0;
  cell_t                           ho_src_cell = {};
  srslte::phy_cfg_t                current_phy_cfg, previous_phy_cfg = {};
  srslte::mac_cfg_t                current_mac_cfg, previous_mac_cfg = {};
  bool                             pending_mob_reconf = false;
  asn1::rrc::rrc_conn_recfg_s      mob_reconf         = {};

  uint8_t k_rrc_enc[32] = {};
  uint8_t k_rrc_int[32] = {};
  uint8_t k_up_enc[32]  = {};
  uint8_t k_up_int[32]  = {}; // Not used: only for relay nodes (3GPP 33.401 Annex A.7)

  srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo = srslte::CIPHERING_ALGORITHM_ID_EEA0;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo  = srslte::INTEGRITY_ALGORITHM_ID_EIA0;

  std::map<uint32_t, asn1::rrc::srb_to_add_mod_s> srbs;
  std::map<uint32_t, asn1::rrc::drb_to_add_mod_s> drbs;

  // RRC constants and timers
  srslte::timers* timers = nullptr;
  uint32_t        n310_cnt, N310 = 0;
  uint32_t        n311_cnt, N311 = 0;
  uint32_t        t300, t301, t302, t310, t311, t304 = 0;

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
  const static int NEIGHBOUR_TIMEOUT   = 5;
  const static int NOF_NEIGHBOUR_CELLS = 8;

  typedef std::unique_ptr<cell_t> unique_cell_t;
  std::vector<unique_cell_t>      neighbour_cells;
  unique_cell_t                   serving_cell = nullptr;
  void set_serving_cell(uint32_t cell_idx);
  void                            set_serving_cell(phy_interface_rrc_lte::phy_cell_t phy_cell);

  unique_cell_t                  remove_neighbour_cell(const uint32_t earfcn, const uint32_t pci);
  cell_t*                        get_neighbour_cell_handle(const uint32_t earfcn, const uint32_t pci);
  bool                           has_neighbour_cell(const uint32_t earfcn, const uint32_t pci);
  int  find_neighbour_cell(uint32_t earfcn, uint32_t pci);
  bool add_neighbour_cell(uint32_t earfcn, uint32_t pci, float rsrp);
  bool                           add_neighbour_cell(phy_interface_rrc_lte::phy_cell_t phy_cell, float rsrp);
  bool                           add_neighbour_cell(unique_cell_t new_cell);
  void sort_neighbour_cells();
  void clean_neighbours();
  void                           delete_last_neighbour();

  bool                     initiated                  = false;
  asn1::rrc::reest_cause_e m_reest_cause              = asn1::rrc::reest_cause_e::nulltype;
  uint16_t                 m_reest_rnti               = 0;
  bool                     reestablishment_started    = false;
  bool                     reestablishment_successful = false;

  // Measurements sub-class
  class rrc_meas {
  public:
    void init(rrc *parent);
    void reset();
    bool parse_meas_config(asn1::rrc::meas_cfg_s* meas_config);
    void new_phy_meas(uint32_t earfcn, uint32_t pci, float rsrp, float rsrq, uint32_t tti);
    void run_tti(uint32_t tti);
    bool timer_expired(uint32_t timer_id);
    void ho_finish();
    void delete_report(uint32_t earfcn, uint32_t pci);
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
      std::map<uint32_t, meas_cell_t> meas_cells;
      std::map<uint32_t, meas_cell_t> found_cells;
    } meas_obj_t;

    typedef struct {
      uint32_t   interval;
      uint32_t   max_cell;
      uint32_t   amount;
      quantity_t trigger_quantity;
      quantity_t report_quantity;
      asn1::rrc::eutra_event_s event;
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

    rrc*                   parent = nullptr;
    srslte::log*           log_h  = nullptr;
    phy_interface_rrc_lte* phy    = nullptr;
    srslte::timers*        timers = nullptr;

    uint32_t filter_k_rsrp, filter_k_rsrq = 0;
    float    filter_a[NOF_MEASUREMENTS] = {};

    meas_value_t pcell_measurement = {};

    bool  s_measure_enabled = false;
    float s_measure_value   = 0.0;

    void stop_reports(meas_t *m);
    void stop_reports_object(uint32_t object_id);
    void remove_meas_object(uint32_t object_id);
    void remove_meas_report(uint32_t report_id);
    void remove_meas_id(uint32_t measId);
    void    remove_meas_id(std::map<uint32_t, meas_t>::iterator it);
    void    calculate_triggers(uint32_t tti);
    void    update_phy();
    void L3_filter(meas_value_t *value, float rsrp[NOF_MEASUREMENTS]);
    bool find_earfcn_cell(uint32_t earfcn, uint32_t pci, meas_obj_t **object, int *cell_idx);
    float   range_to_value(quantity_t quant, uint8_t range);
    uint8_t value_to_range(quantity_t quant, float value);
    bool    process_event(asn1::rrc::eutra_event_s* event,
                          uint32_t                  tti,
                          bool                      enter_condition,
                          bool                      exit_condition,
                          meas_t*                   m,
                          meas_value_t*             cell);

    void generate_report(uint32_t meas_id);
  };

  rrc_meas measurements;

  // Measurement object from phy
  typedef struct {
    float rsrp;
    float rsrq;
    uint32_t tti;
    uint32_t earfcn;
    uint32_t pci;
  } phy_meas_t;

  void process_phy_meas();
  void process_new_phy_meas(phy_meas_t meas);
  srslte::block_queue<phy_meas_t> phy_meas_q;

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

  cell_resel_cfg_t cell_resel_cfg = {};

  float         get_srxlev(float Qrxlevmeas);
  float         get_squal(float Qqualmeas);

  /********************
   *  RRC Procedures
   *******************/

  enum class cs_result_t { changed_cell, same_cell, no_cell };

  // RRC procedures (fwd declared)
  class cell_search_proc;
  class si_acquire_proc;
  class serving_cell_config_proc;
  class cell_selection_proc;
  class connection_request_proc;
  class plmn_search_proc;
  class process_pcch_proc;
  class go_idle_proc;
  srslte::proc_t<cell_search_proc>         cell_searcher;
  srslte::proc_t<si_acquire_proc>          si_acquirer;
  srslte::proc_t<serving_cell_config_proc> serv_cell_cfg;
  srslte::proc_t<cell_selection_proc>      cell_selector;
  srslte::proc_t<go_idle_proc>             idle_setter;
  srslte::proc_t<process_pcch_proc>        pcch_processor;
  srslte::proc_t<connection_request_proc>  conn_req_proc;
  srslte::proc_t<plmn_search_proc>         plmn_searcher;

  srslte::callback_list_t callback_list;

  bool cell_selection_criteria(float rsrp, float rsrq = 0);
  void cell_reselection(float rsrp, float rsrq);

  std::vector<uint32_t> ue_required_sibs;
  srslte::plmn_id_t     selected_plmn_id = {};
  bool                  plmn_is_selected = false;

  bool security_is_activated = false;

  // RLC interface
  void max_retx_attempted();

  // Senders
  void send_con_request(srslte::establishment_cause_t cause);
  void send_con_restablish_request();
  void send_con_restablish_complete();
  void send_con_setup_complete(srslte::unique_byte_buffer_t nas_msg);
  void send_ul_info_transfer(srslte::unique_byte_buffer_t nas_msg);
  void send_security_mode_complete();
  void send_rrc_con_reconfig_complete();

  // Parsers
  void process_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_dl_ccch(srslte::unique_byte_buffer_t pdu);
  void parse_dl_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_dl_info_transfer(uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void parse_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu);
  void parse_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  // Helpers
  bool con_reconfig(asn1::rrc::rrc_conn_recfg_s* reconfig);
  void con_reconfig_failed();
  bool con_reconfig_ho(asn1::rrc::rrc_conn_recfg_s* reconfig);
  bool ho_prepare();
  void ho_failed();
  void start_ho();
  void start_go_idle();
  void rrc_connection_release();
  void radio_link_failure();
  void leave_connected();
  void stop_timers();
  void init_con_restablish_request(asn1::rrc::reest_cause_e cause);
  void proc_con_restablish_request();
  void start_cell_reselection();

  void log_rr_config_common();
  void log_phy_config_dedicated();
  void log_mac_config_dedicated();

  void apply_rr_config_common(asn1::rrc::rr_cfg_common_s* config, bool send_lower_layers);
  bool apply_rr_config_dedicated(asn1::rrc::rr_cfg_ded_s* cnfg);
  void apply_scell_config(asn1::rrc::rrc_conn_recfg_r8_ies_s* reconfig_r8);
  void apply_phy_config_dedicated(const asn1::rrc::phys_cfg_ded_s& phy_cnfg);
  void apply_phy_scell_config(const asn1::rrc::scell_to_add_mod_r10_s& scell_config);

  void apply_mac_config_dedicated_default();

  void handle_sib1();
  void handle_sib2();
  void handle_sib3();
  void handle_sib13();

  void handle_con_setup(asn1::rrc::rrc_conn_setup_s* setup);
  void handle_con_reest(asn1::rrc::rrc_conn_reest_s* setup);
  void handle_rrc_con_reconfig(uint32_t lcid, asn1::rrc::rrc_conn_recfg_s* reconfig);
  void     handle_ue_capability_enquiry(const asn1::rrc::ue_cap_enquiry_s& enquiry);
  void add_srb(asn1::rrc::srb_to_add_mod_s* srb_cnfg);
  void add_drb(asn1::rrc::drb_to_add_mod_s* drb_cnfg);
  void release_drb(uint32_t drb_id);
  uint32_t get_lcid_for_eps_bearer(const uint32_t& eps_bearer_id);
  void add_mrb(uint32_t lcid, uint32_t port);

  // Helpers for setting default values
  void set_phy_default_pucch_srs();
  void set_phy_config_dedicated_default();
  void set_phy_default();
  void set_mac_default();
  void set_rrc_default();
};

} // namespace srsue


#endif // SRSUE_RRC_H
