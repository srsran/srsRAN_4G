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
#include "srsepc/hdr/mme/s1ap_paging.h"
#include "srsepc/hdr/mme/mme.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srslte/common/bcd_helpers.h"
#include "srslte/common/int_helpers.h"
#include <inttypes.h> // for printing uint64_t

namespace srsepc {

s1ap_paging* s1ap_paging::get_instance()
{
  static std::unique_ptr<s1ap_paging> m_instance = std::unique_ptr<s1ap_paging>(new s1ap_paging);
  return m_instance.get();
}

void s1ap_paging::init()
{
  m_s1ap      = s1ap::get_instance();
  m_mme       = mme::get_instance();
  m_s1ap_log  = m_s1ap->m_s1ap_log;
  m_s1ap_args = m_s1ap->m_s1ap_args;
  m_pool      = srslte::byte_buffer_pool::get_instance();
}

bool s1ap_paging::send_paging(uint64_t imsi, uint16_t erab_to_setup)
{
  m_s1ap_log->info("Preparing to Page UE -- IMSI %015" PRIu64 "\n", imsi);

  // Prepare reply PDU
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_init_msg().load_info_obj(ASN1_S1AP_ID_PAGING);
  asn1::s1ap::paging_ies_container& paging = tx_pdu.init_msg().value.paging().protocol_ies;

  // Getting UE NAS Context
  nas* nas_ctx = m_s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == nullptr) {
    m_s1ap_log->error("Could not find UE to page NAS context\n");
    return false;
  }

  // UE Identity Index
  uint16_t ue_index = imsi % 1024;
  paging.ue_id_idx_value.value.from_number(ue_index);

  // UE Paging Id
  paging.ue_paging_id.value.set_s_tmsi();
  paging.ue_paging_id.value.s_tmsi().mmec.from_number(m_s1ap->m_s1ap_args.mme_code);
  paging.ue_paging_id.value.s_tmsi().m_tmsi.from_number(nas_ctx->m_sec_ctx.guti.m_tmsi);

  // CMDomain
  paging.cn_domain.value = asn1::s1ap::cn_domain_opts::ps;

  // TAI List
  paging.tai_list.value.resize(1);
  paging.tai_list.value[0].load_info_obj(ASN1_S1AP_ID_TAI_ITEM);

  uint32_t plmn = m_s1ap->get_plmn();
  paging.tai_list.value[0].value.tai_item().tai.plm_nid.from_number(plmn);

  uint16_t tac = m_s1ap->m_s1ap_args.tac;
  paging.tai_list.value[0].value.tai_item().tai.tac.from_number(tac);

  // Start T3413
  if (!nas_ctx->start_timer(T_3413)) {
    m_s1ap_log->error("Could not start T3413 -- Aborting paging\n");
    // TODO Send data notification failure to SPGW
    return false;
  }

  for (std::map<uint16_t, enb_ctx_t*>::iterator it = m_s1ap->m_active_enbs.begin(); it != m_s1ap->m_active_enbs.end();
       it++) {
    enb_ctx_t* enb_ctx = it->second;
    if (!m_s1ap->s1ap_tx_pdu(tx_pdu, &enb_ctx->sri)) {
      m_s1ap_log->error("Error paging to eNB. eNB Id: 0x%x.\n", enb_ctx->enb_id);
      return false;
    }
  }

  return true;
}

} // namespace srsepc
