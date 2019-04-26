/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

s1ap_paging*    s1ap_paging::m_instance    = NULL;
pthread_mutex_t s1ap_paging_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_paging::s1ap_paging()
{
  return;
}

s1ap_paging::~s1ap_paging()
{
  return;
}

s1ap_paging* s1ap_paging::get_instance(void)
{
  pthread_mutex_lock(&s1ap_paging_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new s1ap_paging();
  }
  pthread_mutex_unlock(&s1ap_paging_instance_mutex);
  return (m_instance);
}

void s1ap_paging::cleanup(void)
{
  pthread_mutex_lock(&s1ap_paging_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_paging_instance_mutex);
}

void s1ap_paging::init(void)
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
  LIBLTE_S1AP_S1AP_PDU_STRUCT pdu;
  bzero(&pdu, sizeof(LIBLTE_S1AP_S1AP_PDU_STRUCT));
  pdu.choice_type = LIBLTE_S1AP_S1AP_PDU_CHOICE_INITIATINGMESSAGE;

  LIBLTE_S1AP_INITIATINGMESSAGE_STRUCT* init = &pdu.choice.initiatingMessage;
  init->procedureCode                        = LIBLTE_S1AP_PROC_ID_PAGING;
  init->choice_type                          = LIBLTE_S1AP_INITIATINGMESSAGE_CHOICE_PAGING;
  LIBLTE_S1AP_MESSAGE_PAGING_STRUCT* paging  = &init->choice.Paging;

  // Getting UE NAS Context
  nas* nas_ctx = m_s1ap->find_nas_ctx_from_imsi(imsi);
  if (nas_ctx == NULL) {
    m_s1ap_log->error("Could not find UE to page NAS context\n");
    return false;
  }

  // UE Identity Index
  uint16_t ue_index = imsi % 1024; // LIBLTE_S1AP_UEIDENTITYINDEXVALUE_BIT_STRING_LEN == 10
  uint8_t* tmp_ptr  = paging->UEIdentityIndexValue.buffer;
  liblte_value_2_bits(ue_index, &tmp_ptr, 10);

  // UE Paging Id
  paging->UEPagingID.choice_type                  = LIBLTE_S1AP_UEPAGINGID_CHOICE_S_TMSI;
  paging->UEPagingID.choice.s_TMSI.ext            = false;
  paging->UEPagingID.choice.s_TMSI.mMEC.buffer[0] = m_s1ap->m_s1ap_args.mme_code;
  uint32_t m_tmsi                                 = nas_ctx->m_sec_ctx.guti.m_tmsi;
  srslte::uint32_to_uint8(m_tmsi, paging->UEPagingID.choice.s_TMSI.m_TMSI.buffer);
  paging->UEPagingID.choice.s_TMSI.iE_Extensions_present = false;

  // Paging DRX
  paging->pagingDRX_present = false;

  // CMDomain
  paging->CNDomain = LIBLTE_S1AP_CNDOMAIN_PS;

  // TAI List
  paging->TAIList.len               = 1;
  paging->TAIList.buffer[0].ext     = false;
  paging->TAIList.buffer[0].tAI.ext = false;
  uint32_t plmn                     = htonl(m_s1ap->get_plmn()); // LIBLTE_S1AP_TBCD_STRING_OCTET_STRING_LEN == 3
  paging->TAIList.buffer[0].tAI.pLMNidentity.buffer[0] = ((uint8_t*)&plmn)[1];
  paging->TAIList.buffer[0].tAI.pLMNidentity.buffer[1] = ((uint8_t*)&plmn)[2];
  paging->TAIList.buffer[0].tAI.pLMNidentity.buffer[2] = ((uint8_t*)&plmn)[3];
  uint16_t tac = htons(m_s1ap->m_s1ap_args.tac); // LIBLTE_S1AP_TAC_OCTET_STRING_LEN == 2
  memcpy(paging->TAIList.buffer[0].tAI.tAC.buffer, &tac, sizeof(uint16_t));
  paging->TAIList.buffer[0].tAI.iE_Extensions_present = false;
  paging->TAIList.buffer[0].iE_Extensions_present     = false;

  // CSG Id List
  paging->CSG_IdList_present = false;

  // Paging Priority
  paging->PagingPriority_present = false;

  // Start T3413
  if (!nas_ctx->start_timer(T_3413)) {
    m_s1ap_log->error("Could not start T3413 -- Aborting paging\n");
    // TODO Send data notification failure to SPGW
    return false;
  }
  // Send Paging to eNBs
  m_s1ap_log->info("Paging UE -- M-TMSI :0x%x\n", m_tmsi);
  srslte::byte_buffer_t* reply_buffer = m_pool->allocate();
  LIBLTE_ERROR_ENUM      err          = liblte_s1ap_pack_s1ap_pdu(&pdu, (LIBLTE_BYTE_MSG_STRUCT*)reply_buffer);
  if (err != LIBLTE_SUCCESS) {
    m_s1ap_log->error("Could not pack Paging Message\n");
    m_pool->deallocate(reply_buffer);
    return false;
  }

  for (std::map<uint16_t, enb_ctx_t*>::iterator it = m_s1ap->m_active_enbs.begin(); it != m_s1ap->m_active_enbs.end();
       it++) {
    enb_ctx_t* enb_ctx = it->second;
    if (!m_s1ap->s1ap_tx_pdu(reply_buffer, &enb_ctx->sri)) {
      m_s1ap_log->error("Error paging to eNB. eNB Id: 0x%x.\n", enb_ctx->enb_id);
      m_pool->deallocate(reply_buffer);
      return false;
    }
  }

  return true;
}

} // namespace srsepc
