/**
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

#include "srsepc/hdr/mme/s1ap_erab_mngmt_proc.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/int_helpers.h"
#include "srslte/common/liblte_security.h"
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
  m_s1ap_log  = m_s1ap->m_s1ap_log;
  m_s1ap_args = m_s1ap->m_s1ap_args;
  m_pool      = srslte::byte_buffer_pool::get_instance();
}

bool s1ap_erab_mngmt_proc::send_erab_release_command(uint32_t               enb_ue_s1ap_id,
                                                     uint32_t               mme_ue_s1ap_id,
                                                     std::vector<uint16_t>  erabs_to_release,
                                                     struct sctp_sndrcvinfo enb_sri)
{
  m_s1ap_log->info("Preparing to send E-RAB Release Command\n");

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_ERAB_RELEASE);

  asn1::s1ap::erab_release_cmd_ies_container& erab_rel_cmd = tx_pdu.init_msg().value.erab_release_cmd().protocol_ies;

  // Add MME and eNB S1AP Ids
  erab_rel_cmd.mme_ue_s1ap_id.value = mme_ue_s1ap_id;
  erab_rel_cmd.enb_ue_s1ap_id.value = enb_ue_s1ap_id;

  // Number of E-RABs to be setup
  erab_rel_cmd.erab_to_be_released_list.value.resize(erabs_to_release.size());
  for (uint32_t i = 0; i < erab_rel_cmd.erab_to_be_released_list.value.size(); i++) {
    erab_rel_cmd.erab_to_be_released_list.value[i].load_info_obj(ASN1_S1AP_ID_ERAB_ITEM);
    erab_rel_cmd.erab_to_be_released_list.value[i].value.erab_item().erab_id = erabs_to_release[i];
    erab_rel_cmd.erab_to_be_released_list.value[i].value.erab_item().cause.set(asn1::s1ap::cause_c::types::misc);
    erab_rel_cmd.erab_to_be_released_list.value[i].value.erab_item().cause.misc() =
        asn1::s1ap::cause_misc_opts::unspecified;
    m_s1ap_log->info("Sending release comman to %d\n", erabs_to_release[i]);
  }

  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &enb_sri)) {
    m_s1ap_log->error("Error sending Initial Context Setup Request.\n");
    return false;
  }
  return true;
}

} // namespace srsepc
