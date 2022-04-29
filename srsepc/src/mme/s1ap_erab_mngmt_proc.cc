/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsepc/hdr/mme/s1ap_erab_mngmt_proc.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srsran/common/bcd_helpers.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/int_helpers.h"
#include "srsran/common/liblte_security.h"
#include <endian.h>

namespace srsepc {

s1ap_erab_mngmt_proc* s1ap_erab_mngmt_proc::m_instance    = NULL;
pthread_mutex_t       s1ap_erab_mngmt_proc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_erab_mngmt_proc::s1ap_erab_mngmt_proc()
{
  return;
}

s1ap_erab_mngmt_proc::~s1ap_erab_mngmt_proc()
{
  return;
}

s1ap_erab_mngmt_proc* s1ap_erab_mngmt_proc::get_instance()
{
  pthread_mutex_lock(&s1ap_erab_mngmt_proc_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new s1ap_erab_mngmt_proc();
  }
  pthread_mutex_unlock(&s1ap_erab_mngmt_proc_instance_mutex);
  return (m_instance);
}

void s1ap_erab_mngmt_proc::cleanup()
{
  pthread_mutex_lock(&s1ap_erab_mngmt_proc_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_erab_mngmt_proc_instance_mutex);
}

void s1ap_erab_mngmt_proc::init()
{
  m_s1ap      = s1ap::get_instance();
  m_s1ap_args = m_s1ap->m_s1ap_args;
}

bool s1ap_erab_mngmt_proc::send_erab_release_command(uint32_t               enb_ue_s1ap_id,
                                                     uint32_t               mme_ue_s1ap_id,
                                                     std::vector<uint16_t>  erabs_to_release,
                                                     struct sctp_sndrcvinfo enb_sri)
{
  m_logger.info("Preparing to send E-RAB Release Command");

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ERAB_RELEASE);

  asn1::s1ap::erab_release_cmd_s& erab_rel_cmd = tx_pdu.init_msg().value.erab_release_cmd();

  // Add MME and eNB S1AP Ids
  erab_rel_cmd->mme_ue_s1ap_id.value = mme_ue_s1ap_id;
  erab_rel_cmd->enb_ue_s1ap_id.value = enb_ue_s1ap_id;

  // Number of E-RABs to be setup
  erab_rel_cmd->erab_to_be_released_list.value.resize(erabs_to_release.size());
  for (uint32_t i = 0; i < erab_rel_cmd->erab_to_be_released_list.value.size(); i++) {
    erab_rel_cmd->erab_to_be_released_list.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);
    erab_rel_cmd->erab_to_be_released_list.value[i]->erab_item().erab_id = erabs_to_release[i];
    erab_rel_cmd->erab_to_be_released_list.value[i]->erab_item().cause.set(asn1::s1ap::cause_c::types::misc);
    erab_rel_cmd->erab_to_be_released_list.value[i]->erab_item().cause.misc() =
        asn1::s1ap::cause_misc_opts::unspecified;
    m_logger.info("Sending release comman to %d", erabs_to_release[i]);
  }

  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &enb_sri)) {
    m_logger.error("Error sending Initial Context Setup Request.");
    return false;
  }
  return true;
}

bool s1ap_erab_mngmt_proc::send_erab_modify_request(uint32_t                     enb_ue_s1ap_id,
                                                    uint32_t                     mme_ue_s1ap_id,
                                                    std::map<uint16_t, uint16_t> erabs_to_modify,
                                                    srsran::byte_buffer_t*       nas_msg,
                                                    struct sctp_sndrcvinfo       enb_sri)
{
  m_logger.info("Preparing to send E-RAB Modify Command");

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ERAB_MODIFY);

  asn1::s1ap::erab_modify_request_s& erab_mod_req = tx_pdu.init_msg().value.erab_modify_request();

  // Add MME and eNB S1AP Ids
  erab_mod_req->enb_ue_s1ap_id.value = enb_ue_s1ap_id;
  erab_mod_req->mme_ue_s1ap_id.value = mme_ue_s1ap_id;

  // Number of E-RABs to be setup
  erab_mod_req->erab_to_be_modified_list_bearer_mod_req.value.resize(erabs_to_modify.size());
  uint32_t i = 0;
  for (auto erab_it = erabs_to_modify.begin(); erab_it != erabs_to_modify.end(); erab_it++) {
    erab_mod_req->erab_to_be_modified_list_bearer_mod_req.value[i].load_info_obj(
        ASN1_S1AP_ID_ERAB_TO_BE_MODIFIED_ITEM_BEARER_MOD_REQ);
    asn1::s1ap::erab_to_be_modified_item_bearer_mod_req_s& erab_to_mod =
        erab_mod_req->erab_to_be_modified_list_bearer_mod_req.value[i]->erab_to_be_modified_item_bearer_mod_req();
    erab_to_mod.erab_id                                               = erab_it->first;
    erab_to_mod.erab_level_qos_params.qci                             = erab_it->second;
    erab_to_mod.erab_level_qos_params.alloc_retention_prio.prio_level = 15; // lowest
    erab_to_mod.erab_level_qos_params.alloc_retention_prio.pre_emption_cap =
        asn1::s1ap::pre_emption_cap_opts::shall_not_trigger_pre_emption;
    erab_to_mod.erab_level_qos_params.alloc_retention_prio.pre_emption_vulnerability =
        asn1::s1ap::pre_emption_vulnerability_opts::not_pre_emptable;
    erab_to_mod.nas_pdu.resize(nas_msg->N_bytes);
    memcpy(erab_to_mod.nas_pdu.data(), nas_msg->msg, nas_msg->N_bytes);
    m_logger.info("Sending release comman to E-RAB Id %d", erab_it->first);
    i++;
  }

  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &enb_sri)) {
    m_logger.error("Error sending Initial Context Setup Request.");
    return false;
  }
  return true;
}

} // namespace srsepc
