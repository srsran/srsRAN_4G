/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srslte/asn1/asn1_utils.h"
#include "srslte/asn1/liblte_mme.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/security.h"
#include "srslte/common/stack_procedure.h"
#include "srslte/common/tti_point.h"
#include "srslte/phy/channel/channel.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/upper/pdcp_entity_base.h"
#include "ue_mac_interfaces.h"

namespace srsue {

// RRC interface for MAC
class rrc_interface_mac_common
{
public:
  virtual void ra_problem() = 0;
};

class rrc_interface_mac : public rrc_interface_mac_common
{
public:
  virtual void ra_completed()      = 0;
  virtual void release_pucch_srs() = 0;
};

struct phy_cell_t {
  uint32_t pci;
  uint32_t earfcn;
  float    cfo_hz;
};

// Measurement object from phy
typedef struct {
  float    rsrp;
  float    rsrq;
  float    cfo_hz;
  uint32_t earfcn;
  uint32_t pci;
} phy_meas_t;

typedef struct {
  float    rsrp;
  float    rsrq;
  float    sinr;
  float    cfo_hz;
  uint32_t arfcn_nr;
  uint32_t pci_nr;
} phy_meas_nr_t;

// RRC interface for RRC NR
class rrc_eutra_interface_rrc_nr
{
public:
  virtual void new_cell_meas_nr(const std::vector<phy_meas_nr_t>& meas) = 0;
  virtual void nr_rrc_con_reconfig_complete(bool status)                = 0;
};

// RRC interface for PHY
class rrc_interface_phy_lte
{
public:
  virtual void in_sync()                                          = 0;
  virtual void out_of_sync()                                      = 0;
  virtual void new_cell_meas(const std::vector<phy_meas_t>& meas) = 0;

  typedef struct {
    enum { CELL_FOUND = 0, CELL_NOT_FOUND, ERROR } found;
    enum { MORE_FREQS = 0, NO_MORE_FREQS } last_freq;
  } cell_search_ret_t;

  virtual void cell_search_complete(cell_search_ret_t ret, phy_cell_t found_cell) = 0;
  virtual void cell_select_complete(bool status)                                  = 0;
  virtual void set_config_complete(bool status)                                   = 0;
  virtual void set_scell_complete(bool status)                                    = 0;
};

// RRC interface for NAS
class rrc_interface_nas
{
public:
  virtual ~rrc_interface_nas()                                                          = default;
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
  virtual bool        has_nr_dc()                                                       = 0;
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

// RRC NR interface for RRC (LTE)
class rrc_nr_interface_rrc
{
public:
  virtual void get_eutra_nr_capabilities(srslte::byte_buffer_t* eutra_nr_caps)   = 0;
  virtual void get_nr_capabilities(srslte::byte_buffer_t* nr_cap)                = 0;
  virtual void phy_set_cells_to_meas(uint32_t carrier_freq_r15)                  = 0;
  virtual void phy_meas_stop()                                                   = 0;
  virtual bool rrc_reconfiguration(bool                endc_release_and_add_r15,
                                   bool                nr_secondary_cell_group_cfg_r15_present,
                                   asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                   bool                sk_counter_r15_present,
                                   uint32_t            sk_counter_r15,
                                   bool                nr_radio_bearer_cfg1_r15_present,
                                   asn1::dyn_octstring nr_radio_bearer_cfg1_r15) = 0;
  virtual bool is_config_pending()                                               = 0;
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

  std::string           dl_nr_arfcn      = "632628"; // comma-separated list of DL NR ARFCNs
  std::vector<uint32_t> dl_nr_arfcn_list = {
      632628}; // vectorized version of dl_nr_arfcn that gets populated during init

  float dl_freq = -1.0f;
  float ul_freq = -1.0f;

  bool     ul_pwr_ctrl_en  = false;
  float    prach_gain      = -1;
  uint32_t pdsch_max_its   = 8;
  bool     meas_evm        = false;
  uint32_t nof_phy_threads = 3;

  int worker_cpu_mask   = -1;
  int sync_cpu_affinity = -1;

  uint32_t    nof_lte_carriers             = 1;
  uint32_t    nof_nr_carriers              = 0;
  uint32_t    nr_nof_prb                   = 50;
  double      nr_freq_hz                   = 2630e6;
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

  srslte::vnf_args_t vnf_args;
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
  virtual void set_activation_deactivation_scell(uint32_t cmd, uint32_t tti) = 0;

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
  virtual bool set_config(srslte::phy_cfg_t config, uint32_t cc_idx = 0)                  = 0;
  virtual bool set_scell(srslte_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn)       = 0;
  virtual void set_config_tdd(srslte_tdd_config_t& tdd_config)                            = 0;
  virtual void set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) = 0;
  virtual void set_config_mbsfn_sib13(const srslte::sib13_t& sib13)                       = 0;
  virtual void set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch)                      = 0;

  virtual void deactivate_scells() = 0;

  /* Measurements interface */
  virtual void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) = 0;
  virtual void meas_stop()                                                       = 0;

  /* Cell search and selection procedures */
  virtual bool cell_search()                = 0;
  virtual bool cell_select(phy_cell_t cell) = 0;
  virtual bool cell_is_camping()            = 0;

  virtual void enable_pregen_signals(bool enable) = 0;
};

// STACK interface for RRC
class stack_interface_rrc
{
public:
  virtual srslte::tti_point get_current_tti() = 0;
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
{};

} // namespace srsue

#endif // SRSLTE_UE_INTERFACES_H
