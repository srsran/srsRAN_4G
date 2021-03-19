/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/asn1/s1ap_utils.h"
#include "srsran/interfaces/enb_rrc_interface_types.h"
#include "srsran/interfaces/sched_interface.h"

#ifndef SRSRAN_ENB_RRC_INTERFACES_H
#define SRSRAN_ENB_RRC_INTERFACES_H

namespace srsenb {

// RRC interface for S1AP
class rrc_interface_s1ap
{
public:
  virtual void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu)                                      = 0;
  virtual void release_complete(uint16_t rnti)                                                   = 0;
  virtual bool setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) = 0;
  virtual bool modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)    = 0;
  virtual bool setup_ue_erabs(uint16_t rnti, const asn1::s1ap::erab_setup_request_s& msg)        = 0;
  virtual void modify_erabs(uint16_t                                 rnti,
                            const asn1::s1ap::erab_modify_request_s& msg,
                            std::vector<uint16_t>*                   erabs_modified,
                            std::vector<uint16_t>*                   erabs_failed_to_modify)                       = 0;
  virtual bool release_erabs(uint32_t rnti)                                                      = 0;
  virtual void release_erabs(uint32_t                              rnti,
                             const asn1::s1ap::erab_release_cmd_s& msg,
                             std::vector<uint16_t>*                erabs_released,
                             std::vector<uint16_t>*                erabs_failed_to_release)                     = 0;
  virtual void add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id)      = 0;

  /**
   * Reports the reception of S1 HandoverCommand / HandoverPreparationFailure or abnormal conditions during
   * S1 Handover preparation back to RRC.
   *
   * @param rnti user
   * @param is_success true if ho cmd was received
   * @param container TargeteNB RRCConnectionReconfiguration message with MobilityControlInfo
   */
  virtual void ho_preparation_complete(uint16_t                     rnti,
                                       bool                         is_success,
                                       const asn1::s1ap::ho_cmd_s&  msg,
                                       srsran::unique_byte_buffer_t container) = 0;
  virtual uint16_t
               start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                          const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container) = 0;
  virtual void set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs) = 0;
};

/// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void read_pdu_pcch(uint8_t* payload, uint32_t payload_size)                    = 0;
  virtual void max_retx_attempted(uint16_t rnti)                                         = 0;
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
};

/// RRC interface for MAC
class rrc_interface_mac
{
public:
  /* Radio Link failure */
  virtual int  add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) = 0;
  virtual void upd_user(uint16_t new_rnti, uint16_t old_rnti)                        = 0;
  virtual void set_activity_user(uint16_t rnti)                                      = 0;
  virtual bool is_paging_opportunity(uint32_t tti, uint32_t* payload_len)            = 0;

  ///< Provide packed SIB to MAC (buffer is managed by RRC)
  virtual uint8_t* read_pdu_bcch_dlsch(const uint8_t enb_cc_idx, const uint32_t sib_index) = 0;
};

/// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACES_H
