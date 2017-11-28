/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
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

#include <iostream>
#include "srslte/asn1/gtpc.h"
#include "mme/mme_gtpc.h"
#include "mme/s1ap.h"
#include "spgw/spgw.h"

namespace srsepc{

mme_gtpc*          mme_gtpc::m_instance = NULL;
boost::mutex  mme_gtpc_instance_mutex;

mme_gtpc::mme_gtpc()
  :m_next_ctrl_teid(1)
{
}

mme_gtpc::~mme_gtpc()
{
}

mme_gtpc*
mme_gtpc::get_instance(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL == m_instance) {
    m_instance = new mme_gtpc();
  }
  return(m_instance);
}

void
mme_gtpc::cleanup(void)
{
  boost::mutex::scoped_lock lock(mme_gtpc_instance_mutex);
  if(NULL != m_instance) {
    delete m_instance;
    m_instance = NULL;
  }
}


bool
mme_gtpc::init()
{
  m_s1ap = s1ap::get_instance();
  m_mme_gtpc_ip = inet_addr("127.0.0.1");//FIXME At the moment, the GTP-C messages are not sent over the wire. So this parameter is not used.
  m_spgw = spgw::get_instance();

  return true;
}

uint64_t
mme_gtpc::get_new_ctrl_teid()
{
  return m_next_ctrl_teid++; //FIXME Use a Id pool?
}
void
mme_gtpc::send_create_session_request(uint64_t imsi, uint32_t mme_ue_s1ap_id)
{
  struct srslte::gtpc_pdu cs_req_pdu;
  struct srslte::gtpc_create_session_request *cs_req = &cs_req_pdu.choice.create_session_request;

  struct srslte::gtpc_pdu cs_resp_pdu;
 

  //Initialize GTP-C message to zero
  bzero(&cs_req_pdu, sizeof(struct srslte::gtpc_pdu));

  //Setup GTP-C Header. FIXME: Length, sequence and other fields need to be added.
  cs_req_pdu.header.piggyback = false;
  cs_req_pdu.header.teid_present = true;
  cs_req_pdu.header.teid = 0; //Send create session request to the butler TEID
  cs_req_pdu.header.type = srslte::GTPC_MSG_TYPE_CREATE_SESSION_REQUEST;

  //Setup GTP-C Create Session Request IEs
  // Control TEID allocated \\
  cs_req->sender_f_teid.teid = get_new_ctrl_teid();
  cs_req->sender_f_teid.ipv4 = m_mme_gtpc_ip;
  // APN \\
  memcpy(cs_req->apn, "internet", sizeof("internet"));
  // RAT Type \\
  cs_req->rat_type = GTPC_RAT_TYPE::EUTRAN;

  //Save RX Control TEID
  m_teid_to_mme_s1ap_id.insert(std::pair<uint64_t,uint32_t>(cs_req->sender_f_teid.teid, mme_ue_s1ap_id));

  m_spgw->handle_create_session_request(cs_req, &cs_resp_pdu);
  handle_create_session_response(&cs_resp_pdu);
 
}

void
mme_gtpc::handle_create_session_response(srslte::gtpc_pdu *cs_resp_pdu)
{
  struct srslte::gtpc_create_session_response *cs_resp = & cs_resp_pdu->choice.create_session_response;

  if (cs_resp_pdu->header.type != srslte::GTPC_MSG_TYPE_CREATE_SESSION_RESPONSE)
  {
     //m_mme_gtpc_log->warning("Could not create GTPC session.\n");
     //TODO Handle err
     return;
  }
  if (cs_resp->cause.cause_value != srslte::GTPC_CAUSE_VALUE_REQUEST_ACCEPTED){
    //m_mme_gtpc_log->warning("Could not create GTPC session.\n");
    //TODO Handle error
    return;
  }

  //Get MME_UE_S1AP_ID from the Ctrl TEID
  std::map<uint64_t,uint32_t>::iterator id_it = m_teid_to_mme_s1ap_id.find(cs_resp_pdu->header.teid);
  if(id_it == m_teid_to_mme_s1ap_id.end())
  {
    //Could not find MME UE S1AP TEID
    return;
  }
  uint32_t mme_s1ap_id = id_it->second;
  m_s1ap->send_initial_context_setup_request(mme_s1ap_id, cs_resp);
}

} //namespace srsepc
