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

#include "srsepc/hdr/mme/s1ap_mngmt_proc.h"
#include "srsepc/hdr/mme/s1ap.h"
#include "srslte/common/bcd_helpers.h"

namespace srsepc {

s1ap_mngmt_proc* s1ap_mngmt_proc::m_instance    = NULL;
pthread_mutex_t  s1ap_mngmt_proc_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

s1ap_mngmt_proc::s1ap_mngmt_proc()
{
  return;
}

s1ap_mngmt_proc::~s1ap_mngmt_proc()
{
  return;
}

s1ap_mngmt_proc* s1ap_mngmt_proc::get_instance(void)
{
  pthread_mutex_lock(&s1ap_mngmt_proc_instance_mutex);
  if (NULL == m_instance) {
    m_instance = new s1ap_mngmt_proc();
  }
  pthread_mutex_unlock(&s1ap_mngmt_proc_instance_mutex);
  return (m_instance);
}

void s1ap_mngmt_proc::cleanup(void)
{
  pthread_mutex_lock(&s1ap_mngmt_proc_instance_mutex);
  if (NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
  pthread_mutex_unlock(&s1ap_mngmt_proc_instance_mutex);
}

void s1ap_mngmt_proc::init(void)
{
  m_s1ap      = s1ap::get_instance();
  m_s1ap_log  = m_s1ap->m_s1ap_log;
  m_s1mme     = m_s1ap->get_s1_mme();
  m_s1ap_args = m_s1ap->m_s1ap_args;
}

bool s1ap_mngmt_proc::handle_s1_setup_request(const asn1::s1ap::s1_setup_request_s& msg,
                                              struct sctp_sndrcvinfo*               enb_sri)
{
  m_s1ap_log->console("Received S1 Setup Request.\n");
  m_s1ap_log->info("Received S1 Setup Request.\n");

  enb_ctx_t enb_ctx = {};

  if (!unpack_s1_setup_request(msg, &enb_ctx)) {
    m_s1ap_log->error("Malformed S1 Setup Request\n");
    return false;
  }

  // Store SCTP sendrecv info
  memcpy(&enb_ctx.sri, enb_sri, sizeof(struct sctp_sndrcvinfo));
  m_s1ap_log->debug("eNB SCTP association Id: %d\n", enb_sri->sinfo_assoc_id);

  // Log S1 Setup Request Info
  m_s1ap->print_enb_ctx_info(std::string("S1 Setup Request"), enb_ctx);

  // Check matching PLMNs
  if (enb_ctx.plmn != m_s1ap->get_plmn()) {
    m_s1ap_log->console("Sending S1 Setup Failure - Unknown PLMN\n");
    m_s1ap_log->warning("Sending S1 Setup Failure - Unknown PLMN\n");
    send_s1_setup_failure(asn1::s1ap::cause_misc_opts::unknown_plmn, enb_sri);
  } else {
    enb_ctx_t* enb_ptr = m_s1ap->find_enb_ctx(enb_ctx.enb_id);
    if (enb_ptr != nullptr) {
      // eNB already registered
      // TODO replace enb_ctx
      m_s1ap_log->warning("eNB Already registered\n");
    } else {
      // new eNB
      m_s1ap->add_new_enb_ctx(enb_ctx, enb_sri);
    }

    send_s1_setup_response(m_s1ap_args, enb_sri);
    m_s1ap_log->console("Sending S1 Setup Response\n");
    m_s1ap_log->info("Sending S1 Setup Response\n");
  }
  return true;
}

/*
 * Packing/Unpacking helper functions.
 */
bool s1ap_mngmt_proc::unpack_s1_setup_request(const asn1::s1ap::s1_setup_request_s& msg, enb_ctx_t* enb_ctx)
{

  uint8_t  enb_id_bits[32];
  uint32_t plmn = 0;
  uint16_t tac, bplmn;

  uint32_t tmp32 = 0;

  const asn1::s1ap::s1_setup_request_ies_container& s1_req = msg.protocol_ies;

  // eNB Name
  enb_ctx->enb_name_present = s1_req.enbname_present;
  if (s1_req.enbname_present) {
    enb_ctx->enb_name = s1_req.enbname.value.to_string();
  }

  // eNB Id
  enb_ctx->enb_id = s1_req.global_enb_id.value.enb_id.macro_enb_id().to_number();

  // PLMN Id
  ((uint8_t*)&plmn)[1] = s1_req.global_enb_id.value.plm_nid[0];
  ((uint8_t*)&plmn)[2] = s1_req.global_enb_id.value.plm_nid[1];
  ((uint8_t*)&plmn)[3] = s1_req.global_enb_id.value.plm_nid[2];

  enb_ctx->plmn = ntohl(plmn);
  srslte::s1ap_plmn_to_mccmnc(enb_ctx->plmn, &enb_ctx->mcc, &enb_ctx->mnc);

  // SupportedTAs
  enb_ctx->nof_supported_ta = s1_req.supported_tas.value.size();
  for (uint16_t i = 0; i < enb_ctx->nof_supported_ta; i++) {
    const asn1::s1ap::supported_tas_item_s& tas = s1_req.supported_tas.value[i];
    // TAC
    ((uint8_t*)&enb_ctx->tac[i])[0]  = tas.tac[0];
    ((uint8_t*)&enb_ctx->tac[i])[1]  = tas.tac[1];
    enb_ctx->tac[i]                  = ntohs(enb_ctx->tac[i]);
    enb_ctx->nof_supported_bplmns[i] = tas.broadcast_plmns.size();
    for (uint16_t j = 0; j < tas.broadcast_plmns.size(); j++) {
      // BPLMNs
      ((uint8_t*)&enb_ctx->bplmns[i][j])[1] = tas.broadcast_plmns[j][0];
      ((uint8_t*)&enb_ctx->bplmns[i][j])[2] = tas.broadcast_plmns[j][1];
      ((uint8_t*)&enb_ctx->bplmns[i][j])[3] = tas.broadcast_plmns[j][2];

      enb_ctx->bplmns[i][j] = ntohl(enb_ctx->bplmns[i][j]);
    }
  }

  // Default Paging DRX
  enb_ctx->drx.value = s1_req.default_paging_drx.value;

  return true;
}

bool s1ap_mngmt_proc::send_s1_setup_failure(asn1::s1ap::cause_misc_opts::options cause, struct sctp_sndrcvinfo* enb_sri)
{
  s1ap_pdu_t tx_pdu;
  tx_pdu.set_unsuccessful_outcome().load_info_obj(ASN1_S1AP_ID_S1_SETUP);

  asn1::s1ap::s1_setup_fail_ies_container& s1_fail = tx_pdu.unsuccessful_outcome().value.s1_setup_fail().protocol_ies;

  s1_fail.cause.value.set(asn1::s1ap::cause_c::types_opts::misc);
  s1_fail.cause.value.misc().value = cause;

  m_s1ap->s1ap_tx_pdu(tx_pdu, enb_sri);
  return true;
}

bool s1ap_mngmt_proc::send_s1_setup_response(s1ap_args_t s1ap_args, struct sctp_sndrcvinfo* enb_sri)
{
  m_s1ap_log->debug("Sending S1 Setup Response\n");

  s1ap_pdu_t tx_pdu;
  tx_pdu.set_successful_outcome().load_info_obj(ASN1_S1AP_ID_S1_SETUP);

  asn1::s1ap::s1_setup_resp_ies_container& s1_resp = tx_pdu.successful_outcome().value.s1_setup_resp().protocol_ies;

  // MME Name
  s1_resp.mm_ename_present = true;
  s1_resp.mm_ename.value.from_string(s1ap_args.mme_name);

  // Served GUMEIs
  s1_resp.served_gummeis.value.resize(1); // TODO Only one served GUMMEI supported

  uint32_t plmn = 0;
  srslte::s1ap_mccmnc_to_plmn(s1ap_args.mcc, s1ap_args.mnc, &plmn);
  plmn = htonl(plmn);

  asn1::s1ap::served_gummeis_item_s& serv_gummei = s1_resp.served_gummeis.value[0];

  serv_gummei.served_plmns.resize(1);
  serv_gummei.served_plmns[0][0] = ((uint8_t*)&plmn)[1];
  serv_gummei.served_plmns[0][1] = ((uint8_t*)&plmn)[2];
  serv_gummei.served_plmns[0][2] = ((uint8_t*)&plmn)[3];

  serv_gummei.served_group_ids.resize(1);
  serv_gummei.served_group_ids[0].from_number(htons(s1ap_args.mme_group));

  serv_gummei.served_mmecs.resize(1); // Only one MMEC served
  serv_gummei.served_mmecs[0].from_number(s1ap_args.mme_code);

  s1_resp.relative_mme_capacity.value = 255;

  if (!m_s1ap->s1ap_tx_pdu(tx_pdu, enb_sri)) {
    m_s1ap_log->error("Error sending S1 Setup Response.\n");
  } else {
    m_s1ap_log->debug("S1 Setup Response sent\n");
  }
  return true;
}

} // namespace srsepc
