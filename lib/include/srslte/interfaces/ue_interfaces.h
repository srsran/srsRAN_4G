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

/******************************************************************************
 * File:        interfaces.h
 * Description: Abstract base class interfaces provided by layers
 *              to other layers.
 *****************************************************************************/

#ifndef SRSLTE_UE_INTERFACES_H
#define SRSLTE_UE_INTERFACES_H

#include <set>
#include <string>

#include "mac_interface_types.h"
#include "pdcp_interface_types.h"
#include "rlc_interface_types.h"
#include "rrc_interface_types.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/common/tti_point.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/upper/pdcp_entity_base.h"

namespace srsue {

typedef enum { AUTH_OK, AUTH_FAILED, AUTH_SYNCH_FAILURE } auth_result_t;

// USIM interface for NAS
class usim_interface_nas
{
public:
  virtual std::string   get_imsi_str()                                                    = 0;
  virtual std::string   get_imei_str()                                                    = 0;
  virtual bool          get_imsi_vec(uint8_t* imsi_, uint32_t n)                          = 0;
  virtual bool          get_imei_vec(uint8_t* imei_, uint32_t n)                          = 0;
  virtual bool          get_home_plmn_id(srslte::plmn_id_t* home_plmn_id)                 = 0;
  virtual auth_result_t generate_authentication_response(uint8_t* rand,
                                                         uint8_t* autn_enb,
                                                         uint16_t mcc,
                                                         uint16_t mnc,
                                                         uint8_t* res,
                                                         int*     res_len,
                                                         uint8_t* k_asme)                 = 0;
  virtual void          generate_nas_keys(uint8_t*                            k_asme,
                                          uint8_t*                            k_nas_enc,
                                          uint8_t*                            k_nas_int,
                                          srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                          srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// USIM interface for RRC
class usim_interface_rrc
{
public:
  virtual void generate_as_keys(uint8_t* k_asme, uint32_t count_ul, srslte::as_security_config_t* sec_cfg)        = 0;
  virtual void generate_as_keys_ho(uint32_t pci, uint32_t earfcn, int ncc, srslte::as_security_config_t* sec_cfg) = 0;
};

// GW interface for NAS
class gw_interface_nas
{
public:
  virtual int setup_if_addr(uint32_t lcid, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_id, char* err_str) = 0;
  virtual int apply_traffic_flow_template(const uint8_t&                                 eps_bearer_id,
                                          const uint8_t&                                 lcid,
                                          const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)                      = 0;

  typedef enum {
    TEST_LOOP_INACTIVE = 0,
    TEST_LOOP_MODE_A_ACTIVE,
    TEST_LOOP_MODE_B_ACTIVE,
    TEST_LOOP_MODE_C_ACTIVE
  } test_loop_mode_state_t;

  /**
   * Updates the test loop mode. The IP delay parameter is only valid for Mode B.
   * @param mode
   * @param ip_pdu_delay_ms The PDU delay in ms
   */
  virtual void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms = 0) = 0;
};

// GW interface for RRC
class gw_interface_rrc
{
public:
  virtual void add_mch_port(uint32_t lcid, uint32_t port) = 0;
};

// GW interface for PDCP
class gw_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)     = 0;
  virtual void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
};

// RRC interface for MAC
class rrc_interface_mac_common
{
public:
  virtual void ra_problem() = 0;
};

class rrc_interface_mac : public rrc_interface_mac_common
{
public:
  virtual void ho_ra_completed(bool ra_successful) = 0;
  virtual void release_pucch_srs()                 = 0;
};

// RRC interface for PHY
class rrc_interface_phy_lte
{
public:
  // Measurement object from phy
  typedef struct {
    float    rsrp;
    float    rsrq;
    float    cfo_hz;
    uint32_t earfcn;
    uint32_t pci;
  } phy_meas_t;

  virtual void in_sync()                                          = 0;
  virtual void out_of_sync()                                      = 0;
  virtual void new_cell_meas(const std::vector<phy_meas_t>& meas) = 0;
};

// RRC interface for NAS
class rrc_interface_nas
{
public:
  typedef struct {
    srslte::plmn_id_t plmn_id;
    uint16_t          tac;
  } found_plmn_t;

  const static int MAX_FOUND_PLMNS = 16;

  virtual ~rrc_interface_nas()                                                           = default;
  virtual void        write_sdu(srslte::unique_byte_buffer_t sdu)                       = 0;
  virtual uint16_t    get_mcc()                                                         = 0;
  virtual uint16_t    get_mnc()                                                         = 0;
  virtual void        enable_capabilities()                                             = 0;
  virtual bool        plmn_search()                                                     = 0;
  virtual void        plmn_select(srslte::plmn_id_t plmn_id)                            = 0;
  virtual bool        connection_request(srslte::establishment_cause_t cause,
                                         srslte::unique_byte_buffer_t  dedicatedInfoNAS) = 0;
  virtual void        set_ue_identity(srslte::s_tmsi_t s_tmsi)                          = 0;
  virtual bool        is_connected()                                                    = 0;
  virtual void        paging_completed(bool outcome)                                    = 0;
  virtual std::string get_rb_name(uint32_t lcid)                                        = 0;
  virtual uint32_t    get_lcid_for_eps_bearer(const uint32_t& eps_bearer_id)            = 0;
};

// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)     = 0;
  virtual void        write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu)           = 0;
  virtual void        write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu)         = 0;
  virtual void        write_pdu_pcch(srslte::unique_byte_buffer_t pdu)               = 0;
  virtual void        write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
  virtual std::string get_rb_name(uint32_t lcid)                                     = 0;
};

// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void        max_retx_attempted()                                       = 0;
  virtual std::string get_rb_name(uint32_t lcid)                                 = 0;
  virtual void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
};

// NAS interface for RRC
class nas_interface_rrc
{
public:
  virtual void     left_rrc_connected()                                       = 0;
  virtual void     set_barring(srslte::barring_t barring)                     = 0;
  virtual bool     paging(srslte::s_tmsi_t* ue_identity)                      = 0;
  virtual bool     is_attached()                                              = 0;
  virtual void     write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
  virtual uint32_t get_k_enb_count()                                          = 0;
  virtual bool     get_k_asme(uint8_t* k_asme_, uint32_t n)                   = 0;
  virtual uint32_t get_ipv4_addr()                                            = 0;
  virtual bool     get_ipv6_addr(uint8_t* ipv6_addr)                          = 0;
  virtual void
               plmn_search_completed(const rrc_interface_nas::found_plmn_t found_plmns[rrc_interface_nas::MAX_FOUND_PLMNS],
                                     int                                   nof_plmns)       = 0;
  virtual bool connection_request_completed(bool outcome) = 0;
};

// NAS interface for UE
class nas_interface_ue
{
public:
  virtual void start_attach_proc(srslte::proc_state_t* proc_result, srslte::establishment_cause_t cause_) = 0;
  virtual bool detach_request(const bool switch_off)                                                      = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reestablish()                                                                                        = 0;
  virtual void reestablish(uint32_t lcid)                                                                           = 0;
  virtual void reset()                                                                                              = 0;
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking)                            = 0;
  virtual void add_bearer(uint32_t lcid, srslte::pdcp_config_t cnfg)                                                = 0;
  virtual void change_lcid(uint32_t old_lcid, uint32_t new_lcid)                                                    = 0;
  virtual void config_security(uint32_t lcid, srslte::as_security_config_t sec_cfg)                                 = 0;
  virtual void config_security_all(srslte::as_security_config_t sec_cfg)                                            = 0;
  virtual void enable_integrity(uint32_t lcid, srslte::srslte_direction_t direction)                                = 0;
  virtual void enable_encryption(uint32_t                   lcid,
                                 srslte::srslte_direction_t direction = srslte::srslte_direction_t::DIRECTION_TXRX) = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu)     = 0;
  virtual void write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu)           = 0;
  virtual void write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu)         = 0;
  virtual void write_pdu_pcch(srslte::unique_byte_buffer_t sdu)               = 0;
  virtual void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
};

class pdcp_interface_gw
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking) = 0;
  virtual bool is_lcid_enabled(uint32_t lcid)                                            = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void reset()                                                                          = 0;
  virtual void reestablish()                                                                    = 0;
  virtual void reestablish(uint32_t lcid)                                                       = 0;
  virtual void add_bearer(uint32_t lcid, const srslte::rlc_config_t& cnfg)                      = 0;
  virtual void add_bearer_mrb(uint32_t lcid)                                                    = 0;
  virtual void del_bearer(uint32_t lcid)                                                        = 0;
  virtual void suspend_bearer(uint32_t lcid)                                                    = 0;
  virtual void resume_bearer(uint32_t lcid)                                                     = 0;
  virtual void change_lcid(uint32_t old_lcid, uint32_t new_lcid)                                = 0;
  virtual bool has_bearer(uint32_t lcid)                                                        = 0;
  virtual bool has_data(const uint32_t lcid)                                                    = 0;
  virtual bool is_suspended(const uint32_t lcid)                                                = 0;
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking = true) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking = true) = 0;
  virtual void discard_sdu(uint32_t lcid, uint32_t discard_sn)                                  = 0;
  virtual bool rb_is_um(uint32_t lcid)                                                          = 0;
};

// RLC interface for MAC
class rlc_interface_mac : public srslte::read_pdu_interface
{
public:
  /* MAC calls has_data() to query whether a logical channel has data to transmit (without
   * knowing how much. This function should return quickly. */
  virtual bool has_data(const uint32_t lcid) = 0;

  /* MAC calls RLC to get the buffer state for a logical channel. */
  virtual uint32_t get_buffer_state(const uint32_t lcid) = 0;

  const static int MAX_PDU_SEGMENTS = 20;

  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int read_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)     = 0;
  virtual void write_pdu_bcch_bch(srslte::unique_byte_buffer_t payload)           = 0;
  virtual void write_pdu_bcch_dlsch(uint8_t* payload, uint32_t nof_bytes)         = 0;
  virtual void write_pdu_pcch(srslte::unique_byte_buffer_t payload)               = 0;
  virtual void write_pdu_mch(uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
};

/** MAC interface
 *
 */
/* Interface PHY -> MAC */
class mac_interface_phy_lte
{
public:
  typedef struct {
    uint32_t nof_mbsfn_services;
  } mac_phy_cfg_mbsfn_t;

  typedef struct {
    uint32_t tbs;
    bool     ndi;
    bool     ndi_present;
    int      rv;
  } mac_tb_t;

  typedef struct {
    mac_tb_t tb[SRSLTE_MAX_TB];
    uint32_t pid;
    uint16_t rnti;
    bool     is_sps_release;
    uint32_t tti;
  } mac_grant_dl_t;

  typedef struct {
    mac_tb_t tb;
    uint32_t pid;
    uint16_t rnti;
    bool     phich_available;
    bool     hi_value;
    bool     is_rar;
    uint32_t tti_tx;
  } mac_grant_ul_t;

  typedef struct {
    bool     enabled;
    uint32_t rv;
    uint8_t* payload;
    union {
      srslte_softbuffer_rx_t* rx;
      srslte_softbuffer_tx_t* tx;
    } softbuffer;
  } tb_action_t;

  typedef struct {
    tb_action_t tb[SRSLTE_MAX_TB];

    bool generate_ack;
  } tb_action_dl_t;

  typedef struct {
    tb_action_t tb;
    uint32_t    current_tx_nb;
    bool        expect_ack;
  } tb_action_ul_t;

  /* Query the MAC for the current RNTI to look for
   */
  virtual uint16_t get_dl_sched_rnti(uint32_t tti) = 0;
  virtual uint16_t get_ul_sched_rnti(uint32_t tti) = 0;

  /* Indicate reception of UL dci.
   * payload_ptr points to memory where MAC PDU must be written by MAC layer */
  virtual void new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action) = 0;

  /* Indicate reception of DL dci. */
  virtual void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action) = 0;

  /* Indicate successful decoding of PDSCH AND PCH TB. */
  virtual void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSLTE_MAX_CODEWORDS]) = 0;

  /* Indicate successful decoding of BCH TB through PBCH */
  virtual void bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len) = 0;

  /* Indicate successful decoding of MCH TB through PMCH */
  virtual void mch_decoded(uint32_t len, bool crc) = 0;

  /* Obtain action for a new MCH subframe. */
  virtual void new_mch_dl(const srslte_pdsch_grant_t& phy_grant, tb_action_dl_t* action) = 0;

  /* Communicate the number of mbsfn services available  */
  virtual void set_mbsfn_config(uint32_t nof_mbsfn_services) = 0;
};

/* Interface RRC -> MAC shared between different RATs */
class mac_interface_rrc_common
{
public:
  // Class to handle UE specific RNTIs between RRC and MAC
  typedef struct {
    uint16_t crnti;
    uint16_t rar_rnti;
    uint16_t temp_rnti;
    uint16_t tpc_rnti;
    uint16_t sps_rnti;
    uint64_t contention_id;
  } ue_rnti_t;
};

/* Interface RRC -> MAC */
class mac_interface_rrc : public mac_interface_rrc_common
{
public:
  virtual void clear_rntis() = 0;

  /* Instructs the MAC to start receiving BCCH */
  virtual void bcch_start_rx(int si_window_start, int si_window_length) = 0;
  virtual void bcch_stop_rx()                                           = 0;

  /* Instructs the MAC to start receiving PCCH */
  virtual void pcch_start_rx() = 0;

  /* RRC configures a logical channel */
  virtual void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) = 0;

  /* Instructs the MAC to start receiving an MCH */
  virtual void mch_start_rx(uint32_t lcid) = 0;

  virtual void set_config(srslte::mac_cfg_t& mac_cfg) = 0;

  virtual void get_rntis(ue_rnti_t* rntis)                      = 0;
  virtual void set_contention_id(uint64_t uecri)                = 0;
  virtual void set_ho_rnti(uint16_t crnti, uint16_t target_pci) = 0;

  virtual void start_noncont_ho(uint32_t preamble_index, uint32_t prach_mask) = 0;
  virtual void start_cont_ho()                                                = 0;

  virtual void reconfiguration(const uint32_t& cc_idx, const bool& enable) = 0;
  virtual void reset()                                                     = 0;
  virtual void wait_uplink()                                               = 0;
};

/** PHY interface
 *
 */

typedef struct {
  std::string            type = "lte";
  srslte::phy_log_args_t log;

  std::string           dl_earfcn      = "3400"; // comma-separated list of DL EARFCNs
  std::string           ul_earfcn      = "";     // comma-separated list of UL EARFCNs
  std::vector<uint32_t> dl_earfcn_list = {3400}; // vectorized version of dl_earfcn that gets populated during init
  std::map<uint32_t, uint32_t> ul_earfcn_map;    // Map linking DL EARFCN and UL EARFCN

  float dl_freq = -1.0f;
  float ul_freq = -1.0f;

  bool     ul_pwr_ctrl_en  = false;
  float    prach_gain      = -1;
  uint32_t pdsch_max_its   = 8;
  bool     meas_evm        = false;
  int      nof_phy_threads = 3;

  int worker_cpu_mask   = -1;
  int sync_cpu_affinity = -1;

  uint32_t    nof_carriers                 = 1;
  uint32_t    nof_rx_ant                   = 1;
  std::string equalizer_mode               = "mmse";
  int         cqi_max                      = 15;
  int         cqi_fixed                    = -1;
  float       snr_ema_coeff                = 0.1f;
  std::string snr_estim_alg                = "refs";
  bool        agc_enable                   = true;
  bool        correct_sync_error           = false;
  bool        cfo_is_doppler               = false;
  bool        cfo_integer_enabled          = false;
  float       cfo_correct_tol_hz           = 1.0f;
  float       cfo_pss_ema                  = DEFAULT_CFO_EMA_TRACK;
  float       cfo_loop_bw_pss              = DEFAULT_CFO_BW_PSS;
  float       cfo_loop_bw_ref              = DEFAULT_CFO_BW_REF;
  float       cfo_loop_ref_min             = DEFAULT_CFO_REF_MIN;
  float       cfo_loop_pss_tol             = DEFAULT_CFO_PSS_MIN;
  float       sfo_ema                      = DEFAULT_SFO_EMA_COEFF;
  uint32_t    sfo_correct_period           = DEFAULT_SAMPLE_OFFSET_CORRECT_PERIOD;
  uint32_t    cfo_loop_pss_conv            = DEFAULT_PSS_STABLE_TIMEOUT;
  uint32_t    cfo_ref_mask                 = 1023;
  bool        interpolate_subframe_enabled = false;
  bool        estimator_fil_auto           = false;
  float       estimator_fil_stddev         = 1.0f;
  uint32_t    estimator_fil_order          = 4;
  float       snr_to_cqi_offset            = 0.0f;
  std::string sss_algorithm                = "full";
  float       rx_gain_offset               = 62;
  bool        pdsch_csi_enabled            = true;
  bool        pdsch_8bit_decoder           = false;
  uint32_t    intra_freq_meas_len_ms       = 20;
  uint32_t    intra_freq_meas_period_ms    = 200;
  float       force_ul_amplitude           = 0.0f;

  float    in_sync_rsrp_dbm_th    = -130.0f;
  float    in_sync_snr_db_th      = 1.0f;
  uint32_t nof_in_sync_events     = 10;
  uint32_t nof_out_of_sync_events = 20;

  srslte::channel::args_t dl_channel_args;
  srslte::channel::args_t ul_channel_args;
} phy_args_t;

/* RAT agnostic Interface MAC -> PHY */
class phy_interface_mac_common
{
public:
  /* Sets a C-RNTI allowing the PHY to pregenerate signals if necessary */
  virtual void set_crnti(uint16_t rnti) = 0;

  /* Time advance commands */
  virtual void set_timeadv_rar(uint32_t ta_cmd) = 0;
  virtual void set_timeadv(uint32_t ta_cmd)     = 0;

  /* Activate / Disactivate SCell*/
  virtual void set_activation_deactivation_scell(uint32_t cmd) = 0;

  /* Sets RAR dci payload */
  virtual void set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti) = 0;

  virtual uint32_t get_current_tti() = 0;

  virtual float get_phr()         = 0;
  virtual float get_pathloss_db() = 0;
};

/* Interface MAC -> PHY */
class phy_interface_mac_lte : public phy_interface_mac_common
{
public:
  typedef struct {
    bool     is_transmitted;
    uint32_t tti_ra;
    uint32_t f_id;
    uint32_t preamble_format;
  } prach_info_t;

  /* Configure PRACH using parameters written by RRC */
  virtual void configure_prach_params() = 0;

  virtual void
                       prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec = 0.0f) = 0;
  virtual prach_info_t prach_get_info() = 0;

  /* Indicates the transmission of a SR signal in the next opportunity */
  virtual void sr_send()        = 0;
  virtual int  sr_last_tx_tti() = 0;

  virtual void set_mch_period_stop(uint32_t stop) = 0;
};

class phy_interface_rrc_lte
{
public:
  virtual void set_config(srslte::phy_cfg_t& config,
                          uint32_t           cc_idx    = 0,
                          uint32_t           earfcn    = 0,
                          srslte_cell_t*     cell_info = nullptr)                             = 0;
  virtual void set_config_tdd(srslte_tdd_config_t& tdd_config)                            = 0;
  virtual void set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) = 0;
  virtual void set_config_mbsfn_sib13(const srslte::sib13_t& sib13)                       = 0;
  virtual void set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch)                      = 0;

  /* Measurements interface */
  virtual void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) = 0;
  virtual void meas_stop()                                                       = 0;

  typedef struct {
    enum { CELL_FOUND = 0, CELL_NOT_FOUND, ERROR } found;
    enum { MORE_FREQS = 0, NO_MORE_FREQS } last_freq;
  } cell_search_ret_t;

  typedef struct {
    uint32_t pci;
    uint32_t earfcn;
    float    cfo_hz;
  } phy_cell_t;

  /* Cell search and selection procedures */
  virtual cell_search_ret_t cell_search(phy_cell_t* cell)                 = 0;
  virtual bool              cell_select(const phy_cell_t* cell = nullptr) = 0;
  virtual bool              cell_is_camping()                             = 0;

  virtual void reset() = 0;

  virtual void enable_pregen_signals(bool enable) = 0;
};

// STACK interface for GW
class stack_interface_gw : public pdcp_interface_gw
{
public:
  virtual bool switch_on() = 0;
};

class gw_interface_stack : public gw_interface_nas, public gw_interface_rrc, public gw_interface_pdcp
{
};

// STACK interface for RRC
class stack_interface_rrc : public srslte::task_handler_interface
{
public:
  virtual void              start_cell_search()                                              = 0;
  virtual void              start_cell_select(const phy_interface_rrc_lte::phy_cell_t* cell) = 0;
  virtual srslte::tti_point get_current_tti()                                                = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_lte : public mac_interface_phy_lte, public rrc_interface_phy_lte
{
public:
  /* Indicate new TTI */
  virtual void run_tti(const uint32_t tti, const uint32_t tti_jump) = 0;
};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_lte : public phy_interface_mac_lte, public phy_interface_rrc_lte
{
};

} // namespace srsue

#endif // SRSLTE_UE_INTERFACES_H
