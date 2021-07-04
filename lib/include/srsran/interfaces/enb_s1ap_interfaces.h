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

#include "srsran/asn1/s1ap_utils.h"
#include "srsran/interfaces/rrc_interface_types.h"

#ifndef SRSRAN_ENB_S1AP_INTERFACES_H
#define SRSRAN_ENB_S1AP_INTERFACES_H

namespace srsenb {

struct s1ap_args_t {
  uint32_t    enb_id;  // 20-bit id (lsb bits)
  uint8_t     cell_id; // 8-bit cell id
  uint16_t    tac;     // 16-bit tac
  uint16_t    mcc;     // BCD-coded with 0xF filler
  uint16_t    mnc;     // BCD-coded with 0xF filler
  std::string mme_addr;
  std::string gtp_bind_addr;
  std::string gtp_advertise_addr;
  std::string s1c_bind_addr;
  uint16_t    s1c_bind_port;
  std::string enb_name;
  uint32_t    ts1_reloc_prep_timeout;
  uint32_t    ts1_reloc_overall_timeout;
};

// S1AP interface for RRC
class s1ap_interface_rrc
{
public:
  using failed_erab_list = std::map<uint32_t, asn1::s1ap::cause_c>;
  struct bearer_status_info {
    uint8_t  erab_id;
    uint16_t pdcp_dl_sn, pdcp_ul_sn;
    uint16_t dl_hfn, ul_hfn;
  };

  virtual void initial_ue(uint16_t                              rnti,
                          uint32_t                              enb_cc_idx,
                          asn1::s1ap::rrc_establishment_cause_e cause,
                          srsran::unique_byte_buffer_t          pdu) = 0;
  virtual void initial_ue(uint16_t                              rnti,
                          uint32_t                              enb_cc_idx,
                          asn1::s1ap::rrc_establishment_cause_e cause,
                          srsran::unique_byte_buffer_t          pdu,
                          uint32_t                              m_tmsi,
                          uint8_t                               mmec)                     = 0;

  virtual void write_pdu(uint16_t rnti, srsran::unique_byte_buffer_t pdu)                 = 0;
  virtual bool user_exists(uint16_t rnti)                                                 = 0;
  virtual void user_mod(uint16_t old_rnti, uint16_t new_rnti)                             = 0;
  virtual bool user_release(uint16_t rnti, asn1::s1ap::cause_radio_network_e cause_radio) = 0;
  virtual bool is_mme_connected()                                                         = 0;

  // Notify S1AP of RRC reconfiguration successful finish.
  // Many S1AP procedures use this notification to indicate successful end (e.g InitialContextSetupRequest)
  virtual void notify_rrc_reconf_complete(uint16_t rnti) = 0;

  /**
   * Command the s1ap to transmit a HandoverRequired message to MME.
   * This message initiates the S1 handover preparation procedure at the Source eNB
   *
   * @param rnti user to perform S1 handover
   * @param target_eci eNB Id + Cell Id of the target eNB
   * @param target_plmn PLMN of the target eNB
   * @param fwd_erabs E-RABs that are candidates to DL forwarding
   * @param rrc_container RRC container with SourceENBToTargetENBTransparentContainer message.
   * @return true if successful
   */
  virtual bool send_ho_required(uint16_t                     rnti,
                                uint32_t                     target_eci,
                                srsran::plmn_id_t            target_plmn,
                                srsran::span<uint32_t>       fwd_erabs,
                                srsran::unique_byte_buffer_t rrc_container,
                                bool                         has_direct_fwd_path) = 0;

  /**
   * Command the s1ap to transmit eNBStatusTransfer message to MME. This message passes the PDCP context of the UE
   * performing S1 handover from source eNB to target eNB.
   *
   * @param rnti user to perform S1 handover
   * @param bearer_status_list PDCP SN and HFN status of the bearers to be preserved at target eNB
   * @return true if successful
   */
  virtual bool send_enb_status_transfer_proc(uint16_t rnti, std::vector<bearer_status_info>& bearer_status_list) = 0;

  /**
   * Cancel on-going S1 Handover. MME should release UE context in target eNB
   * SeNB --> MME
   */
  virtual void send_ho_cancel(uint16_t rnti, const asn1::s1ap::cause_c& cause) = 0;

  /*************************
   *  Target eNB Handover
   ************************/
  virtual bool send_ho_req_ack(const asn1::s1ap::ho_request_s&                msg,
                               uint16_t                                       rnti,
                               uint32_t                                       enb_cc_idx,
                               srsran::unique_byte_buffer_t                   ho_cmd,
                               srsran::span<asn1::s1ap::erab_admitted_item_s> admitted_bearers,
                               srsran::const_span<asn1::s1ap::erab_item_s>    not_admitted_bearers) = 0;
  virtual void send_ho_notify(uint16_t rnti, uint64_t target_eci)                                = 0;

  /**
   * Called during release of a subset of eNB E-RABs. Send E-RAB RELEASE INDICATION to MME.
   * SeNB --> MME
   */
  virtual bool release_erabs(uint16_t rnti, const std::vector<uint16_t>& erabs_successfully_released) = 0;

  virtual bool send_ue_cap_info_indication(uint16_t rnti, srsran::unique_byte_buffer_t ue_radio_cap) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_S1AP_INTERFACES_H
