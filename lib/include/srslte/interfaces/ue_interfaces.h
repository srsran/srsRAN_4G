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

/******************************************************************************
 * File:        interfaces.h
 * Description: Abstract base class interfaces provided by layers
 *              to other layers.
 *****************************************************************************/

#ifndef INTERFACES_H
#define INTERFACES_H

#include <string>

#include "srslte/asn1/liblte_rrc.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/common.h"
#include "srslte/common/security.h"

namespace srsue {

// UE interface
class ue_interface
{
};

// USIM interface for NAS
class usim_interface_nas
{
public:
  virtual void get_imsi_vec(uint8_t* imsi_, uint32_t n) = 0;
  virtual void get_imei_vec(uint8_t* imei_, uint32_t n) = 0;
  virtual void generate_authentication_response(uint8_t  *rand,
                                                uint8_t  *autn_enb,
                                                uint16_t  mcc,
                                                uint16_t  mnc,
                                                bool     *net_valid,
                                                uint8_t  *res) = 0;
  virtual void generate_nas_keys(uint8_t *k_nas_enc,
                                 uint8_t *k_nas_int,
                                 srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                 srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// USIM interface for RRC
class usim_interface_rrc
{
public:
  virtual void generate_as_keys(uint32_t count_ul,
                                uint8_t *k_rrc_enc,
                                uint8_t *k_rrc_int,
                                uint8_t *k_up_enc,
                                uint8_t *k_up_int,
                                srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// GW interface for NAS
class gw_interface_nas
{
public:
  virtual srslte::error_t setup_if_addr(uint32_t ip_addr, char *err_str) = 0;
};

// GW interface for PDCP
class gw_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
};

// NAS interface for RRC
class nas_interface_rrc
{
public:
  virtual bool      is_attached() = 0;
  virtual void      notify_connection_setup() = 0;
  virtual void      write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
  virtual uint32_t  get_ul_count() = 0;
  virtual bool      get_s_tmsi(LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi) = 0;
};

// RRC interface for MAC
class rrc_interface_mac
{
public:
  virtual void release_pucch_srs() = 0;
  virtual void ra_problem() = 0; 
};

// RRC interface for PHY
class rrc_interface_phy
{
public:
  virtual void in_sync() = 0;
  virtual void out_of_sync() = 0;
};

// RRC interface for NAS
class rrc_interface_nas
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual uint16_t get_mcc() = 0;
  virtual uint16_t get_mnc() = 0;
  virtual void enable_capabilities() = 0;
};

// RRC interface for GW
class rrc_interface_gw
{
public:
  virtual bool rrc_connected() = 0;
  virtual void rrc_connect() = 0; 
  virtual bool have_drb() = 0;
};

// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_bcch_bch(srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_bcch_dlsch(srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_pcch(srslte::byte_buffer_t *pdu) = 0;
};

// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void max_retx_attempted() = 0;
};

// PDCP interface for GW
class pdcp_interface_gw
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reset() = 0;
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual void add_bearer(uint32_t lcid, LIBLTE_RRC_PDCP_CONFIG_STRUCT *cnfg=NULL) = 0;
  virtual void config_security(uint32_t lcid,
                               uint8_t *k_rrc_enc_,
                               uint8_t *k_rrc_int_,
                               srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                               srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_) = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_bcch_bch(srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_bcch_dlsch(srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_pcch(srslte::byte_buffer_t *sdu) = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void reset() = 0;
  virtual void add_bearer(uint32_t lcid) = 0;
  virtual void add_bearer(uint32_t lcid, LIBLTE_RRC_RLC_CONFIG_STRUCT *cnfg) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint32_t lcid,  srslte::byte_buffer_t *sdu) = 0;
};

//RLC interface for MAC
class rlc_interface_mac : public srslte::read_pdu_interface
{
public:
  /* MAC calls RLC to get buffer state for a logical channel.
   * This function should return quickly. */
  virtual uint32_t get_buffer_state(uint32_t lcid) = 0;
  virtual uint32_t get_total_buffer_state(uint32_t lcid) = 0; 


  const static int MAX_PDU_SEGMENTS = 20;

  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int     read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes) = 0;
};



/** MAC interface 
 *
 */
/* Interface PHY -> MAC */
class mac_interface_phy
{
public:
    
  typedef struct {
    uint32_t    pid;    
    uint32_t    tti;
    uint32_t    last_tti;
    bool        ndi; 
    bool        last_ndi; 
    uint32_t    n_bytes;
    int         rv; 
    uint16_t    rnti; 
    bool        is_from_rar;
    bool        is_sps_release;
    bool        has_cqi_request;
    srslte_rnti_type_t rnti_type; 
    srslte_phy_grant_t phy_grant; 
  } mac_grant_t; 
  
  typedef struct {
    bool                    decode_enabled;
    int                     rv;
    uint16_t                rnti; 
    bool                    generate_ack; 
    bool                    default_ack; 
    // If non-null, called after tb_decoded_ok to determine if ack needs to be sent
    bool                  (*generate_ack_callback)(void*); 
    void                   *generate_ack_callback_arg;
    uint8_t                *payload_ptr; 
    srslte_softbuffer_rx_t *softbuffer;
    srslte_phy_grant_t      phy_grant;
  } tb_action_dl_t;

  typedef struct {
    bool                    tx_enabled;
    bool                    expect_ack;
    uint32_t                rv;
    uint16_t                rnti; 
    uint32_t                current_tx_nb;
    srslte_softbuffer_tx_t *softbuffer;
    srslte_phy_grant_t      phy_grant;
    uint8_t                *payload_ptr; 
  } tb_action_ul_t;
  
  /* Indicate reception of UL grant. 
   * payload_ptr points to memory where MAC PDU must be written by MAC layer */
  virtual void new_grant_ul(mac_grant_t grant, tb_action_ul_t *action) = 0;

  /* Indicate reception of UL grant + HARQ information throught PHICH in the same TTI. */
  virtual void new_grant_ul_ack(mac_grant_t grant, bool ack, tb_action_ul_t *action) = 0;

  /* Indicate reception of HARQ information only through PHICH.   */
  virtual void harq_recv(uint32_t tti, bool ack, tb_action_ul_t *action) = 0;
  
  /* Indicate reception of DL grant. */ 
  virtual void new_grant_dl(mac_grant_t grant, tb_action_dl_t *action) = 0;
  
  /* Indicate successfull decoding of PDSCH TB. */
  virtual void tb_decoded(bool ack, srslte_rnti_type_t rnti_type, uint32_t harq_pid) = 0;
  
  /* Indicate successfull decoding of BCH TB through PBCH */
  virtual void bch_decoded_ok(uint8_t *payload, uint32_t len) = 0;  
  
  /* Indicate successfull decoding of PCH TB through PDSCH */
  virtual void pch_decoded_ok(uint32_t len) = 0;  
  
  /* Function called every start of a subframe (TTI). Warning, this function is called 
   * from a high priority thread and should terminate asap 
   */
  virtual void tti_clock(uint32_t tti) = 0;
  
};


/* Interface RRC -> MAC */
class mac_interface_rrc
{
public:
  
  typedef struct {
    LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT           main; 
    LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT        rach;     
    LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT sr; 
    uint32_t prach_config_index; 
  } mac_cfg_t; 

  
  // Class to handle UE specific RNTIs between RRC and MAC
  typedef struct {
    uint16_t crnti; 
    uint16_t temp_rnti; 
    uint16_t tpc_rnti; 
    uint16_t sps_rnti; 
    uint64_t contention_id; 
  } ue_rnti_t;  
  
  /* Instructs the MAC to start receiving BCCH */
  virtual void    bcch_start_rx() = 0; 
  virtual void    bcch_stop_rx() = 0; 
  virtual void    bcch_start_rx(int si_window_start, int si_window_length) = 0;

  /* Instructs the MAC to start receiving PCCH */
  virtual void    pcch_start_rx() = 0; 
  virtual void    pcch_stop_rx() = 0; 
  
  /* RRC configures a logical channel */
  virtual void    setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) = 0;

  virtual uint32_t get_current_tti() = 0;
  
  virtual void set_config(mac_cfg_t *mac_cfg) = 0;
  virtual void set_config_main(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT *main_cfg) = 0;
  virtual void set_config_rach(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT *rach_cfg, uint32_t prach_config_index) = 0;
  virtual void set_config_sr(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT *sr_cfg) = 0;
  virtual void get_config(mac_cfg_t *mac_cfg) = 0;
  
  virtual void get_rntis(ue_rnti_t *rntis) = 0;
  virtual void set_contention_id(uint64_t uecri) = 0;

  
  virtual void reconfiguration() = 0;
  virtual void reset() = 0;
};




/** PHY interface 
 *
 */

typedef struct {
  bool ul_pwr_ctrl_en; 
  float prach_gain;
  int pdsch_max_its;
  bool attach_enable_64qam; 
  int nof_phy_threads;
  
  int worker_cpu_mask;
  int sync_cpu_affinity;
  
  uint32_t nof_rx_ant;   
  std::string equalizer_mode; 
  int cqi_max; 
  int cqi_fixed; 
  float snr_ema_coeff; 
  std::string snr_estim_alg; 
  bool cfo_integer_enabled; 
  float cfo_correct_tol_hz; 
  int time_correct_period; 
  bool sfo_correct_disable; 
  std::string sss_algorithm; 
  float estimator_fil_w;   
  bool rssi_sensor_enabled;
} phy_args_t; 
  
/* Interface MAC -> PHY */
class phy_interface_mac
{
public:
  /* Configure PRACH using parameters written by RRC */
  virtual void configure_prach_params() = 0;
  
  /* Start synchronization with strongest cell in the current carrier frequency */
  virtual void sync_start() = 0; 
  virtual void sync_stop() = 0;
  
  /* Sets a C-RNTI allowing the PHY to pregenerate signals if necessary */
  virtual void set_crnti(uint16_t rnti) = 0; 
  
  virtual void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm) = 0;  
  virtual int  prach_tx_tti() = 0; 
  
  /* Indicates the transmission of a SR signal in the next opportunity */
  virtual void sr_send() = 0;  
  virtual int  sr_last_tx_tti() = 0; 
  
  /* Time advance commands */
  virtual void set_timeadv_rar(uint32_t ta_cmd) = 0;
  virtual void set_timeadv(uint32_t ta_cmd) = 0;
  
  /* Sets RAR grant payload */
  virtual void set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN]) = 0; 

  /* Instruct the PHY to decode PDCCH with the CRC scrambled with given RNTI */
  virtual void pdcch_ul_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1) = 0;
  virtual void pdcch_dl_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1) = 0;
  virtual void pdcch_ul_search_reset() = 0;
  virtual void pdcch_dl_search_reset() = 0;
  
  virtual uint32_t get_current_tti() = 0;
  
  virtual float get_phr() = 0; 
  virtual float get_pathloss_db() = 0;
    
};

class phy_interface_rrc
{
public:

  typedef struct {
    LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT          prach_cnfg;
    LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT       pdsch_cnfg;
    LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT       pusch_cnfg;
    LIBLTE_RRC_PHICH_CONFIG_STRUCT              phich_cnfg;
    LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT       pucch_cnfg;
    LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT      srs_ul_cnfg;
    LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT   ul_pwr_ctrl;
    LIBLTE_RRC_TDD_CONFIG_STRUCT                tdd_cnfg;
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM         ant_info;      
  } phy_cfg_common_t; 
  
  typedef struct {
    LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT dedicated;
    phy_cfg_common_t                            common; 
    bool                                        enable_64qam; 
  } phy_cfg_t; 

  virtual void get_current_cell(srslte_cell_t *cell) = 0;
  virtual void get_config(phy_cfg_t *phy_cfg) = 0; 
  virtual void set_config(phy_cfg_t *phy_cfg) = 0; 
  virtual void set_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *dedicated) = 0;
  virtual void set_config_common(phy_cfg_common_t *common) = 0; 
  virtual void set_config_tdd(LIBLTE_RRC_TDD_CONFIG_STRUCT *tdd) = 0; 
  virtual void set_config_64qam_en(bool enable) = 0;
  
  /* Is the PHY downlink synchronized? */
  virtual bool status_is_sync() = 0;

  /* Configure UL using parameters written with set_param() */
  virtual void configure_ul_params(bool pregen_disabled = false) = 0;

  virtual void reset() = 0;
  
  virtual void resync_sfn() = 0;   

};
  

} // namespace srslte

#endif // INTERFACES_H
