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

#include "srslte/srslte.h"

#include "pdcp_interface_types.h"
#include "rlc_interface_types.h"
#include "rrc_interface_types.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/asn1/s1ap_asn1.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/security.h"
#include "srslte/interfaces/sched_interface.h"
#include <vector>

#ifndef SRSLTE_ENB_INTERFACES_H
#define SRSLTE_ENB_INTERFACES_H

namespace srsenb {

/* Interface PHY -> MAC */
class mac_interface_phy_lte
{
public:
  const static int MAX_GRANTS = 64;

  /**
   * DL grant structure per UE
   */
  typedef struct {
    srslte_dci_dl_t         dci;
    uint8_t*                data[SRSLTE_MAX_TB];
    srslte_softbuffer_tx_t* softbuffer_tx[SRSLTE_MAX_TB];
  } dl_sched_grant_t;

  /**
   * DL Scheduling result per cell/carrier
   */
  typedef struct {
    dl_sched_grant_t pdsch[MAX_GRANTS]; //< DL Grants
    uint32_t         nof_grants;        //< Number of DL grants
    uint32_t         cfi;               //< Current CFI of the cell, it can vary across cells
  } dl_sched_t;

  /**
   * List of DL scheduling results, one entry per cell/carrier
   */
  typedef std::vector<dl_sched_t> dl_sched_list_t;

  typedef struct {
    uint16_t rnti;
    bool     ack;
  } ul_sched_ack_t;

  /**
   * UL grant information per UE
   */
  typedef struct {
    srslte_dci_ul_t         dci;
    uint32_t                current_tx_nb;
    uint8_t*                data;
    bool                    needs_pdcch;
    srslte_softbuffer_rx_t* softbuffer_rx;
  } ul_sched_grant_t;

  /**
   * UL Scheduling result per cell/carrier
   */
  typedef struct {
    ul_sched_grant_t pusch[MAX_GRANTS];
    ul_sched_ack_t   phich[MAX_GRANTS];
    uint32_t         nof_grants;
    uint32_t         nof_phich;
  } ul_sched_t;

  /**
   * List of UL scheduling results, one entry per cell/carrier
   */
  typedef std::vector<ul_sched_t> ul_sched_list_t;

  virtual int  sr_detected(uint32_t tti, uint16_t rnti)                                                       = 0;
  virtual void rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) = 0;

  /**
   * PHY callback for giving MAC the Rank Indicator information of a given RNTI for an eNb cell/carrier.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param ri_value the actual Rank Indicator value, 0 for 1 layer, 1 for two layers and so on.
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value) = 0;

  /**
   * PHY callback for giving MAC the Pre-coding Matrix Indicator information of a given RNTI for an eNb cell/carrier.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param pmi_value the actual PMI value
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value) = 0;

  /**
   * PHY callback for for giving MAC the Channel Quality information of a given RNTI, TTI and eNb cell/carrier
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the measurement corresponds
   * @param cqi_value the corresponding Channel Quality Information
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value) = 0;

  /**
   * PHY callback for giving MAC the SNR in dB of an UL transmission for a given RNTI at a given carrier
   *
   * @param tti The measurement was made
   * @param rnti The UE identifier in the eNb
   * @param cc_idx The eNb Cell/Carrier where the UL transmission was received
   * @param snr_db The actual SNR of the received signal
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int snr_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, float snr_db) = 0;

  /**
   * PHY callback for giving MAC the Time Aligment information in microseconds of a given RNTI during a TTI processing
   *
   * @param tti The measurement was made
   * @param rnti The UE identifier in the eNb
   * @param ta_us The actual time alignment in microseconds
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int ta_info(uint32_t tti, uint16_t rnti, float ta_us) = 0;

  /**
   * PHY callback for giving MAC the HARQ DL ACK/NACK feedback information for a given RNTI, TTI, eNb cell/carrier and
   * Transport block.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx the eNb Cell/Carrier identifier
   * @param tb_idx the transport block index
   * @param ack true for ACK, false for NACK, do not call for DTX
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int ack_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t tb_idx, bool ack) = 0;

  /**
   * Informs MAC about a received PUSCH transmission for given RNTI, TTI and eNb Cell/carrier.
   *
   * @param tti the given TTI
   * @param rnti the UE identifier in the eNb
   * @param cc_idx the eNb Cell/Carrier identifier
   * @param nof_bytes the number of grants carrierd by the PUSCH message
   * @param crc_res the CRC check, set to true if the message was decoded succesfully
   * @return SRSLTE_SUCCESS if no error occurs, SRSLTE_ERROR* if an error occurs
   */
  virtual int crc_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t nof_bytes, bool crc_res) = 0;

  virtual int  get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res)                = 0;
  virtual int  get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) = 0;
  virtual int  get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res)                = 0;
  virtual void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs)               = 0;

  // Radio-Link status
  virtual void rl_failure(uint16_t rnti) = 0;
  virtual void rl_ok(uint16_t rnti)      = 0;
};

/* Interface MAC -> PHY */
class phy_interface_mac_lte
{
public:
  /**
   * Interface for MAC to add or modify user in the active UE database setting. This function requires a primary cell
   * (PCell) index and a list of secondary cells (SCell) for the UE. The elements in the list SCell list must follow the
   * UE's SCell indexes order.
   *
   * @param rnti identifier of the user
   * @param pcell_index Primary cell (PCell) index
   * @param is_temporal Indicates whether the UE is temporal
   */
  virtual int add_rnti(uint16_t rnti, uint32_t pcell_index, bool is_temporal) = 0;

  /**
   * Removes an RNTI context from all the physical layer components, including secondary cells
   * @param rnti identifier of the user
   */
  virtual void rem_rnti(uint16_t rnti) = 0;

  /**
   *
   * @param stop
   */
  virtual void set_mch_period_stop(uint32_t stop) = 0;

  /**
   * Activates and/or deactivates Secondary Cells in the PHY for a given RNTI. Requires the RNTI of the given UE and a
   * vector with the activation/deactivation values. Use true for activation and false for deactivation. The index 0 is
   * reserved for PCell and will not be used.
   *
   * @param rnti identifier of the user
   * @param activation vector with the activate/deactivate.
   */
  virtual void set_activation_deactivation_scell(uint16_t                                     rnti,
                                                 const std::array<bool, SRSLTE_MAX_CARRIERS>& activation) = 0;
};

/* Interface RRC -> PHY */
class phy_interface_rrc_lte
{
public:
  struct phy_cfg_mbsfn_t {
    asn1::rrc::mbsfn_sf_cfg_s       mbsfn_subfr_cnfg;
    asn1::rrc::mbms_notif_cfg_r9_s  mbsfn_notification_cnfg;
    asn1::rrc::mbsfn_area_info_r9_s mbsfn_area_info;
    asn1::rrc::mcch_msg_s           mcch;
  };

  typedef struct {
    phy_cfg_mbsfn_t mbsfn;
  } phy_rrc_cfg_t;

  virtual void configure_mbsfn(asn1::rrc::sib_type2_s*      sib2,
                               asn1::rrc::sib_type13_r9_s*  sib13,
                               const asn1::rrc::mcch_msg_s& mcch) = 0;

  typedef struct {
    bool              configured = false; ///< Indicates whether PHY shall consider configuring this cell/carrier
    uint32_t          enb_cc_idx = 0;     ///< eNb Cell index
    srslte::phy_cfg_t phy_cfg    = {};    ///< Dedicated physical layer configuration
  } phy_rrc_dedicated_t;

  typedef std::vector<phy_rrc_dedicated_t> phy_rrc_dedicated_list_t;

  /**
   * Sets the physical layer dedicated configuration for a given RNTI, a cell index and a secondary cell index.
   * The cc_idx indicates the eNb cell to configure and the scell_idx is the UE's cell index
   *
   * @param rnti the given RNTI
   * @param dedicated_list Physical layer configuration for the indicated eNb cell
   */
  virtual void set_config_dedicated(uint16_t rnti, const phy_rrc_dedicated_list_t& dedicated_list) = 0;

  /**
   * Instructs the physical layer the configuration has been complete from upper layers for a given RNTI
   *
   * @param rnti the given UE identifier (RNTI)
   */
  virtual void complete_config_dedicated(uint16_t rnti) = 0;
};

class mac_interface_rrc
{
public:
  /* Provides cell configuration including SIB periodicity, etc. */
  virtual int  cell_cfg(const std::vector<sched_interface::cell_cfg_t>& cell_cfg) = 0;
  virtual void reset()                                                            = 0;

  /* Manages UE configuration context */
  virtual int ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t* cfg) = 0;
  virtual int ue_rem(uint16_t rnti)                                 = 0;

  /**
   * Called after Msg3 reception to set the UE C-RNTI, resolve contention, and alter the UE's configuration in the
   * scheduler and phy.
   *
   * @param temp_crnti temporary C-RNTI of the UE
   * @param crnti chosen C-RNTI for the UE
   * @param cfg new UE scheduler configuration
   */
  virtual int ue_set_crnti(uint16_t temp_crnti, uint16_t crnti, sched_interface::ue_cfg_t* cfg) = 0;

  /* Manages UE bearers and associated configuration */
  virtual int  bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg) = 0;
  virtual int  bearer_ue_rem(uint16_t rnti, uint32_t lc_id)                                        = 0;
  virtual void phy_config_enabled(uint16_t rnti, bool enabled)                                     = 0;
  virtual void
  write_mcch(asn1::rrc::sib_type2_s* sib2, asn1::rrc::sib_type13_r9_s* sib13, asn1::rrc::mcch_msg_s* mcch) = 0;

  /**
   * Allocate a C-RNTI for a new user, without adding it to the phy layer and scheduler yet
   * @return value of the allocated C-RNTI
   */
  virtual uint16_t allocate_rnti() = 0;
};

class mac_interface_rlc
{
public:
  virtual int rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue) = 0;
};

// RLC interface for MAC
class rlc_interface_mac
{
public:
  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;

  virtual void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
  virtual void discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t sn)                    = 0;
  virtual bool rb_is_um(uint16_t rnti, uint32_t lcid)                                    = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void clear_buffer(uint16_t rnti)                                               = 0;
  virtual void add_user(uint16_t rnti)                                                   = 0;
  virtual void rem_user(uint16_t rnti)                                                   = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg)       = 0;
  virtual void add_bearer_mrb(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
  virtual bool has_bearer(uint16_t rnti, uint32_t lcid)                                  = 0;
  virtual bool suspend_bearer(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual bool resume_bearer(uint16_t rnti, uint32_t lcid)                               = 0;
};

// PDCP interface for GTPU
class pdcp_interface_gtpu
{
public:
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reset(uint16_t rnti)                                                                = 0;
  virtual void add_user(uint16_t rnti)                                                             = 0;
  virtual void rem_user(uint16_t rnti)                                                             = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)           = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cnfg)                = 0;
  virtual void config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg) = 0;
  virtual void enable_integrity(uint16_t rnti, uint32_t lcid)                                      = 0;
  virtual void enable_encryption(uint16_t rnti, uint32_t lcid)                                     = 0;
  virtual bool
  get_bearer_status(uint16_t rnti, uint32_t lcid, uint16_t* dlsn, uint16_t* dlhfn, uint16_t* ulsn, uint16_t* ulhfn) = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
};

// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void read_pdu_pcch(uint8_t* payload, uint32_t payload_size)                    = 0;
  virtual void max_retx_attempted(uint16_t rnti)                                         = 0;
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) = 0;
};

// RRC interface for MAC
class rrc_interface_mac
{
public:
  /* Radio Link failure */
  virtual void rl_failure(uint16_t rnti)                                             = 0;
  virtual void add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) = 0;
  virtual void upd_user(uint16_t new_rnti, uint16_t old_rnti)                        = 0;
  virtual void set_activity_user(uint16_t rnti)                                      = 0;
  virtual bool is_paging_opportunity(uint32_t tti, uint32_t* payload_len)            = 0;

  ///< Provide packed SIB to MAC (buffer is managed by RRC)
  virtual uint8_t* read_pdu_bcch_dlsch(const uint8_t enb_cc_idx, const uint32_t sib_index) = 0;
};

// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
};

// RRC interface for S1AP
class rrc_interface_s1ap
{
public:
  virtual void write_dl_info(uint16_t rnti, srslte::unique_byte_buffer_t sdu)                    = 0;
  virtual void release_complete(uint16_t rnti)                                                   = 0;
  virtual bool setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) = 0;
  virtual bool modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)    = 0;
  virtual bool setup_ue_erabs(uint16_t rnti, const asn1::s1ap::erab_setup_request_s& msg)        = 0;
  virtual bool release_erabs(uint32_t rnti)                                                      = 0;
  virtual void add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id)      = 0;

  /**
   * Reports the reception of S1 HandoverCommand / HandoverPreparationFailure or abnormal conditions during
   * S1 Handover preparation back to RRC.
   *
   * @param rnti user
   * @param is_success true if ho cmd was received
   * @param container TargeteNB RRCConnectionReconfiguration message with MobilityControlInfo
   */
  virtual void ho_preparation_complete(uint16_t rnti, bool is_success, srslte::unique_byte_buffer_t container) = 0;
};

// GTPU interface for PDCP
class gtpu_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
};

// GTPU interface for RRC
class gtpu_interface_rrc
{
public:
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, uint32_t* teid_in) = 0;
  virtual void rem_bearer(uint16_t rnti, uint32_t lcid)                                                      = 0;
  virtual void rem_user(uint16_t rnti)                                                                       = 0;
};

// S1AP interface for RRC
class s1ap_interface_rrc
{
public:
  struct bearer_status_info {
    uint8_t  erab_id;
    uint16_t pdcp_dl_sn, pdcp_ul_sn;
    uint16_t dl_hfn, ul_hfn;
  };

  virtual void
               initial_ue(uint16_t rnti, asn1::s1ap::rrc_establishment_cause_e cause, srslte::unique_byte_buffer_t pdu) = 0;
  virtual void initial_ue(uint16_t                              rnti,
                          asn1::s1ap::rrc_establishment_cause_e cause,
                          srslte::unique_byte_buffer_t          pdu,
                          uint32_t                              m_tmsi,
                          uint8_t                               mmec) = 0;

  virtual void write_pdu(uint16_t rnti, srslte::unique_byte_buffer_t pdu)                              = 0;
  virtual bool user_exists(uint16_t rnti)                                                              = 0;
  virtual bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio)              = 0;
  virtual void ue_ctxt_setup_complete(uint16_t rnti, const asn1::s1ap::init_context_setup_resp_s& res) = 0;
  virtual void ue_erab_setup_complete(uint16_t rnti, const asn1::s1ap::erab_setup_resp_s& res)         = 0;
  virtual bool is_mme_connected()                                                                      = 0;

  /**
   * Command the s1ap to transmit a HandoverRequired message to MME.
   * This message initiates the S1 handover preparation procedure at the Source eNB
   *
   * @param rnti user to perform S1 handover
   * @param target_eci eNB Id + Cell Id of the target eNB
   * @param target_plmn PLMN of the target eNB
   * @param rrc_container RRC container with SourceENBToTargetENBTransparentContainer message.
   * @return true if successful
   */
  virtual bool send_ho_required(uint16_t                     rnti,
                                uint32_t                     target_eci,
                                srslte::plmn_id_t            target_plmn,
                                srslte::unique_byte_buffer_t rrc_container) = 0;

  /**
   * Command the s1ap to transmit eNBStatusTransfer message to MME. This message passes the PDCP context of the UE
   * performing S1 handover from source eNB to target eNB.
   *
   * @param rnti user to perform S1 handover
   * @param bearer_status_list PDCP SN and HFN status of the bearers to be preserved at target eNB
   * @return true if successful
   */
  virtual bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) = 0;
};

// Combined interface for PHY to access stack (MAC and RRC)
class stack_interface_phy_lte : public mac_interface_phy_lte
{
public:
  virtual void tti_clock() = 0;
};

// Combined interface for stack (MAC and RRC) to access PHY
class phy_interface_stack_lte : public phy_interface_mac_lte, public phy_interface_rrc_lte
{};

typedef struct {
  uint32_t    enb_id;  // 20-bit id (lsb bits)
  uint8_t     cell_id; // 8-bit cell id
  uint16_t    tac;     // 16-bit tac
  uint16_t    mcc;     // BCD-coded with 0xF filler
  uint16_t    mnc;     // BCD-coded with 0xF filler
  std::string mme_addr;
  std::string gtp_bind_addr;
  std::string s1c_bind_addr;
  std::string enb_name;
} s1ap_args_t;

typedef struct {
  uint32_t                      nof_prb; ///< Needed to dimension MAC softbuffers for all cells
  sched_interface::sched_args_t sched;
  int                           link_failure_nof_err;
} mac_args_t;

class stack_interface_s1ap_lte
{
public:
  virtual void add_mme_socket(int fd)    = 0;
  virtual void remove_mme_socket(int fd) = 0;
};

class stack_interface_gtpu_lte
{
public:
  virtual void add_gtpu_s1u_socket_handler(int fd) = 0;
  virtual void add_gtpu_m1u_socket_handler(int fd) = 0;
};

// STACK interface for MAC
class stack_interface_mac_lte : public srslte::task_handler_interface
{};

} // namespace srsenb

#endif // SRSLTE_ENB_INTERFACES_H
