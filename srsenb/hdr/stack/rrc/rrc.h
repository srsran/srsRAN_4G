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

#ifndef SRSENB_RRC_H
#define SRSENB_RRC_H

#include "rrc_metrics.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/block_queue.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/common/timeout.h"
#include "srslte/interfaces/enb_interfaces.h"
#include <map>
#include <queue>

namespace srsenb {

struct rrc_cfg_sr_t {
  uint32_t                                                   period;
  asn1::rrc::sched_request_cfg_c::setup_s_::dsr_trans_max_e_ dsr_max;
  uint32_t                                                   nof_prb;
  uint32_t                                                   sf_mapping[80];
  uint32_t                                                   nof_subframes;
};

typedef enum {
  RRC_CFG_CQI_MODE_PERIODIC = 0,
  RRC_CFG_CQI_MODE_APERIODIC,
  RRC_CFG_CQI_MODE_N_ITEMS
} rrc_cfg_cqi_mode_t;

static const char rrc_cfg_cqi_mode_text[RRC_CFG_CQI_MODE_N_ITEMS][20] = {"periodic", "aperiodic"};

typedef struct {
  uint32_t           sf_mapping[80];
  uint32_t           nof_subframes;
  uint32_t           nof_prb;
  uint32_t           period;
  uint32_t           m_ri;
  bool               simultaneousAckCQI;
  rrc_cfg_cqi_mode_t mode; 
} rrc_cfg_cqi_t; 

typedef struct {
  bool                                          configured;
  asn1::rrc::lc_ch_cfg_s::ul_specific_params_s_ lc_cfg;
  asn1::rrc::pdcp_cfg_s                         pdcp_cfg;
  asn1::rrc::rlc_cfg_c                          rlc_cfg;
} rrc_cfg_qci_t;

#define MAX_NOF_QCI 10
  
typedef struct {
  asn1::rrc::sib_type1_s     sib1;
  asn1::rrc::sib_info_item_c sibs[ASN1_RRC_MAX_SIB];
  asn1::rrc::mac_main_cfg_s  mac_cnfg;

  asn1::rrc::pusch_cfg_ded_s         pusch_cfg;
  asn1::rrc::ant_info_ded_s          antenna_info;
  asn1::rrc::pdsch_cfg_ded_s::p_a_e_ pdsch_cfg;
  rrc_cfg_sr_t                       sr_cfg;
  rrc_cfg_cqi_t                      cqi_cfg;
  rrc_cfg_qci_t                      qci_cfg[MAX_NOF_QCI];
  srslte_cell_t                      cell;
  bool                               enable_mbsfn;
  uint32_t                           inactivity_timeout_ms;
  srslte::CIPHERING_ALGORITHM_ID_ENUM
      eea_preference_list[srslte::CIPHERING_ALGORITHM_ID_N_ITEMS];
  srslte::INTEGRITY_ALGORITHM_ID_ENUM
      eia_preference_list[srslte::INTEGRITY_ALGORITHM_ID_N_ITEMS];
} rrc_cfg_t;

static const char rrc_state_text[RRC_STATE_N_ITEMS][100] = {"IDLE",
                                                            "WAIT FOR CON SETUP COMPLETE",
                                                            "WAIT FOR SECURITY MODE COMPLETE",
                                                            "WAIT FOR UE CAPABILITIY INFORMATION",
                                                            "WAIT FOR CON RECONF COMPLETE",
                                                            "RRC CONNECTED"
                                                            "RELEASE REQUEST"};

class rrc : public rrc_interface_pdcp, 
            public rrc_interface_mac, 
            public rrc_interface_rlc,
            public rrc_interface_s1ap,
            public thread
{
public:
  rrc() : act_monitor(this), cnotifier(NULL), running(false), nof_si_messages(0), thread("RRC")
  {
    users.clear();
    pending_paging.clear();

    pool = NULL;
    phy = NULL;
    mac = NULL;
    rlc = NULL;
    pdcp = NULL;
    gtpu = NULL;
    s1ap = NULL;
    rrc_log = NULL;

    bzero(&sr_sched, sizeof(sr_sched));
    bzero(&cqi_sched, sizeof(cqi_sched));
    bzero(&cfg.sr_cfg, sizeof(cfg.sr_cfg));
    bzero(&cfg.cqi_cfg, sizeof(cfg.cqi_cfg));
    bzero(&cfg.qci_cfg, sizeof(cfg.qci_cfg));
    bzero(&cfg.cell, sizeof(cfg.cell));
  }

  void init(rrc_cfg_t*               cfg,
            phy_interface_stack_lte* phy,
            mac_interface_rrc*       mac,
            rlc_interface_rrc*       rlc,
            pdcp_interface_rrc*      pdcp,
            s1ap_interface_rrc*      s1ap,
            gtpu_interface_rrc*      gtpu,
            srslte::log*             log_rrc);

  void stop(); 
  void get_metrics(rrc_metrics_t &m);
  
  // rrc_interface_mac
  void rl_failure(uint16_t rnti);  
  void add_user(uint16_t rnti); 
  void upd_user(uint16_t new_rnti, uint16_t old_rnti);
  void set_activity_user(uint16_t rnti);
  bool is_paging_opportunity(uint32_t tti, uint32_t *payload_len); 
  
  // rrc_interface_rlc
  void read_pdu_bcch_dlsch(uint32_t sib_idx, uint8_t *payload);
  void read_pdu_pcch(uint8_t *payload, uint32_t buffer_size); 
  void max_retx_attempted(uint16_t rnti);
  
  // rrc_interface_s1ap
  void write_dl_info(uint16_t rnti, srslte::unique_byte_buffer_t sdu);
  void release_complete(uint16_t rnti);
  bool setup_ue_ctxt(uint16_t rnti, LIBLTE_S1AP_MESSAGE_INITIALCONTEXTSETUPREQUEST_STRUCT *msg);
  bool modify_ue_ctxt(uint16_t rnti, LIBLTE_S1AP_MESSAGE_UECONTEXTMODIFICATIONREQUEST_STRUCT *msg);
  bool setup_ue_erabs(uint16_t rnti, LIBLTE_S1AP_MESSAGE_E_RABSETUPREQUEST_STRUCT *msg);
  bool release_erabs(uint32_t rnti);
  void add_paging_id(uint32_t ueid, LIBLTE_S1AP_UEPAGINGID_STRUCT UEPagingID);
  
  // rrc_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu);

  void     parse_sibs();
  uint32_t get_nof_users();

  // logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void log_rrc_message(const std::string& source, direction_t dir, const srslte::byte_buffer_t* pdu, const T& msg);

  // Notifier for user connect 
  class connect_notifier {
  public:
    virtual void user_connected(uint16_t rnti) = 0; 
  }; 
  void set_connect_notifer(connect_notifier *cnotifier); 
  
  class activity_monitor : public thread
  {
  public:
    activity_monitor(rrc* parent_); 
    void stop(); 
  private:
    rrc* parent;
    bool running;
    void run_thread(); 
  };

  class ue
  {
  public:
    ue();
    bool is_connected();
    bool is_idle();
    bool is_timeout();
    void set_activity();

    uint32_t rl_failure();

    rrc_state_t get_state();

    void send_connection_setup(bool is_setup = true);
    void send_connection_reest();
    void send_connection_reject();
    void send_connection_release();
    void send_connection_reest_rej();
    void send_connection_reconf(srslte::unique_byte_buffer_t sdu);
    void send_connection_reconf_new_bearer(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e);
    void send_connection_reconf_upd(srslte::unique_byte_buffer_t pdu);
    void send_security_mode_command();
    void send_ue_cap_enquiry();
    void parse_ul_dcch(uint32_t lcid, srslte::unique_byte_buffer_t pdu);

    void handle_rrc_con_req(asn1::rrc::rrc_conn_request_s* msg);
    void handle_rrc_con_reest_req(asn1::rrc::rrc_conn_reest_request_r8_ies_s* msg);
    void handle_rrc_con_setup_complete(asn1::rrc::rrc_conn_setup_complete_s* msg, srslte::unique_byte_buffer_t pdu);
    void handle_rrc_reconf_complete(asn1::rrc::rrc_conn_recfg_complete_s* msg, srslte::unique_byte_buffer_t pdu);
    void handle_security_mode_complete(asn1::rrc::security_mode_complete_s* msg);
    void handle_security_mode_failure(asn1::rrc::security_mode_fail_s* msg);
    bool handle_ue_cap_info(asn1::rrc::ue_cap_info_s* msg);

    void set_bitrates(LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT *rates);
    void set_security_capabilities(LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT *caps);
    void set_security_key(uint8_t* key, uint32_t length);

    bool setup_erabs(LIBLTE_S1AP_E_RABTOBESETUPLISTCTXTSUREQ_STRUCT *e);
    bool setup_erabs(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e);
    void setup_erab(uint8_t id, LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT *qos,
                    LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT *addr, uint32_t teid_out,
                    LIBLTE_S1AP_NAS_PDU_STRUCT *nas_pdu);
    bool release_erabs();

    void notify_s1ap_ue_ctxt_setup_complete();
    void notify_s1ap_ue_erab_setup_response(LIBLTE_S1AP_E_RABTOBESETUPLISTBEARERSUREQ_STRUCT *e);

    int  sr_allocate(uint32_t period, uint8_t* I_sr, uint16_t* N_pucch_sr);
    void sr_get(uint8_t* I_sr, uint16_t* N_pucch_sr);
    int sr_free();

    int  cqi_allocate(uint32_t period, uint16_t* pmi_idx, uint16_t* n_pucch);
    void cqi_get(uint16_t* pmi_idx, uint16_t* n_pucch);
    int  cqi_free();

    int ri_get(uint32_t m_ri, uint16_t* ri_idx);

    bool select_security_algorithms();
    void send_dl_ccch(asn1::rrc::dl_ccch_msg_s* dl_ccch_msg);
    void send_dl_dcch(asn1::rrc::dl_dcch_msg_s*    dl_dcch_msg,
                      srslte::unique_byte_buffer_t pdu = srslte::unique_byte_buffer_t());

    uint16_t rnti;
    rrc*     parent;

    bool connect_notified;

    bool is_csfb;

  private:
    srslte::byte_buffer_pool  *pool;

    struct timeval t_last_activity;

    asn1::rrc::establishment_cause_e establishment_cause;

    // S-TMSI for this UE
    bool      has_tmsi;
    uint32_t  m_tmsi;
    uint8_t   mmec;

    uint32_t    rlf_cnt;
    uint8_t     transaction_id;
    rrc_state_t state;

    std::map<uint32_t, asn1::rrc::srb_to_add_mod_s> srbs;
    std::map<uint32_t, asn1::rrc::drb_to_add_mod_s> drbs;

    uint8_t               k_enb[32];      // Provided by MME
    uint8_t               k_rrc_enc[32];
    uint8_t               k_rrc_int[32];
    uint8_t               k_up_enc[32];
    uint8_t               k_up_int[32];   // Not used: only for relay nodes (3GPP 33.401 Annex A.7)

    srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo;
    srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo;

    LIBLTE_S1AP_UEAGGREGATEMAXIMUMBITRATE_STRUCT  bitrates;
    LIBLTE_S1AP_UESECURITYCAPABILITIES_STRUCT     security_capabilities;
    asn1::rrc::ue_eutra_cap_s                     eutra_capabilities;

    typedef struct {
      uint8_t                                     id;
      LIBLTE_S1AP_E_RABLEVELQOSPARAMETERS_STRUCT  qos_params;
      LIBLTE_S1AP_TRANSPORTLAYERADDRESS_STRUCT    address;
      uint32_t                                    teid_out;
      uint32_t                                    teid_in;
    }erab_t;
    std::map<uint8_t, erab_t> erabs;
    int sr_sched_sf_idx;
    int sr_sched_prb_idx;
    bool sr_allocated;
    uint32_t sr_N_pucch;
    uint32_t sr_I;
    uint32_t cqi_pucch;
    uint32_t cqi_idx;
    bool cqi_allocated;
    int cqi_sched_sf_idx;
    int cqi_sched_prb_idx;
    int                       get_drbid_config(asn1::rrc::drb_to_add_mod_s* drb, int drbid);
    bool nas_pending;
    srslte::byte_buffer_t erab_info;
  };

private:
  std::map<uint16_t,ue> users;

  std::map<uint32_t, LIBLTE_S1AP_UEPAGINGID_STRUCT > pending_paging;

  activity_monitor act_monitor;

  std::vector<srslte::unique_byte_buffer_t> sib_buffer;

  // user connect notifier 
  connect_notifier *cnotifier; 

  void process_release_complete(uint16_t rnti);
  void process_rl_failure(uint16_t rnti);
  void rem_user(uint16_t rnti); 
  uint32_t generate_sibs();
  void     configure_mbsfn_sibs(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13);

  void config_mac();
  void                      parse_ul_dcch(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu);
  void                      parse_ul_ccch(uint16_t rnti, srslte::unique_byte_buffer_t pdu);
  void configure_security(uint16_t rnti,
                          uint32_t lcid,
                          uint8_t *k_rrc_enc,
                          uint8_t *k_rrc_int,
                          uint8_t *k_up_enc,
                          uint8_t *k_up_int,
                          srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                          srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo);
  void enable_integrity(uint16_t rnti, uint32_t lcid);
  void enable_encryption(uint16_t rnti, uint32_t lcid);
  srslte::byte_buffer_pool* pool;
  srslte::byte_buffer_t     byte_buf_paging;

  phy_interface_stack_lte* phy;
  mac_interface_rrc*  mac;
  rlc_interface_rrc*  rlc;
  pdcp_interface_rrc* pdcp;
  gtpu_interface_rrc* gtpu;
  s1ap_interface_rrc* s1ap;
  srslte::log*        rrc_log;

  typedef struct{
    uint16_t                   rnti;
    uint32_t                   lcid;
    srslte::unique_byte_buffer_t pdu;
  }rrc_pdu;

  const static uint32_t LCID_EXIT     = 0xffff0000;
  const static uint32_t LCID_REM_USER = 0xffff0001;
  const static uint32_t LCID_REL_USER = 0xffff0002;
  const static uint32_t LCID_RLF_USER = 0xffff0003;
  const static uint32_t LCID_ACT_USER = 0xffff0004;
  
  bool                  running;
  static const int      RRC_THREAD_PRIO = 65;
  srslte::block_queue<rrc_pdu> rx_pdu_queue;

  struct sr_sched_t {
    uint32_t nof_users[100][80];
  };

  sr_sched_t             sr_sched;
  sr_sched_t             cqi_sched;
  asn1::rrc::mcch_msg_s  mcch;
  bool                   enable_mbms;
  rrc_cfg_t              cfg;
  uint32_t               nof_si_messages;
  asn1::rrc::sib_type2_s sib2;
  asn1::rrc::sib_type7_s sib7;

  void run_thread();
  void rem_user_thread(uint16_t rnti);
  pthread_mutex_t user_mutex;
  
  pthread_mutex_t paging_mutex; 
};

} // namespace srsenb

#endif // SRSENB_RRC_H
