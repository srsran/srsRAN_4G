/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsran/interfaces/enb_rrc_interface_types.h"

#ifndef SRSRAN_ENB_RRC_INTERFACES_H
#define SRSRAN_ENB_RRC_INTERFACES_H

namespace srsenb {

// RRC interface for S1AP
class rrc_interface_s1ap
{
public:
  using failed_erab_list = std::map<uint32_t, asn1::s1ap::cause_c>;

  virtual void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu)                    = 0;
  virtual void release_ue(uint16_t rnti)                                                         = 0;
  virtual bool setup_ue_ctxt(uint16_t rnti, const asn1::s1ap::init_context_setup_request_s& msg) = 0;
  virtual bool modify_ue_ctxt(uint16_t rnti, const asn1::s1ap::ue_context_mod_request_s& msg)    = 0;
  virtual bool has_erab(uint16_t rnti, uint32_t erab_id) const                                   = 0;
  virtual bool release_erabs(uint32_t rnti)                                                      = 0;

  virtual int  get_erab_addr_in(uint16_t rnti, uint16_t erab_id, transp_addr_t& addr_in, uint32_t& teid_in) const  = 0;
  virtual void set_aggregate_max_bitrate(uint16_t rnti, const asn1::s1ap::ue_aggregate_maximum_bitrate_s& bitrate) = 0;

  /**
   * TS 36.413, 8.2.1 and 8.3.1 - Setup E-RAB / Initial Context Setup
   * @return if error, cause argument is updated with cause
   */
  virtual int setup_erab(uint16_t                                   rnti,
                         uint16_t                                   erab_id,
                         const asn1::s1ap::erab_level_qos_params_s& qos_params,
                         srsran::const_span<uint8_t>                nas_pdu,
                         const transp_addr_t&                       addr,
                         uint32_t                                   gtpu_teid_out,
                         asn1::s1ap::cause_c&                       cause) = 0;
  /**
   * TS 36.413, 8.2.2 - Modify E-RAB
   * @return if error, cause argument is updated with cause
   */
  virtual int modify_erab(uint16_t                                   rnti,
                          uint16_t                                   erab_id,
                          const asn1::s1ap::erab_level_qos_params_s& qos_params,
                          srsran::const_span<uint8_t>                nas_pdu,
                          asn1::s1ap::cause_c&                       cause) = 0;
  /**
   * TS 36.413, 8.2.3 - Release E-RAB id
   * @return error if E-RAB id or rnti were not found
   */
  virtual int release_erab(uint16_t rnti, uint16_t erab_id) = 0;

  virtual void add_paging_id(uint32_t ueid, const asn1::s1ap::ue_paging_id_c& ue_paging_id) = 0;

  /// TS 36.413, 8.2.1, 8.2.2, 8.2.3 - Notify UE of ERAB updates (done via RRC Reconfiguration Message)
  virtual int notify_ue_erab_updates(uint16_t rnti, srsran::const_span<uint8_t> nas_pdu) = 0;

  /**
   * Reports the reception of S1 HandoverCommand / HandoverPreparationFailure or abnormal conditions during
   * S1 Handover preparation back to RRC.
   *
   * @param rnti user
   * @param is_success true if ho cmd was received
   * @param container TargeteNB RRCConnectionReconfiguration message with MobilityControlInfo
   */
  enum class ho_prep_result { success, failure, timeout };
  virtual void ho_preparation_complete(uint16_t                     rnti,
                                       ho_prep_result               result,
                                       const asn1::s1ap::ho_cmd_s&  msg,
                                       srsran::unique_byte_buffer_t container) = 0;
  virtual uint16_t
               start_ho_ue_resource_alloc(const asn1::s1ap::ho_request_s&                                   msg,
                                          const asn1::s1ap::sourceenb_to_targetenb_transparent_container_s& container,
                                          asn1::s1ap::cause_c&                                              failure_cause)                                     = 0;
  virtual void set_erab_status(uint16_t rnti, const asn1::s1ap::bearers_subject_to_status_transfer_list_l& erabs) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACES_H
